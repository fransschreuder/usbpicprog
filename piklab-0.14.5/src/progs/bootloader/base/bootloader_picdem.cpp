/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "bootloader_picdem.h"

#include "bootloader_prog.h"
#include "bootloader_data.h"

#include "progs/icd2/base/microchip.h"

//-----------------------------------------------------------------------------
uint PicdemBootloader::Config::readVendorId()
{
  return qMin(readUIntEntry("vendor_id", Microchip::VENDOR_ID), uint(0xFFFF));
}
void PicdemBootloader::Config::writeVendorId(uint id)
{
  writeEntry("vendor_id", id);
}

uint PicdemBootloader::Config::readProductId()
{
  return qMin(readUIntEntry("product_id", 0x000B), uint(0xFFFF));
}
void PicdemBootloader::Config::writeProductId(uint id)
{
  writeEntry("product_id", id);
}

//-----------------------------------------------------------------------------
PicdemBootloader::Port::Port(Log::Base &log)
  : Port::USB(log)
{
  Config config;
  init(config.readVendorId(), config.readProductId(), 1);
}

bool PicdemBootloader::Port::receive(uint nb, QMemArray<uchar> &data)
{
  data.resize(nb);
  if ( !read(0x81, (char *)data.data(), nb) ) return false;
  log(Log::MaxDebug, QString("received: \"%1\"").arg(toPrintable(data, PrintEscapeAll)));
  return true;
}

bool PicdemBootloader::Port::send(const QMemArray<uchar> &cmd)
{
  log(Log::ExtraDebug, QString("send: \"%1\"").arg(toPrintable(cmd, PrintEscapeAll)));
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
  cmd[1] = address & 0xFF;
  cmd[2] = (address >> 8) & 0xFF;
  cmd[3] = (address >> 16) & 0xFF;
  cmd[4] = len;
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
  log(Log::Information, i18n("Bootloader version %1 detected").arg(version.pretty()));
  if ( version.majorNum()!=1 ) {
    log(Log::Error, i18n("Only bootloader version 1.x is supported"));
    return false;
  }
  return true;
}

bool PicdemBootloader::Hardware::write(Pic::MemoryRangeType type, const Device::Array &data)
{
  Q_ASSERT( type!=Pic::MemoryEeprom );
  Q_ASSERT( data.count()==device().nbWords(type) );
  if ( type==Pic::MemoryCode ) { // check that there is nothing in bootloader reserved area
    for (uint i=0; i<data.count(); i++) {
      if ( i>=0x800 ) continue;
      if ( data[i]==device().mask(Pic::MemoryCode) ) continue;
      uint address = device().addressIncrement(Pic::MemoryCode) * i;
      log(Log::Warning, "  " + i18n("Code is present in bootloader reserved area (at address %1). It will be ignored.").arg(toHexLabel(address, device().nbCharsAddress())));
      break;
    }
  }
  uint offset = (type==Pic::MemoryCode ? 0x0800 : 0);
  for (; offset<device().nbWords(type); offset+=16) {
    QMemArray<uchar> cmd = port().command(0x02, device().range(type).start + offset, 16, 16);
    for (uint i=0; i<16; i++) cmd[5+i] = data[offset+i];
    if ( !port().sendAndReceive(cmd, 1) ) return false;
  }
  return true;
}

bool PicdemBootloader::Hardware::read(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata)
{
  Q_ASSERT( type!=Pic::MemoryEeprom );
  data.resize(device().nbWords(type));
  Device::Array varray;
  if (vdata) varray = static_cast<const Pic::Memory &>(vdata->memory).arrayForWriting(type);
  for (uint offset=0; offset<device().nbWords(type); offset+=32) {
    QMemArray<uchar> cmd = port().command(0x01, device().range(type).start + offset, 32, 0);
    if ( !port().sendAndReceive(cmd, 5+32) ) return false;
    for (uint k=0; k<16; k++) {
      uint index = offset + k;
      data[index]= cmd[5 + 2*k] & 0xFF | (cmd[6 + 2*k] << 8);
      if ( vdata && index>=0x0800 && data[index]!=varray[index] ) {
        log(Log::Error, i18n("Device memory does not match hex file (at address 0x%2: reading 0x%3 and expecting 0x%4).")
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
  if ( type==Pic::MemoryEeprom ) {
    Pic::Memory memory(device());
    Device::Array data = memory.arrayForWriting(Pic::MemoryEeprom);
    return write(Pic::MemoryEeprom, data);
  }
  uint offset = (type==Pic::MemoryCode ? 0x0800 : 0);
  for (; offset<device().nbWords(type); offset+=64) {
    QMemArray<uchar> cmd = port().command(0x03, device().range(type).start + offset, 1, 0);
    if ( !port().sendAndReceive(cmd, 1) ) return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
PicdemBootloader::Base::Base(const Programmer::Group &group, const Pic::Data *data)
  : Bootloader::Base(group, data, "picdem_bootloader_programmer_base")
{}
