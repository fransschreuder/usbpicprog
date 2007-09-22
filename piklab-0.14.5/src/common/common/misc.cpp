/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "misc.h"

#include <unistd.h>

#include <qregexp.h>
#include <qtimer.h>

//-----------------------------------------------------------------------------
uchar bin2bcd(uchar bin)
{
  char h = bin / 10;
  char l = bin % 10;
  return (h*16) + l;
}

uchar bcd2bin(uchar bcd)
{
  char h = bcd / 16;
  char l = bcd % 16;
  return (h*10) + l;
}

QString escapeXml(const QString &cs)
{
  QString s;
  for (uint i=0; i<uint(cs.length()); i++) {
    if ( cs[i]=='<' ) s += "&lt;";
    else if ( cs[i]=='>' ) s += "&gt;";
    else s += cs[i];
  }
  return s;
}

QString htmlTableRow(const QString &title, const QString &value)
{
  return "<tr><td>" + title + ":</td><td>" + value + "</td></tr>";
}

Q_UINT32 getULong(const QByteArray &data, uint offset, uint nbBytes, bool *ok)
{
  Q_ASSERT( nbBytes<=4 );
  if ( offset+nbBytes>uint(data.size()) ) {
    if (ok) *ok = false;
    return 0;
  }
  if (ok) *ok = true;
  Q_UINT32 r = 0;
  for (uint i=0; i<nbBytes; i++) r += Q_UINT8(data[offset+i]) << (8*i);
  return r;
}

//-----------------------------------------------------------------------------
VersionData VersionData::fromString(const QString &s)
{
  VersionData vd;
  QRegExp re("([0-9]+)\\.([0-9]+)\\.([0-9]+)(.*)");
  if ( !re.exactMatch(s) ) return vd;
  vd._valid = true;
  vd._majorNum = re.cap(1).toUInt();
  vd._minorNum = re.cap(2).toUInt();
  vd._dotNum = re.cap(3).toUInt();
  vd._sub = re.cap(4);
  return vd;
}

VersionData VersionData::fromHexString(const QString &s)
{
  VersionData vd;
  if ( s.length()!=6 ) return vd;
  vd._valid = true;
  vd._majorNum = ::fromString(Number::Hex, s.mid(0, 2), 0);
  vd._minorNum = ::fromString(Number::Hex, s.mid(2, 2), 0);
  vd._dotNum = ::fromString(Number::Hex, s.mid(4, 2), 0);
  return vd;
}

QString VersionData::pretty() const
{
  if ( !isValid() ) return "---";
  return QString::number(_majorNum) + '.' + QString::number(_minorNum) + '.' + QString::number(_dotNum) + _sub;
}

uint VersionData::toUInt() const
{
  Q_ASSERT(_valid);
  return (_majorNum << 16) | (_minorNum << 8) | _dotNum;
}
