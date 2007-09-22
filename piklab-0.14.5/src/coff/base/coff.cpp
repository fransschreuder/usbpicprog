/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "coff.h"

#include <time.h>

#include "common/common/misc.h"
#include "devices/list/device_list.h"
#include "devices/base/device_group.h"
#include "devices/pic/base/pic_register.h"
#include "coff_data.h"
#include "common/global/pfile.h"

//----------------------------------------------------------------------------
bool Coff::getULong(const QByteArray &data, uint &offset, uint nbBytes, Log::Base &log, Q_UINT32 &v)
{
  bool ok;
  v = ::getULong(data, offset, nbBytes, &ok);
  if ( !ok ) {
    log.log(Log::Error, i18n("COFF file is truncated (offset: %1  nbBytes: %2  size:%3).").arg(offset).arg(nbBytes).arg(data.count()));
    return false;
  }
  offset += nbBytes;
  return true;
}

bool Coff::getName(const QByteArray &data, uint &offset, uint nbChars, uint stringTableOffset,
                   Log::Base &log, QString &name)
{
  Q_UINT32 v;
  if ( !getULong(data, offset, 4, log, v) ) return false;
  if ( v==0 ) { // name is in string table
    if ( !getULong(data, offset, 4, log, v) ) return false;
    name = QString(data.data()+stringTableOffset+v);
  } else {
    offset -= 4;
    name = QString::fromAscii(data.data()+offset, nbChars);
    offset += nbChars;
  }
  return true;
}

const Coff::FormatData Coff::FORMAT_DATA[Nb_Formats] = {
  { 0x1234, { 20, 16, 40, 18, 16, 12 } }, // old microchip
  { 0x1236, { 20, 28, 40, 18,  8, 10 } }, // PIC30 format
  { 0x1240, { 20, 18, 40, 20, 16, 12 } }  // new microchip (C18 >= 3.0)
};

const Coff::OptHeaderData Coff::OPT_HEADER_DATA[] = {
  { 0x5678, OptHeaderOldMicrochipFormat, true  },
  { 0x0000, OptHeaderNewMicrochipFormat, true  },
  { 0x0001, OptHeaderNewMicrochipFormat, true  }, // PIC30 with debug
  { 0x1388, OptHeaderPiccFormat,         false }, // PICC
  { 0x1B78, OptHeaderCcscFormat,         false }, // CCSC
  { 0x0000, OptHeaderInvalidFormat,      false }
};

//----------------------------------------------------------------------------
QByteArray Coff::Base::readFile(const PURL::Url &url, Log::Base &log)
{
  log.log(Log::Information, i18n("Parsing COFF file: %1").arg(url.pretty()));
  PURL::File file(url, 0);
  if ( !file.openForRead() ) {
    log.log(Log::Error, i18n("Could not open COFF file."));
    return QByteArray();
  }
  QByteArray data = file.readAll();
  if ( file.hasError() ) {
    log.log(Log::Error, i18n("Error reading COFF file."));
    return QByteArray();
  }
  return data;
}

//----------------------------------------------------------------------------
Coff::AuxSymbol *Coff::AuxSymbol::factory(const Object &object, Type type, const QByteArray &data, uint offset, uint stringTableOffset, Log::Base &log)
{
  switch (type) {
    case Direct:  return new AuxSymbolDirect(object, data, offset, stringTableOffset, log);
    case File:    return new AuxSymbolFile(object, data, offset, stringTableOffset, log);
    case Ident:   return new AuxSymbolIdent(object, data, offset, stringTableOffset, log);
    case Section: return new AuxSymbolSection(object, data, offset, stringTableOffset, log);
    case None:    return 0;
  }
  Q_ASSERT(false);
  return 0;
}

Coff::AuxSymbolDirect::AuxSymbolDirect(const Object &object, const QByteArray &data, uint start, uint stringTableOffset, Log::Base &log)
  : AuxSymbol(object)
{
  uint offset = start;
  Q_UINT32 v;
  if ( !getULong(data, offset, 1, log, v) ) return;
  _command = v;
  if ( !getULong(data, offset, 4, log, v) ) return;
  _string = QString(data.data()+stringTableOffset+v);
}

