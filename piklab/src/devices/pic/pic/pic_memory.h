/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_MEMORY_H
#define PIC_MEMORY_H

#include "common/global/global.h"
#include "devices/base/generic_memory.h"
#include "devices/base/hex_buffer.h"
#include "devices/pic/base/pic_config.h"

namespace Pic
{

class Memory : public Device::Memory
{
public:
  Memory(const Data &data);
  const Data &device() const { return static_cast<const Data &>(_device); }
  virtual void fill(BitValue value);
  void checksumCheckFill(); // a special memory fill for checksum check (cf datasheets)
  virtual void fill(MemoryRangeType type, BitValue value);
  virtual void clear() { Device::Memory::clear(); }
  bool isClear(MemoryRangeType type) const;
  void clear(MemoryRangeType type) { fill(type, BitValue()); }
  Device::Array arrayForWriting(MemoryRangeType type) const;
  BitValue word(MemoryRangeType type, uint offset) const;
  BitValue normalizeWord(MemoryRangeType type, uint offset, BitValue value) const;
  BitValue normalizedWord(MemoryRangeType type, uint offset) const;
  void setWord(MemoryRangeType type, uint offset, BitValue value);
  void setArray(MemoryRangeType type, const Device::Array &array);

  AddressRange bootRange() const;
  AddressRange blockRange(uint i) const;
  bool isBootProtected(Protection::Type ptype) const { return !bootProtectedRange(ptype).isEmpty(); }
  bool isBlockProtected(Protection::Type ptype, uint i) const { return !blockProtectedRange(ptype, i).isEmpty(); }
  bool isProtected(Protection::Type ptype, MemoryRangeType type) const { return !protectedRanges(ptype, type).isEmpty(); }
  AddressRangeVector protectedRanges(Protection::Type ptype, MemoryRangeType type) const;
  void setConfigValue(const QString &maskName, const QString &valueName);
  bool hasDebugOn() const { return hasFlagOn("DEBUG", false); }
  void setDebugOn(bool on) { setFlagOn("DEBUG", on); }
  bool hasWatchdogTimerOn() const { return hasFlagOn("WDT", false); }
  void setWatchdogTimerOn(bool on) { return setFlagOn("WDT", on); }
  void setBootProtection(bool on, Protection::Type ptype);
  void setBlockProtection(bool on, Protection::Type ptype, uint block);
  void setProtection(bool on, Protection::Type ptype, MemoryRangeType type);

  virtual BitValue checksum() const;
  BitValue unprotectedChecksum() const;
  void setUserIdToUnprotectedChecksum();

  virtual HexBuffer toHexBuffer() const;
  virtual void copyFrom(const Device::Memory &memory);
  void copyFrom(MemoryRangeType type, const Memory &memory);
  void fromHexBuffer(MemoryRangeType type, const HexBuffer &hb, WarningTypes &warningTypes,
                     QStringList &warnings, QMap<uint, bool> &inRange);

private:
  QValueVector<Device::Array> _ranges;

  void toHexBuffer(MemoryRangeType type, HexBuffer &hb) const;
  virtual void savePartial(QTextStream &stream, HexBuffer::Format format) const;
  virtual void fromHexBuffer(const HexBuffer &hb, WarningTypes &warningTypes,
                             QStringList &warnings, QMap<uint, bool> &inRange);
  QString findValue(const QString &maskName) const;
  bool hasFlagOn(const QString &maskName, bool valueIfNotPresent) const;
  void setFlagOn(const QString &maskName, bool on);
  void setProtection(bool on, const QString &maskName, Protection::Type ptype);
  AddressRange bootProtectedRange(Protection::Type ptype) const;
  AddressRange blockProtectedRange(Protection::Type ptype, uint block) const;
};

} // namespace

#endif
