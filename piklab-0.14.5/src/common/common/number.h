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

//----------------------------------------------------------------------------
namespace Number
{
  enum Base { Dec = 0, Hex, Bin, Oct, String, Nb_Bases };
  struct Data {
    uint base;
    const char *prefix, *label;
  };
  extern const Data DATA[Nb_Bases];
} // namespace

extern uint nbChars(Number::Base base, ulong value);
extern ulong maxValue(Number::Base base, uint nbChars);
inline uint convertNbChars(uint nb, Number::Base from, Number::Base to) { return nbChars(to, maxValue(from, nb)); }

extern char toChar(Number::Base base, uint value);
extern QString toString(Number::Base base, ulong value, uint nbChars);
extern QString toLabel(Number::Base base, ulong value, uint nbChars);
extern QString toLabels(Number::Base base, const QMemArray<ulong> &values, uint nbChars);

extern uint fromChar(Number::Base base, char c, bool *ok);
extern ulong fromString(Number::Base base, const char *s, uint size, bool *ok);
extern ulong fromString(Number::Base base, const QString &s, bool *ok);
extern ulong fromLabel(Number::Base base, const QString &s, bool *ok);
extern ulong fromLabel(Number::Base base, const QString &s, uint nbChars, bool *ok);
extern ulong fromLabel(Number::Base base, const char *s, uint size, bool *ok);

extern ulong fromAnyLabel(const QString &s, bool *ok);

//----------------------------------------------------------------------------
inline QString toHex(ulong value, uint nbChars) { return toString(Number::Hex, value, nbChars); }
inline QString toHexLabel(ulong value, uint nbChars) { return toLabel(Number::Hex, value, nbChars); }
inline QString toHexLabelAbs(ulong value) { return toLabel(Number::Hex, value, nbChars(Number::Hex, value)); }

inline uint fromHex(char c, bool *ok) { return fromChar(Number::Hex, c, ok); }
inline uint fromHex(QChar c, bool *ok) { return fromChar(Number::Hex, c.latin1(), ok); }
inline ulong fromHex(const char *s, uint size, bool *ok) { return fromString(Number::Hex, s, size, ok); }
inline ulong fromHex(const QString &s, bool *ok) { return fromString(Number::Hex, s, ok); }
inline ulong fromHexLabel(const QString &s, bool *ok) { return fromLabel(Number::Hex, s, ok); }
inline ulong fromHexLabel(const QString &s, uint nbChars, bool *ok)  { return fromLabel(Number::Hex, s, nbChars, ok); }
inline ulong fromHexLabel(const char *s, uint size, bool *ok)  { return fromLabel(Number::Hex, s, size, ok); }

//----------------------------------------------------------------------------
inline uint nbBits(ulong value) { return nbChars(Number::Bin, value); }
inline uint nbBitsToNbChars(uint nbBits) { return nbBits/4 + (nbBits%4 ? 1 : 0); }
inline uint nbBitsToNbBytes(uint nbBits) { return nbBits/8 + (nbBits%8 ? 1 : 0); }
inline uint nbChars(ulong value) { return nbBitsToNbChars(nbBits(value)); }
inline uint nbBytes(ulong value) { return nbBitsToNbBytes(nbBits(value)); }

//----------------------------------------------------------------------------
extern QString formatNumber(ulong v);
extern QByteArray toAscii(const QString &s);

#endif
