/***************************************************************************
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DIRECT_PIC_H
#define DIRECT_PIC_H

#include "devices/pic/pic/pic_memory.h"
#include "direct.h"

namespace Direct
{
//-----------------------------------------------------------------------------
class PulseEngine
{
public:
  PulseEngine(::Programmer::Base &base);
  virtual ~PulseEngine() {}
  BitValue pulseEngine(const QString &command, BitValue value = 0);

protected:
  ::Programmer::Base &_pbase;
  uint _clockDelay; // additionnal delay for buggy hardware

  virtual bool pulse(const char *&cmd, BitValue value, BitValue &res);
  virtual Hardware &hardware() { return static_cast<Hardware &>(*_pbase.hardware()); }
};

//-----------------------------------------------------------------------------
class PicDeviceSpecific : public ::Programmer::PicDeviceSpecific, public PulseEngine
{
public:
  PicDeviceSpecific(::Programmer::Base &base);
  virtual bool canEraseAll() const { return true; }
  virtual bool canEraseRange(Pic::MemoryRangeType type) const { return ( type==Pic::MemoryRangeType::Code || type==Pic::MemoryRangeType::Eeprom ); }
  virtual bool canReadRange(Pic::MemoryRangeType) const { return true; }
  virtual bool canWriteRange(Pic::MemoryRangeType) const { return true; }
  virtual bool setPowerOff();
  virtual bool setTargetPowerOn(bool on);

protected:
  virtual bool pulse(const char *&cmd, BitValue value, BitValue &res);
  bool setPowerOnVddFirst();
  bool setPowerOnVppFirst();
};

//-----------------------------------------------------------------------------
class Pic8DeviceSpecific : public PicDeviceSpecific
{
public:
  Pic8DeviceSpecific(::Programmer::Base &base) : PicDeviceSpecific(base) {}

protected:
  virtual bool pulse(const char *&cmd, BitValue value, BitValue &res);
  virtual void send_word(BitValue word) = 0;
  virtual void send_bits(BitValue d, uint nbBits) = 0;
  virtual void send_cmd(BitValue d) = 0;
  virtual BitValue get_word() = 0;
  virtual BitValue get_byte() = 0;
};

} // namespace

#endif
