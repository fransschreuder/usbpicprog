/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BOOTLOADER_TBL_H
#define BOOTLOADER_TBL_H

#include "bootloader.h"
#include "bootloader_prog.h"
#include "common/port/serial.h"
#include "common/global/generic_config.h"

namespace TinyBootloader
{
//-----------------------------------------------------------------------------
class Config : public GenericConfig
{
public:
  Config() : GenericConfig("tiny_bootloader") {}
  Port::Serial::Speed readSpeed();
  void writeSpeed(Port::Serial::Speed speed);
  uint readTimeout(); // ms
  void writeTimeout(uint timeout); // ms
  uint readRetries();
  void writeRetries(uint nb);
};

//-----------------------------------------------------------------------------
class Hardware : public ::Bootloader::Hardware
{
public:
  Hardware(::Programmer::Base &base, const QString &portDevice);
  Port::Serial *port() { return static_cast<Port::Serial *>(_port); }
  bool verifyDeviceId();
  virtual bool write(Pic::MemoryRangeType type, const Device::Array &data);
  virtual bool read(Pic::MemoryRangeType, Device::Array &, const ::Programmer::VerifyData *) { return false; }
  bool writeCode(const Device::Array &data, bool erase);
  bool writeConfig(const Device::Array &data);
  bool writeEeprom(const Device::Array &data);
  virtual bool internalConnectHardware();
  virtual bool openPort();

private:
  uchar _id;
  uint _timeout; // ms
  uint _retries;

  bool waitReady(bool *checkCRC);
  bool sendChar(char c, uchar *crc);
  bool sendCodeAddress(uint address, uchar &crc);
  bool endWrite(uchar crc, uint &retries, bool &ok);
};

//-----------------------------------------------------------------------------
class DeviceSpecific : public ::Bootloader::DeviceSpecific
{
public:
  DeviceSpecific(::Programmer::Base &base) : ::Bootloader::DeviceSpecific(base) {}
  virtual bool canEraseAll() const { return true; }
  virtual bool canEraseRange(Pic::MemoryRangeType type) const { return canWriteRange(type); }
  virtual bool emulatedErase() const { return true; }
  virtual bool canReadRange(Pic::MemoryRangeType) const { return false; }
  virtual bool canWriteRange(Pic::MemoryRangeType type) const;
  virtual bool doEraseRange(Pic::MemoryRangeType type);
  virtual bool doErase(bool);
};

//-----------------------------------------------------------------------------
class Base : public ::Bootloader::Base
{
Q_OBJECT
public:
  Base(const Programmer::Group &group, const Pic::Data *data);
  virtual bool verifyDeviceId() { return static_cast<Hardware &>(hardware()).verifyDeviceId(); }
};

} // namespace

#endif
