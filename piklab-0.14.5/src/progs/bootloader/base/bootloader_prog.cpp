/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "bootloader_prog.h"

#include "progs/base/prog_config.h"
#include "bootloader_data.h"
#include "bootloader_tbl.h"
#include "bootloader_pickit2.h"
#include "bootloader_picdem.h"

//-----------------------------------------------------------------------------
Bootloader::Base::Base(const Programmer::Group &group, const Pic::Data *data, const char *name)
  : Programmer::PicBase(group, data, name)
{}

bool Bootloader::Base::internalProgram(const Device::Memory &memory, const Device::MemoryRange &range)
{
  if ( !initProgramming(Erase) ) return false;
  addProgress(OPERATION_DATA[Erase].nbSteps);
  const Pic::Memory &pmemory = static_cast<const Pic::Memory &>(memory);
  Pic::MemoryRangeType type = Pic::MemoryCode;
  if ( !range.all() ) type = static_cast<const Pic::MemoryRange &>(range)._type;
  if ( !programRange(type, pmemory.arrayForWriting(type)) ) return false;
  if ( range.all() ) {
   Config config(static_cast<const Group &>(_group));
    if ( config.readProgramEeprom() )
      if ( !programRange(Pic::MemoryEeprom, pmemory.arrayForWriting(Pic::MemoryEeprom)) ) return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
bool Bootloader::Group::isPortSupported(Port::Type type) const
{
  return ( type==TYPE_DATA[_type].portType );
}

::Programmer::Base *Bootloader::Group::createBase(const Device::Data *data) const
{
  switch (_type) {
    case TinyBootloader: return new ::TinyBootloader::Base(*this, static_cast<const Pic::Data *>(data));
    case Pickit2Bootloader: return new ::Pickit2Bootloader::Base(*this, static_cast<const Pic::Data *>(data));
    case PicdemBootloader: return new ::PicdemBootloader::Base(*this, static_cast<const Pic::Data *>(data));
    case Nb_Types: break;
  }
  Q_ASSERT(false);
  return 0;
}

Programmer::Hardware *Bootloader::Group::createHardware(::Programmer::Base &base, const Port::Description &pd) const
{
  switch (_type) {
    case TinyBootloader: return new ::TinyBootloader::Hardware(base, pd.device);
    case Pickit2Bootloader: return new ::Pickit2Bootloader::Hardware(base);
    case PicdemBootloader: return new ::PicdemBootloader::Hardware(base);
    case Nb_Types: break;
  }
  Q_ASSERT(false);
  return 0;
}

Programmer::DeviceSpecific *Bootloader::Group::createDeviceSpecific(::Programmer::Base &base) const
{
  switch (_type) {
    case TinyBootloader: return new ::TinyBootloader::DeviceSpecific(base);
    case Pickit2Bootloader: return new ::Pickit2Bootloader::DeviceSpecific(base);
    case PicdemBootloader: return new ::PicdemBootloader::DeviceSpecific(base);
    case Nb_Types: break;
  }
  Q_ASSERT(false);
  return 0;
}

bool Bootloader::Group::checkConnection(const Port::Description &pd) const
{
  ::Programmer::Base *base = createProgrammer(false, 0, pd);
  bool ok = static_cast<Hardware *>(base->hardware())->openPort();
  delete base;
  return ok;
}

//-----------------------------------------------------------------------------
bool Bootloader::Config::readProgramEeprom()
{
  return readBoolEntry("program_eeprom", false);
}
void Bootloader::Config::writeProgramEeprom(bool program)
{
  writeEntry("program_eeprom", program);
}
