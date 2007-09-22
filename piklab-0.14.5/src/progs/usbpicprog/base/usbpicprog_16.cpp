/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "usbpicprog_16.h"

#include "common/global/global.h"

//----------------------------------------------------------------------------
void Usbpicprog::pic16::send_bits(uint d, uint nbb)
{
    hardware().setWrite();
    for (uint x = nbb; x; --x) {
        hardware().setPin(Clock, High);
        if ( d & 0x01 ) hardware().setPin(DataOut, High);
        else hardware().setPin(DataOut, Low);
        Port::usleep(1+_clockDelay);
        hardware().setPin(Clock, Low);
        Port::usleep(1+_clockDelay);
        d >>= 1;
    }
    hardware().setPin(DataOut, High);
    hardware().setRead();
}

void Usbpicprog::pic16::send_word(uint d)
{
    hardware().setWrite();
    d <<= 1;      // insert start bit
    for (uint x = 0; x<16; x++) {
        hardware().setPin(Clock, High);
        if ( d & 0x01 ) hardware().setPin(DataOut, High);
        else hardware().setPin(DataOut, Low);
        Port::usleep(1+_clockDelay) ;  // needed for slow programmers/fast PCs
        hardware().setPin(Clock, Low);
        Port::usleep(1+_clockDelay) ;
        d >>= 1;                     // Move the data over 1 bit
    }
    hardware().setPin(DataOut, High);  // Added for ITU-1 support
    hardware().setRead();
}

// Read 14 bits of data from the PIC
// clock idles low, change data. 1 start bit, 1 stop bit, data valid on falling edge.
uint Usbpicprog::pic16::get_word()
{
    hardware().setRead();
    uint ind = 0;
    hardware().setPin(DataOut, High);
    for (uint x = 16; x ; --x) {
        hardware().setPin(Clock, High);
        Port::usleep(1+_clockDelay);
        if ( hardware().readBit() ) ind |= 0x8000;
        else ind &= 0x7FFF;
        ind >>= 1;
        hardware().setPin(Clock, Low);
        Port::usleep(1+_clockDelay);
    }
    return ind;
}

bool Usbpicprog::pic16::incrementPC(uint steps)
{
  for (uint i=0; i<steps; i++) pulseEngine("k6,");
  return true;
}

uint Usbpicprog::pic16::readWord(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryEeprom ) return pulseEngine("k5,r,");
  return pulseEngine("k4,R,");
}

bool Usbpicprog::pic16::doRead(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata)
{
  data.resize(device().nbWords(type));
  gotoMemory(type);
  uint nbWords = data.count();
  bool only = false;
  if (vdata) {
    bool only = vdata->actions & ::Programmer::OnlyProgrammedVerify;
    const Device::Array wdata = static_cast<const Pic::Memory &>(vdata->memory).arrayForWriting(type);
    if (only) nbWords = findNonMaskEnd(type, wdata)+1;
  }
  uint mask = device().mask(type);
  for (uint i = 0; i<nbWords; i++) {
    if ( !only || data[i]!=mask || type!=Pic::MemoryCode ) {
      data[i] = readWord(type) & mask;
      if ( vdata && !hardware().verifyWord(i, data[i], type, *vdata) ) return false;
    }
    incrementPC(1);
  }
  if ( type==Pic::MemoryCode || type==Pic::MemoryEeprom ) addProgress(data.count());
  return true;
}

bool Usbpicprog::pic16::doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force)
{
  gotoMemory(type);
  for (uint i = 0; i<data.count(); ) {
    if ( !writeWords(type, data, i, force) ) {
      log(Log::Error, i18n("Error programming %1 at %2.").arg(i18n(Pic::MEMORY_RANGE_TYPE_DATA[type].label)).arg(toHexLabel(i, 8)));
      return false;
    }
  }
  return true;
}

bool Usbpicprog::pic16::skipMaskWords(Pic::MemoryRangeType type, const Device::Array &data, uint &i, bool force)
{
  if (force) return false;
  uint nb = (type==Pic::MemoryCode ? nbWordsCodeProg() : 1);
  for (uint k=0; k<nb; k++)
    if ( data[i+k]!=device().mask(type) ) return false;
  incrementPC(nb);
  i += nb;
  return true;
}

bool Usbpicprog::pic16::writeWords(Pic::MemoryRangeType type, const Device::Array &data, uint i)
{
  if ( type==Pic::MemoryEeprom) pulseEngine("k3,S,", data[i]);
  else {
    uint nb = (type==Pic::MemoryCode ? nbWordsCodeProg() : 1);
    for (uint k=0; k<nb; k++) {
      if ( k!=0 ) incrementPC(1);
      pulseEngine("k2,S,", data[i+k]);
    }
  }
  startProg(type);
  QString cmd = QString("w%1").arg(waitProgTime(type));
  pulseEngine(cmd.latin1());
  endProg(type);
  return true;
}

bool Usbpicprog::pic16::writeWords(Pic::MemoryRangeType type, const Device::Array &data, uint &i, bool force)
{
  if ( skipMaskWords(type, data, i, force) ) return true;
  if ( !writeWords(type, data, i) ) return false;
  incrementPC(1);
  i += (type==Pic::MemoryCode ? nbWordsCodeProg() : 1);
  return true;
}
