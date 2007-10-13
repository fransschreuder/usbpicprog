/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "direct_mem24.h"

#include <qdatetime.h>

#include "common/global/global.h"
#include "common/common/misc.h"

Direct::Mem24DeviceSpecific::Mem24DeviceSpecific(::Programmer::Base &base)
  : ::Programmer::Mem24DeviceSpecific(base)
{}

bool Direct::Mem24DeviceSpecific::setPowerOn()
{
  hardware().setPin(Clock, Low);
  hardware().setPin(DataOut, Low);
  hardware().setPin(Vpp, Off);
  if ( hardware().isGroundPin(Vdd) ) {
    hardware().setPin(Clock, High);
    Port::usleep(500000);
  } else {
    hardware().setPin(Vdd, On);
    Port::usleep(10000);
  }
  return true;
}

bool Direct::Mem24DeviceSpecific::setPowerOff()
{
  hardware().setPin(Clock, Low);
  hardware().setPin(DataOut, Low);
  hardware().setPin(Vpp, Off);
  hardware().setPin(Vdd, Off);
  Port::usleep(10000);
  return true;
}

bool Direct::Mem24DeviceSpecific::verifyPresence()
{
  if ( !start() ) return false;
  bool acked;
  if ( !writeByte(controlByte(0x0, Write), acked) ) return false;
  if ( !acked ) {
    log(Log::Error, i18n("Could not detect EEPROM"));
    return false;
  }
  log(Log::Information, i18n("EEPROM detected"));
  return stop();
}

uint Direct::Mem24DeviceSpecific::controlByte(uint address, Operation operation) const
{
  uint cbyte = (operation==Write ? 0xA0 : 0xA1);
  uint bsize = device().nbBytes() / device().nbBlocks();
  uint block = address / bsize;
  uint nbb = nbBits(device().nbBlocks()-1);
  for (uint i=0; i<nbb; i++)
    if ( block & (1<<i) ) cbyte |= 1 << (4-nbb+i);
  return cbyte;
}

bool Direct::Mem24DeviceSpecific::setAddress(uint address)
{
  log(Log::ExtraDebug, QString("set address %1").arg(toHexLabel(address, nbChars(Number::Hex, address))));
  if ( !start() ) return false;
  uint bsize = device().nbBytes() / device().nbBlocks();
  uint block = address / bsize;
  log(Log::ExtraDebug, QString("  in block #%1/%2").arg(block).arg(device().nbBlocks()));
  uint cbyte = controlByte(address, Write);
  log(Log::MaxDebug, QString("  control byte is %1").arg(toHexLabel(cbyte, 2)));
  if ( !writeByteAck(cbyte) ) return false;
  uint nb = nbBytes(bsize-1);
  for (int i=nb-1; i>=0; i--) {
    uint add = (address >> 8*i) & 0xFF;
    log(Log::MaxDebug, QString("  byte #%1: %2").arg(i).arg(toHexLabel(add, 2)));
    if ( !writeByteAck(add) ) return false;
  }
  return true;
}

bool Direct::Mem24DeviceSpecific::doRead(Device::Array &data, const ::Programmer::VerifyData *vdata)
{
  // sequential read: all device memory
  if ( !setAddress(0x0) ) return false;
  if ( !start() ) return false;
  if ( !writeByteAck(controlByte(0x0, Read)) ) return false;
  data.resize(device().nbBytes());
  for (uint i=0; i<data.count()-1; i++) data[i] = readByte(Low);
  data[data.count()-1] = readByte(High);
  if (vdata) {
    for (uint i=0; i<data.count(); i++)
      if ( !verifyByte(i, data[i], *vdata) ) return false;
  }
  return stop();
}

bool Direct::Mem24DeviceSpecific::doWrite(const Device::Array &data)
{
  QTime time;
  // page by page (page_size==1: byte by byte)
  uint nbPages = device().nbBytes() / device().nbBytesPage();
  for (uint i=0; i<nbPages; i++) {
    log(Log::ExtraDebug, QString("write page #%1/%2").arg(i).arg(nbPages));
    uint address = i * device().nbBytesPage();
    // write bytes
    if ( !setAddress(address) ) return false;
    for (uint k=0; k<device().nbBytesPage(); k++)
      if ( !writeByteAck(data[address+k]) ) return false;
    if ( !stop() ) return false;
    // poll
    time.start();
    for (;;) {
      if ( !start() ) return false;
      bool acked;
      if ( !writeByte(controlByte(address, Write), acked) ) return false;
      if (acked) break;
      if ( time.elapsed()>200 ) { // 200 ms timeout
        log(Log::Error, i18n("Timeout writing at address %1").arg(toHexLabel(address, nbChars(device().nbBytes()))));
        return false;
      }
    }
  }
  return true;
}

void Direct::Mem24DeviceSpecific::set(State clock, State data)
{
  hardware().setPin(Clock, clock);
  hardware().setPin(DataOut, data);
  Port::usleep(5); // #### needed ?
}

void Direct::Mem24DeviceSpecific::setData(State data)
{
  set(Low, data);
  set(High, data);
  set(Low, data);
}

BitValue Direct::Mem24DeviceSpecific::readByte(State ack)
{
  hardware().setRead();
  set(Low, High);
  BitValue b = 0;
  for (uint i=0; i<8; i++) {
    set(High, High);
    b <<= 1;
    if ( hardware().readBit() ) b |= 0x1;
    set(Low, High);
  }
  hardware().setWrite();
  setData(ack);
  return b;
}

bool Direct::Mem24DeviceSpecific::writeByteAck(BitValue value)
{
  bool acked;
  if ( !writeByte(value, acked) ) return false;
  if (!acked) {
    log(Log::Error, i18n("Acknowledge bit incorrect"));
    return false;
  }
  return true;
}

bool Direct::Mem24DeviceSpecific::writeByte(BitValue value, bool &acked)
{
  Q_ASSERT( value<=0xFF );
  hardware().setWrite();
  set(Low, Low);
  for (int i=7; i>=0; i--) setData(value.bit(i) ? High : Low);
  hardware().setRead();
  set(Low, High);
  set(High, High);
  acked = !hardware().readBit();
  hardware().setWrite();
  set(Low, High);
  return true;
}

bool Direct::Mem24DeviceSpecific::start()
{
  hardware().setWrite();
  set(Low, High);
  set(High, High);
  set(High, Low);
  set(Low, Low);
  return true;
}

bool Direct::Mem24DeviceSpecific::stop()
{
  hardware().setWrite();
  set(Low, Low);
  set(High, Low);
  set(High, High);
  set(Low, High);
  return true;
}
