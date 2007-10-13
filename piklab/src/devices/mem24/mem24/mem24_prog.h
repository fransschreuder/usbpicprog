/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MEM24_PROG_H
#define MEM24_PROG_H

#include "progs/base/generic_prog.h"
#include "mem24_memory.h"

namespace Programmer
{
//-----------------------------------------------------------------------------
class Mem24DeviceSpecific : public DeviceSpecific
{
public:
  Mem24DeviceSpecific(::Programmer::Base &base) : DeviceSpecific(base) {}
  const Mem24::Data &device() const { return static_cast<const Mem24::Data &>(*_base.device()); }
  bool read(Device::Array &data, const VerifyData *vdata);
  bool write(const Device::Array &data);
  bool verifyByte(uint index, BitValue d, const VerifyData &vdata);
  virtual bool verifyPresence() = 0;

protected:
  virtual bool doRead(Device::Array &data, const VerifyData *vdata) = 0;
  virtual bool doWrite(const Device::Array &data) = 0;
};

//-----------------------------------------------------------------------------
class Mem24Hardware : public Hardware
{
public:
  Mem24Hardware(::Programmer::Base &base, Port::Base *port, const QString &name) : Hardware(base, port, name) {}
  const Mem24::Data &device() const { return static_cast<const Mem24::Data &>(*_base.device()); }
};

//-----------------------------------------------------------------------------
class Mem24Base : public Base
{
public:
  Mem24Base(const Group &group, const Mem24::Data *data, const char *name) : Base(group, data, name) {}
  const Mem24::Data *device() const { return static_cast<const Mem24::Data *>(_device); }

protected:
  Mem24DeviceSpecific *specific() const { return static_cast<Mem24DeviceSpecific *>(_specific); }
  virtual bool verifyDeviceId();
  virtual uint nbSteps(Operation operation) const;
  virtual bool initProgramming() { return true; }
  virtual bool checkErase() { return true; }
  virtual bool internalErase(const Device::MemoryRange &range);
  virtual bool checkRead() { return true; }
  virtual bool internalRead(Device::Memory *memory, const Device::MemoryRange &range, const VerifyData *vdata);
  virtual bool checkProgram(const Device::Memory &) { return true; }
  virtual bool internalProgram(const Device::Memory &memory, const Device::MemoryRange &range);
};

} // namespace

#endif
