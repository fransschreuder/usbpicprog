/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "psp_prog.h"

//----------------------------------------------------------------------------
bool Psp::Base::readFirmwareVersion()
{
  return hardware().getFirmwareVersion(_firmwareVersion);
}

//----------------------------------------------------------------------------
Programmer::Properties Psp::Group::properties() const
{
   return ::Programmer::Programmer | ::Programmer::HasFirmware | ::Programmer::CanReadMemory | ::Programmer::HasConnectedState; // | ::Programmer::CanUploadFirmware;
}

Programmer::Hardware *Psp::Group::createHardware(::Programmer::Base &base, const ::Programmer::HardwareDescription &hd) const
{
  return new Hardware(base, hd.port.device);
}

Programmer::DeviceSpecific *Psp::Group::createDeviceSpecific(::Programmer::Base &base) const
{
  return new DeviceSpecific(base);
}
