/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ICD1_PROG_H
#define ICD1_PROG_H

#include "common/global/global.h"
#include "progs/icd2/base/icd_prog.h"
#include "icd1.h"
#include "progs/base/prog_group.h"

namespace Icd1
{
class Hardware;

//-----------------------------------------------------------------------------
class ProgrammerBase : public Icd::ProgrammerBase
{
Q_OBJECT
public:
  ProgrammerBase(const Programmer::Group &group, const Pic::Data *data)
    : Icd::ProgrammerBase(group, data, "icd1_programmer_base") {}
  virtual bool selfTest(bool ask);
  ::Programmer::ResultType selfTestResult() const { return _selfTestResult; }

protected:
  Hardware &hardware() { return static_cast<Hardware &>(*_hardware); }
  virtual void clear();

private:
  ::Programmer::ResultType _selfTestResult;
};

//-----------------------------------------------------------------------------
class Group : public Icd::Group
{
public:
  virtual QString name() const { return "icd1"; }
  virtual QString label() const { return i18n("ICD1 Programmer"); }
  virtual QString xmlName() const { return "icd1"; }
  virtual ::Programmer::Properties properties() const;
  virtual ::Programmer::TargetPowerMode targetPowerMode() const { return ::Programmer::TargetExternallyPowered; }
  virtual bool isPortSupported(PortType type) const { return ( type==PortType::Serial ); }
  virtual bool canReadVoltage(Pic::VoltageType type) const { return ( type==Pic::TargetVdd || type==Pic::TargetVpp ); }

protected:
  virtual void initSupported();
  virtual ::Programmer::Base *createBase(const Device::Data *data) const { return new ProgrammerBase(*this, static_cast<const Pic::Data *>(data)); }
  virtual ::Programmer::Hardware *createHardware(::Programmer::Base &base, const ::Programmer::HardwareDescription &hd) const;
  virtual ::Programmer::DeviceSpecific *createDeviceSpecific(::Programmer::Base &base) const;
};

} // namespace

#endif
