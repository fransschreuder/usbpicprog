/***************************************************************************
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "direct_baseline.h"

//----------------------------------------------------------------------------
bool Direct::Baseline::gotoMemory(Pic::MemoryRangeType type)
{
  switch (type) {
    case Pic::MemoryRangeType::Config: return true;
    case Pic::MemoryRangeType::Eeprom: return true;
    case Pic::MemoryRangeType::Code:
    case Pic::MemoryRangeType::Cal:
    case Pic::MemoryRangeType::UserId:
    case Pic::MemoryRangeType::CalBackup: return incrementPC(device().range(type).start.toUInt()+1);
    case Pic::MemoryRangeType::DeviceId:
    case Pic::MemoryRangeType::DebugVector:
    case Pic::MemoryRangeType::HardwareStack:
    case Pic::MemoryRangeType::ProgramExecutive:
    case Pic::MemoryRangeType::Nb_Types: break;
  }
  Q_ASSERT(false);
  return false;
}

//-----------------------------------------------------------------------------
bool Direct::P12C5XX::writeWords(Pic::MemoryRangeType type, const Device::Array &data, uint i)
{
  BitValue word = data[i];
  // config requires a total of 10ms Pulses == 100 x 100 us
  uint total = (type==Pic::MemoryRangeType::Config ? 100 : 8);
  uint n = 0;
  for (; n<total; n++) {
    pulseEngine("k2,S,k8,w100,k14,w5000", word);
    if ( readWord(type)==word ) break;
  }
  if ( n==total ) return false;
  if ( type!=Pic::MemoryRangeType::Config ) {
    // 11 x (n+1) additionnal passes
    for (uint k = 0; k<11*(n+1); k++)
      pulseEngine("k2,S,k8,w100,k14,w5000", word);
  }
  return true;
}

//-----------------------------------------------------------------------------
bool Direct::P10F2XX::doEraseRange(Pic::MemoryRangeType type)
{
  Q_ASSERT( type==Pic::MemoryRangeType::Code );
  Device::Array array;
  if ( !doRead(Pic::MemoryRangeType::Config, array, 0) ) return false;
  gotoMemory(Pic::MemoryRangeType::Config);
  pulseEngine("k9,w10000"); // 10ms
  return doWrite(Pic::MemoryRangeType::Config, array, true);
}

bool Direct::P10F2XX::doErase(bool)
{
  gotoMemory(Pic::MemoryRangeType::UserId);
  pulseEngine("k9,w10000"); // 10ms
  return true;
}

//-----------------------------------------------------------------------------
bool Direct::P12C67X::writeWords(Pic::MemoryRangeType type, const Device::Array &data, uint i)
{
  BitValue word = data[i];
  // config requires a total of 10ms Pulses == 100 x 100 us
  uint total = (type==Pic::MemoryRangeType::Config ? 100 : 25);
  uint n = 0;
  for (; n<total; n++) {
    pulseEngine("k2,S,k8,w100,k14,w5000", word);
    if ( readWord(type)==word ) break;
  }
  if ( n==total ) return false;
  if ( type!=Pic::MemoryRangeType::Config ) {
    // 3 x (n+1) additionnal passes
    for (uint k = 0; k<3*(n+1); k++)
      pulseEngine("k2,S,k8,w100,k14,w5000", word);
  }
  return true;
}
