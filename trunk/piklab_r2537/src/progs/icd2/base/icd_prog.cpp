/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "icd_prog.h"

#include "common/global/pfile.h"
#include "progs/base/prog_config.h"
#include "devices/list/device_list.h"

bool Icd::ProgrammerBase::doUploadFirmware(PURL::File &file)
{
  const Device::Data &data = *Device::lister().data("16F876");
  Pic::Memory memory(static_cast<const Pic::Data &>(data));
  QStringList errors, warnings;
  Pic::Memory::WarningTypes warningTypes;
  if ( !memory.load(file.stream(), errors, warningTypes, warnings) ) {
    log(Log::LineType::Error, i18n("Could not read firmware hex file \"%1\": %2.").arg(file.url().pretty()).arg(errors[0]));
    return false;
  }
  if ( warningTypes!=Pic::Memory::NoWarning ) {
    log(Log::LineType::Error, i18n("Firmware hex file seems incompatible with device 16F876 inside ICD."));
    return false;
  }
  if ( !hardware().uploadFirmware(memory) ) {
    log(Log::LineType::Error, i18n("Failed to upload firmware."));
    return false;
  }
  return true;
}

bool Icd::ProgrammerBase::readFirmwareVersion()
{
  return hardware().getFirmwareVersion(_firmwareVersion);
}
