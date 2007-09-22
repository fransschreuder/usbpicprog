/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef COFF_H
#define COFF_H

#include "common/global/global.h"
#include "devices/pic/base/pic.h"
#include "common/global/purl.h"
#include "common/global/log.h"
#include "devices/base/register.h"

//----------------------------------------------------------------------------
namespace Coff
{
extern bool getULong(const QByteArray &data, uint &offset, uint nbBytes, Log::Base &log, Q_UINT32 &v);
extern bool getName(const QByteArray &data, uint &offset, uint nbChars, uint stringTableOffset, Log::Base &log, QString &name);
extern int disassemble(long int opcode, long int opcode2, int org,
                       Pic::Architecture architecture, char *buffer, size_t sizeof_buffer);

enum Format { OldMicrochipFormat = 0, PIC30Format, NewMicrochipFormat, Nb_Formats };
enum SizeType { HeaderSize = 0, OptHeaderSize, SectionHeaderSize, SymbolSize,
                LineNumberSize, RelocationSize, Nb_SizeTypes };
struct FormatData {
  uint magic;
  uint sizes[Nb_SizeTypes];
};
extern const FormatData FORMAT_DATA[Nb_Formats];

enum OptHeaderFormat { OptHeaderOldMicrochipFormat, OptHeaderNewMicrochipFormat,
                       OptHeaderPiccFormat, OptHeaderCcscFormat, OptHeaderInvalidFormat };
struct OptHeaderData {
  uint magic;
  OptHeaderFormat optHeaderFormat;
  bool parsed;
};
extern const OptHeaderData OPT_HEADER_DATA[];

class Object;
class Section;

//----------------------------------------------------------------------------
class Element
{
public:
  Element(const Object &object) : _object(object) {}
  virtual ~Element() {}

protected:
  const Object &_object;
};

class Base
{
public:
  Base(const PURL::Url &url) : _url(url) {}
  virtual ~Base() {}
  PURL::Url url() const { return _url; }

protected:
  PURL::Url _url;

