/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "cdb_parser.h"

#include <ctype.h>
#include "common/global/pfile.h"

//----------------------------------------------------------------------------
const CDB::ScopeType::Data CDB::ScopeType::DATA[Nb_Types] = {
  { "G", I18N_NOOP("Global")    },
  { "F", I18N_NOOP("File")      },
  { "L", I18N_NOOP("Local")     },
  { "S", I18N_NOOP("Structure") } // ??
};

const CDB::VarType::Data CDB::VarType::DATA[Nb_Types] = {
  { "DA", I18N_NOOP("Array") },
  { "DF", I18N_NOOP("Function") },
  { "DG", I18N_NOOP("Generic Pointer") },
  { "DC", I18N_NOOP("Code Pointer") },
  { "DX", I18N_NOOP("External RAM Pointer") },
  { "DD", I18N_NOOP("Internal RAM Pointer") },
  { "DP", I18N_NOOP("Paged Pointer") },
  { "DI", I18N_NOOP("Upper-128-byte Pointer") },
  { "SL", I18N_NOOP("Long") },
  { "SI", I18N_NOOP("Int") },
  { "SC", I18N_NOOP("Char") },
  { "SS", I18N_NOOP("Short") },
  { "SV", I18N_NOOP("Void") },
  { "SF", I18N_NOOP("Float") },
  { "ST", I18N_NOOP("Structure") },
  { "SX", I18N_NOOP("SBIT") },
  { "SB", I18N_NOOP("Bit Field") }
};

const CDB::Sign::Data CDB::Sign::DATA[Nb_Types] = {
  { "S", I18N_NOOP("Signed")   },
  { "U", I18N_NOOP("Unsigned") }
};

const CDB::AddressSpaceType::Data CDB::AddressSpaceType::DATA[Nb_Types] = {
  { "A", I18N_NOOP("External Stack") },
  { "B", I18N_NOOP("Internal Stack") },
  { "C", I18N_NOOP("Code") },
  { "D", I18N_NOOP("Code / Static Segment") },
  { "E", I18N_NOOP("Lower-128-byte Internal RAM") },
  { "F", I18N_NOOP("External RAM") },
  { "G", I18N_NOOP("Internal RAM") },
  { "H", I18N_NOOP("Bit Addressable") },
  { "I", I18N_NOOP("SFR Space") },
  { "J", I18N_NOOP("SBIT Space") },
  { "R", I18N_NOOP("Register Space") },
  { "Z", I18N_NOOP("Function or Undefined Space") },
};

//----------------------------------------------------------------------------
CDB::Object::Object(const PURL::Url &url, Log::Base &log)
  : _log(log)
{
  PURL::File file(url, log);
  if ( !file.openForRead() ) return;
  QStringList lines = file.readLines();

  for (_line=0; _line<uint(lines.count()); _line++) {
    _col = 0;
    _current = lines[_line];
    if ( _current.isEmpty() ) continue;
    char c;
    if ( !readChar(c) ) return;
    if ( !readAndCheckChar(':') ) return;
    Record *record = 0;
    bool ok;
    switch (c) {
      case 'M': ok = parseModuleRecord(record); break;
      case 'F': ok = parseFunctionRecord(record); break;
      case 'S': ok = parseSymbolRecord(record); break;
      case 'T': ok = parseTypeRecord(record); break;
      case 'L': ok = parseLinkerRecord(record); break;
      default:
        log.log(Log::LineType::Error, i18n("Unrecognized record"));
        return;
    }
    if (!ok) {
      delete record;
      return;
    }
    _records.append(record);
  }
}

CDB::Object::~Object()
{
  for (uint i=0; i<uint(_records.count()); i++) delete _records[i];
}

void CDB::Object::log(Log::LineType type, const QString &message)
{
  _log.log(type, message + " " + i18n("at line #%1, column #%2").arg(_line+1).arg(_col+1));
}

void CDB::Object::logMalformed(const QString &detail)
{
  log(Log::LineType::Error, i18n("Malformed record: ") + detail);
}

bool CDB::Object::readChar(char &c)
{
  if ( _col==uint(_current.length()) ) {
    logMalformed(i18n("unexpected end of line"));
    return false;
  }
  c = _current[_col].latin1();
  _col++;
  return true;
}

bool CDB::Object::readAndCheckChar(char c)
{
  char r;
  if ( !readChar(r) ) return false;
  if ( r!=c ) {
    logMalformed(i18n("was expecting '%1'").arg(c));
    return false;
  }
  return true;
}

