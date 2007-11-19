/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef NUMBER_H
#define NUMBER_H

#include <ctype.h>

#include "common/global/global.h"
#include "key_enum.h"

//----------------------------------------------------------------------------
struct NumberBaseData {
  uint base;
  const char *prefix, *label,* key;
};

BEGIN_DECLARE_ENUM(NumberBase)
  Dec = 0, Hex, Bin, Oct, String
END_DECLARE_ENUM(NumberBase, NumberBaseData)

extern uint nbChars(NumberBase base, ulong value);
extern ulong maxValue(NumberBase base, uint nbChars);
inline uint convertNbChars(uint nb, NumberBase from, NumberBase to) { return nbChars(to, maxValue(from, nb)); }

extern char toChar(NumberBase base, uint value);
extern QString toString(NumberBase base, ulong value, uint nbChars);
extern QString toLabel(NumberBase base, ulong value, uint nbChars);
extern QString toLabels(NumberBase base, const QMemArray<ulong> &values, uint nbChars);

extern uint fromChar(NumberBase base, char c, bool *ok);
extern ulong fromString(NumberBase base, const char *s, uint size, bool *ok);
extern ulong fromString(NumberBase base, const QString &s, bool *ok);
extern ulong fromLabel(NumberBase base, const QString &s, bool *ok);
extern ulong fromLabel(NumberBase base, const QString &s, uint nbChars, bool *ok);
extern ulong fromLabel(NumberBase base, const char *s, uint size, bool *ok);

extern ulong fromAnyLabel(const QString &s, bool *ok);

//----------------------------------------------------------------------------
inline QString toHex(ulong value, uint nbChars) { return toString(NumberBase::Hex, value, nbChars); }
inline QString toHexLabel(ulong value, uint nbChars) { return toLabel(NumberBase::Hex, value, nbChars); }
inline QString toHexLabelAbs(ulong value) { return toLabel(NumberBase::Hex, value, nbChars(NumberBase::Hex, value)); }

inline uint fromHex(char c, bool *ok) { return fromChar(NumberBase::Hex, c, ok); }
inline uint fromHex(QChar c, bool *ok) { return fromChar(NumberBase::Hex, c.latin1(), ok); }
inline ulong fromHex(const char *s, uint size, bool *ok) { return fromString(NumberBase::Hex, s, size, ok); }
inline ulong fromHex(const QString &s, bool *ok) { return fromString(NumberBase::Hex, s, ok); }
inline ulong fromHexLabel(const QString &s, bool *ok) { return fromLabel(NumberBase::Hex, s, ok); }
inline ulong fromHexLabel(const QString &s, uint nbChars, bool *ok)  { return fromLabel(NumberBase::Hex, s, nbChars, ok); }
inline ulong fromHexLabel(const char *s, uint size, bool *ok)  { return fromLabel(NumberBase::Hex, s, size, ok); }

//----------------------------------------------------------------------------
inline uint nbBits(ulong value) { return nbChars(NumberBase::Bin, value); }
inline uint nbBitsToNbChars(uint nbBits) { return nbBits/4 + (nbBits%4 ? 1 : 0); }
inline uint nbBitsToNbBytes(uint nbBits) { return nbBits/8 + (nbBits%8 ? 1 : 0); }
inline uint nbChars(ulong value) { return nbBitsToNbChars(nbBits(value)); }
inline uint nbBytes(ulong value) { return nbBitsToNbBytes(nbBits(value)); }

//----------------------------------------------------------------------------
extern QString formatNumber(ulong v);
extern QByteArray toAscii(const QString &s);

//----------------------------------------------------------------------------
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

#endif