Coff::AuxSymbolFile::AuxSymbolFile(const Object &object, const QByteArray &data, uint start, uint stringTableOffset, Log::Base &log)
  : AuxSymbol(object)
{
  uint offset = start;
  Q_UINT32 v;
  if ( object.format()==PIC30Format ) {
    if ( !getName(data, offset, 14, stringTableOffset, log, _filename) ) return;
    _line = 0;
  } else {
    if ( !getULong(data, offset, 4, log, v) ) return;
    _filename = QString(data.data()+stringTableOffset+v);
    if ( !getULong(data, offset, 4, log, v) ) return;
    _line = v;
  }
}

Coff::AuxSymbolIdent::AuxSymbolIdent(const Object &object, const QByteArray &data, uint start, uint stringTableOffset, Log::Base &log)
  : AuxSymbol(object)
{
  uint offset = start;
  Q_UINT32 v;
  if ( !getULong(data, offset, 4, log, v) ) return;
  _string = QString(data.data()+stringTableOffset+v);
}

Coff::AuxSymbolSection::AuxSymbolSection(const Object &object, const QByteArray &data, uint start, uint, Log::Base &log)
  : AuxSymbol(object)
{
  uint offset = start;
  Q_UINT32 v;
  if ( !getULong(data, offset, 4, log, v) ) return;
  _length = v;
  if ( !getULong(data, offset, 2, log, v) ) return;
  _nbRelocations = v;
  if ( !getULong(data, offset, 2, log, v) ) return;
  _nbLineNumbers = v;
}

//----------------------------------------------------------------------------
Coff::Symbol::Symbol(const Object &object, const QByteArray &data, uint start,
                     uint stringTableOffset, Log::Base &log)
  : Element(object)
{
  uint offset = start;
  Q_UINT32 v;
  if ( !getName(data, offset, 8, stringTableOffset, log, _name) ) return;
  if ( !getULong(data, offset, 4, log, v) ) return;
  _value = v;
  if ( !getULong(data, offset, 2, log, v) ) return;
  _section = v;
  uint nb = (object.format()==NewMicrochipFormat ? 4 : 2);
  if ( !getULong(data, offset, nb, log, v) ) return;
  _type = Type(v & 0x001111);
  _dtype = DerivedType(v & 0x110000);
  if ( !getULong(data, offset, 1, log, v) ) return;
  _class = Class(v);
  if ( !getULong(data, offset, 1, log, v) ) return;
  uint nbAux = v;
  //qDebug("symbol: %s value=%s type=%i dtype=%i class=%i nbAux=%i section=%i", _name.latin1(), toHexLabel(_value, 4).latin1(), _type, _dtype, _class, nbAux, _section);

  AuxSymbol::Type auxType = AuxSymbol::None;
  if ( _name==".direct" ) auxType = AuxSymbol::Direct;
  else if ( _name==".ident" ) auxType = AuxSymbol::Ident;
  else if ( _class==CFile ) auxType = AuxSymbol::File;
  else if ( _class==CSection ) auxType = AuxSymbol::Section;
  if ( auxType!=AuxSymbol::None && nbAux==0 ) log.log(Log::Warning, i18n("Symbol without needed auxilliary symbol (type=%1)").arg(auxType));
  Q_ASSERT( (offset-start)==object.size(SymbolSize) );
  for (uint i=0; i<nbAux; i++) {
    AuxSymbol *aux = AuxSymbol::factory(object, auxType, data, offset, stringTableOffset, log);
    _aux.append(aux);
    offset += object.size(SymbolSize);
    if ( log.hasError() ) return;
    if ( _class==Symbol::CFile && aux->type()==AuxSymbol::File )
      _filename = static_cast<AuxSymbolFile *>(aux)->_filename;
  }
}

Coff::Symbol::~Symbol()
{
  for (uint i=0; i<uint(_aux.count()); i++) delete _aux[i];
}

//----------------------------------------------------------------------------
Coff::Relocation::Relocation(const Object &object, const Section &section,
                             const QByteArray &data, uint start, Log::Base &log)
  : Element(object), _symbol(0)
{
  uint offset = start;
  Q_UINT32 v;
  if ( !getULong(data, offset, 4, log, v) ) return;
  _address = v;
  if ( _address>section._size ) log.log(Log::Warning, i18n("Relocation address beyong section size: %1/%2").arg(v).arg(section._size));
  if ( !getULong(data, offset, 4, log, v) ) return;
  _symbol = object.symbol(v);
  if ( _symbol==0 ) log.log(Log::Warning, i18n("Relocation has unknown symbol: %1").arg(v));
  if ( object.format()!=PIC30Format ) {
    if ( !getULong(data, offset, 2, log, v) ) return;
    _offset = short(v);
  }
  if ( !getULong(data, offset, 2, log, v) ) return;
  _type = v;
  //qDebug("reloc %s: address=%s offset=%i type=%i", _symbol->_name.latin1(), toHexLabel(_address, 4).latin1(), _offset, _type);
}