bool CDB::Object::getString(const QString &s, QString &r)
{
  r = s;
  if ( r.isEmpty() ) {
    logMalformed(i18n("empty name"));
    return false;
  }
  return true;
}

bool CDB::Object::readFixedLengthString(QString &s, uint size)
{
  s = QString::null;
  for (uint i=0; i<size; i++) {
    char c;
    if ( !readChar(c) ) return false;
    s += c;
  }
  return true;
}

bool CDB::Object::readStoppedString(QString &name, char stop)
{
  QString s;
  for (;;) {
    if ( _col==uint(_current.length()) ) {
      if ( stop!=0 ) {
        logMalformed(i18n("unexpected end of line"));
        return false;
      }
      break;
    }
    if ( _current[_col]==stop ) break;
    s += _current[_col];
    _col++;
  }
  return getString(s, name);
}

bool CDB::Object::getUInt(const QString &s, uint &v)
{
  bool ok;
  v = s.toUInt(&ok);
  if ( !ok ) logMalformed(i18n("was expecting an uint"));
  return ok;
}

bool CDB::Object::readUInt(uint &v)
{
  QString s;
  for (;;) {
    if ( _col==uint(_current.length()) ) break;
    if ( !isdigit(_current[_col].latin1()) ) break;
    s += _current[_col];
    _col++;
  }
  return getUInt(s, v);
}

bool CDB::Object::readBool(bool &b)
{
  char c;
  if ( !readChar(c) ) return false;
  if ( c=='0' ) b = false;
  else if ( c=='1' ) b = true;
  else {
    logMalformed(i18n("was expecting a bool ('%1')").arg(c));
    return false;
  }
  return true;
}

bool CDB::Object::readHex(uint &v)
{
  QString s;
  for (;;) {
    if ( _col==uint(_current.length()) ) break;
    if ( !isxdigit(_current[_col].latin1()) ) break;
    s += _current[_col];
    _col++;
  }
  return getUInt(s, v);
}

bool CDB::Object::parseModuleRecord(Record * &record)
{
  ModuleRecord *mr = new ModuleRecord;
  record = mr;
  return readStoppedString(mr->filename, 0);
}

bool CDB::Object::parse(Scope &scope, QString &name)
{
  QString s;
  if ( !readFixedLengthString(s, 1) ) return false;
  scope.type = ScopeType::fromKey(s);
  switch (scope.type.type()) {
    case ScopeType::Structure:
    case ScopeType::Global: break;
    case ScopeType::File:
    case ScopeType::Local:
      if ( !readStoppedString(scope.name, '$') ) return false;
      break;
    case ScopeType::Nb_Types:
      logMalformed(i18n("unknown ScopeType"));
      return false;
  }
  if ( !readAndCheckChar('$') ) return false;
  if ( !readStoppedString(name, '$') ) return false;
  if ( !readAndCheckChar('$') ) return false;
  if ( !readUInt(scope.level) ) return false;
  if ( !readAndCheckChar('$') ) return false;
  if ( !readUInt(scope.block) ) return false;
  return true;
}

bool CDB::Object::parse(TypeChain &typeChain)
{
  uint nb;
  if ( !readAndCheckChar('{') ) return false;
  if ( !readUInt(nb) ) return false;
  if ( !readAndCheckChar('}') ) return false;
  QString s;
  if ( !readStoppedString(s, ':') ) return false;
  QStringList list = QStringList::split(',', s, true);
  for (uint i=0; i<uint(list.count()); i++) {
    DCLType type;
    QString key = list[i].mid(0, 2);
    type.type = VarType::fromKey(key);
    switch (type.type.type()) {
      case VarType::Array:
      case VarType::BitField:
        if ( !getUInt(list[i].mid(2), type.nb) ) return false;
        break;
      case VarType::Structure:
        if ( !getString(list[i].mid(2), type.name) ) return false;
        break;
      case VarType::Nb_Types:
        logMalformed(i18n("unknown DCLType"));
        return false;
      default: break;
    }
    typeChain.types.append(type);
  }
  if ( !readAndCheckChar(':') ) return false;
  if ( !readFixedLengthString(s, 1) ) return false;
  typeChain.sign = Sign::fromKey(s);
  if ( typeChain.sign==Sign::Nb_Types ) {
    logMalformed(i18n("unknown Sign"));
    return false;
  }
  return true;
}

