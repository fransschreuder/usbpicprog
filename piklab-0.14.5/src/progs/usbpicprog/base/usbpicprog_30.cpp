/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "usbpicprog_30.h"

const uint BULK_ERASE_SEQUENCE[] = { // for all dsPICs
  0x000000, 0x000000, 0x040100, 0x000000, // step 1
  Usbpicprog::Pic30::LoopStart + 2,
    0x24008A, 0x883B0A,                     // step 2
    0x200F80, 0x880190, 0x200067,           // step 3
    0xBB0300,                               // step 4
    0xBB1B86,                               // step 5
    0x200558, 0x200AA9, 0x883B38, 0x883B39, // step 6
    0xA8E761, 0x000000, 0x000000,           // step 7
    Usbpicprog::Pic30::Pause + 2000000,
    0xA9E761, 0x000000, 0x000000,
  Usbpicprog::Pic30::LoopEnd,
  0x2407FA, 0x883B0A,                     // step 9
  0x200558, 0x883B38, 0x200AA9, 0x883B39, // step 10
  0xA8E761, 0x000000, 0x000000,           // step 11
  Usbpicprog::Pic30::Pause + 2000000,
  0xA9E761, 0x000000, 0x000000,
  Usbpicprog::Pic30::End
};
const uint READ_APPLICATION_ID_SEQUENCE[] = {
  0x000000, 0x000000, 0x040100, 0x000000, // step 1
  0x200800, 0x880190, 0x205FE0, 0x207841, // step 2
  0xBA0890, 0x000000, 0x000000,
  Usbpicprog::Pic30::RegisterOut, 0x000000,   // step 3
  Usbpicprog::Pic30::End
};
const uint READ_CONFIG_SEQUENCE[] = {
  0x000000, 0x000000, 0x040100, 0x000000, // step 1
  0x200F80, 0x880190, 0xEB0300, 0xEB0380, // step 2
  Usbpicprog::Pic30::LoopStart + 7,
    0xBA0BB6, 0x000000, 0x000000, 0x883C20, 0x000000, // step 3
    Usbpicprog::Pic30::RegisterOut, 0x000000,   // step 4
    0x040100, 0x000000,                     // step 5
  Usbpicprog::Pic30::LoopEnd,
  Usbpicprog::Pic30::End
};
const uint WRITE_CONFIG_SEQUENCE[] = {
  0x000000, 0x000000, 0x040100, 0x000000, // step 1
  Usbpicprog::Pic30::FillAddressLow + 0x200007, // step 2
  0x24008A, 0x883B0A,                     // step 3
  0x200F80, 0x880190,                     // step 4
  Usbpicprog::Pic30::FillData + 0x200000,     // step 5
  //0xEB0300, ???
  0xBB1B96, 0x000000, 0x000000,           // step 8
  0x200558, 0x883B38, 0x200AA9, 0x883B39, // step 7
  0xA8E761, 0x000000, 0x000000,           // step 8
  Usbpicprog::Pic30::Pause + 2000000,
  0xA9E761, 0x000000, 0x000000,
  Usbpicprog::Pic30::End
};
const uint READ_DATA_SEQUENCE[] = {
  0x000000, 0x000000, 0x040100, 0x000000, // step 1
  0x2007F0, 0x880190, Usbpicprog::Pic30::FillAddressLow + 0x200006, // step 2
  0xEB0380,                               // step 3
  0xBA1BB6, 0x000000, 0x000000,
  0xBA1BB6, 0x000000, 0x000000,
  0xBA1BB6, 0x000000, 0x000000,
  0xBA1BB6, 0x000000, 0x000000,
  0x883C20, 0x000000, Usbpicprog::Pic30::RegisterOut, 0x000000, // step 4
  0x883C21, 0x000000, Usbpicprog::Pic30::RegisterOut, 0x000000,
  0x883C22, 0x000000, Usbpicprog::Pic30::RegisterOut, 0x000000,
  0x883C23, 0x000000, Usbpicprog::Pic30::RegisterOut, 0x000000,
  Usbpicprog::Pic30::End
};
const uint WRITE_DATA_SEQUENCE[] = {
  0x000000, 0x000000, 0x040100, 0x000000, // step 1
  0x24005A, 0x883B0A,                     // step 2
  0x2007F0, 0x880190, Usbpicprog::Pic30::FillAddressLow + 0x200007, // step 3
  Usbpicprog::Pic30::LoopStart + 4,
    Usbpicprog::Pic30::FillData + 0x200000,   // step 4
    Usbpicprog::Pic30::FillData + 0x200001,
    Usbpicprog::Pic30::FillData + 0x200002,
    Usbpicprog::Pic30::FillData + 0x200003,
    0xEB0300, 0x000000,                   // step 5
    0xBB1BB6, 0x000000, 0x000000,
    0xBB1BB6, 0x000000, 0x000000,
    0xBB1BB6, 0x000000, 0x000000,
    0xBB1BB6, 0x000000, 0x000000,
  Usbpicprog::Pic30::LoopEnd,
  0x200558, 0x883B38, 0x200AA9, 0x883B39, // step 7
  0xA8E761, 0x000000, 0x000000,           // step 8
  Usbpicprog::Pic30::Pause + 2000000,
  0xA9E761, 0x000000, 0x000000,
  Usbpicprog::Pic30::End
};
const uint READ_CODE_SEQUENCE[] = {
  0x000000, 0x000000, 0x040100, 0x000000, // step 1
  Usbpicprog::Pic30::FillAddressHigh + 0x200000,     // step 2
  0x880190, Usbpicprog::Pic30::FillAddressLow + 0x200006,
  0xEB0380,                               // step 3
  0xBA1B96, 0x000000, 0x000000, 0xBADBB6, 0x000000, 0x000000,
  0xBADBD6, 0x000000, 0x000000, 0xBA1BB6, 0x000000, 0x000000,
  0xBA1B96, 0x000000, 0x000000, 0xBADBB6, 0x000000, 0x000000,
  0xBADBD6, 0x000000, 0x000000, 0xBA0BB6, 0x000000, 0x000000,
  0x883C20, 0x000000, Usbpicprog::Pic30::RegisterOut, 0x000000, // step 4
  0x883C21, 0x000000, Usbpicprog::Pic30::RegisterOut, 0x000000,
  0x883C22, 0x000000, Usbpicprog::Pic30::RegisterOut, 0x000000,
  0x883C23, 0x000000, Usbpicprog::Pic30::RegisterOut, 0x000000,
  0x883C24, 0x000000, Usbpicprog::Pic30::RegisterOut, 0x000000,
  0x883C25, 0x000000, Usbpicprog::Pic30::RegisterOut, 0x000000,
  Usbpicprog::Pic30::End
};
const uint WRITE_CODE_SEQUENCE[] = {
  0x000000, 0x000000, 0x040100, 0x000000, // step 1
  0x24001A, 0x883B0A,                     // step 2
  Usbpicprog::Pic30::FillAddressHigh + 0x200000, // step 3
  0x880190, Usbpicprog::Pic30::FillAddressLow + 0x200007,
  Usbpicprog::Pic30::LoopStart + 8,
    Usbpicprog::Pic30::FillData + 0x200000,   // step 4
    Usbpicprog::Pic30::FillData + 0x200001,
    Usbpicprog::Pic30::FillData + 0x200002,
    Usbpicprog::Pic30::FillData + 0x200003,
    Usbpicprog::Pic30::FillData + 0x200004,
    Usbpicprog::Pic30::FillData + 0x200005,
    0xEB0300, 0x000000,                   // step 5
    0xBB0BB6, 0x000000, 0x000000, 0xBBDBB6, 0x000000, 0x000000,
    0xBBEBB6, 0x000000, 0x000000, 0xBB1BB6, 0x000000, 0x000000,
    0xBB0BB6, 0x000000, 0x000000, 0xBBDBB6, 0x000000, 0x000000,
    0xBBEBB6, 0x000000, 0x000000, 0xBB1BB6, 0x000000, 0x000000,
  Usbpicprog::Pic30::LoopEnd,
  0x200558, 0x883B38, 0x200AA9, 0x883B39, // step 7
  0xA8E761, 0x000000, 0x000000, 0x000000, 0x000000, // step 8
  Usbpicprog::Pic30::Pause + 2000000,
  0xA9E761, 0x000000, 0x000000, 0x000000, 0x000000,
  Usbpicprog::Pic30::End
};

