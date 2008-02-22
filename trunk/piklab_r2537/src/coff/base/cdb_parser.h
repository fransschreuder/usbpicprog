/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SDCC_CDB_PARSER_H
#define SDCC_CDB_PARSER_H

#include "common/common/key_enum.h"
#include "common/global/log.h"
#include "common/global/purl.h"

namespace CDB
{

//----------------------------------------------------------------------------
class Record
{
public:
};

class ModuleRecord : public Record
{
public:
  QString filename;
};

BEGIN_DECLARE_ENUM(ScopeType)
  Global = 0, File, Local, Structure
END_DECLARE_ENUM_STD(ScopeType)

class Scope
{
public:
  ScopeType type;
  QString name; // file or function name
  uint level, block;
};

BEGIN_DECLARE_ENUM(VarType)
  Array = 0, Function, GenericPointer, CodePointer, ExternalRamPointer,
  InternalRamPointer, PagedPointer, Upper128bytesPointer, Long, Int, Char,
  Short, Void, Float, Structure, Sbit, BitField
END_DECLARE_ENUM_STD(VarType)

class DCLType
{
public:
  VarType type;
  uint nb; // for Array and BitField
  QString name; // for Structure
};

BEGIN_DECLARE_ENUM(Sign)
  Signed = 0, Unsigned
END_DECLARE_ENUM_STD(Sign)

class TypeChain
{
public:
  QValueVector<DCLType> types;
  Sign sign;
};

BEGIN_DECLARE_ENUM(AddressSpaceType)
  ExternalStack = 0, InternalStack, Code, CodeStaticSegment,
  Lower128bytesInternalRam, ExternalRam, InternalRam, BitAddressable,
  SFR, SBIT, Register, FunctionOrUndefined
END_DECLARE_ENUM_STD(AddressSpaceType)

class AddressSpace {
public:
  AddressSpaceType type;
  bool onStack;
  uint stackOffset; // valid if onStack is true
  QStringList registers; // for Register type
};

class SymbolRecord : public Record
{
public:
  QString name;
  Scope scope;
  TypeChain typeChain;
  AddressSpace addressSpace;
};

class FunctionRecord : public SymbolRecord
{
public:
  bool isInterruptHandler;
  uint interruptHandler, registerBank; // if isInterruptHandler is true
};

class TypeMember
{
public:
  uint offset;
  SymbolRecord symbol;
};

class TypeRecord : public Record
{
public:
  QString filename, name;
  QValueVector<TypeMember> members;
};

class LinkerRecord : public Record
{
public:
  enum Type { Address = 0, EndAddress, AsmLine, CLine, Nb_Types };
  Type type;
  Scope scope; // for Address and EndAddress
  QString name; // for Address and EndAddress
  uint address;
  uint line; // for AsmLine and CLine
  QString filename; // for AsmLine and CLine
  uint block, level; // for CLine
};

//----------------------------------------------------------------------------
class Object
{
public:
  Object(const PURL::Url &url, Log::Base &log);
  virtual ~Object();

private:
  Log::Base &_log;
  QString _current;
  uint _line, _col;
  QValueVector<Record *> _records;

  void log(Log::LineType type, const QString &message);
  void logMalformed(const QString &detail);
  bool readBool(bool &b);
  bool getUInt(const QString &s, uint &r);
  bool readUInt(uint &v);
  bool readChar(char &c);
  bool readAndCheckChar(char c);
  bool getString(const QString &s, QString &r);
  bool readStoppedString(QString &s, char stop);
  bool readFixedLengthString(QString &s, uint size);
  bool readHex(uint &v);

  bool parse(Scope &scope, QString &name);
  bool parse(TypeChain &typeChain);
  bool parse(TypeRecord &typeRecord);
  bool parse(SymbolRecord &sr);
  bool parse(AddressSpace &addressSpace);
  bool parse(TypeMember &typeMember);

  bool parseModuleRecord(Record * &record);
  bool parseFunctionRecord(Record * &record);
  bool parseSymbolRecord(Record * &record);
  bool parseTypeRecord(Record * &record);
  bool parseLinkerRecord(Record * &record);
};

} // namespace

#endif
