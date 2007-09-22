/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ICD1_H
#define ICD1_H

#include "progs/icd2/base/icd.h"
#include "icd1_data.h"
#include "icd1_serial.h"

namespace Icd1
{
//-----------------------------------------------------------------------------
class Hardware : public Icd::Hardware
{
public:
  Hardware(::Programmer::Base &base, const QString &portDevice);
  SerialPort *port() { return static_cast<SerialPort *>(_port); }
  // initialization
  virtual bool uploadFirmware(const Pic::Memory &memory);
  virtual bool setTarget();

// status
  virtual bool getFirmwareVersion(VersionData &version);
  virtual bool readVoltages(VoltagesData &voltages);
  virtual bool setTargetReset(Pic::ResetMode mode);
  bool selfTest();

// programming
  virtual bool readMemory(Pic::MemoryRangeType type, uint wordOffset, uint nbWords, Device::Array &data, const ::Programmer::VerifyData *vdata);
  virtual bool writeMemory(Pic::MemoryRangeType type, const Device::Array &data, uint wordOffset, uint nbWords);
  virtual bool eraseAll();

// debugging
  virtual bool readRegister(uint address, uint &value, uint nbBytes);
  virtual bool writeRegister(uint address, uint value, uint nbBytes);
  virtual bool resumeRun();
  virtual bool step();
  virtual bool haltRun();
  virtual uint getProgramCounter();

private:
  virtual bool internalConnect(const QString &mode);
  virtual QString receivedData() const { return _rx; }
  bool sendCommand(uint cmd, uint *res = 0, uint timeout = Port::Serial::DEFAULT_TIMEOUT);
  bool readBlockCommand(uint nbBytes);
  bool gotoMemory(Pic::MemoryRangeType type, uint offset);
};

} // namespace

#endif
