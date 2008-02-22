/***************************************************************************\
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DIRECT_MEM24_H
#define DIRECT_MEM24_H

#include "devices/mem24/prog/mem24_prog.h"
#include "direct.h"

namespace Direct
{

class Mem24DeviceSpecific : public ::Programmer::Mem24DeviceSpecific
{
public:
  Mem24DeviceSpecific(::Programmer::Base &base);
  virtual Hardware &hardware() { return static_cast<Hardware &>(*_base.hardware()); }
  const Mem24::Data &device() const { return static_cast<const Mem24::Data &>(*::Programmer::DeviceSpecific::_base.device()); }
  virtual bool init() { return true; }
  virtual bool setPowerOff();
  virtual bool setPowerOn();
  virtual bool setTargetPowerOn(bool) { return true; }
  virtual bool verifyPresence();

private:
  virtual bool doRead(Device::Array &data, const ::Programmer::VerifyData *vdata);
  virtual bool doWrite(const Device::Array &data);
  bool start();
  bool stop();
  BitValue readByte(State ack);
  bool writeByte(BitValue value, bool &acked);
  bool writeByteAck(BitValue value);
  void set(State clock, State data);
  void setData(State data);
  enum Operation { Write, Read };
  uint controlByte(uint address, Operation operation) const;
  bool setAddress(uint address);
};

} // namespace

#endif
