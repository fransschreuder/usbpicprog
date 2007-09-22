/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GENERIC_MEMORY_H
#define GENERIC_MEMORY_H

#include "devices/base/generic_device.h"
#include "devices/base/hex_buffer.h"

namespace Device
{

class Memory
{
public:
  const Data &device() const { return _device; }
  virtual ~Memory() {}
  virtual void fill(uint value) = 0;
  virtual void clear() { fill(Device::INVALID); }
  virtual void copyFrom(const Memory &memory) = 0;
  virtual uint checksum() const = 0;

  virtual HexBuffer toHexBuffer() const = 0;
  bool save(QTextStream &stream, HexBuffer::Format format) const;
  enum WarningType { NoWarning = 0, ValueTooLarge = 1, ValueOutsideRange = 2 };
  Q_DECLARE_FLAGS(WarningTypes, WarningType)
  WarningTypes fromHexBuffer(const HexBuffer &hb, QStringList &warnings);
  bool load(QTextStream &stream, QStringList &errors, WarningTypes &warningTypes, QStringList &warnings);

protected:
  const Data &_device;

  Memory(const Data &device) : _device(device) {}
  virtual void fromHexBuffer(const HexBuffer &hb, WarningTypes &warningTypes,
                             QStringList &warnings, QMap<uint, bool> &inRange) = 0;
  virtual void savePartial(QTextStream &stream, HexBuffer::Format format) const = 0;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Memory::WarningTypes)

} // namespace

#endif
