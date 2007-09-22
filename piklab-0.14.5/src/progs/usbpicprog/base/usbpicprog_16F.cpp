/***************************************************************************
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2004 Keith Baker <syzygy@pubnix.org>  [16F7X]           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "usbpicprog_16F.h"

#include "common/global/global.h"

//-----------------------------------------------------------------------------
bool Usbpicprog::P16F::gotoTestMemory()
{
  pulseEngine("k0,S,", 0x3FFF); // PC set at 0x2000
  return true;
}

bool Usbpicprog::P16F::gotoMemory(Pic::MemoryRangeType type)
{
  switch (type) {
    case Pic::MemoryCode: return true;
    case Pic::MemoryEeprom: return true;
    case Pic::MemoryUserId: return gotoTestMemory();
    case Pic::MemoryDeviceId: return (gotoTestMemory() && incrementPC(6));
    case Pic::MemoryConfig: return (gotoTestMemory() && incrementPC(7));
    case Pic::MemoryCal:
      if ( device().range(type).start==device().range(Pic::MemoryCode).end+1 )
        return incrementPC(device().range(type).start);
      return (gotoTestMemory() && incrementPC(8));
    case Pic::MemoryCalBackup:
    case Pic::MemoryDebugVector:
    case Pic::MemoryHardwareStack:
    case Pic::MemoryProgramExecutive:
    case Pic::Nb_MemoryRangeTypes: break;
  }
  Q_ASSERT(false);
  return false;
}

//-----------------------------------------------------------------------------
bool Usbpicprog::P16F8X::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryCode ) {
    pulseEngine("k2,S,k1,k7,k8,w10000,k1,k7", 0x3FFF); // bulk erase code
    return true;
  }
  if ( type==Pic::MemoryEeprom ) {
    pulseEngine("k3,S,k1,k7,k8,w10000,k1,k7", 0x3FFF); // bulk erase data
    return true;
  }
  return false;
}

bool Usbpicprog::P16F8X::doErase(bool isProtected)
{
  if (isProtected) { // disable code protection + erase code and data
    gotoMemory(Pic::MemoryConfig);
    pulseEngine("k1,k7,k8,w10000,k1,k7");
  } else {
    doEraseRange(Pic::MemoryCode);
    doEraseRange(Pic::MemoryEeprom);
  }
  doEmulatedEraseRange(Pic::MemoryUserId);
  doEmulatedEraseRange(Pic::MemoryConfig);
  return true;
}

//-----------------------------------------------------------------------------
uint Usbpicprog::P16F84A::waitProgTime(Pic::MemoryRangeType type) const
{
  if ( type==Pic::MemoryConfig || type==Pic::MemoryUserId ) return 4000 + 4000; // prog +erase
  return 4000;
}

void Usbpicprog::P16F84A::startProg(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryCode || type==Pic::MemoryEeprom ) pulseEngine("k24,");
  else pulseEngine("k8,");
}

bool Usbpicprog::P16F84A::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryCode ) {
    pulseEngine("k2,S,k9,k8,w10000", 0x3FFF);  // bulk erase code
    return true;
  }
  if ( type==Pic::MemoryEeprom ) {
    pulseEngine("k3,S,k11,k8,w10000", 0x3FFF); // bulk erase data
    return true;
  }
  return false;
}

bool Usbpicprog::P16F84A::doErase(bool isProtected)
{
  if (isProtected) { // disable code protection and erase
    gotoMemory(Pic::MemoryConfig);
    pulseEngine("k1,k7,k8,w10000,k1,k7");
  } else {
    doEraseRange(Pic::MemoryCode);
    doEraseRange(Pic::MemoryEeprom);
  }
  doEmulatedEraseRange(Pic::MemoryUserId);
  doEmulatedEraseRange(Pic::MemoryConfig);
  return true;
}

//-----------------------------------------------------------------------------
uint Usbpicprog::P16F7X::waitProgTime(Pic::MemoryRangeType) const
{
  if ( device().name()=="16F72" ) return 3000;
  return 1000;
}

bool Usbpicprog::P16F7X::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type!=Pic::MemoryCode ) return false;
  Device::Array array;
  if ( !doRead(Pic::MemoryConfig, array, 0) ) return false;
  pulseEngine("k9w400000", 0x3FFF); // chip erase (should only need 30ms according to prog sheet)
  return doWrite(Pic::MemoryConfig, array, true);
}

bool Usbpicprog::P16F7X::doErase(bool)
{
  pulseEngine("k9w400000", 0x3FFF); // chip erase (should only need 30ms according to prog sheet)
  return doEmulatedEraseRange(Pic::MemoryUserId);
}

//-----------------------------------------------------------------------------
// 16F628 seems to have problems with the standard 16F84 bulk
// erase when disabling code protection : the data memory is not set to 0xFF.
// This code adds a erase/programming pass on the data memory
bool Usbpicprog::P16F62X::doErase(bool isProtected)
{
  P16F84A::doErase(isProtected);
  if (isProtected) return doEmulatedEraseRange(Pic::MemoryEeprom);
  return true;
}

//-----------------------------------------------------------------------------
bool Usbpicprog::P16F81X::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryCode ) {
    pulseEngine("k9w2000"); // bulk erase code: 2ms
    return true;
  }
  if ( type==Pic::MemoryEeprom ) {
    pulseEngine("k11w2000"); // bulk erase data: 2ms
    return true;
  }
  return false;
}

bool Usbpicprog::P16F81X::doErase(bool)
{
  pulseEngine("k31w8000"); // chip erase: 8ms
  return true;
}

//-----------------------------------------------------------------------------
uint Usbpicprog::P12F675::waitProgTime(Pic::MemoryRangeType type) const
{
  if ( type==Pic::MemoryEeprom ) return 6000;
  return 2500;
}

bool Usbpicprog::P12F675::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryCode ) {
    pulseEngine("k9,w9000");  // bulk erase code
    return true;
  }
  if ( type==Pic::MemoryEeprom ) {
    pulseEngine("k11,w9000"); // bulk erase data
    return true;
  }
  return false;
}

bool Usbpicprog::P12F675::doErase(bool)
{
  pulseEngine("k0,S,", 0x3FFF);
  doEraseRange(Pic::MemoryCode);
  doEraseRange(Pic::MemoryEeprom);
  return true;
}

//-----------------------------------------------------------------------------
uint Usbpicprog::P16F62XA::waitProgTime(Pic::MemoryRangeType type) const
{
  if ( type==Pic::MemoryEeprom ) return 6000;
  return 2500;
}

bool Usbpicprog::P16F62XA::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryCode ) {
    pulseEngine("k9,w12000");  // bulk erase code
    return true;
  }
  if ( type==Pic::MemoryEeprom ) {
    pulseEngine("k11,w12000"); // bulk erase data
    return true;
  }
  return false;
}

bool Usbpicprog::P16F62XA::doErase(bool)
{
  pulseEngine("k0,S,w100", 0x3FFF);
  doEraseRange(Pic::MemoryCode);
  doEraseRange(Pic::MemoryEeprom);
  return true;
}

//-----------------------------------------------------------------------------
bool Usbpicprog::P16F87XA::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryCode ) {
    pulseEngine("k9,w8000");  // bulk erase code
    return true;
  }
  if ( type==Pic::MemoryEeprom ) {
    pulseEngine("k11,w8000"); // bulk erase data
    return true;
  }
  return false;
}

bool Usbpicprog::P16F87XA::doErase(bool)
{
  // I use the command 31 twice because, sometimes, the prog
  // memory is not totally erased. Not very elegant, but it works.
  pulseEngine("k0,S,k31w8000,k31w8000", 0x3FFF);
  return doEmulatedEraseRange(Pic::MemoryUserId);
}

//-----------------------------------------------------------------------------
bool Usbpicprog::P16F913::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryCode ) {
    pulseEngine("k9,w6000");  // bulk erase code
    return true;
  }
  if ( type==Pic::MemoryEeprom ) {
    pulseEngine("k11,w6000"); // bulk erase data
    return true;
  }
  return false;
}

bool Usbpicprog::P16F913::doErase(bool)
{
  // flow chart of figure 3.21 of prog sheet
  doEraseRange(Pic::MemoryCode);
  pulseEngine("k0,S,", 0x3FFF);
  doEraseRange(Pic::MemoryCode);
  doEraseRange(Pic::MemoryEeprom);
  return true;
}

//-----------------------------------------------------------------------------
bool Usbpicprog::P16F785::doErase(bool)
{
  // flow chart of figure 3.20 of prog sheet
  pulseEngine("k0,S,", 0x3FFF);
  doEraseRange(Pic::MemoryCode);
  doEraseRange(Pic::MemoryEeprom);
  return true;
}
