/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "coff_object.h"

#include "common/common/misc.h"
#include "devices/list/device_list.h"
#include "devices/base/device_group.h"
#include "devices/pic/base/pic_register.h"
#include "coff_data.h"
#include "common/global/pfile.h"

//----------------------------------------------------------------------------
bool Coff::getName(const QByteArray &data, uint &offset, uint nbChars, uint stringTableOffset,
                   Log::Base &log, QString &name)
{
  Q_UINT32 v;
  if ( !getULong(data, offset, 4, log, v) ) return false;
  if ( v!=0 ) { // name is not in string table
    offset -= 4;
    return getString(data, offset, nbChars, log, name);
  }
  if ( !getULong(data, offset, 4, log, v) ) return false;
  // ### do a sanity check here
  name = QString(data.data()+stringTableOffset+v);
  return true;
}

const Coff::OptHeaderFormat::Data Coff::OptHeaderFormat::DATA[Nb_Types] = {
  { 0, I18N_NOOP("Old Microchip") },
  { 0, I18N_NOOP("New Microchip") },
  { 0, I18N_NOOP("PICC Compiler") },
  { 0, I18N_NOOP("CCS Compiler")  }
};

const Coff::OptHeaderData Coff::OPT_HEADER_DATA[] = {
  { 0x5678, OptHeaderFormat::OldMicrochip, true  },
  { 0x0000, OptHeaderFormat::NewMicrochip, true  },
  { 0x0001, OptHeaderFormat::NewMicrochip, true  }, // PIC30 with debug
  { 0x1388, OptHeaderFormat::Picc,         false }, // PICC
  { 0x1B78, OptHeaderFormat::Ccsc,         false }, // CCSC
  { 0x0000, OptHeaderFormat::Nb_Types,     false }
};

//----------------------------------------------------------------------------
const Coff::AuxSymbolType::Data Coff::AuxSymbolType::DATA[Nb_Types] = {
  { 0, I18N_NOOP("Direct") },
  { 0, I18N_NOOP("File") },
  { 0, I18N_NOOP("Indentifier") },
  { 0, I18N_NOOP("Section") }
};