//----------------------------------------------------------------------------
Coff::CodeLine::CodeLine(const Object &object, const Section &section,
                         const QByteArray &data, uint start, Log::Base &log)
  : Element(object), _section(section), _symbol(0)
{
  uint offset = start;
  Q_UINT32 v;
  if ( !getULong(data, offset, 4, log, v) ) return;
  uint tmp = v;
  if ( object.format()==PIC30Format ) {
    if ( !getULong(data, offset, 4, log, v) ) return;
    _line = v;
    if ( _line==0 ) {
      _address = tmp;
      _symbol = 0;
      //qDebug("code line %i: %s", _line, toHexLabel(_address, nbChars(_address)).latin1());
    } else {
      _symbol = object.symbol(tmp);
      //qDebug("code line %i: %s", _line, _symbol->_name.latin1());
    }
  } else {
    _symbol = object.symbol(tmp);
    if ( !getULong(data, offset, 2, log, v) ) return;
    _line = v;
    if ( object.optHeaderFormat()==OptHeaderPiccFormat && _line>=2 ) _line -= 2; // #### ??
    if ( !getULong(data, offset, 4, log, v) ) return;
    _address = v;
    if ( !getULong(data, offset, 2, log, v) ) return;
    // flags
    if ( !getULong(data, offset, 4, log, v) ) return;
    // function index
    //qDebug("code line %i: %s", _line, toHexLabel(_address, nbChars(_address)).latin1());
  }
//  if ( _symbol && _symbol->_class!=Symbol::CFile )
//    log.log(Log::Warning, i18n("Line without file symbol associated (%1:%2 %3).")
//                         .arg(_section._name).arg(toHexLabel(_address, nbChars(_address))).arg(_symbol->_class));
}

//----------------------------------------------------------------------------
Coff::Section::Section(const Device::Data &device, const Object &object,
                       const QByteArray &data, uint start, uint stringTableOffset, Log::Base &log)
  : Element(object)
{
  uint offset = start;
  Q_UINT32 v;
  if ( !getName(data, offset, 8, stringTableOffset, log, _name) ) return;
  if ( !getULong(data, offset, 4, log, v) ) return;
  _address = v;
  if ( !getULong(data, offset, 4, log, v) ) return;
  //if ( _address!=v ) log.log(Log::Warning, i18n("Virtual address (%1) does not match physical address (%2) in %3.")
  //                                         .arg(toHexLabel(v, 4)).arg(toHexLabel(_address, 4)).arg(_name));
  if ( !getULong(data, offset, 4, log, v) ) return;
  _size = v;
  if ( !getULong(data, offset, 4, log, v) ) return;
  uint dataOffset = v;
  if ( !getULong(data, offset, 4, log, v) ) return;
  uint relocationOffset = v;
  if ( !getULong(data, offset, 4, log, v) ) return;
  uint lineNumberOffset = v;
  if ( !getULong(data, offset, 2, log, v) ) return;
  uint nbRelocations = v;
  if ( !getULong(data, offset, 2, log, v) ) return;
  uint nbLineNumbers = v;
  if ( !getULong(data, offset, 4, log, v) ) return;
  _flags = Flags(v);

  // read data
  Q_ASSERT ( device.group().name()=="pic" );
  const Pic::Data &pdata = static_cast<const Pic::Data &>(device);
  //qDebug("section %s: address=%s size=%i flags=%i", _name.data(), toHexLabel(_address, 4).latin1(), _size, int(_flags));
  if ( _size!=0 && dataOffset!=0 ) {
    uint inc = 1;
    uint nbWords = _size;
    uint nbBytesWord = 1;
    bool b = ( (_flags & FText) || (_flags & FDataRom) );
    if (b) {
      nbBytesWord = pdata.nbBytesWord(Pic::MemoryCode);
      nbWords /= nbBytesWord;
      inc = pdata.addressIncrement(Pic::MemoryCode);
    }
    for (uint i=0; i<nbWords; i++) {
      uint address = _address + inc*i;
      if ( !getULong(data, dataOffset, nbBytesWord, log, v) ) return;
      _data[address].value = v;
    }
    for (uint i=0; i<nbWords; i++) {
      uint address = _address + inc*i;
      uint op = _data[address].value;
      if ( _flags & FText ) {
        char buffer[512];
        buffer[0] = 0;
        uint op2 = ((i+1)<nbWords ? _data[address+inc].value : 0);
        uint nbop = disassemble(op, op2, address/inc, pdata.architecture(), buffer, 512);
        _data[address].disasm = QString(buffer);
        _data[address].opcode = toHex(op, pdata.nbCharsWord(Pic::MemoryCode));
        if ( nbop==2 ) {
          _data[address+inc].opcode = toHex(op2, pdata.nbCharsWord(Pic::MemoryCode));
          i++;
        }
        //qDebug("  %s: %s (%s %s)", toHex(address, 4).data(), _data[address].disasm.data(), _data[address].opcode.data(), (nbop==2 ? _data[address+inc].opcode.data() : ""));
      } else if ( _flags & FDataRom ) _data[address].opcode = toHex(op, 4);
      else if ( _flags & FData ) _data[address].opcode = toHex(op & 0xFF, 2);
    }
  }

  // read relocations
  if ( nbRelocations!=0 && relocationOffset!=0 ) {
    for (uint i=0; i<nbRelocations; i++) {
      _relocations.append(new Relocation(object, *this, data, relocationOffset, log));
      relocationOffset += object.size(RelocationSize);
      if ( log.hasError() ) return;
    }
  }

  // read line numbers
  if ( nbLineNumbers!=0 && lineNumberOffset!=0 ) {
    for (uint i=0; i<nbLineNumbers; i++) {
      _lines.append(new CodeLine(object, *this, data, lineNumberOffset, log));
      lineNumberOffset += object.size(LineNumberSize);
      if ( log.hasError() ) return;
    }
  }
}

