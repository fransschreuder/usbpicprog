/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICDEM_BOOTLOADER_PROG_H
#define PICDEM_BOOTLOADER_PROG_H

#include "progs/bootloader/base/bootloader_prog.h"
#include "picdem_bootloader.h"

namespace PicdemBootloader
{

//-----------------------------------------------------------------------------
class ProgrammerBase : public Bootloader::ProgrammerBase
{
Q_OBJECT
public:
  ProgrammerBase(const Programmer::Group &group, const Pic::Data *data);
};

//-----------------------------------------------------------------------------
class Group : public ::Bootloader::Group
{
public:
  virtual QString name() const { return "picdem_bootloader"; }
  virtual QString label() const { return i18n("Picdem Bootloader"); }
  virtual ::Programmer::Properties properties() const { return ::Programmer::Programmer | ::Programmer::CanReadMemory; }
  virtual ::Programmer::TargetPowerMode targetPowerMode() const { return ::Programmer::TargetSelfPowered; }
  virtual bool isPortSupported(::Port::Type type) const { return type==::Port::USBType; }
  virtual bool canReadVoltage(Pic::VoltageType) const { return false; }

protected:
  virtual void initSupported();
  virtual ::Programmer::Base *createBase(const Device::Data *data) const { return new ProgrammerBase(*this, static_cast<const Pic::Data *>(data)); }
  virtual ::Programmer::Hardware *createHardware(::Programmer::Base &base, const ::Programmer::HardwareDescription &) const { return new Hardware(base); }
  virtual ::Programmer::DeviceSpecific *createDeviceSpecific(::Programmer::Base &base) const { return new DeviceSpecific(base); }
};

} // namespace

#endif
