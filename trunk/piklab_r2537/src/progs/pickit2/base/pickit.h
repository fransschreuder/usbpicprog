/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICKIT_H
#define PICKIT_H

#include "progs/icd2/base/microchip.h"
#include "common/port/usb_port.h"
#include "devices/pic/pic/pic_memory.h"
#include "devices/pic/prog/pic_prog.h"

namespace Pickit
{
enum PowerType { PowerOff = 0, PowerOn = 1, Osc2_5kHz = 2 };

//-----------------------------------------------------------------------------
class Array
{
public:
  uint length() const { return _data.count(); }
  QString pretty() const;
  uchar &operator[](uint i) { return _data[i]; }
  uchar operator[](uint i) const { return _data[i]; }

protected:
  Array(uint length, uchar fillChar, PrintMode mode);

private:
  uchar _fillChar;
  PrintMode _mode;
  QMemArray<uchar> _data;

  friend class USBPort;
};

//-----------------------------------------------------------------------------
class USBPort : public Port::USB
{
public:
  USBPort(uint deviceId, Log::Base &manager);
  virtual Array array() const = 0;
  bool command(const Array &cmd);
  bool command(uchar c);
  bool command(const char *s);
  bool receive(Array &data);
  bool getMode(VersionData &version, ::Programmer::Mode &mode);
  bool receiveWords(uint nbBytesWord, uint nbRead, QValueVector<uint> &data, uint offset = 0);

protected:
  virtual uint writeEndPoint() const = 0;
  virtual uint readEndPoint() const = 0;

private:
  virtual uint timeout(uint) const { return 10000; } // 10s
  enum { CONFIG_HID = 1, // use HID for pickit configuration
         CONFIG_VENDOR = 2 // vendor specific configuration
  };
};

//-----------------------------------------------------------------------------
class Hardware : public ::Programmer::PicHardware
{
public:
  Hardware(::Programmer::Base &base, USBPort *port);
  bool setTargetPower(uint value);
  USBPort &port() { return static_cast<USBPort &>(*_port); }
  const USBPort &port() const { return static_cast<USBPort &>(*_port); }
  bool writeWords(uint max, char c, uint nbBytesWord,
                  uint &i, const Device::Array &data);
  bool regenerateOsccal(BitValue &newValue);

protected:
  virtual bool internalConnectHardware();
};

//-----------------------------------------------------------------------------
class DeviceSpecific : public ::Programmer::PicDeviceSpecific
{
public:
  DeviceSpecific(::Programmer::Base &base) : ::Programmer::PicDeviceSpecific(base) {}
  virtual bool canEraseAll() const { return true; }
  virtual bool canEraseRange(Pic::MemoryRangeType type) const { return ( type==Pic::MemoryRangeType::Code || type==Pic::MemoryRangeType::Eeprom ); }
  virtual bool canReadRange(Pic::MemoryRangeType) const { return true; }
  virtual bool canWriteRange(Pic::MemoryRangeType) const { return true; }
  Hardware &hardware() { return static_cast<Hardware &>(*_base.hardware()); }
  virtual bool init() = 0;
  virtual bool setPowerOn();
  virtual bool setPowerOff();
  virtual bool setTargetPowerOn(bool on);
  virtual char entryMode() const = 0;
};

//-----------------------------------------------------------------------------
class BMDeviceSpecific : public DeviceSpecific
{
public:
  BMDeviceSpecific(::Programmer::Base &base) : DeviceSpecific(base) {}
  virtual bool gotoMemory(Pic::MemoryRangeType type) = 0;
  virtual bool doRead(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata);
  virtual bool incrementPC(uint nb);
  virtual uint nbWrites(Pic::MemoryRangeType type) const = 0;
  virtual char writeCode() const = 0;
  virtual char writeData() const = 0;
  virtual bool doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force);
  virtual bool doEraseRange(Pic::MemoryRangeType type);
};

//-----------------------------------------------------------------------------
class Baseline : public BMDeviceSpecific
{
public:
  Baseline(::Programmer::Base &base) : BMDeviceSpecific(base) {}
  virtual bool doErase(bool) { return doEraseRange(Pic::MemoryRangeType::Code); }
  virtual bool gotoMemory(Pic::MemoryRangeType type);
  virtual char writeCode() const { return 'w'; }
  virtual char writeData() const { return 'D'; }
};

class P16F : public BMDeviceSpecific
{
public:
  P16F(::Programmer::Base &base) : BMDeviceSpecific(base) {}
  virtual bool gotoMemory(Pic::MemoryRangeType type);
  virtual bool doEraseRange(Pic::MemoryRangeType type);
  virtual bool doErase(bool);
  virtual char writeCode() const { return 'W'; }
  virtual char writeData() const { return 'D'; }
};

} // namespace

#endif
