/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit2_bootloader.h"

//-----------------------------------------------------------------------------
Pickit2Bootloader::Hardware::Hardware(::Programmer::Base &base)
  : Bootloader::Hardware(base, new Pickit2::USBPort(base))
{}

bool Pickit2Bootloader::Hardware::internalConnectHardware()
{
  if ( !openPort() ) return false;
  ::Programmer::Mode mode;
  VersionData version;
  if ( !port().getMode(version, mode) ) return false;
  if ( mode!=::Programmer::BootloadMode ) {
    log(Log::LineType::Information, i18n("Trying to enter bootloader mode..."));
    if ( !port().resetFirmwareDevice(::Programmer::BootloadMode) ) return false;
    for (uint i=0;; i++) {
      port().close();
      Port::usleep(500000); // ??
      if ( !openPort() ) continue;
      if ( !port().getMode(version, mode) ) continue;
      if ( i==4 || mode!=::Programmer::BootloadMode ) {
        log(Log::LineType::Error, i18n("Could not detect device in bootloader mode."));
        return false;
      }
    }
  }
  log(Log::LineType::Information, i18n("Bootloader version %1 detected.").arg(version.pretty()));
  if ( version.majorNum()!=2 ) {
    log(Log::LineType::Error, i18n("Only bootloader version 2.x is supported."));
    return false;
  }
  return true;
}

bool Pickit2Bootloader::Hardware::write(Pic::MemoryRangeType type, const Device::Array &data)
{
  Q_ASSERT( data.count()==device().nbWords(type) );
  Q_ASSERT( type==Pic::MemoryRangeType::Code );
  // check that there is nothing in bootloader reserved area
  for (uint i=0; i<data.count(); i++) {
    if ( i>=0x1000 && i<0x3FF0 ) continue;
    if ( data[i]==device().mask(Pic::MemoryRangeType::Code) ) continue;
    uint address = device().addressIncrement(Pic::MemoryRangeType::Code) * i;
    log(Log::LineType::Warning, "  " + i18n("Code is present in bootloader reserved area (at address %1). It will be ignored.").arg(toHexLabel(address, device().nbCharsAddress())));
    break;
  }
  return port().writeFirmwareCodeMemory(data, _base.progressMonitor());
}

bool Pickit2Bootloader::Hardware::read(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata)
{
  Q_ASSERT( type==Pic::MemoryRangeType::Code );
  data.resize(device().nbWords(type));
  Device::Array varray;
  if (vdata) varray = static_cast<const Pic::Memory &>(vdata->memory).arrayForWriting(Pic::MemoryRangeType::Code);
  return port().readFirmwareCodeMemory(data, vdata ? &varray : 0, _base.progressMonitor());
}

//-----------------------------------------------------------------------------
bool Pickit2Bootloader::DeviceSpecific::doEraseRange(Pic::MemoryRangeType type)
{
  Q_ASSERT( type==Pic::MemoryRangeType::Code );
  return static_cast<Hardware &>(hardware()).port().eraseFirmwareCodeMemory();
}
