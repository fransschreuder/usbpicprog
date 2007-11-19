/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "picdem_bootloader.h"

#include "progs/icd2/base/microchip.h"

//-----------------------------------------------------------------------------
uint PicdemBootloader::Config::readVendorId()
{
  Config config;
  return qMin(config.readUIntEntry("vendor_id", Microchip::VENDOR_ID), uint(0xFFFF));
}
void PicdemBootloader::Config::writeVendorId(uint id)
{
  Config config;
  config.writeEntry("vendor_id", id);
}

uint PicdemBootloader::Config::readProductId()
{
  Config config;
  return qMin(config.readUIntEntry("product_id", 0x000B), uint(0xFFFF));
}
void PicdemBootloader::Config::writeProductId(uint id)
{
  Config config;
  config.writeEntry("product_id", id);
}

//-----------------------------------------------------------------------------
PicdemBootloader::Port::Port(Log::Base &log)
  : Port::USB(log, Config::readVendorId(), Config::readProductId(), 1, 0)
{}

bool PicdemBootloader::Port::receive(uint nb, QMemArray<uchar> &data)
{
  data.resize(nb);
  if ( !read(0x81, (char *)data.data(), nb) ) return false;
  log(Log::DebugLevel::Max, QString("received: \"%1\"").arg(toPrintable(data, PrintEscapeAll)));
  return true;
}

bool PicdemBootloader::Port::send(const QMemArray<uchar> &cmd)
{
  log(Log::DebugLevel::Extra, QString("send: \"%1\"").arg(toPrintable(cmd, PrintEscapeAll)));
  return write(0x01, (const char *)cmd.data(), cmd.count());
}

bool PicdemBootloader::Port::sendAndReceive(QMemArray<uchar> &data, uint nb)
{
  if ( !send(data) ) return false;
  return receive(nb, data);
}

QMemArray<uchar> PicdemBootloader::Port::command(uchar instruction, uint address, uint len, uint nb) const
{
  QMemArray<uchar> cmd(5+nb);
  cmd[0] = instruction;
  cmd[1] = len;
  cmd[2] = address & 0xFF;
  cmd[3] = (address >> 8) & 0xFF;
  cmd[4] = (address >> 16) & 0xFF;
  return cmd;
}

//-----------------------------------------------------------------------------
PicdemBootloader::Hardware::Hardware(::Programmer::Base &base)
  : Bootloader::Hardware(base, new Port(base))
{}

bool PicdemBootloader::Hardware::internalConnectHardware()
{
  if ( !openPort() ) return false;
  QMemArray<uchar> cmd(5);
  cmd.fill(0);
  if ( !port().sendAndReceive(cmd, 4) ) return false;
  VersionData version(cmd[3], cmd[2], 0);
  log(Log::LineType::Information, i18n("Bootloader version %1 detected").arg(version.pretty()));
  if ( version.majorNum()!=1 ) {
    log(Log::LineType::Error, i18n("Only bootloader version 1.x is supported"));
    return false;
  }
  return true;
}

uchar PicdemBootloader::Hardware::writeInstruction(Pic::MemoryRangeType type)
{
  switch (type.type()) {
    case Pic::MemoryRangeType::Code: return 0x02;
    case Pic::MemoryRangeType::Eeprom: return 0x05;
    case Pic::MemoryRangeType::UserId: return 0x07;
    default: Q_ASSERT(false); break;
  }
  return 0x00;
}

bool PicdemBootloader::Hardware::write(Pic::MemoryRangeType type, const Device::Array &data)
{
  Q_ASSERT( data.count()==device().nbWords(type) );
  if ( type==Pic::MemoryRangeType::Code ) { // check that there is nothing in bootloader reserved area
    for (uint i=0; i<data.count(); i++) {
      if ( i>=0x400 ) continue;
      if ( data[i]==device().mask(Pic::MemoryRangeType::Code) ) continue;
      uint address = device().addressIncrement(Pic::MemoryRangeType::Code) * i;
      log(Log::LineType::Warning, "  " + i18n("Code is present in bootloader reserved area (at address %1). It will be ignored.").arg(toHexLabel(address, device().nbCharsAddress())));
      break;
    }
  }
  uint nbBytesWord = device().nbBytesWord(type);
  uint nbBytes = nbBytesWord * device().nbWords(type);
  uint offset = (type==Pic::MemoryRangeType::Code ? 0x0800 : 0x00);
  for (; offset<nbBytes; offset+=16) {
    QMemArray<uchar> cmd = port().command(0x02, device().range(type).start.toUInt() + offset, 16, 16);
    for (uint k=0; k<16; k += nbBytesWord) {
      uint index = (offset + k) / nbBytesWord;
      cmd[5 + k] = data[index].byte(0);
      if ( nbBytesWord==2 ) cmd[5 + k+1] = data[index].byte(1);
    }
    if ( !port().sendAndReceive(cmd, 1) ) return false;
  }
  return true;
}

uchar PicdemBootloader::Hardware::readInstruction(Pic::MemoryRangeType type)
{
  switch (type.type()) {
    case Pic::MemoryRangeType::Code: return 0x01;
    case Pic::MemoryRangeType::Eeprom: return 0x04;
    case Pic::MemoryRangeType::Config:
    case Pic::MemoryRangeType::DeviceId:
    case Pic::MemoryRangeType::UserId: return 0x06;
    default: Q_ASSERT(false); break;
  }
  return 0x00;
}

bool PicdemBootloader::Hardware::read(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata)
{
  data.resize(device().nbWords(type));
  Device::Array varray;
  if (vdata) varray = static_cast<const Pic::Memory &>(vdata->memory).arrayForWriting(type);
  uint nbBytesWord = device().nbBytesWord(type);
  uint nbBytes = nbBytesWord * device().nbWords(type);
  uint nb = QMIN(uint(16), nbBytes);
  for (uint offset=0; offset<nbBytes; offset+=16) {
    QMemArray<uchar> cmd = port().command(readInstruction(type), device().range(type).start.toUInt() + offset, nb, 0);
    if ( !port().sendAndReceive(cmd, 5+nb) ) return false;
    for (uint k=0; k<nb; k += nbBytesWord) {
      uint index = (offset + k) / nbBytesWord;
      data[index]= cmd[5 + k] & 0xFF;
      if ( nbBytesWord==2 ) data[index] |= (cmd[5 + k+1] << 8);
      if ( vdata && index>=0x0800 && data[index]!=varray[index] ) {
        log(Log::LineType::Error, i18n("Device memory does not match hex file (at address 0x%2: reading 0x%3 and expecting 0x%4).")
                    .arg(QString(toHex(index/2, device().nbCharsAddress())))
                    .arg(QString(toHex(data[index], device().nbCharsWord(type))))
                    .arg(QString(toHex(varray[index], device().nbCharsWord(type)))));
         return false;
      }
    }
  }
  return true;
}

bool PicdemBootloader::Hardware::erase(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryRangeType::Eeprom ) {
    Pic::Memory memory(device());
    Device::Array data = memory.arrayForWriting(Pic::MemoryRangeType::Eeprom);
    return write(Pic::MemoryRangeType::Eeprom, data);
  }
  uint nbBytesWord = device().nbBytesWord(type);
  uint nbBytes = nbBytesWord * device().nbWords(type);
  for (uint offset=0x0800; offset<nbBytes; offset+=64) {
    QMemArray<uchar> cmd = port().command(0x03, device().range(type).start.toUInt() + offset, 1, 0);
    if ( !port().sendAndReceive(cmd, 1) ) return false;
  }
  return true;
}
