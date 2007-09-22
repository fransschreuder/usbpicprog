/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BOOTLOADER_PICKIT2_H
#define BOOTLOADER_PICKIT2_H

#include "bootloader.h"
#include "bootloader_prog.h"
#include "progs/pickit2/base/pickit2.h"

namespace Pickit2Bootloader
{
//-----------------------------------------------------------------------------
class Hardware : public Bootloader::Hardware
{
public:
  Hardware(::Programmer::Base &base);
  Pickit2::USBPort &port() { return static_cast<Pickit2::USBPort &>(*_port); }
  virtual bool write(Pic::MemoryRangeType type, const Device::Array &data);
  virtual bool read(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata);
  virtual bool internalConnectHardware();
};

//-----------------------------------------------------------------------------
class DeviceSpecific : public Bootloader::DeviceSpecific
{
public:
  DeviceSpecific(::Programmer::Base &base) : Bootloader::DeviceSpecific(base) {}
  virtual bool canEraseAll() const { return true; }
  virtual bool canEraseRange(Pic::MemoryRangeType type) const { return ( type==Pic::MemoryCode ); }
  virtual bool canReadRange(Pic::MemoryRangeType type) const { return ( type==Pic::MemoryCode ); }
  virtual bool canWriteRange(Pic::MemoryRangeType type) const { return ( type==Pic::MemoryCode ); }
  virtual bool doEraseRange(Pic::MemoryRangeType type);
  virtual bool doErase(bool) { return doEraseRange(Pic::MemoryCode); }
};

//-----------------------------------------------------------------------------
class Base : public ::Bootloader::Base
{
Q_OBJECT
public:
  Base(const Programmer::Group &group, const Pic::Data *data);
  virtual bool verifyDeviceId() { return true; }
};

} // namespace

#endif
