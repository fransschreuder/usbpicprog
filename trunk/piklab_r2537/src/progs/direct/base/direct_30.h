/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DIRECT_30_H
#define DIRECT_30_H

#include "direct_pic.h"

namespace Direct
{
//----------------------------------------------------------------------------
class Pic30 : public PicDeviceSpecific
{
public:
  Pic30(::Programmer::Base &base) : PicDeviceSpecific(base) {}
  virtual bool doErase(bool isProtected);
  virtual bool doRead(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *data);
  virtual bool doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force);
  virtual bool setPowerOn() { return setPowerOnVddFirst(); }

public:
  enum Operation { SendSix        = 0x0000000, RegisterOut     = 0x1000000, Pause    = 0x2000000,
                   LoopStart      = 0x3000000, LoopEnd         = 0x4000000, FillData = 0x5000000,
                   FillAddressLow = 0x6000000, FillAddressHigh = 0x7000000, End      = 0x8000000 };
  enum Mask { OperationMask = 0xF000000, OpcodeMask    = 0x0FFFFFF };

private:
  bool enterStdpMode() { return pulseEngine("cdB"); }
  bool exitStdpMode() { return pulseEngine("cdb"); }
  bool doStdpSequence(const uint *sequence, uint address, const char *outData, char *inData);
  enum Command { SIX = 0x0, REGOUT = 0x1 };
  bool sendCommand(Command command);
  bool executeSIX(uint opcode);
  bool executeREGOUT(uint &value);
};

} // namespace

#endif