Coff::AuxSymbol *Coff::AuxSymbol::factory(const Object &object, AuxSymbolType type, const QByteArray &data, uint offset, uint stringTableOffset, Log::Base &log)
{
  switch (type.type()) {
    case AuxSymbolType::Direct:     return new AuxSymbolDirect(object, data, offset, stringTableOffset, log);
    case AuxSymbolType::File:       return new AuxSymbolFile(object, data, offset, stringTableOffset, log);
    case AuxSymbolType::Identifier: return new AuxSymbolIdentifier(object, data, offset, stringTableOffset, log);
    case AuxSymbolType::Section:    return new AuxSymbolSection(object, data, offset, stringTableOffset, log);
    case AuxSymbolType::Nb_Types:   return new AuxSymbolUnknown(object);
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
  if ( object.format()==Format::PIC30 ) {
    if ( !getName(data, offset, 14, stringTableOffset, log, _filename) ) return;
    _line = 0;
  } else {
    if ( !getULong(data, offset, 4, log, v) ) return;
    _filename = QString(data.data()+stringTableOffset+v);
    if ( !getULong(data, offset, 4, log, v) ) return;
    _line = v;
  }
}

Coff::AuxSymbolIdentifier::AuxSymbolIdentifier(const Object &object, const QByteArray &data, uint start, uint stringTableOffset, Log::Base &log)
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
const Coff::SymbolSectionType::Data Coff::SymbolSectionType::DATA[Nb_Types] = {
  { 0, I18N_NOOP("Inside Section") },
  { 0, I18N_NOOP("Undefined Section") },
  { 0, I18N_NOOP("Absolute Value") },
  { 0, I18N_NOOP("Debug Symbol") }
};

const Coff::SymbolClass::Data Coff::SymbolClass::DATA[Nb_Types] = {
  { 0, I18N_NOOP("Automatic Variable"), 1 },
  { 0, I18N_NOOP("External Symbol"), 2 },
  { 0, I18N_NOOP("Static Symbol"), 3 },
  { 0, I18N_NOOP("Register Variable"), 4 },
  { 0, I18N_NOOP("External Definition"), 5 },
  { 0, I18N_NOOP("Label"), 6 },
  { 0, I18N_NOOP("Undefined Label"), 7 },
  { 0, I18N_NOOP("Member of Structure"), 8 },
  { 0, I18N_NOOP("Function Argument"), 9 },
  { 0, I18N_NOOP("Structure Tag"), 10 },
  { 0, I18N_NOOP("Member of Union"), 11 },
  { 0, I18N_NOOP("Union Tag"), 12 },
  { 0, I18N_NOOP("Type Definition"), 13 },
  { 0, I18N_NOOP("Undefined Static"), 14 },
  { 0, I18N_NOOP("Enumeration Tag"), 15 },
  { 0, I18N_NOOP("Member of Enumeration"), 16 },
  { 0, I18N_NOOP("Register Parameter"), 17 },
  { 0, I18N_NOOP("Bit Field"), 18 },
  { 0, I18N_NOOP("Auto Argument"), 19 },
  { 0, I18N_NOOP("Dummy Entry (end of block)"), 20 },
  { 0, I18N_NOOP("Beginning or End of Block"), 100 },
  { 0, I18N_NOOP("Beginning or End of Function"), 101 },
  { 0, I18N_NOOP("End of Structure"), 102 },
  { 0, I18N_NOOP("Filename"), 103 },
  { 0, I18N_NOOP("Line Number"), 104 },
  { 0, I18N_NOOP("Duplicate Tag"), 105 },
  { 0, I18N_NOOP("Section"), 109 }
};

const Coff::SymbolType::Data Coff::SymbolType::DATA[Nb_Types] = {
  { 0, I18N_NOOP("Void"), 0x0001 },
  { 0, I18N_NOOP("Char"), 0x0010 },
  { 0, I18N_NOOP("Short"), 0x0011 },
  { 0, I18N_NOOP("Int"), 0x0100 },
  { 0, I18N_NOOP("Long"), 0x0101 },
  { 0, I18N_NOOP("Float"), 0x0110 },
  { 0, I18N_NOOP("Double"), 0x0111 },
  { 0, I18N_NOOP("Structure"), 0x1000 },
  { 0, I18N_NOOP("Union"), 0x1001 },
  { 0, I18N_NOOP("Enumeration"), 0x1010 },
  { 0, I18N_NOOP("Member Of Enumeration"), 0x1011 },
  { 0, I18N_NOOP("Unsigned Char"), 0x1100 },
  { 0, I18N_NOOP("Unsigned Short"), 0x1101 },
  { 0, I18N_NOOP("Unsigned Int"), 0x1110 },
  { 0, I18N_NOOP("Unsigned Long"), 0x1111 },
  { 0, I18N_NOOP("Long Double"), 0x10000 }
};

const Coff::SymbolDerivedType::Data Coff::SymbolDerivedType::DATA[Nb_Types] = {
  { 0, I18N_NOOP("Pointer"),     0x010000 },
  { 0, I18N_NOOP("Function"),    0x100000 },
  { 0, I18N_NOOP("Array"),       0x110000 }
};

Coff::Symbol::Symbol(const Object &object, const QByteArray &data, uint start,
                     uint stringTableOffset, const QString &lastFilename, Log::Base &log)
  : BaseSymbol(object)
{
  uint offset = start;
  Q_UINT32 v;
  if ( !getName(data, offset, 8, stringTableOffset, log, _name) ) return;
  if ( !getULong(data, offset, 4, log, v) ) return;
  _value = v;
  if ( !getULong(data, offset, 2, log, v) ) return;
  _section = v;
  uint nb = (object.format()==Format::NewMicrochip ? 4 : 2);
  if ( !getULong(data, offset, nb, log, v) ) return;
  _type = SymbolType::Nb_Types;
  FOR_EACH(SymbolType, type)
    if ( (v & 0x001111)==type.data().id ) { _type = type; break; }
  _dtype = SymbolDerivedType::Nb_Types;
  FOR_EACH(SymbolDerivedType, dtype)
    if ( (v & 0x110000)==dtype.data().id ) { _dtype = dtype; break; }
  if ( !getULong(data, offset, 1, log, v) ) return;
  _sclass = SymbolClass::Nb_Types;
  FOR_EACH(SymbolClass, sclass)
    if ( v==sclass.data().id ) { _sclass = sclass; break; }
  if ( !getULong(data, offset, 1, log, v) ) return;
  uint nbAux = v;
  //qDebug("symbol: %s value=%s type=%i dtype=%i class=%i nbAux=%i section=%i", _name.latin1(), toHexLabel(_value, 4).latin1(), _type, _dtype, _class, nbAux, _section);

  AuxSymbolType auxType = AuxSymbolType::Nb_Types;
  if ( _name==".direct" ) auxType = AuxSymbolType::Direct;
  else if ( _name==".ident" ) auxType = AuxSymbolType::Identifier;
  else if ( _sclass==SymbolClass::Filename ) auxType = AuxSymbolType::File;
  else if ( _sclass==SymbolClass::Section ) auxType = AuxSymbolType::Section;
  if ( auxType!=AuxSymbolType::Nb_Types && nbAux==0 ) log.log(Log::LineType::Warning, i18n("Symbol without needed auxilliary symbol (type=%1)").arg(auxType.type()));
  Q_ASSERT( (offset-start)==object.size(SymbolSize) );
  _aux.resize(nbAux);
  for (uint i=0; i<nbAux; i++) {
    _aux[i] = AuxSymbol::factory(object, auxType, data, offset, stringTableOffset, log);
    offset += object.size(SymbolSize);
    if ( log.hasError() ) return;
  }

  _filename = lastFilename;
  for (uint i=0; i<uint(_aux.count()); i++)
    if ( _aux[i]->type()==AuxSymbolType::File ) _filename = static_cast<AuxSymbolFile *>(_aux[i])->filename();
}

Coff::SymbolSectionType Coff::Symbol::sectionType() const
{
  switch (_section) {
    case 0x0000: return SymbolSectionType::UndefinedSection;
    case 0xFFFF: return SymbolSectionType::AbsoluteValue;
    case 0xFFFE: return SymbolSectionType::DebugSymbol;
  }
  return SymbolSectionType::InsideSection;
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
  if ( _address>section.size() ) log.log(Log::LineType::Warning, i18n("Relocation address beyong section size: %1/%2").arg(v).arg(section.size()));
  if ( !getULong(data, offset, 4, log, v) ) return;
  if ( v>=object.nbSymbols() ) {
    log.log(Log::LineType::Error, i18n("Relocation has unknown symbol: %1").arg(v));
    return;
  }
  if ( object.symbol(v)->isAuxSymbol() ) {
    log.log(Log::LineType::Error, i18n("Relocation is an auxiliary symbol: %1").arg(v));
    return;
  }
  _symbol = static_cast<const Symbol *>(object.symbol(v));
  if ( object.format()!=Format::PIC30 ) {
    if ( !getULong(data, offset, 2, log, v) ) return;
    _offset = short(v);
  }
  if ( !getULong(data, offset, 2, log, v) ) return;
  _type = v;
  //qDebug("reloc %s: address=%s offset=%i type=%i", _symbol->_name.latin1(), toHexLabel(_address, 4).latin1(), _offset, _type);
}

//----------------------------------------------------------------------------
Coff::CodeLine::CodeLine(const Object &object, const Section &section,
                         const QByteArray &data, uint start, const QString &lastFilename, Log::Base &log)
  : Element(object), _section(section), _symbol(0)
{
  uint offset = start;
  Q_UINT32 v;
  if ( !getULong(data, offset, 4, log, v) ) return;
  uint tmp = v;
  if ( object.format()==Format::PIC30 ) {
    if ( !getULong(data, offset, 4, log, v) ) return;
    _line = v;
    if ( _line!=0 ) {
      _address = tmp;
      _filename = lastFilename;
      //qDebug("code line %i: %s", _line, toHexLabel(_address, nbChars(_address)).latin1());
    } else {
      if ( tmp>=object.nbSymbols() ) {
        log.log(Log::LineType::Error, i18n("Codeline has unknown symbol: %1").arg(tmp));
        return;
      }
      if ( object.symbol(tmp)->isAuxSymbol() ) {
        log.log(Log::LineType::Error, i18n("Codeline is an auxiliary symbol: %1").arg(tmp));
        return;
      }
      _symbol = static_cast<const Symbol *>(object.symbol(tmp));
      _filename = _symbol->filename();
      //qDebug("code line %i: %s", _line, _symbol->_name.latin1());
    }
  } else {
    if ( tmp>=object.nbSymbols() ) {
      log.log(Log::LineType::Error, i18n("Codeline has unknown symbol: %1").arg(tmp));
      return;
    }
    if ( object.symbol(tmp)->isAuxSymbol() ) {
      log.log(Log::LineType::Error, i18n("Codeline is an auxiliary symbol: %1").arg(tmp));
      return;
    }
    _symbol = static_cast<const Symbol *>(object.symbol(tmp));
    _filename = _symbol->filename();
    if ( !getULong(data, offset, 2, log, v) ) return;
    _line = v;
    if ( object.optHeaderFormat()==OptHeaderFormat::Picc && _line>=2 ) _line -= 2; // #### ??
    if ( !getULong(data, offset, 4, log, v) ) return;
    _address = v;
    if ( !getULong(data, offset, 2, log, v) ) return;
    // flags
    if ( !getULong(data, offset, 4, log, v) ) return;
    // function index
    //qDebug("code line %i: %s", _line, toHexLabel(_address, nbChars(_address)).latin1());
  }
//  if ( _symbol && _symbol->_class!=Symbol::CFile )
//    log.log(Log::LineType::Warning, i18n("Line without file symbol associated (%1:%2 %3).")
//                         .arg(_section._name).arg(toHexLabel(_address, nbChars(_address))).arg(_symbol->_class));
}

//----------------------------------------------------------------------------
const Coff::SectionType::Data Coff::SectionType::DATA[Nb_Types] = {
  { 0, I18N_NOOP("Config") },
  { 0, I18N_NOOP("Device Id") },
  { 0, I18N_NOOP("User Ids") },
  { 0, I18N_NOOP("Uninitialized Data") },
  { 0, I18N_NOOP("Initialized Data") },
  { 0, I18N_NOOP("Rom Data") },
  { 0, I18N_NOOP("Code") }
};

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
  //if ( _address!=v ) log.log(Log::LineType::Warning, i18n("Virtual address (%1) does not match physical address (%2) in %3.")
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
  _flags = v;

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
      nbBytesWord = pdata.nbBytesWord(Pic::MemoryRangeType::Code);
      nbWords /= nbBytesWord;
      inc = pdata.addressIncrement(Pic::MemoryRangeType::Code);
    }
    for (uint i=0; i<nbWords; i++) {
      Address address = _address + inc*i;
      if ( !getULong(data, dataOffset, nbBytesWord, log, v) ) return;
      _instructions[address].value = v;
    }
    for (uint i=0; i<nbWords; i++) {
      Address address = _address + inc*i;
      BitValue op = _instructions[address].value;
      if ( _flags & FText ) {
        char buffer[512];
        buffer[0] = 0;
        BitValue op2 = ((i+1)<nbWords ? _instructions[address+inc].value : 0);
        uint nbop = disassemble(op.toUInt(), op2.toUInt(), address.toUInt()/inc, pdata.architecture(), buffer, 512);
        _instructions[address].disasm = QString(buffer);
        _instructions[address].opcode = toHex(op, pdata.nbCharsWord(Pic::MemoryRangeType::Code));
        if ( nbop==2 ) {
          _instructions[address+inc].opcode = toHex(op2, pdata.nbCharsWord(Pic::MemoryRangeType::Code));
          i++;
        }
        //qDebug("  %s: %s (%s %s)", toHex(address, 4).data(), _data[address].disasm.data(), _data[address].opcode.data(), (nbop==2 ? _data[address+inc].opcode.data() : ""));
      } else if ( _flags & FDataRom ) _instructions[address].opcode = toHex(op, 4);
      else if ( _flags & FData ) _instructions[address].opcode = toHex(op.maskWith(0xFF), 2);
    }
  }

  // read relocations
  if ( relocationOffset!=0 ) {
    _relocations.resize(nbRelocations);
    for (uint i=0; i<nbRelocations; i++) {
      _relocations[i] = new Relocation(object, *this, data, relocationOffset, log);
      relocationOffset += object.size(RelocationSize);
      if ( log.hasError() ) return;
    }
  }

  // read line numbers
  if ( lineNumberOffset!=0 ) {
    QString lastFilename;
    _lines.resize(nbLineNumbers);
    for (uint i=0; i<nbLineNumbers; i++) {
      _lines[i] = new CodeLine(object, *this, data, lineNumberOffset, lastFilename, log);
      lastFilename = _lines[i]->filename();
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

Coff::SectionType Coff::Section::type() const
{
  if ( _name==".config" ) return SectionType::Config;
  if ( _name==".devid" ) return SectionType::DeviceId;
  if ( _name==".idlocs" ) return SectionType::UserIds;
  if ( _flags & FText ) return SectionType::Code;
  if ( _flags & FData ) return SectionType::InitializedData;
  if ( _flags & FBSS ) return SectionType::UninitializedData;
  if ( _flags & FDataRom ) return SectionType::DataRom;
  return SectionType::Nb_Types;
}

//----------------------------------------------------------------------------
Coff::Object::Object(const Device::Data *device, const PURL::Url &url)
  : Base(url), _device(device)
{}

bool Coff::Object::parse(Log::Base &log)
{
  // header
  QByteArray data;
  uint offset = 0;
  if ( !initParse(CoffType::Object, data, offset, log) ) return false;
  if ( _format==Format::Nb_Types ) {
    log.log(Log::LineType::Error, i18n("COFF format not supported: magic number is %1.").arg(toHexLabel(_magic, 4)));
    return false;
  }
  log.log(Log::DebugLevel::Extra, QString("COFF format: %1").arg(toHexLabel(_magic, 4)));
  if ( !parseHeader(data, offset, log) ) return false;

  // optionnal header
  Q_ASSERT( offset==size(HeaderSize) );
  if ( !getULong(data, offset, 2, log, _optHeaderMagic) ) return false;
  log.log(Log::DebugLevel::Extra, QString("COFF optionnal header format: %1").arg(toHexLabel(_optHeaderMagic, 4)));
  _optHeaderFormat = OptHeaderFormat::Nb_Types;
  uint i = 0;
  for (; OPT_HEADER_DATA[i].optHeaderFormat!=OptHeaderFormat::Nb_Types; i++) if ( _optHeaderMagic==OPT_HEADER_DATA[i].magic ) break;
  _optHeaderFormat = OPT_HEADER_DATA[i].optHeaderFormat;
  if ( _optHeaderFormat==OptHeaderFormat::Nb_Types ) {
    log.log(Log::LineType::Warning, i18n("Optional header format not supported: magic number is %1").arg(toHexLabel(_optHeaderMagic, 4)));
    offset += size(OptHeaderSize)-2;
  } else if ( !OPT_HEADER_DATA[i].parsed ) {
    log.log(Log::DebugLevel::Normal, QString("Optional header not parsed: magic number is %1").arg(toHexLabel(_optHeaderMagic, 4)));
    offset += size(OptHeaderSize)-2;
  } else if ( !parseOptionnalHeader(data, offset, log) ) return false;

  // parse symbol table
  uint stringTableOffset = _symbolOffset + _nbSymbols*size(SymbolSize);
  QString lastFilename;
  _symbols.resize(_nbSymbols);
  for (uint i=0; i<_nbSymbols; i++) {
    Symbol *s = new Symbol(*this, data, _symbolOffset, stringTableOffset, lastFilename, log);
    if ( log.hasError() ) return false;
    if ( !s->filename().isEmpty() ) lastFilename = s->filename();
    _symbols[i] = s;
    _msymbols[s->name()] = s;
    _symbolOffset += size(SymbolSize);
    for (uint k=0; k<uint(s->auxSymbols().count()); k++) {
      i++;
      _symbols[i] = s->auxSymbols()[k];
      _symbolOffset += size(SymbolSize);
    }
  }

  // parse sections
  Q_ASSERT( offset==(size(HeaderSize) + size(OptHeaderSize)) );
  _sections.resize(_nbSections);
  for (uint i=0; i<_nbSections; i++) {
    _sections[i] = new Section(*_device, *this, data, offset, stringTableOffset, log);
    offset += size(SectionHeaderSize);
    if ( log.hasError() ) return false;
  }

  // extract filenames
  for (uint i=0; i<_nbSymbols; i++) {
    if ( _symbols[i]==0 || _symbols[i]->isAuxSymbol() ) continue;
    QString s = static_cast<const Symbol *>(_symbols[i])->filename();
    if ( s.isEmpty() || s=="fake" || _filenames.contains(s) ) continue;
    _filenames.append(s);
  }

  // extract variables
  for (uint i=0; i<nbSymbols(); i++) {
    if ( _symbols[i]==0 || _symbols[i]->isAuxSymbol() ) continue;
    const Symbol *sym = static_cast<const Symbol *>(_symbols[i]);
    if ( sym->symbolClass()!=SymbolClass::Static ) continue;
    if ( sym->sectionType()!=SymbolSectionType::InsideSection ) continue;
    QString name = sym->name();
    if ( name.startsWith("_$_") || name.startsWith("__") || name.startsWith(".") ) continue; // special variables (?)
    _variables[name] = sym->value() & 0xFFF; // #### ??
  }

  return true;
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
    log.log(Log::LineType::Error, i18n("Optionnal header size is not %1: %2").arg(size(OptHeaderSize)).arg(v));
    return false;
  }
  if ( !getULong(data, offset, 2, log, v) ) return false;
  _flags = Flags(v);
  return true;
}

bool Coff::Object::parseOptionnalHeader(const QByteArray &data, uint &offset, Log::Base &log)
{
  Q_UINT32 v;
  int nb = (_format==Format::NewMicrochip ? 4 : 2);
  if ( !getULong(data, offset, nb, log, v) ) return false; // version stamp
  if ( _format==Format::PIC30 ) {
    if ( !getULong(data, offset, 4, log, v) ) return false; // text               size in bytes, padded to firmware boundary
    if ( !getULong(data, offset, 4, log, v) ) return false; // initialized data   "
    if ( !getULong(data, offset, 4, log, v) ) return false; // uninitialized data "
    if ( !getULong(data, offset, 4, log, v) ) return false; // entry point
    if ( !getULong(data, offset, 4, log, v) ) return false; // offset of text
    if ( !getULong(data, offset, 4, log, v) ) return false; // offset of data
    if ( _device==0 ) _device = Device::lister().data("30F2010"); // for e.g.
  } else {
    if ( !getULong(data, offset, 4, log, v) ) return false;
    // #### at least for C18 compiler, it can be compiled for generic processor: in such case
    // the pic type will be 18C452 in non-extended mode and 18F4620 for extended mode...
    QString name = Coff::findId(v);
    log.log(Log::DebugLevel::Normal, QString("Device name: \"%1\"").arg(name));
    if ( name.isEmpty() ) {
      log.log(Log::DebugLevel::Normal, QString("Unknown processor type: %1").arg(toHexLabel(v, 4)));
      log.log(Log::LineType::Error, i18n("Could not determine processor (%1).").arg(toHexLabel(v, 4)));
      return false;
    } else if ( _device==0 ) _device = Device::lister().data(name);
    else if ( name!=_device->name() ) log.log(Log::DebugLevel::Normal, QString("Different processor name: %1").arg(name));
    if ( !getULong(data, offset, 4, log, v) ) return false;
    const Pic::Data *pdata = static_cast<const Pic::Data *>(_device);
    if (pdata) {
      uint nbBits = pdata->nbBitsWord(Pic::MemoryRangeType::Code) / pdata->addressIncrement(Pic::MemoryRangeType::Code);
      if ( v!=nbBits ) log.log(Log::DebugLevel::Normal, QString("Rom width is not %1: %2").arg(nbBits).arg(v));
    }
    if ( !getULong(data, offset, 4, log, v) ) return false;
    if (pdata) {
      uint nbBits = pdata->registersData().nbBits();
      if ( v!=nbBits ) log.log(Log::DebugLevel::Normal, QString("Ram width is not %1: %2").arg(nbBits).arg(v));
    }
  }
  return true;
}

Coff::Object::~Object()
{
  for (uint i=0; i<nbSymbols(); i++) delete _symbols[i];
  for (uint i=0; i<nbSections(); i++) delete _sections[i];
}

QString Coff::Object::variableName(Address address) const
{
  QMap<QString, Address>::const_iterator it;
  for (it=_variables.begin(); it!=_variables.end(); ++it)
    if ( it.data()==address ) return it.key();
  return QString::null;
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
  if ( data.architecture()==Pic::Architecture::P16X )
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
  QMap<QString, Address> variables = coff.variables();
  QMap<QString, Address>::const_iterator vit;
  for (vit=variables.begin(); vit!=variables.end(); ++vit) {
    Register::TypeData rtd(vit.data(), rdata.nbChars());
    list.append(Pic::RegisterNameData(vit.key() + " (" + toHexLabel(vit.data(), rdata.nbCharsAddress()) + ")", rtd));
  }
  qHeapSort(list);
  return list;
}