bool Usbpicprog::Pic30::sendCommand(Command cmd)
{
  for (uint i=0; i<4; i++) {
    hardware().setPin(DataOut, ((cmd>>i) & 1 ? High : Low));
    hardware().setPin(Clock, High);
    Port::nsleep(100);
    hardware().setPin(Clock, Low);
    Port::nsleep(100);
  }
  return true;
}

bool Usbpicprog::Pic30::executeSIX(uint opcode)
{
  if ( !sendCommand(SIX) ) return false;
  for (uint i=0; i<24; i++) {
    hardware().setPin(DataOut, ((opcode>>i) & 1 ? High : Low));
    hardware().setPin(Clock, High);
    Port::nsleep(100);
    hardware().setPin(Clock, Low);
    // seem to need this longer delay here -- maybe
    // specific to my programmer hardware? #### from dspicprg
    Port::nsleep(5000);
  }
  return true;
}

bool Usbpicprog::Pic30::executeREGOUT(uint &v)
{
  v = 0;
  if ( !sendCommand(REGOUT) ) return false;
  hardware().setPin(DataOut, High);
  for (uint i=0; i<24; i++) {
    hardware().setPin(Clock, High);
    Port::nsleep(100);
    hardware().setPin(Clock, Low);
    // as above, need extra delay here, but even
    // longer in this case -- maybe data line takes
    // longer to settle when it is being driven by
    // the PIC than by the programmer? #### from dspicprg
    Port::nsleep(10000);
    if ( i>=8 ) {
      uint r = hardware().readBit();
      v |= (r << (i-8));
    }
  }
  return true;
}

