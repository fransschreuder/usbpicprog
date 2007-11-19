/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PSP_PROG_H
#define PSP_PROG_H

#include "common/global/global.h"
#include "psp.h"

namespace Psp
{
//-----------------------------------------------------------------------------
class Base : public Programmer::PicBase
{
Q_OBJECT
public:
  Base(const Programmer::Group &group, const Pic::Data *data)
    : Programmer::PicBase(group, data, "psp_programmer_base") {}
  virtual bool readFirmwareVersion();
  virtual bool setTarget() { return hardware().setTarget(); }

protected:
  Hardware &hardware() { return static_cast<Hardware &>(*_hardware); }
  virtual VersionData firmwareVersion(Programmer::FirmwareVersionType type) const { return (type==Programmer::FirmwareVersionType::Min ? VersionData(4, 30, 3) : VersionData()); }
};

//-----------------------------------------------------------------------------
class Group : public ::Programmer::PicGroup
{
public:
  virtual QString name() const { return "psp"; }
  virtual QString label() const { return i18n("Picstart Plus"); }
  virtual QString xmlName() const { return "psp"; }
  virtual ::Programmer::Properties properties() const;
  virtual ::Programmer::TargetPowerMode targetPowerMode() const { return ::Programmer::TargetExternallyPowered; }
  virtual bool isPortSupported(PortType type) const { return ( type==PortType::Serial ); }
  virtual bool canReadVoltage(Pic::VoltageType type) const { return ( type==Pic::TargetVdd || type==Pic::TargetVpp ); }

protected:
  virtual void initSupported();
  virtual ::Programmer::Base *createBase(const Device::Data *data) const { return new ::Psp::Base(*this, static_cast<const Pic::Data *>(data)); }
  virtual ::Programmer::Hardware *createHardware(::Programmer::Base &base, const ::Programmer::HardwareDescription &hd) const;
  virtual ::Programmer::DeviceSpecific *createDeviceSpecific(::Programmer::Base &base) const;
};

} // namespace

#endif
