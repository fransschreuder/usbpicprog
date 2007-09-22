/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICKIT1_PROG_H
#define PICKIT1_PROG_H

#include "progs/pickit2/base/pickit_prog.h"

namespace Pickit1
{
//----------------------------------------------------------------------------
class Base : public Pickit::Base
{
Q_OBJECT
public:
  Base(const Programmer::Group &group, const Pic::Data *data) : Pickit::Base(group, data) {}
  virtual bool deviceHasOsccalRegeneration() const;
  virtual bool canReadVoltage(Pic::VoltageType) const { return false; }
  virtual bool setTarget();

private:
  virtual VersionData minVersion() const { return VersionData(2, 0, 0); }
  virtual VersionData maxVersion() const { return VersionData(2, 0, 0); }
};

//----------------------------------------------------------------------------
class Group : public Pickit::Group
{
public:
  virtual QString name() const { return "pickit1"; }
  virtual QString label() const { return i18n("PICkit1"); }
  virtual Programmer::Properties properties() const { return ::Programmer::Programmer | ::Programmer::HasFirmware | ::Programmer::CanReadMemory | ::Programmer::HasConnectedState; }

protected:
  virtual void initSupported();
  virtual Programmer::Base *createBase(const Device::Data *data) const { return new ::Pickit1::Base(*this, static_cast<const Pic::Data *>(data)); }
  virtual Programmer::Hardware *createHardware(Programmer::Base &base, const Port::Description &pd) const;
  virtual Programmer::DeviceSpecific *createDeviceSpecific(Programmer::Base &base) const;
};

} // namespace

#endif
