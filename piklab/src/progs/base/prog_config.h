/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROGRAMMER_CONFIG_H
#define PROGRAMMER_CONFIG_H

#include "common/port/port.h"
#include "generic_prog.h"
#include "prog_group.h"
#include "common/global/generic_config.h"

namespace Programmer
{
//----------------------------------------------------------------------------
class GroupConfig : public GenericConfig
{
public:
  GroupConfig(const Group &group) : GenericConfig(group.name()), _group(group) {}
  Port::Type portType();
  void writePortType(Port::Type type);
  QString portDevice(Port::Type type);
  void writePortDevice(Port::Type type, const QString &device);
  Port::Description portDescription();
  void writePortDescription(const Port::Description &pd);
  QString firmwareDirectory();
  void writeFirmwareDirectory(const QString &path);

private:
  const Group &_group;
};

//----------------------------------------------------------------------------
class Config : public GenericConfig
{
public:
  Config() : GenericConfig("programmer") {}
  bool onlyProgramNonMask();
  void writeOnlyProgramNonMask(bool only);
  bool onlyVerifyProgrammed();
  void writeOnlyVerifyProgrammed(bool only);
  bool powerDownAfterProgramming();
  void writePowerDownAfterProgramming(bool powerDown);
  bool isTargetSelfPowered();
  void writeTargetSelfPowered(bool selfPowered);
  void writeBlankCheckAfterErase(bool check);
  bool blankCheckAfterErase();
  void writePreserveEeprom(bool preserve);
  bool preserveEeprom();
  void writeProgramEeprom(bool program);
  bool programEeprom();
};

} // namespace

#endif
