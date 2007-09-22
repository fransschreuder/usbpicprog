/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICKIT2_PROG_H
#define PICKIT2_PROG_H

#include "common/global/global.h"
#include "pickit_prog.h"

namespace Pickit2
{
//----------------------------------------------------------------------------
class Base : public Pickit::Base
{
Q_OBJECT
public:
  Base(const Programmer::Group &group, const Pic::Data *data) : Pickit::Base(group, data) {}
  virtual bool deviceHasOsccalRegeneration() const;
  virtual bool setTarget();

private:
  virtual VersionData minFirmwareVersion() const { return VersionData(1, 10, 0); }
  virtual VersionData minRecommendedFirmwareVersion() const { return VersionData(1, 20, 0); }
  virtual VersionData maxFirmwareVersion() const { return VersionData(1, 20, 0); }
  virtual bool internalEnterMode(::Programmer::Mode mode);
  virtual bool doUploadFirmware(PURL::File &file);
};

//----------------------------------------------------------------------------
class Group : public Pickit::Group
{
public:
  virtual QString name() const { return "pickit2"; }
  virtual QString label() const { return i18n("PICkit2 Firmware 1.x"); }
  virtual Programmer::Properties properties() const { return ::Programmer::Programmer | ::Programmer::HasFirmware | ::Programmer::CanUploadFirmware | ::Programmer::CanReadMemory | ::Programmer::HasConnectedState; }
  virtual bool canReadVoltage(Pic::VoltageType type) const { return ( type==Pic::TargetVdd || type==Pic::TargetVpp ); }

protected:
  virtual void initSupported();
  virtual Programmer::Base *createBase(const Device::Data *data) const { return new ::Pickit2::Base(*this, static_cast<const Pic::Data *>(data)); }
  virtual Programmer::Hardware *createHardware(Programmer::Base &base, const Port::Description &pd) const;
  virtual Programmer::DeviceSpecific *createDeviceSpecific(Programmer::Base &base) const;
};

} // namespace

#endif
