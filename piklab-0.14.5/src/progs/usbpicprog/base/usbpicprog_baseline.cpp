/***************************************************************************
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "usbpicprog_baseline.h"

//----------------------------------------------------------------------------
bool Usbpicprog::Baseline::gotoMemory(Pic::MemoryRangeType type)
{
  switch (type) {
    case Pic::MemoryConfig: return true;
    case Pic::MemoryEeprom: return true;
    case Pic::MemoryCode:
    case Pic::MemoryCal:
    case Pic::MemoryUserId:
    case Pic::MemoryCalBackup: return incrementPC(1+device().range(type).start);
    case Pic::MemoryDeviceId:
    case Pic::MemoryDebugVector:
    case Pic::MemoryHardwareStack:
    case Pic::MemoryProgramExecutive:
    case Pic::Nb_MemoryRangeTypes: break;
  }
  Q_ASSERT(false);
  return false;
}

//-----------------------------------------------------------------------------
bool Usbpicprog::P12C5XX::writeWords(Pic::MemoryRangeType type, const Device::Array &data, uint i)
{
  uint word = data[i];
  // config requires a total of 10ms Pulses == 100 x 100 us
  uint total = (type==Pic::MemoryConfig ? 100 : 8);
  uint n = 0;
  for (; n<total; n++) {
    pulseEngine("k2,S,k8,w100,k14,w5000", word);
    if ( readWord(type)==word ) break;
  }
  if ( n==total ) return false;
  if ( type!=Pic::MemoryConfig ) {
    // 11 x (n+1) additionnal passes
    for (uint k = 0; k<11*(n+1); k++)
      pulseEngine("k2,S,k8,w100,k14,w5000", word);
  }
  return true;
}

//-----------------------------------------------------------------------------
bool Usbpicprog::P10F2XX::doEraseRange(Pic::MemoryRangeType type)
{
  Q_ASSERT( type==Pic::MemoryCode );
  Device::Array array;
  if ( !doRead(Pic::MemoryConfig, array, 0) ) return false;
  gotoMemory(Pic::MemoryConfig);
  pulseEngine("k9,w10000"); // 10ms
  return doWrite(Pic::MemoryConfig, array, true);
}

bool Usbpicprog::P10F2XX::doErase(bool)
{
  gotoMemory(Pic::MemoryUserId);
  pulseEngine("k9,w10000"); // 10ms
  return true;
}

//-----------------------------------------------------------------------------
bool Usbpicprog::P12C67X::writeWords(Pic::MemoryRangeType type, const Device::Array &data, uint i)
{
  uint word = data[i];
  // config requires a total of 10ms Pulses == 100 x 100 us
  uint total = (type==Pic::MemoryConfig ? 100 : 25);
  uint n = 0;
  for (; n<total; n++) {
    pulseEngine("k2,S,k8,w100,k14,w5000", word);
    if ( readWord(type)==word ) break;
  }
  if ( n==total ) return false;
  if ( type!=Pic::MemoryConfig ) {
    // 3 x (n+1) additionnal passes
    for (uint k = 0; k<3*(n+1); k++)
      pulseEngine("k2,S,k8,w100,k14,w5000", word);
  }
  return true;
}
