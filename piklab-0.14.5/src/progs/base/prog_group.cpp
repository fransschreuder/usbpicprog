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
#include "devices/pic/pic/pic_debug.h"

// order is important
const PURL::FileType Programmer::INPUT_FILE_TYPE_DATA[Nb_InputFileTypes] = {
  PURL::Coff, PURL::Cod, PURL::Hex
};

Programmer::Base *Programmer::Group::createProgrammer(bool targetSelfPowered, const Device::Data *data, const Port::Description &pd) const
{
  ::Programmer::Base *base = createBase(data);
  Hardware *hardware = createHardware(*base, pd);
  DeviceSpecific *ds = (data ? createDeviceSpecific(*base) : 0);
  base->init(targetSelfPowered, hardware, ds);
  return base;
}

Debugger::Base *Programmer::Group::createDebugger(::Programmer::Base &base) const
{
  ::Debugger::Base *dbase = createDebuggerBase(base);
  if (dbase) {
    ::Debugger::DeviceSpecific *dspecific = 0;
    const Device::Data *data = base.device();
    if ( data && data->group().name()=="pic" ) {
      switch (static_cast<const Pic::Data &>(*data).architecture()) {
        case Pic::Arch_10X:
        case Pic::Arch_16X: dspecific = new ::Debugger::P16FSpecific(*dbase); break;
        case Pic::Arch_18C:
        case Pic::Arch_18F:
        case Pic::Arch_18J: dspecific = new ::Debugger::P18FSpecific(*dbase); break;
        case Pic::Arch_24F:
        case Pic::Arch_24H:
        case Pic::Arch_30X:
        case Pic::Arch_17X:
        case Pic::Nb_Architectures: Q_ASSERT(false); break;
      }
    }
    ::Debugger::Specific *specific = createDebuggerSpecific(*dbase);
    dbase->init(dspecific, specific);
  }
  return dbase;
}

bool Programmer::Group::checkConnection(const Port::Description &pd) const
{
  ::Programmer::Base *base = createProgrammer(false, 0, pd);
  bool ok = base->simpleConnectHardware();
  delete base;
  return ok;
}

bool Programmer::Group::isSoftware() const
{
  for (uint i=0; i<Port::Nb_Types; i++)
    if ( isPortSupported(Port::Type(i)) ) return false;
  return true;
}
