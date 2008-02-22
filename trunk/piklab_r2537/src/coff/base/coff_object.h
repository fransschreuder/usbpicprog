/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef COFF_OBJECT_H
#define COFF_OBJECT_H

#include "coff.h"
#include "devices/base/register.h"

namespace Coff
{
//----------------------------------------------------------------------------
extern bool getName(const QByteArray &data, uint &offset, uint nbChars, uint stringTableOffset, Log::Base &log, QString &name);
extern int disassemble(long int opcode, long int opcode2, int org, Pic::Architecture architecture, char *buffer, size_t sizeof_buffer);

BEGIN_DECLARE_ENUM(OptHeaderFormat)
  OldMicrochip = 0, NewMicrochip, Picc, Ccsc
END_DECLARE_ENUM_STD(OptHeaderFormat)

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

//----------------------------------------------------------------------------
class BaseSymbol : public Element
{
public:
  BaseSymbol(const Object &object) : Element(object) {}
  virtual bool isAuxSymbol() const = 0;
};

BEGIN_DECLARE_ENUM(AuxSymbolType)
  Direct = 0, File, Identifier, Section
END_DECLARE_ENUM_STD(AuxSymbolType)

class AuxSymbol : public BaseSymbol
{
public:
  virtual bool isAuxSymbol() const { return true; }
  static AuxSymbol *factory(const Object &object, AuxSymbolType type, const QByteArray &data,
                            uint offset, uint stringTableOffset, Log::Base &log);

public:
  AuxSymbol(const Object &object) : BaseSymbol(object) {}
  virtual AuxSymbolType type() const = 0;
};

class AuxSymbolDirect : public AuxSymbol
{
public:
  AuxSymbolDirect(const Object &object, const QByteArray &data, uint offset, uint stringTableOffset, Log::Base &log);
  virtual AuxSymbolType type() const { return AuxSymbolType::Direct; }

private:
  uchar   _command;
  QString _string;
};

class AuxSymbolFile : public AuxSymbol
{
public:
  AuxSymbolFile(const Object &object, const QByteArray &data, uint offset, uint stringTableOffset, Log::Base &log);
  virtual AuxSymbolType type() const { return AuxSymbolType::File; }
  QString filename() const { return _filename; }
  uint line() const { return _line; }

private:
  uint    _line;
  QString _filename;
};

class AuxSymbolIdentifier : public AuxSymbol
{
public:
  AuxSymbolIdentifier(const Object &object, const QByteArray &data, uint offset, uint stringTableOffset, Log::Base &log);
  virtual AuxSymbolType type() const { return AuxSymbolType::Identifier; }
  QString string() const { return _string; }

private:
  QString _string;
};

class AuxSymbolSection : public AuxSymbol
{
public:
  AuxSymbolSection(const Object &object, const QByteArray &data, uint offset, uint stringTableOffset, Log::Base &log);
  virtual AuxSymbolType type() const { return AuxSymbolType::Section; }

private:
  uint _length, _nbRelocations, _nbLineNumbers;
};

class AuxSymbolUnknown : public AuxSymbol
{
public:
  AuxSymbolUnknown(const Object &object) : AuxSymbol(object) {}
  virtual AuxSymbolType type() const { return AuxSymbolType::Nb_Types; }
};

//----------------------------------------------------------------------------
BEGIN_DECLARE_ENUM(SymbolSectionType)
  InsideSection = 0, UndefinedSection, AbsoluteValue, DebugSymbol
END_DECLARE_ENUM_STD(SymbolSectionType)

struct SymbolClassData {
  const char *key, *label;
  uint id;
};
BEGIN_DECLARE_ENUM(SymbolClass)
  Automatic = 0, External, Static, Register, ExternalDefinition,
  Label, UndefinedLabel, MemberOfStructure, FunctionArgument, StructureTag,
  MemberOfUnion, UnionTag, TypeDefinition, UndefinedStatic, EnumerationTag,
  MemberOfEnumeration, RegisterParameter, BitField, AutoArgument, EndOfBlock,
  BeginEndOfBlock, BeginEndOfFunction, EndOfStructure, Filename, LineNumber,
  DuplicateTag, Section
END_DECLARE_ENUM(SymbolClass, SymbolClassData)

struct SymbolTypeData {
  const char *key, *label;
  uint id;
};
BEGIN_DECLARE_ENUM(SymbolType)
  Void = 0, Char, Short, Int, Long, Float, Double, Struct, Union,
  Enum, MemberOfEnum, UChar, UShort, UInt, ULong, LongDouble
END_DECLARE_ENUM(SymbolType, SymbolTypeData)

struct SymbolDerivedTypeData {
  const char *key, *label;
  uint id;
};
BEGIN_DECLARE_ENUM(SymbolDerivedType)
  Pointer = 0, Function, Array
END_DECLARE_ENUM(SymbolDerivedType, SymbolDerivedTypeData)

class Symbol : public BaseSymbol
{
public:
  Symbol(const Object &object, const QByteArray &data, uint offset, uint stringTableOffset,
         const QString &lastFilename, Log::Base &log);
  virtual bool isAuxSymbol() const { return false; }
  QString name() const { return _name; }
  QString filename() const { return _filename; }
  const QValueVector<AuxSymbol *> &auxSymbols() const { return _aux; }
  SymbolClass symbolClass() const { return _sclass; }
  SymbolSectionType sectionType() const;
  SymbolType type() const { return _type; }
  SymbolDerivedType derivedType() const { return _dtype; }
  uint value() const { return _value; }
  uint section() const { Q_ASSERT( sectionType()==SymbolSectionType::InsideSection ); return _section; }

private:
  QString _name, _filename;
  uint _value, _section;
  SymbolClass _sclass;
  SymbolType  _type;
  SymbolDerivedType _dtype;
  QValueVector<AuxSymbol *> _aux;
};

//----------------------------------------------------------------------------
class Relocation : public Element
{
public:
  Relocation(const Object &object, const Section &section, const QByteArray &data,
             uint offset, Log::Base &log);

private:
  ulong  _address, _type;
  short  _offset;
  const Symbol *_symbol;
};

//----------------------------------------------------------------------------
class CodeLine : public Element
{
public:
  CodeLine(const Object &object, const Section &section, const QByteArray &data,
           uint offset, const QString &lastFilename, Log::Base &log);
  const Section &section() const { return _section; }
  QString filename() const { return _filename; }
  uint line() const { return _line; }
  Address address() const { return _address; }
  const Symbol *symbol() const { return _symbol; }

private:
  const Section &_section;
  uint           _line;
  Address        _address;
  QString        _filename;
  const Symbol  *_symbol;
};

//----------------------------------------------------------------------------
BEGIN_DECLARE_ENUM(SectionType)
  Config = 0, DeviceId, UserIds, UninitializedData, InitializedData, DataRom, Code
END_DECLARE_ENUM_STD(SectionType)

class Section : public Element
{
public:
  class InstructionData {
  public:
    BitValue value;
    QString opcode, disasm;
  };

public:
  Section(const Device::Data &device, const Object &object, const QByteArray &data, uint offset,
          uint stringTableOffset, Log::Base &log);
  ~Section();
  SectionType type() const;
  QString name() const { return _name; }
  Address address() const { return _address; }
  uint size() const { return _size; }
  uint flags() const { return _flags; }
  const QMap<Address, InstructionData> &instructions() const { return _instructions; }
  const QValueVector<Relocation *> &relocations() const { return _relocations; }
  const QValueVector<CodeLine *> &lines() const { return _lines; }

private:
  QString _name;
  Address _address;
  uint _size, _flags;
  QMap<Address, InstructionData> _instructions;
  QValueVector<Relocation *> _relocations;
  QValueVector<CodeLine *> _lines;

