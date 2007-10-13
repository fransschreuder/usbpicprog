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
#include "direct_16F.h"

#include "common/global/global.h"

//-----------------------------------------------------------------------------
bool Direct::P16F::gotoTestMemory()
{
  pulseEngine("k0,S,", 0x3FFF); // PC set at 0x2000
  return true;
}

bool Direct::P16F::gotoMemory(Pic::MemoryRangeType type)
{
  switch (type) {
    case Pic::MemoryRangeType::Code: return true;
    case Pic::MemoryRangeType::Eeprom: return true;
    case Pic::MemoryRangeType::UserId: return gotoTestMemory();
    case Pic::MemoryRangeType::DeviceId: return (gotoTestMemory() && incrementPC(6));
    case Pic::MemoryRangeType::Config: return (gotoTestMemory() && incrementPC(7));
    case Pic::MemoryRangeType::Cal:
      if ( device().range(type).start==device().range(Pic::MemoryRangeType::Code).end+1 )
        return incrementPC(device().range(type).start.toUInt());
      return (gotoTestMemory() && incrementPC(8));
    case Pic::MemoryRangeType::CalBackup:
    case Pic::MemoryRangeType::DebugVector:
    case Pic::MemoryRangeType::HardwareStack:
    case Pic::MemoryRangeType::ProgramExecutive:
    case Pic::MemoryRangeType::Nb_Types: break;
  }
  Q_ASSERT(false);
  return false;
}

//-----------------------------------------------------------------------------
bool Direct::P16F8X::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryRangeType::Code ) {
    pulseEngine("k2,S,k1,k7,k8,w10000,k1,k7", 0x3FFF); // bulk erase code
    return true;
  }
  if ( type==Pic::MemoryRangeType::Eeprom ) {
    pulseEngine("k3,S,k1,k7,k8,w10000,k1,k7", 0x3FFF); // bulk erase data
    return true;
  }
  return false;
}

bool Direct::P16F8X::doErase(bool isProtected)
{
  if (isProtected) { // disable code protection + erase code and data
    gotoMemory(Pic::MemoryRangeType::Config);
    pulseEngine("k1,k7,k8,w10000,k1,k7");
  } else {
    doEraseRange(Pic::MemoryRangeType::Code);
    doEraseRange(Pic::MemoryRangeType::Eeprom);
  }
  doEmulatedEraseRange(Pic::MemoryRangeType::UserId);
  doEmulatedEraseRange(Pic::MemoryRangeType::Config);
  return true;
}

//-----------------------------------------------------------------------------
uint Direct::P16F84A::waitProgTime(Pic::MemoryRangeType type) const
{
  if ( type==Pic::MemoryRangeType::Config || type==Pic::MemoryRangeType::UserId ) return 4000 + 4000; // prog +erase
  return 4000;
}

void Direct::P16F84A::startProg(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryRangeType::Code || type==Pic::MemoryRangeType::Eeprom ) pulseEngine("k24,");
  else pulseEngine("k8,");
}

bool Direct::P16F84A::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryRangeType::Code ) {
    pulseEngine("k2,S,k9,k8,w10000", 0x3FFF);  // bulk erase code
    return true;
  }
  if ( type==Pic::MemoryRangeType::Eeprom ) {
    pulseEngine("k3,S,k11,k8,w10000", 0x3FFF); // bulk erase data
    return true;
  }
  return false;
}

bool Direct::P16F84A::doErase(bool isProtected)
{
  if (isProtected) { // disable code protection and erase
    gotoMemory(Pic::MemoryRangeType::Config);
    pulseEngine("k1,k7,k8,w10000,k1,k7");
  } else {
    doEraseRange(Pic::MemoryRangeType::Code);
    doEraseRange(Pic::MemoryRangeType::Eeprom);
  }
  doEmulatedEraseRange(Pic::MemoryRangeType::UserId);
  doEmulatedEraseRange(Pic::MemoryRangeType::Config);
  return true;
}

//-----------------------------------------------------------------------------
uint Direct::P16F7X::waitProgTime(Pic::MemoryRangeType) const
{
  if ( device().name()=="16F72" ) return 3000;
  return 1000;
}

bool Direct::P16F7X::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type!=Pic::MemoryRangeType::Code ) return false;
  Device::Array array;
  if ( !doRead(Pic::MemoryRangeType::Config, array, 0) ) return false;
  pulseEngine("k9w400000", 0x3FFF); // chip erase (should only need 30ms according to prog sheet)
  return doWrite(Pic::MemoryRangeType::Config, array, true);
}

