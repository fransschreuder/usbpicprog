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

#include <qstring.h>

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

extern bool checkAvailable(const QByteArray &data, uint offset, uint nbBytes);
extern Q_UINT32 getULong(const QByteArray &data, uint offset, uint nbBytes, bool *ok);

extern QString escapeXml(const QString &s);
extern QString htmlTableRow(const QString &title, const QString &value);
extern void crash(const char *assert, const char *file, int line);
#define CRASH_ASSERT(x) ((x) ? void(0) : crash(#x, __FILE__, __LINE__))

#endif