  QByteArray readFile(const PURL::Url &url, Log::Base &log);
};

//----------------------------------------------------------------------------
class AuxSymbol : public Element
{
public:
  enum Type { Direct, File, Ident, Section, None };
  static AuxSymbol *factory(const Object &object, Type type, const QByteArray &data, uint offset, uint stringTableOffset, Log::Base &log);

public:
  AuxSymbol(const Object &object) : Element(object) {}
  virtual Type type() const = 0;
};

class AuxSymbolDirect : public AuxSymbol
{
public:
  AuxSymbolDirect(const Object &object, const QByteArray &data, uint offset, uint stringTableOffset, Log::Base &log);
  virtual Type type() const { return Direct; }
  uchar   _command;
  QString _string;
};

class AuxSymbolFile : public AuxSymbol
{
public:
  AuxSymbolFile(const Object &object, const QByteArray &data, uint offset, uint stringTableOffset, Log::Base &log);
  virtual Type type() const { return File; }
  uint    _line;
  QString _filename;
};

class AuxSymbolIdent : public AuxSymbol
{
public:
  AuxSymbolIdent(const Object &object, const QByteArray &data, uint offset, uint stringTableOffset, Log::Base &log);
  virtual Type type() const { return Ident; }
  QString _string;
};

class AuxSymbolSection : public AuxSymbol
{
public:
  AuxSymbolSection(const Object &object, const QByteArray &data, uint offset, uint stringTableOffset, Log::Base &log);
  virtual Type type() const { return Section; }
  uint _length, _nbRelocations, _nbLineNumbers;
};

//----------------------------------------------------------------------------
class Symbol : public Element
{
public:
  Symbol(const Object &object, const QByteArray &data, uint offset, uint stringTableOffset, Log::Base &log);
  ~Symbol();
  enum { NUndefined = 0x0000, NAbsolute = 0xFFFF, NDebug = 0xFFFE };
  enum Class { CExternal = 2, CStatic = 3, CLabel = 6, CFile = 103, CSection = 109 };
  enum Type { TNull = 0x0000, TVoid = 0x0001, TChar = 0x0010, TShort = 0x0011,
              TInt = 0x0100, TLong = 0x0101, TFloat = 0x0110, TDouble = 0x0111,
              TStruct = 0x1000, TUnion = 0x1001, TEnum = 0x1010, TMoe = 0x1011,
              TUChar = 0x1100, TUShort = 0x1101, TUInt = 0x1110, TULong = 0x1111,
              TLongDouble = 0x10000 };
  enum DerivedType { DTNonDerived = 0x000000, DTPointer = 0x010000, DTFunction = 0x100000,
                     DTArray = 0x110000 };
  QString _name, _filename;
  uint _value, _section;
  Class _class;
  Type  _type;
  DerivedType _dtype;
  QValueVector<AuxSymbol *> _aux;
};

//----------------------------------------------------------------------------
class Relocation : public Element
{
public:
  Relocation(const Object &object, const Section &section, const QByteArray &data,
             uint offset, Log::Base &log);
  ulong  _address, _type;
  short  _offset;
  const Symbol *_symbol;
};

//----------------------------------------------------------------------------
class CodeLine : public Element
{
public:
  CodeLine(const Object &object, const Section &section, const QByteArray &data,
           uint offset, Log::Base &log);
  QString filename() const { return (_symbol ? _symbol->_filename : QString::null); }
  const Section &_section;
  uint           _line, _address;
  const Symbol  *_symbol;
};

//----------------------------------------------------------------------------
class Section : public Element
{
public:
  Section(const Device::Data &device, const Object &object, const QByteArray &data, uint offset,
          uint stringTableOffset, Log::Base &log);
  ~Section();
  enum Type { Config, DevId, IdLocs, UData, IData, DataRom, Code, Unknown };
  enum Flag { FText = 0x00020, FData = 0x00040, FBSS = 0x00080, FDataRom = 0x00100,
              FAbs = 0x01000, FShared = 0x02000, FOverlay = 0x04000, FAccess = 0x08000,
              FActivationRecord = 0x10000 };
  Q_DECLARE_FLAGS(Flags, Flag)
  Type type() const;
  QString _name;
  ulong _address, _size;
  Flags _flags;
  class Data {
  public:
    uint value;
    QString opcode, disasm;
  };
  QMap<uint, Data>           _data; // address -> Data
  QValueVector<Relocation *> _relocations;
  QValueVector<CodeLine *>   _lines;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Section::Flags)

//----------------------------------------------------------------------------
class Object : public Base
{
public:
  Object(const Device::Data &device, const PURL::Url &url, Log::Base &log, bool *identified = 0);
  virtual ~Object();
  Format format() const { return _format; }
  uint size(SizeType stype) const { return FORMAT_DATA[_format].sizes[stype]; }
  OptHeaderFormat optHeaderFormat() const { return _optHeaderFormat; }
  const Symbol *symbol(uint i) const;
  uint nbSections() const { return _sections.count(); }
  const Section *section(uint i) const { return _sections[i]; }
  const QStringList &filenames() const { return _filenames; }
  const QMap<QString, uint> &variables() const { return _variables; }
  QString variableName(uint address) const;

  enum Flag { RelocationStripped = 0x0001, Executable = 0x0002, LineNumberStripped = 0x0004,
              SymbolStripped = 0x0080, Extended18 = 0x4000, Generic = 0x8000 };
  Q_DECLARE_FLAGS(Flags, Flag)

protected:
  Format                  _format;
  OptHeaderFormat         _optHeaderFormat;
  const Device::Data     &_device;
  uint                    _nbSections, _nbSymbols, _symbolOffset;
  Flags                   _flags;
  QMap<uint, Symbol *>    _symbols; // index -> Symbol
  QValueVector<Section *> _sections;
  QStringList             _filenames;
  QMap<QString, uint>     _variables; // name -> address

  virtual bool parseHeader(const QByteArray &data, uint &offset, Log::Base &log);
  virtual bool parseOptionnalHeader(const QByteArray &data, uint &offset, Log::Base &log);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Object::Flags)

//----------------------------------------------------------------------------
class Archive : public Base
{
public:
  Archive(const PURL::Url &url, Log::Base &log, bool *identified = 0);
};

} // namespace

//----------------------------------------------------------------------------
namespace Pic
{

class RegisterNameData
{
public:
  RegisterNameData() {}
  RegisterNameData(const QString &label, const Register::TypeData &data) : _label(label), _data(data) {}
  QString label() const { return _label; }
  const Register::TypeData &data() const { return _data; }
  bool operator <(const RegisterNameData &rnd) const { return _label<rnd._label; };

private:
  QString _label;
  Register::TypeData _data;
};
extern QValueVector<RegisterNameData> sfrList(const Pic::Data &data);
extern QValueVector<RegisterNameData> gprList(const Pic::Data &data, const Coff::Object *coff);
extern QValueVector<RegisterNameData> variableList(const Pic::Data &data, const Coff::Object &coff);

} // namespace

#endif
