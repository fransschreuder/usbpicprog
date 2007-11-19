/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef VERSION_DATA_H
#define VERSION_DATA_H

#include "common/global/global.h"

class VersionData
{
public:
  static VersionData fromString(const QString &s);
  static VersionData fromHexString(const QString &s);

public:
  VersionData() : _valid(false) {}
  VersionData(uchar majorNum, uchar minorNum, uchar dotNum)
    : _valid(true), _majorNum(majorNum), _minorNum(minorNum), _dotNum(dotNum) {}
  bool isValid() const { return _valid; }
  void clear() { _valid = false; }
  uchar majorNum() const { return _majorNum; }
  uchar minorNum() const { return _minorNum; }
  uchar dotNum() const { return _dotNum; }
  QString sub() const { return _sub; }
  VersionData toWithoutDot() const { return VersionData(_majorNum, _minorNum, 0); }
  QString pretty() const;
  QString prettyWithoutDot() const;
  uint toUInt() const;
  bool operator <(const VersionData &vd) const { return toUInt()<vd.toUInt(); }
  bool operator <=(const VersionData &vd) const { return toUInt()<=vd.toUInt(); }
  bool operator >(const VersionData &vd) const { return toUInt()>vd.toUInt(); }
  bool operator >=(const VersionData &vd) const { return toUInt()>=vd.toUInt(); }
  bool operator ==(const VersionData &vd) const { return toUInt()==vd.toUInt(); }
  bool operator !=(const VersionData &vd) const { return toUInt()!=vd.toUInt(); }

private:
  bool    _valid;
  uchar   _majorNum, _minorNum, _dotNum;
  QString _sub;
};

#endif
