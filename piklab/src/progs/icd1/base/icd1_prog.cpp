/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "icd1_prog.h"

#include <qdir.h>

#include "progs/base/prog_config.h"
#include "devices/list/device_list.h"
#include "icd1_serial.h"

//-----------------------------------------------------------------------------
void Icd1::ProgrammerBase::clear()
{
  Icd::ProgrammerBase::clear();
  _selfTestResult = ::Programmer::Nb_ResultTypes;
}

bool Icd1::ProgrammerBase::selfTest(bool ask)
{
  log(Log::NormalDebug, "  Self-test");
  _selfTestResult = (hardware().selfTest() ? ::Programmer::Pass : ::Programmer::Fail);
  if ( _selfTestResult==::Programmer::Fail ) {
    if ( ask && !askContinue(i18n("Self-test failed. Do you want to continue anyway?")) ) {
      logUserAbort();
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------
Programmer::Properties Icd1::Group::properties() const
{
   return ::Programmer::Programmer | ::Programmer::HasFirmware | ::Programmer::CanUploadFirmware | ::Programmer::HasSelfTest | ::Programmer::CanReadMemory | ::Programmer::HasConnectedState;
}

Programmer::Hardware *Icd1::Group::createHardware(::Programmer::Base &base, const ::Programmer::HardwareDescription &hd) const
{
  return new Hardware(base, hd.port.device);
}

Programmer::DeviceSpecific *Icd1::Group::createDeviceSpecific(::Programmer::Base &base) const
{
  return new Icd::DeviceSpecific(base);
}