  enum Flag { FText = 0x00020, FData = 0x00040, FBSS = 0x00080, FDataRom = 0x00100,
              FAbs = 0x01000, FShared = 0x02000, FOverlay = 0x04000, FAccess = 0x08000,
              FActivationRecord = 0x10000 };
};

//----------------------------------------------------------------------------
class Object : public Base
{
public:
  Object(const Device::Data *device, const PURL::Url &url);
  virtual ~Object();
  virtual bool parse(Log::Base &log);
  Format format() const { return _format; }
  const Device::Data *device() const { return _device; }
  uint size(SizeType stype) const { return _format.data().sizes[stype]; }
  OptHeaderFormat optHeaderFormat() const { return _optHeaderFormat; }
  uint optHeaderMagic() const { return _optHeaderMagic; }
  uint nbSymbols() const { return _symbols.count(); }
  const BaseSymbol *symbol(uint i) const { return _symbols[i]; }
  const Symbol *symbol(const QString &name) const { return (_msymbols.contains(name) ? _msymbols[name] : 0); }
  uint nbSections() const { return _sections.count(); }
  const Section *section(uint i) const { return _sections[i]; }
  const QStringList &filenames() const { return _filenames; }
  const QMap<QString, Address> &variables() const { return _variables; }
  QString variableName(Address address) const;

  enum Flag { RelocationStripped = 0x0001, Executable = 0x0002, LineNumberStripped = 0x0004,
              SymbolStripped = 0x0080, Extended18 = 0x4000, Generic = 0x8000 };
  Q_DECLARE_FLAGS(Flags, Flag)

protected:
  Q_UINT32                _optHeaderMagic;
  OptHeaderFormat         _optHeaderFormat;
  const Device::Data     *_device;
  uint                    _nbSections, _nbSymbols, _symbolOffset;
  Flags                   _flags;
  QValueVector<BaseSymbol *> _symbols;
  QMap<QString, Symbol *>  _msymbols; // name -> Symbol *
  QValueVector<Section *> _sections;
  QStringList             _filenames;
  QMap<QString, Address>  _variables; // name -> address

  virtual bool parseHeader(const QByteArray &data, uint &offset, Log::Base &log);
  virtual bool parseOptionnalHeader(const QByteArray &data, uint &offset, Log::Base &log);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Object::Flags)

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