Coff::Section::~Section()
{
  for (uint i=0; i<uint(_relocations.count()); i++) delete _relocations[i];
  for (uint i=0; i<uint(_lines.count()); i++) delete _lines[i];
}

Coff::Section::Type Coff::Section::type() const
{
  if ( _name==".config" ) return Config;
  if ( _name==".devid" ) return DevId;
  if ( _name==".idlocs" ) return IdLocs;
  if ( _flags & FText ) return Code;
  if ( _flags & FData ) return IData;
  if ( _flags & FBSS ) return UData;
  if ( _flags & FDataRom ) return DataRom;
  return Unknown;
}

//----------------------------------------------------------------------------
Coff::Object::Object(const Device::Data &device, const PURL::Url &url, Log::Base &log, bool *identified)
  : Base(url), _device(device)
{
  QByteArray data = readFile(url, log);
  if ( log.hasError() ) return;

  // header
  Q_UINT32 v;
  uint offset = 0;
  if ( !getULong(data, offset, 2, log, v) ) return;
  log.log(Log::ExtraDebug, QString("COFF format: %1").arg(toHexLabel(v, 4)));
  _format = Nb_Formats;
  for (uint i=0; i<Nb_Formats; i++)
    if ( v==FORMAT_DATA[i].magic ) _format = Format(i);
  if ( _format==Nb_Formats ) {
    log.log(Log::Error, i18n("COFF format not supported: magic number is %1.").arg(toHexLabel(v, 4)));
    if (identified) *identified = false;
    return;
  }
  if (identified) *identified = true;
  if ( !parseHeader(data, offset, log) ) return;

  // optionnal header
  Q_ASSERT( offset==size(HeaderSize) );
  if ( !getULong(data, offset, 2, log, v) ) return;
  log.log(Log::ExtraDebug, QString("COFF optionnal header format: %1").arg(toHexLabel(v, 4)));
  _optHeaderFormat = OptHeaderInvalidFormat;
  uint i = 0;
  for (; OPT_HEADER_DATA[i].optHeaderFormat!=OptHeaderInvalidFormat; i++) if ( v==OPT_HEADER_DATA[i].magic ) break;
  _optHeaderFormat = OPT_HEADER_DATA[i].optHeaderFormat;
  if ( _optHeaderFormat==OptHeaderInvalidFormat ) {
    log.log(Log::Warning, i18n("Optional header format not supported: magic number is %1").arg(toHexLabel(v, 4)));
    offset += size(OptHeaderSize)-2;
  } else if ( !OPT_HEADER_DATA[i].parsed ) {
    log.log(Log::NormalDebug, i18n("Optional header not parsed: magic number is %1").arg(toHexLabel(v, 4)));
    offset += size(OptHeaderSize)-2;
  } else if ( !parseOptionnalHeader(data, offset, log) ) return;

  // parse symbol table
  uint stringTableOffset = _symbolOffset + _nbSymbols*size(SymbolSize);
  for (uint i=0; i<_nbSymbols; i++) {
    Symbol *s = new Symbol(*this, data, _symbolOffset, stringTableOffset, log);
    _symbols[i] = s;
    i += s->_aux.count();
    _symbolOffset += (s->_aux.count()+1)*size(SymbolSize);
    if ( log.hasError() ) return;
  }

  // parse sections
  Q_ASSERT( offset==(size(HeaderSize) + size(OptHeaderSize)) );
  for (uint i=0; i<_nbSections; i++) {
    _sections.append(new Section(_device, *this, data, offset, stringTableOffset, log));
    offset += size(SectionHeaderSize);
    if ( log.hasError() ) return;
  }

  // extract filenames
  QMap<uint, Symbol *>::const_iterator it;
  for (it=_symbols.begin(); it!=_symbols.end(); ++it) {
    QString s = it.data()->_filename;
    if ( s.isEmpty() || s=="fake" || _filenames.contains(s) ) continue;
    _filenames.append(s);
  }

  // extract variables
  for (it=_symbols.begin(); it!=_symbols.end(); ++it) {
    if ( it.data()->_class!=Symbol::CStatic ) continue;
    if ( it.data()->_section==Symbol::NUndefined || it.data()->_section==Symbol::NAbsolute
         || it.data()->_section==Symbol::NDebug ) continue;
    QString name = it.data()->_name;
    if ( name.startsWith("_$_") || name.startsWith("__") || name.startsWith(".") ) continue; // special variables (?)
    _variables[name] = it.data()->_value & 0xFFF; // #### ??
  }
}

