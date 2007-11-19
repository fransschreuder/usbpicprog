/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "prog_group.h"

#include "common/global/global.h"
#include "generic_prog.h"
#include "prog_config.h"
#include "generic_debug.h"
#include "devices/base/device_group.h"

// order is important
const PURL::FileType Programmer::INPUT_FILE_TYPE_DATA[Nb_InputFileTypes] = {
  PURL::Coff, PURL::Cod, PURL::Hex
};

QString Programmer::Group::statusLabel(PortType type) const
{
  uint nb = 0;
  FOR_EACH(PortType, ptype) if ( isPortSupported(ptype) ) nb++;
  if ( nb<=0 ) return label();
  return label() + " (" + i18n(type.label()) + ")";
}

Programmer::Base *Programmer::Group::createProgrammer(bool targetSelfPowered, const Device::Data *data, const HardwareDescription &hd) const
{
  ::Programmer::Base *base = createBase(data);
  Hardware *hardware = createHardware(*base, hd);
  DeviceSpecific *ds = (data ? createDeviceSpecific(*base) : 0);
  base->init(targetSelfPowered, hardware, ds);
  return base;
}

Debugger::Base *Programmer::Group::createDebugger(::Programmer::Base &base) const
{
  ::Debugger::Base *dbase = createDebuggerBase(base);
  if (dbase) {
    ::Debugger::DeviceSpecific *dspecific = createDebuggerDeviceSpecific(*dbase);
    ::Debugger::Specific *specific = createDebuggerSpecific(*dbase);
    dbase->init(dspecific, specific);
  }
  return dbase;
}

bool Programmer::Group::checkConnection(const HardwareDescription &hd) const
{
  ::Programmer::Base *base = createProgrammer(false, 0, hd);
  bool ok = base->simpleConnectHardware();
  delete base;
  return ok;
}

bool Programmer::Group::isSoftware() const
{
  FOR_EACH(PortType, type) if ( isPortSupported(type) ) return false;
  return true;
}
