/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "coff_archive.h"

//----------------------------------------------------------------------------
Coff::Member::Member(const QByteArray &data, uint &offset, Log::Base &log)
{
  // parse header
  QString s;
  if ( !getString(data, offset, 256, log, s) ) return;
  int i = s.find('/');
  if ( i==-1 ) {
    log.log(Log::LineType::Error, i18n("Member name not terminated by '/' (\"%1\").").arg(s));
    return;
  }
  _name = s.mid(0, i);
  if ( !getString(data, offset, 12, log, s) ) return; // mtime
  if ( !getString(data, offset, 10, log, s) ) return;
  i = s.find('l');
  if ( i==-1 ) {
    log.log(Log::LineType::Error, i18n("File size not terminated by 'l' (\"%1\").").arg(s));
    return;
  }
  bool ok;
  _nbBytes = s.mid(0, i).toUInt(&ok);
  if ( !ok ) {
    log.log(Log::LineType::Error, i18n("Wrong format for file size \"%1\".").arg(s));
    return;
  }
  Q_UINT32 v;
  if ( !getULong(data, offset, 2, log, v) ) return;
  log.log(Log::DebugLevel::Extra, i18n("Magic number: \"%1\".").arg(toHexLabel(v, 4)));
//  if ( v!=0x600A ) {
//    log.log(Log::LineType::Error, i18n("Wrong magic for Microchip archive (\"%1\").").arg(toHexLabel(v, 4)));
//    return;
//  }
  offset += _nbBytes;
}

//----------------------------------------------------------------------------
Coff::Archive::Archive(const PURL::Url &url)
  : Base(url)
{}

bool Coff::Archive::parse(Log::Base &log)
{
  QByteArray data;
  uint offset = 0, symbolEnd = 0;
  Member *symbol = 0;
  if ( !initParse(CoffType::Archive, data, offset, log) ) return false;
  for (;;) {
    if ( offset==uint(data.count()) ) break; // end of archive
    uint start = offset;
    Member *member = new Member(data, offset, log);
    if ( log.hasError() ) return false;
    if ( member->name().isEmpty() ) {
      symbolEnd = offset;
      symbol = member;
    } else {
      _members[member->name()] = member;
      _offsets[start] = member;
    }
  }
  if (symbol) {
    if ( !readSymbols(data, symbolEnd - symbol->nbBytes(), log) ) return false;
    delete symbol;
  }
  return true;
}

Coff::Archive::~Archive()
{
  QMap<QString, Member *>::const_iterator it;
  for (it=_members.begin(); it!=_members.end(); ++it) delete it.data();
}

bool Coff::Archive::readSymbols(const QByteArray &data, uint offset, Log::Base &log)
{
  Q_UINT32 nb;
  if ( !getULong(data, offset, 4, log, nb) ) return false;
  QValueVector<Member *> members(nb);
  for (uint i=0; i<nb; i++) {
    Q_UINT32 start;
    if ( !getULong(data, offset, 4, log, start) ) return false;
    if ( !_offsets.contains(start) ) {
      log.log(Log::LineType::Error, i18n("Uknown file member offset: %1").arg(toHexLabel(start, 8)));
      return false;
    }
    members[i] = _offsets[start];
  }
  for (uint i=0; i<nb; i++) {
    QString name(data.data() + offset);
    offset += name.length() + 1;
    _symbols[name] = members[i];
  }
  return true;
}

Log::KeyList Coff::Archive::information() const
{
  Log::KeyList keys;
  keys.append(i18n("No. of file members:"), QString::number(members().count()));
  keys.append(i18n("No. of symbols:"), QString::number(symbols().count()));
  return keys;
}
