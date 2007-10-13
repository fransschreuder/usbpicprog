/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PSP_H
#define PSP_H

#include "devices/pic/pic/pic_prog.h"
#include "psp_serial.h"
#include "psp_data.h"

namespace Psp
{
  extern QMemArray<uchar> createConfigInfo(const Pic::Data &data);
  extern QMemArray<uchar> createDeviceInfo(const Pic::Data &data);

//-----------------------------------------------------------------------------
class Hardware : public Programmer::PicHardware
{
public:
  Hardware(::Programmer::Base &base, const QString &portDevice);
  SerialPort *port() { return static_cast<SerialPort *>(_port); }
  virtual bool uploadFirmware(const Pic::Memory &) { return false; }
  virtual bool setTarget();
  virtual bool getFirmwareVersion(VersionData &version);
  virtual bool readMemory(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata);
  virtual bool writeMemory(Pic::MemoryRangeType type, const Device::Array &data, bool force);
  virtual bool eraseAll();
  virtual bool internalConnectHardware();

private:
  bool setRange(uint start, uint end);
  static char readCommand(Pic::MemoryRangeType type);
  static char writeCommand(Pic::MemoryRangeType type);
};

//-----------------------------------------------------------------------------
class DeviceSpecific : public ::Programmer::PicDeviceSpecific
{
public:
  DeviceSpecific(::Programmer::Base &base) : ::Programmer::PicDeviceSpecific(base) {}
  virtual bool canEraseAll() const { return true; }
  virtual bool canEraseRange(Pic::MemoryRangeType) const { return false; }
  virtual bool canReadRange(Pic::MemoryRangeType type) const { return ( type!=Pic::MemoryRangeType::DeviceId ); }
  virtual bool canWriteRange(Pic::MemoryRangeType) const { return true; }
  Hardware &hardware() { return static_cast<Hardware &>(*_base.hardware()); }
  virtual bool setPowerOff() { return false; }
  virtual bool setPowerOn() { return false; }
  virtual bool setTargetPowerOn(bool) { return true; }
  virtual bool doEraseRange(Pic::MemoryRangeType) { return false; }
  virtual bool doErase(bool) { return hardware().eraseAll(); }
  virtual bool doRead(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata) { return hardware().readMemory(type, data, vdata); }
  virtual bool doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force) { return hardware().writeMemory(type, data, force); }
};

} // namespace

#endif
