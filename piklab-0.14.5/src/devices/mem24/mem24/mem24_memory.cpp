/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "mem24_memory.h"

#include <qfile.h>

#include "common/global/global.h"
#include "common/common/misc.h"

Mem24::Memory::Memory(const Data &data)
  : Device::Memory(data)
{
  fill(Device::INVALID);
}

void Mem24::Memory::fill(uint value)
{
  _data = Device::Array(device().nbBytes());
  for (uint i=0; i<_data.count(); i++) _data[i] = value;
}

void Mem24::Memory::copyFrom(const Device::Memory &memory)
{
  Q_ASSERT( device().name()==memory.device().name() );
  _data = static_cast<const Memory &>(memory)._data;
}

Device::Array Mem24::Memory::arrayForWriting() const
{
  Device::Array data = _data;
  for (uint i=0; i<data.count(); i++) data[i] &= 0xFF;
  return data;
}

uint Mem24::Memory::byte(uint offset) const
{
  Q_ASSERT( _data.size()>offset );
  return _data[offset];
}

void Mem24::Memory::setByte(uint offset, uint value)
{
  Q_ASSERT( _data.size()>offset );
  Q_ASSERT( value<=0xFF );
  _data[offset] = value;
}

uint Mem24::Memory::checksum() const
{
  uint cs = 0;
  for (uint i=0; i<_data.count(); i++) cs += _data[i];
  return cs & 0xFFFF;
}

//-----------------------------------------------------------------------------
void Mem24::Memory::savePartial(QTextStream &stream, HexBuffer::Format format) const
{
  HexBuffer hb = toHexBuffer();
  hb.savePartial(stream, format);
}

//-----------------------------------------------------------------------------
HexBuffer Mem24::Memory::toHexBuffer() const
{
  HexBuffer hb;
  for (uint k=0; k<device().nbBytes(); k++)
    hb.set(k, _data[k]);
  return hb;
}

void Mem24::Memory::fromHexBuffer(const HexBuffer &hb, WarningTypes &result,
                                      QStringList &warnings, QMap<uint, bool> &inRange)
{
  uint mask = 0xFF;
  for (uint k=0; k<device().nbBytes(); k++) {
    _data[k] = hb[k];
    if ( _data[k]!=Device::INVALID ) {
      inRange[k] = true;
      if ( !(result & ValueTooLarge) && (_data[k] & mask)!=_data[k] ) {
        result |= ValueTooLarge;
        warnings += i18n("At least one word (at offset %1) is larger (%2) than the corresponding mask (%3).")
                    .arg(toHexLabel(k, 8)).arg(toHexLabel(_data[k], 8)).arg(toHexLabel(mask, 8));
      }
      _data[k] &= mask;
    }
  }
}
