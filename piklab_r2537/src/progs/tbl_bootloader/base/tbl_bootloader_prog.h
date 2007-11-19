/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TBL_BOOTLOADER_PROG_H
#define TBL_BOOTLOADER_PROG_H

#include "tbl_bootloader.h"

namespace TinyBootloader
{

//-----------------------------------------------------------------------------
class ProgrammerBase : public ::Bootloader::ProgrammerBase
{
Q_OBJECT
public:
  ProgrammerBase(const Programmer::Group &group, const Pic::Data *data);
  virtual bool verifyDeviceId() { return static_cast<Hardware &>(hardware()).verifyDeviceId(); }
};

//-----------------------------------------------------------------------------
class Group : public ::Bootloader::Group
{
public:
  virtual QString name() const { return "tbl_bootloader"; }
  virtual QString label() const { return i18n("Tiny Bootloader"); }
  virtual ::Programmer::Properties properties() const { return ::Programmer::Programmer; }
  virtual ::Programmer::TargetPowerMode targetPowerMode() const { return ::Programmer::TargetSelfPowered; }
  virtual bool isPortSupported(PortType type) const { return type==PortType::Serial; }
  virtual bool canReadVoltage(Pic::VoltageType) const { return false; }

protected:
  virtual void initSupported();
  virtual ::Programmer::Base *createBase(const Device::Data *data) const { return new ProgrammerBase(*this, static_cast<const Pic::Data *>(data)); }
  virtual ::Programmer::Hardware *createHardware(::Programmer::Base &base, const ::Programmer::HardwareDescription &hd) const { return new Hardware(base, hd.port.device); }
  virtual ::Programmer::DeviceSpecific *createDeviceSpecific(::Programmer::Base &base) const { return new DeviceSpecific(base); }
};

} // namespace

#endif
