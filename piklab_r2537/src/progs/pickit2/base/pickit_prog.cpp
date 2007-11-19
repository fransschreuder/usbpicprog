/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit_prog.h"

#include "common/global/global.h"
#include "pickit.h"
#include "progs/base/prog_config.h"

Pickit::Base::Base(const Programmer::Group &group, const Pic::Data *data)
  : ::Programmer::PicBase(group, data, "pickit_programmer")
{}

Pickit::Hardware &Pickit::Base::hardware()
{
  return static_cast<Hardware &>(*_hardware);
}

bool Pickit::Base::readFirmwareVersion()
{
  return hardware().port().getMode(_firmwareVersion, _mode);
}

bool Pickit::Base::regenerateOsccal(const PURL::Url &url)
{
  log(Log::DebugLevel::Normal, QString("  Calibration firmware file: %1").arg(url.pretty()));
  Log::StringView sview;
  PURL::File file(url, sview);
  if ( !file.openForRead() ) {
    log(Log::LineType::Error, i18n("Could not open firmware file \"%1\".").arg(url.pretty()));
    return false;
  }
  Pic::Memory memory(*device());
  QStringList errors, warnings;
  Pic::Memory::WarningTypes warningTypes;
  if ( !memory.load(file.stream(), errors, warningTypes, warnings) ) {
    log(Log::LineType::Error, i18n("Could not read calibration firmware file \"%1\" (%2).").arg(url.pretty()).arg(errors[0]));
    return false;
  }
  if ( warningTypes!=Pic::Memory::NoWarning ) {
    log(Log::LineType::Error, i18n("Calibration firmware file seems incompatible with selected device %1.").arg(device()->name()));
    return false;
  }
  if ( !askContinue(i18n("This will overwrite the device code memory. Continue anyway?")) ) return false;
  if ( !program(memory, Pic::MemoryRange(Pic::MemoryRangeType::Nb_Types)) ) return false;
  Device::Array array(1);
  if ( !static_cast<Hardware &>(hardware()).regenerateOsccal(array[0]) ) return false;
  return programCalibration(array);
}
