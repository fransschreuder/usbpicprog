/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "generic_memory.h"

bool Device::Memory::load(QTextStream &stream, QStringList &errors,
			  WarningTypes &warningTypes, QStringList &warnings)
{
  HexBuffer hb;
  if ( !hb.load(stream, errors) ) return false;
  warningTypes = fromHexBuffer(hb, warnings);
  return true;
}

Device::Memory::WarningTypes Device::Memory::fromHexBuffer(const HexBuffer &hb, QStringList &warnings)
{
  clear();
  WarningTypes result = NoWarning;
  warnings.clear();
  QMap<uint, bool> inRange;
  fromHexBuffer(hb, result, warnings, inRange);

   // check that all values in FragBuffer are within memory ranges
  HexBuffer::const_iterator it = hb.begin();
  for (; it!=hb.end(); ++it) {
    if ( !it.data().isInitialized() || inRange[it.key()] ) continue;
    if ( !(result & ValueOutsideRange) ) {
      result |= ValueOutsideRange;
      warnings += i18n("At least one value (at address %1) is defined outside memory ranges.").arg(toHexLabel(it.key(), 8));
    }
    break;
  }

  return result;
}

bool Device::Memory::save(QTextStream &stream, HexBuffer::Format format) const
{
  savePartial(stream, format);
  HexBuffer hb;
  hb.saveEnd(stream);
  return true;
}
