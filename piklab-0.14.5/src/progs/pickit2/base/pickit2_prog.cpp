/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit2_prog.h"

#include "devices/list/device_list.h"
#include "pickit2.h"

//----------------------------------------------------------------------------
bool Pickit2::Base::deviceHasOsccalRegeneration() const
{
  return data(device()->name()).regenerateOsccal;
}

bool Pickit2::Base::setTarget()
{
  // #### FIXME: this needs to test for 18J first to protected them
  if ( !static_cast<Pickit2::Hardware &>(hardware()).setVddVpp(5.0, 12.0) ) return false;
  return static_cast<Pickit::DeviceSpecific *>(_specific)->init();
}

bool Pickit2::Base::internalEnterMode(::Programmer::Mode mode)
{
  USBPort &port = static_cast<USBPort &>(hardware().port());
  if ( !port.resetFirmwareDevice(mode) ) return false;
  log(Log::ExtraDebug, "disconnecting and try to reconnect PICkit2 in new mode...");
  disconnectHardware();
  Port::usleep(3000000);
  return connectHardware();
}

bool Pickit2::Base::doUploadFirmware(PURL::File &file)
{
  const Device::Data &data = *Device::lister().data("18F2550");
  Pic::Memory memory(static_cast<const Pic::Data &>(data));
  QStringList errors, warnings;
  Pic::Memory::WarningTypes warningTypes;
  if ( !memory.load(file.stream(), errors, warningTypes, warnings) ) {
    log(Log::Error, i18n("Could not read firmware hex file \"%1\" (%2).").arg(file.url().pretty()).arg(errors[0]));
    return false;
  }
  if ( warningTypes!=Pic::Memory::NoWarning ) {
    log(Log::Error, i18n("Firmware hex file seems incompatible with device 18F2550 inside PICkit2."));
    return false;
  }
  log(Log::Information, i18n("  Uploading PICkit2 firmware..."));
  if ( !enterMode(::Programmer::BootloadMode) ) return false;
  if ( !static_cast<USBPort &>(hardware().port()).uploadFirmware(memory) ) {
    log(Log::Error, i18n("Failed to upload firmware."));
    return false;
  }
  log(Log::Information, i18n("Firmware succesfully uploaded."));
  return enterMode(::Programmer::NormalMode);
}

//----------------------------------------------------------------------------
Programmer::Hardware *Pickit2::Group::createHardware(Programmer::Base &base, const Port::Description &) const
{
  return new Hardware(base);
}
