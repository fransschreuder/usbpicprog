/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICKIT2_BOOTLOADER_PROG_H
#define PICKIT2_BOOTLOADER_PROG_H

#include "progs/bootloader/base/bootloader_prog.h"
#include "pickit2_bootloader.h"

namespace Pickit2Bootloader
{

//-----------------------------------------------------------------------------
class ProgrammerBase : public ::Bootloader::ProgrammerBase
{
Q_OBJECT
public:
  ProgrammerBase(const Programmer::Group &group, const Pic::Data *data);
  virtual bool verifyDeviceId() { return true; }
};

//-----------------------------------------------------------------------------
class Group : public ::Bootloader::Group
{
public:
  virtual QString name() const { return "pickit2_bootloader"; }
  virtual QString label() const { return i18n("Pickit2 Bootloader"); }
  virtual ::Programmer::Properties properties() const { return ::Programmer::Programmer | ::Programmer::CanReadMemory; }
  virtual ::Programmer::TargetPowerMode targetPowerMode() const { return ::Programmer::TargetSelfPowered; }
  virtual bool isPortSupported(PortType type) const { return type==PortType::USB; }
  virtual bool canReadVoltage(Pic::VoltageType) const { return false; }

protected:
  virtual void initSupported();
  virtual ::Programmer::Base *createBase(const Device::Data *data) const { return new ProgrammerBase(*this, static_cast<const Pic::Data *>(data)); }
  virtual ::Programmer::Hardware *createHardware(::Programmer::Base &base, const ::Programmer::HardwareDescription &) const { return new Hardware(base); }
  virtual ::Programmer::DeviceSpecific *createDeviceSpecific(::Programmer::Base &base) const { return new DeviceSpecific(base); }
};

} // namespace

#endif