bool Direct::P16F7X::doErase(bool)
{
  pulseEngine("k9w400000", 0x3FFF); // chip erase (should only need 30ms according to prog sheet)
  return doEmulatedEraseRange(Pic::MemoryRangeType::UserId);
}

//-----------------------------------------------------------------------------
// 16F628 seems to have problems with the standard 16F84 bulk
// erase when disabling code protection : the data memory is not set to 0xFF.
// This code adds a erase/programming pass on the data memory
bool Direct::P16F62X::doErase(bool isProtected)
{
  P16F84A::doErase(isProtected);
  if (isProtected) return doEmulatedEraseRange(Pic::MemoryRangeType::Eeprom);
  return true;
}

//-----------------------------------------------------------------------------
bool Direct::P16F81X::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryRangeType::Code ) {
    pulseEngine("k9w2000"); // bulk erase code: 2ms
    return true;
  }
  if ( type==Pic::MemoryRangeType::Eeprom ) {
    pulseEngine("k11w2000"); // bulk erase data: 2ms
    return true;
  }
  return false;
}

bool Direct::P16F81X::doErase(bool)
{
  pulseEngine("k31w8000"); // chip erase: 8ms
  return true;
}

//-----------------------------------------------------------------------------
uint Direct::P12F675::waitProgTime(Pic::MemoryRangeType type) const
{
  if ( type==Pic::MemoryRangeType::Eeprom ) return 6000;
  return 2500;
}

bool Direct::P12F675::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryRangeType::Code ) {
    pulseEngine("k9,w9000");  // bulk erase code
    return true;
  }
  if ( type==Pic::MemoryRangeType::Eeprom ) {
    pulseEngine("k11,w9000"); // bulk erase data
    return true;
  }
  return false;
}

bool Direct::P12F675::doErase(bool)
{
  pulseEngine("k0,S,", 0x3FFF);
  doEraseRange(Pic::MemoryRangeType::Code);
  doEraseRange(Pic::MemoryRangeType::Eeprom);
  return true;
}

//-----------------------------------------------------------------------------
uint Direct::P16F62XA::waitProgTime(Pic::MemoryRangeType type) const
{
  if ( type==Pic::MemoryRangeType::Eeprom ) return 6000;
  return 2500;
}

bool Direct::P16F62XA::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryRangeType::Code ) {
    pulseEngine("k9,w12000");  // bulk erase code
    return true;
  }
  if ( type==Pic::MemoryRangeType::Eeprom ) {
    pulseEngine("k11,w12000"); // bulk erase data
    return true;
  }
  return false;
}

bool Direct::P16F62XA::doErase(bool)
{
  pulseEngine("k0,S,w100", 0x3FFF);
  doEraseRange(Pic::MemoryRangeType::Code);
  doEraseRange(Pic::MemoryRangeType::Eeprom);
  return true;
}

//-----------------------------------------------------------------------------
bool Direct::P16F87XA::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryRangeType::Code ) {
    pulseEngine("k9,w8000");  // bulk erase code
    return true;
  }
  if ( type==Pic::MemoryRangeType::Eeprom ) {
    pulseEngine("k11,w8000"); // bulk erase data
    return true;
  }
  return false;
}

bool Direct::P16F87XA::doErase(bool)
{
  // I use the command 31 twice because, sometimes, the prog
  // memory is not totally erased. Not very elegant, but it works.
  pulseEngine("k0,S,k31w8000,k31w8000", 0x3FFF);
  return doEmulatedEraseRange(Pic::MemoryRangeType::UserId);
}

//-----------------------------------------------------------------------------
bool Direct::P16F913::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryRangeType::Code ) {
    pulseEngine("k9,w6000");  // bulk erase code
    return true;
  }
  if ( type==Pic::MemoryRangeType::Eeprom ) {
    pulseEngine("k11,w6000"); // bulk erase data
    return true;
  }
  return false;
}

bool Direct::P16F913::doErase(bool)
{
  // flow chart of figure 3.21 of prog sheet
  doEraseRange(Pic::MemoryRangeType::Code);
  pulseEngine("k0,S,", 0x3FFF);
  doEraseRange(Pic::MemoryRangeType::Code);
  doEraseRange(Pic::MemoryRangeType::Eeprom);
  return true;
}

//-----------------------------------------------------------------------------
bool Direct::P16F785::doErase(bool)
{
  // flow chart of figure 3.20 of prog sheet
  pulseEngine("k0,S,", 0x3FFF);
  doEraseRange(Pic::MemoryRangeType::Code);
  doEraseRange(Pic::MemoryRangeType::Eeprom);
  return true;
}
