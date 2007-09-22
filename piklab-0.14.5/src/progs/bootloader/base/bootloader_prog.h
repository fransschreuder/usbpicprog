/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BOOTLOADER_PROG_H
#define BOOTLOADER_PROG_H

#include "common/global/generic_config.h"
#include "devices/pic/pic/pic_prog.h"
#include "bootloader.h"
#include "bootloader_data.h"

namespace Bootloader
{
//-----------------------------------------------------------------------------
class Base : public Programmer::PicBase
{
Q_OBJECT
public:
  Base(const Programmer::Group &group, const Pic::Data *data, const char *name);

protected:
  Hardware &hardware() { return static_cast<Hardware &>(*_hardware); }
  virtual bool internalProgram(const Device::Memory &memory, const Device::MemoryRange &range);
};

//-----------------------------------------------------------------------------
class Group : public ::Programmer::PicGroup
{
public:
  Group(Type type) : _type(type) {}
  Type type() const { return _type; }
  virtual QString name() const { return TYPE_DATA[_type].name; }
  virtual QString label() const { return i18n(TYPE_DATA[_type].label); }
  virtual ::Programmer::Properties properties() const { return ::Programmer::Programmer; }
  virtual ::Programmer::TargetPowerMode targetPowerMode() const { return ::Programmer::TargetSelfPowered; }
  virtual bool isPortSupported(Port::Type type) const;
  virtual bool canReadVoltage(Pic::VoltageType) const { return false; }
  virtual bool checkConnection(const Port::Description &pd) const;

protected:
  virtual void initSupported();
  virtual ::Programmer::Base *createBase(const Device::Data *data) const;
  virtual ::Programmer::Hardware *createHardware(::Programmer::Base &base, const Port::Description &pd) const;
  virtual ::Programmer::DeviceSpecific *createDeviceSpecific(::Programmer::Base &base) const;

private:
  Type _type;
};

//-----------------------------------------------------------------------------
class Config : public GenericConfig
{
public:
  Config(const Group &group) : GenericConfig(group.name()) {}
  bool readProgramEeprom();
  void writeProgramEeprom(bool program);
};

} // namespace

#endif
