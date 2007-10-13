/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MEM24_MEMORY_H
#define MEM24_MEMORY_H

#include "common/global/global.h"
#include "devices/base/generic_memory.h"
#include "devices/base/hex_buffer.h"
#include "devices/mem24/base/mem24.h"

namespace Mem24
{

class Memory : public Device::Memory
{
public:
  Memory(const Data &data);
  const Data &device() const { return static_cast<const Data &>(_device); }
  virtual void fill(BitValue value);
  virtual void checksumCheckFill() { clear(); }
  Device::Array arrayForWriting() const;
  BitValue byte(uint offset) const;
  void setByte(uint offset, BitValue value);
  virtual BitValue checksum() const;

  virtual HexBuffer toHexBuffer() const;
  virtual void copyFrom(const Device::Memory &memory);

private:
  Device::Array _data;

  virtual void savePartial(QTextStream &stream, HexBuffer::Format format) const;
  virtual void fromHexBuffer(const HexBuffer &hb, WarningTypes &warningTypes,
                             QStringList &warnings, QMap<uint, bool> &inRange);
};

} // namespace

#endif
