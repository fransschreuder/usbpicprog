/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "version_data.h"

#include <qregexp.h>

#include "number.h"

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
  vd._majorNum = ::fromString(NumberBase::Hex, s.mid(0, 2), 0);
  vd._minorNum = ::fromString(NumberBase::Hex, s.mid(2, 2), 0);
  vd._dotNum = ::fromString(NumberBase::Hex, s.mid(4, 2), 0);
  return vd;
}

QString VersionData::pretty() const
{
  if ( !isValid() ) return "---";
  return QString::number(_majorNum) + '.' + QString::number(_minorNum) + '.' + QString::number(_dotNum) + _sub;
}

QString VersionData::prettyWithoutDot() const
{
  if ( !isValid() ) return "---";
  return QString::number(_majorNum) + '.' + QString::number(_minorNum);
}

uint VersionData::toUInt() const
{
  Q_ASSERT(_valid);
  return (_majorNum << 16) | (_minorNum << 8) | _dotNum;
}
