/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MISC_H
#define MISC_H

#include <ctype.h>
#include "common/global/global.h"
#include <qstringlist.h>
#include <qmap.h>

#include "number.h"

//-----------------------------------------------------------------------------
inline QString repeat(const char *r, uint nb)
{
  QString s;
  for (uint i=0; i<nb; i++) s += r;
  return s;
}

inline QString stripEndingWhiteSpaces(const QString &s) {
  int k = s.length()-1;
  for (; k>=0; k--) if ( s[k]!=' ' ) break;
  return s.mid(0, k+1);
}

extern uchar bin2bcd(uchar bin);
extern uchar bcd2bin(uchar bcd);
inline bool XOR(bool b1, bool b2) { return ( (!b1 && b2) || (b1 && !b2) ); }
extern Q_UINT32 getULong(const QByteArray &data, uint offset, uint nbBytes, bool *ok);

enum PrintMode { PrintAlphaNum, PrintEscapeAll };
inline QString toPrintable(char c, PrintMode mode)
{
  if ( mode==PrintAlphaNum && isalnum(c) ) return QChar(c);
  return "\\" + toHex(uchar(c), 2);
}
inline QString toPrintable(const char *data, uint size, PrintMode mode)
{
  QString s;
  for (uint i=0; i<size; i++) s += toPrintable(data[i], mode);
  return s;
}
inline QString toPrintable(const QString &s, PrintMode mode)
{
  QByteArray a = toAscii(s);
  return toPrintable(a.data(), a.count(), mode);
}
inline QString toPrintable(const QMemArray<uchar> &data, PrintMode mode)
{
  return toPrintable((const char *)data.data(), data.size(), mode);
}

extern QString escapeXml(const QString &s);
extern QString htmlTableRow(const QString &title, const QString &value);

inline void qCrash()
{
  char *ptr = 0;
  *ptr = 0;
}

//-----------------------------------------------------------------------------
class Range
{
public:
  Range() : start(0), end(0) {}
  Range(uint s, uint e) : start(s), end(e) {}
  bool isEmpty() const { return end<=start; }
  bool contains(uint v) const { return !isEmpty() && v>=start && v<=end; }

  uint start, end;
};

class RangeVector : public QValueVector<Range>
{
public:
  RangeVector() {}
  RangeVector(const Range &range) { append(range); }
  bool isEmpty() const {
    for (uint i=0; i<uint(count()); i++) if ( !at(i).isEmpty() ) return false;
    return true;
  }
  bool contains(uint v) const {
    for (uint i=0; i<uint(count()); i++) if ( at(i).contains(v) ) return true;
    return false;
  }
};

//-----------------------------------------------------------------------------
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
  QString pretty() const;
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
