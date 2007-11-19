/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "direct_16.h"

#include "common/global/global.h"

//----------------------------------------------------------------------------
void Direct::pic16::send_bits(BitValue d, uint nbb)
{
    hardware().setWrite();
    for (uint x = nbb; x; --x) {
        hardware().setPin(Clock, High);
        if ( d.bit(0) ) hardware().setPin(DataOut, High);
        else hardware().setPin(DataOut, Low);
        Port::usleep(1+_clockDelay);
        hardware().setPin(Clock, Low);
        Port::usleep(1+_clockDelay);
        d >>= 1;
    }
    hardware().setPin(DataOut, High);
    hardware().setRead();
}

void Direct::pic16::send_word(BitValue d)
{
    hardware().setWrite();
    d <<= 1;      // insert start bit
    for (uint x = 0; x<16; x++) {
        hardware().setPin(Clock, High);
        if ( d.bit(0) ) hardware().setPin(DataOut, High);
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
BitValue Direct::pic16::get_word()
{
    hardware().setRead();
    BitValue ind = 0;
    hardware().setPin(DataOut, High);
    for (uint x = 16; x ; --x) {
        hardware().setPin(Clock, High);
        Port::usleep(1+_clockDelay);
        if ( hardware().readBit() ) ind |= 0x8000;
        else ind = ind.maskWith(0x7FFF);
        ind >>= 1;
        hardware().setPin(Clock, Low);
        Port::usleep(1+_clockDelay);
    }
    return ind;
}

bool Direct::pic16::incrementPC(uint steps)
{
  for (uint i=0; i<steps; i++) pulseEngine("k6,");
  return true;
}

BitValue Direct::pic16::readWord(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryRangeType::Eeprom ) return pulseEngine("k5,r,");
  return pulseEngine("k4,R,");
}

bool Direct::pic16::doRead(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata)
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
  BitValue mask = device().mask(type);
  for (uint i = 0; i<nbWords; i++) {
    if ( !only || data[i]!=mask || type!=Pic::MemoryRangeType::Code ) {
      data[i] = readWord(type).maskWith(mask);
      if ( vdata && !hardware().verifyWord(i, data[i], type, *vdata) ) return false;
    }
    incrementPC(1);
  }
  if ( type==Pic::MemoryRangeType::Code || type==Pic::MemoryRangeType::Eeprom )
    _base.progressMonitor().addTaskProgress(data.count());
  return true;
}

bool Direct::pic16::doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force)
{
  gotoMemory(type);
  for (uint i = 0; i<data.count(); ) {
    if ( !writeWords(type, data, i, force) ) {
      log(Log::LineType::Error, i18n("Error programming %1 at %2.").arg(i18n(type.label())).arg(toHexLabel(i, 8)));
      return false;
    }
  }
  return true;
}

bool Direct::pic16::skipMaskWords(Pic::MemoryRangeType type, const Device::Array &data, uint &i, bool force)
{
  if (force) return false;
  uint nb = (type==Pic::MemoryRangeType::Code ? nbWordsCodeProg() : 1);
  for (uint k=0; k<nb; k++)
    if ( data[i+k]!=device().mask(type) ) return false;
  incrementPC(nb);
  i += nb;
  return true;
}

bool Direct::pic16::writeWords(Pic::MemoryRangeType type, const Device::Array &data, uint i)
{
  if ( type==Pic::MemoryRangeType::Eeprom) pulseEngine("k3,S,", data[i]);
  else {
    uint nb = (type==Pic::MemoryRangeType::Code ? nbWordsCodeProg() : 1);
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

bool Direct::pic16::writeWords(Pic::MemoryRangeType type, const Device::Array &data, uint &i, bool force)
{
  if ( skipMaskWords(type, data, i, force) ) return true;
  if ( !writeWords(type, data, i) ) return false;
  incrementPC(1);
  i += (type==Pic::MemoryRangeType::Code ? nbWordsCodeProg() : 1);
  return true;
}