bool Coff::Object::parseHeader(const QByteArray &data, uint &offset, Log::Base &log)
{
  Q_UINT32 v;
  if ( !getULong(data, offset, 2, log, v) ) return false;
  _nbSections = v;
  if ( !getULong(data, offset, 4, log, v) ) return false;
//  time_t time = v;
  if ( !getULong(data, offset, 4, log, v) ) return false;
  _symbolOffset = v;
  if ( !getULong(data, offset, 4, log, v) ) return false;
  _nbSymbols = v;
  if ( !getULong(data, offset, 2, log, v) ) return false;
  if ( v!=size(OptHeaderSize) ) {
    log.log(Log::Error, i18n("Optionnal header size is not %1: %2").arg(size(OptHeaderSize)).arg(v));
    return false;
  }
  if ( !getULong(data, offset, 2, log, v) ) return false;
  _flags = Flags(v);
  return true;
}

bool Coff::Object::parseOptionnalHeader(const QByteArray &data, uint &offset, Log::Base &log)
{
  Q_UINT32 v;
  int nb = (_format==NewMicrochipFormat ? 4 : 2);
  if ( !getULong(data, offset, nb, log, v) ) return false;
  // version stamp
  if ( _format==PIC30Format ) {
    offset += size(OptHeaderSize)-4;
    // #### TODO
  } else {
    if ( !getULong(data, offset, 4, log, v) ) return false;
    // #### at least for C18 compiler, it can be compiled for generic processor: in such case
    // the pic type will be 18C452 in non-extended mode and 18F4620 for extended mode...
    QString name = Coff::findId(v);
    if ( name.isEmpty() ) log.log(Log::NormalDebug, i18n("Unknown processor type: %1").arg(toHexLabel(v, 4)));
    else if ( name!=_device.name() ) log.log(Log::NormalDebug, i18n("Different processor name: %1").arg(name));
    if ( !getULong(data, offset, 4, log, v) ) return false;
    Q_ASSERT( _device.group().name()=="pic" );
    const Pic::Data &pdata = static_cast<const Pic::Data &>(_device);
    uint nbBits = pdata.nbBitsWord(Pic::MemoryCode) / pdata.addressIncrement(Pic::MemoryCode);
    if ( v!=nbBits ) log.log(Log::NormalDebug, i18n("Rom width is not %1: %2").arg(nbBits).arg(v));
    if ( !getULong(data, offset, 4, log, v) ) return false;
    nbBits = pdata.registersData().nbBits();
    if ( v!=nbBits ) log.log(Log::NormalDebug, i18n("Ram width is not %1: %2").arg(nbBits).arg(v));
  }
  return true;
}