bool CDB::Object::parse(AddressSpace &as)
{
  QString s;
  if ( !readFixedLengthString(s, 1) ) return false;
  as.type = AddressSpaceType::fromKey(s);
  if ( as.type==AddressSpaceType::Nb_Types ) {
    logMalformed(i18n("unknown AddressSpaceType"));
    return false;
  }
  if ( !readAndCheckChar(',') ) return false;
  if ( !readBool(as.onStack) ) return false;
  if ( !readAndCheckChar(',') ) return false;
  if ( !readUInt(as.stackOffset) ) return false;
  if ( as.type==AddressSpaceType::Register ) {
    if ( !readAndCheckChar(',') ) return false;
    if ( !readAndCheckChar('[') ) return false;
    if ( !readStoppedString(s, ']') ) return false;
    as.registers = QStringList::split(',', s, true);
    if ( as.registers.count()==0 ) {
      logMalformed(i18n("no register defined"));
      return false;
    }
    if ( !readAndCheckChar(']') ) return false;
  }
  return true;
}

bool CDB::Object::parse(SymbolRecord &sr)
{
  if ( !parse(sr.scope, sr.name) ) return false;
  if ( !readAndCheckChar('(') ) return false;
  if ( !parse(sr.typeChain) ) return false;
  if ( !readAndCheckChar(')') ) return false;
  if ( !readAndCheckChar(',') ) return false;
  if ( !parse(sr.addressSpace) ) return false;
  return true;
}

bool CDB::Object::parse(TypeMember &tm)
{
  if ( !readAndCheckChar('(') ) return false;
  if ( !readAndCheckChar('{') ) return false;
  if ( !readUInt(tm.offset) ) return false;
  if ( !readAndCheckChar('}') ) return false;
  if ( !readAndCheckChar('S') ) return false;
  if ( !readAndCheckChar(':') ) return false;
  if ( !parse(tm.symbol) ) return false;
  if ( !readAndCheckChar(')') ) return false;
  return true;
}

bool CDB::Object::parseSymbolRecord(Record * &record)
{
  SymbolRecord *sr = new SymbolRecord;
  record = sr;
  if ( !parse(*sr) ) return false;
  return true;
}

bool CDB::Object::parseFunctionRecord(Record * &record)
{
  FunctionRecord *fr = new FunctionRecord;
  record = fr;
  if ( !parse(*fr) ) return false;
  if ( !readAndCheckChar(',') ) return false;
  if ( !readBool(fr->isInterruptHandler) ) return false;
  if ( !readAndCheckChar(',') ) return false;
  if ( !readUInt(fr->interruptHandler) ) return false;
  if ( !readAndCheckChar(',') ) return false;
  if ( !readUInt(fr->registerBank) ) return false;
  return true;
}

bool CDB::Object::parseTypeRecord(Record * &record)
{
  TypeRecord *tr = new TypeRecord;
  record = tr;
  if ( !readAndCheckChar('F') ) return false;
  if ( !readStoppedString(tr->filename, '$') ) return false;
  if ( !readAndCheckChar('$') ) return false;
  if ( !readStoppedString(tr->name, '[') ) return false;
  if ( !readAndCheckChar('[') ) return false;
  for (;;) {
    TypeMember tm;
    if ( !parse(tm) ) return false;
    tr->members.append(tm);
    if ( _current[_col]==']' ) break;
  }
  if ( !readAndCheckChar(']') ) return false;
  return true;
}

bool CDB::Object::parseLinkerRecord(Record * &record)
{
  LinkerRecord *lr = new LinkerRecord;
  record = lr;
  char c;
  if ( !readChar(c) ) return false;
  switch (c) {
    case 'A':
      lr->type = LinkerRecord::AsmLine;
      if ( !readAndCheckChar('$') ) return false;
      if ( !readStoppedString(lr->filename, '$') ) return false;
      if ( !readAndCheckChar('$') ) return false;
      if ( !readUInt(lr->line) ) return false;
      break;
    case 'C':
      lr->type = LinkerRecord::CLine;
      if ( !readAndCheckChar('$') ) return false;
      if ( !readStoppedString(lr->filename, '$') ) return false;
      if ( !readAndCheckChar('$') ) return false;
      if ( !readUInt(lr->line) ) return false;
      if ( !readAndCheckChar('$') ) return false;
      if ( !readUInt(lr->level) ) return false;
      if ( !readAndCheckChar('$') ) return false;
      if ( !readUInt(lr->block) ) return false;
      break;
    case 'X':
      lr->type = LinkerRecord::EndAddress;
      if ( !parse(lr->scope, lr->name) ) return false;
      break;
    default:
      lr->type = LinkerRecord::Address;
      if ( !parse(lr->scope, lr->name) ) return false;
      break;
  }
  if ( !readAndCheckChar(':') ) return false;
  if ( !readHex(lr->address) ) return false;
  return true;
}
