/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ICD_H
#define ICD_H

#include "microchip.h"
#include "devices/pic/pic/pic_prog.h"
#include "devices/pic/pic/pic_memory.h"

namespace Icd
{
//-----------------------------------------------------------------------------
class Hardware : public ::Programmer::PicHardware
{
public:
  Hardware(::Programmer::Base &base, Port::Base *port) : ::Programmer::PicHardware(base, port, QString::null) {}
// initialization
  virtual bool uploadFirmware(const Pic::Memory &memory) = 0;
  virtual bool setTarget() = 0;

// status
  virtual bool getFirmwareVersion(VersionData &version) = 0;
  virtual bool setTargetPowerOn(bool) { return true; }

// programming
  virtual bool readMemory(Pic::MemoryRangeType type, uint wordOffset, Device::Array &data, const ::Programmer::VerifyData *vdata) = 0;
  virtual bool writeMemory(Pic::MemoryRangeType type, uint wordOffset, const Device::Array &data) = 0;
  virtual bool eraseAll() = 0;

// debugging
  virtual bool readRegister(Address address, BitValue &value, uint nbBytes) = 0;
  virtual bool writeRegister(Address address, BitValue value, uint nbBytes) = 0;
  virtual bool resumeRun() = 0;
  virtual bool step() = 0;
  virtual bool haltRun() = 0;
  virtual BitValue getProgramCounter() = 0;

protected:
  QString _rx;
  virtual bool internalConnect(const QString &mode) = 0;
  virtual QString receivedData() const = 0;
  virtual bool internalConnectHardware() { return internalConnect("U"); }
};

//-----------------------------------------------------------------------------
class DeviceSpecific : public ::Programmer::PicDeviceSpecific
{
public:
  DeviceSpecific(::Programmer::Base &base) : ::Programmer::PicDeviceSpecific(base) {}
  virtual bool canEraseAll() const { return true; }
  virtual bool canEraseRange(Pic::MemoryRangeType) const { return false; }
  virtual bool canReadRange(Pic::MemoryRangeType) const { return true; }
  virtual bool canWriteRange(Pic::MemoryRangeType) const { return true; }
  Hardware &hardware() { return static_cast<Hardware &>(*_base.hardware()); }
  virtual bool setPowerOff() { return false; }
  virtual bool setPowerOn() { return false; }
  virtual bool setTargetPowerOn(bool on) { return hardware().setTargetPowerOn(on); }
  virtual bool doEraseRange(Pic::MemoryRangeType) { return false; }
  virtual bool doErase(bool);
  virtual bool doRead(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata);
  Device::Array prepareRange(Pic::MemoryRangeType type, const Device::Array &data, bool force, uint &wordOffset);
  virtual bool doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force);
};

} // namespace

#endif
