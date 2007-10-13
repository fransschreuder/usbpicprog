/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ICD2_PROG_H
#define ICD2_PROG_H

#include "common/global/global.h"
#include "icd_prog.h"
#include "icd2.h"
#include "progs/base/prog_group.h"

namespace Icd2
{
class Hardware;

//-----------------------------------------------------------------------------
class ProgrammerBase : public Icd::ProgrammerBase
{
Q_OBJECT
public:
  ProgrammerBase(const Programmer::Group &group, const Pic::Data *data, const char *name)
    : Icd::ProgrammerBase(group, data, name) {}
  Hardware *hardware() { return static_cast<Hardware *>(_hardware); }
  const TestData &testData() const { return _testData; }
  virtual bool selfTest(bool ask);
  virtual bool readFirmwareVersion();
  uchar firmwareId() const { return _firmwareId; }
  virtual bool setTarget() { return hardware()->setTarget(); }

protected:
  virtual void clear();
  virtual bool setupFirmware();
  virtual VersionData minFirmwareVersion() const;
  virtual VersionData maxFirmwareVersion() const;
  virtual bool internalSetupHardware();

private:
  uchar    _firmwareId;
  TestData _testData;
};

//-----------------------------------------------------------------------------
class Programmer : public ProgrammerBase
{
Q_OBJECT
public:
  Programmer(const ::Programmer::Group &group, const Pic::Data *data);
};

//-----------------------------------------------------------------------------
class Group : public Icd::Group
{
public:
  virtual QString xmlName() const { return "icd2"; }
  virtual ::Programmer::Properties properties() const;
  virtual ::Programmer::TargetPowerMode targetPowerMode() const { return ::Programmer::TargetPowerModeFromConfig; }
  virtual bool isPortSupported(Port::Type type) const { return ( type==Port::SerialType || type==Port::USBType ); }
  virtual bool canReadVoltage(Pic::VoltageType type) const;

protected:
  virtual void initSupported();
  virtual ::Programmer::Hardware *createHardware(::Programmer::Base &base, const ::Programmer::HardwareDescription &hd) const;
  virtual ::Programmer::DeviceSpecific *createDeviceSpecific(::Programmer::Base &base) const;
};

//-----------------------------------------------------------------------------
class ProgrammerGroup : public Group
{
public:
  virtual QString name() const { return "icd2"; }
  virtual QString label() const { return i18n("ICD2 Programmer"); }

protected:
  virtual ::Programmer::Base *createBase(const Device::Data *data) const { return new Programmer(*this, static_cast<const Pic::Data *>(data)); }
};

} // namespace

#endif