Coff::Object::~Object()
{
  QMap<uint, Symbol *>::iterator it;
  for (it=_symbols.begin(); it!=_symbols.end(); ++it) delete it.data();
  for (uint i=0; i<uint(_sections.count()); i++) delete _sections[i];
}

const Coff::Symbol *Coff::Object::symbol(uint i) const
{
  if ( !_symbols.contains(i) ) return 0;
  return _symbols[i];
}

QString Coff::Object::variableName(uint address) const
{
  QMap<QString, uint>::const_iterator it;
  for (it=_variables.begin(); it!=_variables.end(); ++it)
    if ( it.data()==address ) return it.key();
  return QString::null;
}

//----------------------------------------------------------------------------
Coff::Archive::Archive(const PURL::Url &url, Log::Base &log, bool *identified)
  : Base(url)
{
  QByteArray data = readFile(url, log);
  if ( log.hasError() ) return;
  if ( QString(data)!="!<arch>\012" ) {
    if (identified) *identified = false;
    log.log(Log::Error, i18n("Incorrect magic number for archive COFF file."));
    return;
  }
  if (identified) *identified = true;
  // #### TODO
}

//----------------------------------------------------------------------------
QValueVector<Pic::RegisterNameData> Pic::sfrList(const Pic::Data &data)
{
  QValueVector<Pic::RegisterNameData> list;
  const Pic::RegistersData &rdata = data.registersData();
  for (uint i=0; i<rdata.nbRegisters(); i++) {
    uint address = rdata.nbBytes() * i;
    Pic::RegisterType type = rdata.type(address);
    Device::RegisterProperties rp = rdata.properties(address);
    if ( !(rp & Device::Readable) ) continue;
    Register::TypeData rtd(address, rdata.nbChars());
    if ( type==Pic::Sfr ) list.append(Pic::RegisterNameData(rdata.label(address), rtd));
  }
  QMap<QString, Pic::CombinedData>::const_iterator it;
  for (it=rdata.combined.begin(); it!=rdata.combined.end(); ++it) {
    Register::TypeData td(it.key(), it.data().address, it.data().nbChars);
    list.append(Pic::RegisterNameData(it.key(), td));
  }
  if ( data.architecture()==Pic::Arch_16X )
    list.append(Pic::RegisterNameData("WREG", Register::TypeData("WREG", rdata.nbChars())));
  qHeapSort(list);
  return list;
}

QValueVector<Pic::RegisterNameData> Pic::gprList(const Pic::Data &data, const Coff::Object *coff)
{
  QValueVector<Pic::RegisterNameData> list;
  const Pic::RegistersData &rdata = data.registersData();
  for (uint i=0; i<rdata.nbRegisters(); i++) {
    uint address = rdata.nbBytes() * i;
    Pic::RegisterType type = rdata.type(address);
    Device::RegisterProperties rp = rdata.properties(address);
    if ( !(rp & Device::Readable) ) continue;
    if (type==Pic::Gpr ) {
      QString s = toHexLabel(address, rdata.nbCharsAddress());
      if (coff) {
        QString name = coff->variableName(address);
        if ( !name.isEmpty() ) s += " (" + name + ")";
      }
      Register::TypeData rtd(address, rdata.nbChars());
      list.append(Pic::RegisterNameData(s, rtd));
    }
  }
  return list;
}

QValueVector<Pic::RegisterNameData> Pic::variableList(const Pic::Data &data, const Coff::Object &coff)
{
  QValueVector<Pic::RegisterNameData> list;
  const Pic::RegistersData &rdata = data.registersData();
  QMap<QString, uint> variables = coff.variables();
  QMap<QString, uint>::const_iterator vit;
  for (vit=variables.begin(); vit!=variables.end(); ++vit) {
    Register::TypeData rtd(vit.data(), rdata.nbChars());
    list.append(Pic::RegisterNameData(vit.key() + " (" + toHexLabel(vit.data(), rdata.nbCharsAddress()) + ")", rtd));
  }
  qHeapSort(list);
  return list;
}
