/***************************************************************************
 * Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICKIT2V2_PROG_H
#define PICKIT2V2_PROG_H

#include "progs/pickit2/base/pickit2_prog.h"

namespace Pickit2V2
{
class Hardware;

//----------------------------------------------------------------------------
class Base : public Pickit2::Base
{
Q_OBJECT
public:
  Base(const Programmer::Group &group, const Pic::Data *data);
  virtual bool setTarget();

private:
  Hardware &hardware();
  virtual VersionData firmwareVersion(Programmer::FirmwareVersionType type) const;
  virtual bool verifyDeviceId() { return true; } // it is done by "setTarget"
  virtual bool selfTest(bool ask);
  bool identifyDevice();
};

//----------------------------------------------------------------------------
class Group : public Programmer::PicGroup
{
public:
  virtual QString name() const { return "pickit2v2"; }
  virtual QString label() const { return i18n("PICkit2 Firmware 2.x"); }
  virtual Programmer::Properties properties() const { return ::Programmer::Programmer | ::Programmer::HasFirmware | ::Programmer::CanUploadFirmware | ::Programmer::CanReadMemory | ::Programmer::HasConnectedState; }
  virtual bool canReadVoltage(Pic::VoltageType type) const { return ( type==Pic::TargetVdd || type==Pic::TargetVpp ); }
  virtual ::Programmer::TargetPowerMode targetPowerMode() const { return ::Programmer::TargetPowerModeFromConfig; }
  virtual bool isPortSupported(PortType type) const { return ( type==PortType::USB ); }

protected:
  virtual void initSupported();
  virtual Programmer::Base *createBase(const Device::Data *data) const { return new ::Pickit2V2::Base(*this, static_cast<const Pic::Data *>(data)); }
  virtual Programmer::Hardware *createHardware(Programmer::Base &base, const Programmer::HardwareDescription &hd) const;
  virtual Programmer::DeviceSpecific *createDeviceSpecific(Programmer::Base &base) const;
};

} // namespace

#endif