bool Usbpicprog::Pic30::doStdpSequence(const uint *sequence, uint address, const char *outData, char *inData)
{
  uint loopStart = 0, loopCound = 0;
  for (uint i=0; sequence[i]!=End; i++) {
    uint opcode = (sequence[i] & OpcodeMask);
    switch (Operation(sequence[i] & OperationMask)) {
    case SendSix:
      if ( !executeSIX(opcode) ) return false;
      break;
    case RegisterOut: {
      uint v;
      if ( !executeREGOUT(v) ) return false;
      (*inData) = v & 0xFF;
      inData++;
      (*inData) = (v >> 8) & 0xFF;
      inData++;
      break;
    }
    case Pause:
      Port::nsleep(opcode);
      break;
    case LoopStart:
      loopCound = opcode;
      loopStart = i;
      break;
    case LoopEnd:
      loopCound--;
      if (loopCound) i = loopStart;
      break;
    case FillData: {
      uint v = *outData;
      v <<= 4;
      outData++;
      v |= *outData;
      outData++;
      v <<= 4;
      executeSIX(v | opcode);
      break;
    }
    case FillAddressLow: {
      uint v = address & 0xFFFF;
      v <<= 4;
      executeSIX(v | opcode);
      break;
    }
    case FillAddressHigh: {
      uint v = (address & 0xFF0000) >> 16;
      v <<= 4;
      executeSIX(v | opcode);
      break;
    }
    case End: Q_ASSERT(false); break;
    }
  }
  return true;
}

bool Usbpicprog::Pic30::doErase(bool)
{
  if ( !enterStdpMode() ) return false;
  if ( !doStdpSequence(BULK_ERASE_SEQUENCE, 0, 0, 0) ) return false;
  return exitStdpMode();
}

bool Usbpicprog::Pic30::doRead(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *data)
{
  // #### TODO
  return false;
}

bool Usbpicprog::Pic30::doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force)
{
  // #### TODO
  return false;
}
