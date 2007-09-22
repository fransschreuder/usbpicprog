/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICKIT2_H
#define PICKIT2_H

#include "pickit.h"
#include "pickit2_data.h"

namespace Pickit2
{
//-----------------------------------------------------------------------------
class Array : public Pickit::Array
{
public:
  Array() : Pickit::Array(64, 'Z', PrintAlphaNum) {}
};

//-----------------------------------------------------------------------------
class USBPort : public Pickit::USBPort
{
public:
  USBPort(Log::Base &log) : Pickit::USBPort(0x0033, log) {}
  virtual Pickit::Array array() const { return Array(); }
  void fillCommand(Pickit::Array &cmd, uchar cmd, uint nbBytes, uint address, uint i, bool longAddress = true) const;
  Pickit::Array createCommand(uchar cmd, uint nbBytes, uint address, bool longAddress = true) const;

  bool readFirmwareCodeMemory(Device::Array &data, const Device::Array *vdata);
  //bool readFirmwareEepromMemory(Device::Array &data);
  bool eraseFirmwareCodeMemory();
  bool writeFirmwareCodeMemory(const Device::Array &data);
  //bool writeFirmwareEepromMemory(const Device::Array &data);
  bool resetFirmwareDevice(::Programmer::Mode mode) { return command(mode==::Programmer::BootloadMode ? 'B' : 0xFF); }
  bool uploadFirmware(const Pic::Memory &memory);

private:
  virtual uint readEndPoint() const { return 0x81; }
  virtual uint writeEndPoint() const { return 0x01; }
};

//-----------------------------------------------------------------------------
class Hardware : public Pickit::Hardware
{
public:
  Hardware(::Programmer::Base &base) : Pickit::Hardware(base, new USBPort(base)) {}
  virtual bool readVoltages(VoltagesData &voltages);
  bool setVddVpp(double vdd, double vpp);
};

//-----------------------------------------------------------------------------
class Baseline : public Pickit::Baseline
{
public:
  Baseline(::Programmer::Base &base) : Pickit::Baseline(base) {}
  virtual bool init();
  virtual char entryMode() const { return data(device().name()).entryMode; }
  virtual uint nbWrites(Pic::MemoryRangeType type) const { return (type==Pic::MemoryEeprom ? 4 : 16); }
};

//-----------------------------------------------------------------------------
class P16F : public Pickit::P16F
{
public:
  P16F(::Programmer::Base &base) : Pickit::P16F(base) {}
  virtual bool init();
  virtual char entryMode() const { return data(device().name()).entryMode; }
  virtual uint nbWrites(Pic::MemoryRangeType type) const { return (type==Pic::MemoryCode ? 16 : 4); }
};

class P16F87XA : public P16F
{
public:
  P16F87XA(::Programmer::Base &base) : P16F(base) {}
  virtual bool init();
};

class P16F7X : public P16F
{
public:
  P16F7X(::Programmer::Base &base) : P16F(base) {}
  virtual bool init();
  virtual char writeCode() const { return 'w'; }
};

class P16F716 : public P16F
{
public:
  P16F716(::Programmer::Base &base) : P16F(base) {}
  virtual bool init();
};

//-----------------------------------------------------------------------------
class P18F : public Pickit::DeviceSpecific
{
public:
  P18F(::Programmer::Base &base) : Pickit::DeviceSpecific(base) {}
  Hardware &hardware() { return static_cast<Hardware &>(Pickit::DeviceSpecific::hardware()); }
  virtual bool init();
  virtual bool doEraseRange(Pic::MemoryRangeType type);
  virtual bool doErase(bool) { return doEraseCommand(0x87, 0x0F); }
  bool doEraseCommand(uint cmd1, uint cmd2);
  virtual bool doRead(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata);
  virtual bool doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force);
  virtual char entryMode() const { return data(device().name()).entryMode; }
};

class P18F2X20 : public P18F
{
public:
  P18F2X20(::Programmer::Base &base) : P18F(base) {}
  virtual bool doEraseRange(Pic::MemoryRangeType type);
  virtual bool doErase(bool) { return doEraseCommand(0x80); }
  bool doEraseCommand(uint cmd);
};

} // namespace

#endif
