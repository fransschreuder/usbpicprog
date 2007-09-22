/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BOOTLOADER_PICDEM_H
#define BOOTLOADER_PICDEM_H

#include "bootloader.h"
#include "bootloader_prog.h"
#include "common/port/usb_port.h"

namespace PicdemBootloader
{
//-----------------------------------------------------------------------------
class Config : public GenericConfig
{
public:
  Config() : GenericConfig("picdem_bootloader") {}
  uint readVendorId();
  void writeVendorId(uint id);
  uint readProductId();
  void writeProductId(uint id);
};

//-----------------------------------------------------------------------------
class Port : public ::Port::USB
{
public:
  Port(Log::Base &base);
  bool receive(uint nb, QMemArray<uchar> &array);
  bool send(const QMemArray<uchar> &array);
  bool sendAndReceive(QMemArray<uchar> &data, uint nb);
  QMemArray<uchar> command(uchar instruction, uint address, uint len, uint nb) const;
};

//-----------------------------------------------------------------------------
class Hardware : public Bootloader::Hardware
{
public:
  Hardware(::Programmer::Base &base);
  Port &port() { return static_cast<Port &>(*_port); }
  virtual bool write(Pic::MemoryRangeType type, const Device::Array &data);
  virtual bool read(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata);
  virtual bool erase(Pic::MemoryRangeType type);
  virtual bool internalConnectHardware();
};

//-----------------------------------------------------------------------------
class DeviceSpecific : public Bootloader::DeviceSpecific
{
public:
  DeviceSpecific(::Programmer::Base &base) : Bootloader::DeviceSpecific(base) {}
  virtual bool canEraseAll() const { return true; }
  virtual bool canEraseRange(Pic::MemoryRangeType type) const { return ( type!=Pic::MemoryEeprom ); }
  virtual bool canReadRange(Pic::MemoryRangeType type) const { return ( type!=Pic::MemoryEeprom && type!=Pic::MemoryDeviceId ); }
  virtual bool canWriteRange(Pic::MemoryRangeType type) const { return ( type!=Pic::MemoryEeprom ); }
  virtual bool doEraseRange(Pic::MemoryRangeType type) { return static_cast<Hardware &>(hardware()).erase(type); }
  virtual bool doErase(bool) { return doEraseRange(Pic::MemoryCode); }
};

//-----------------------------------------------------------------------------
class Base : public Bootloader::Base
{
Q_OBJECT
public:
  Base(const Programmer::Group &group, const Pic::Data *data);
};

} // namespace

#endif
