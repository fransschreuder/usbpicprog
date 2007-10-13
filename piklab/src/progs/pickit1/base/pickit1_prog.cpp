/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit1_prog.h"

#include "pickit1.h"

//----------------------------------------------------------------------------
bool Pickit1::Base::deviceHasOsccalRegeneration() const
{
  return data(device()->name()).regenerateOsccal;
}

bool Pickit1::Base::setTarget()
{
  return static_cast<Pickit::DeviceSpecific *>(_specific)->init();
}

//----------------------------------------------------------------------------
Programmer::Hardware *Pickit1::Group::createHardware(Programmer::Base &base, const ::Programmer::HardwareDescription &) const
{
  return new Hardware(base);
}
