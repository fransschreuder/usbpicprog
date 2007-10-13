/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "bitvalue.h"

const uint GenericValue::INVALID = 0xFFFFFFFFU;

BitValue BitValue::XORn(uint n) const
{
  uint nb = nbBits(_value);
  uint mask = maxValue(Number::Bin, n);
  uint res = 0x0;
  for (uint i=0; i<nb; i +=n) {
    res ^= (_value >> i) & mask;
    //qDebug("%i %s %s", i, toHexLabel((value>>i) & mask, 4).latin1(), toHexLabel(res, 4).latin1());
  }
  return res;
}

BitValue BitValue::XNORn(uint n) const
{
  BitValue res = XORn(n);
  BitValue mask = maxValue(Number::Bin, n);
  return res.complementInMask(mask);
}
