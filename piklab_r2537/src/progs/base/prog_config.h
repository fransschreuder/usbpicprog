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
//#include "generic_prog.h"
#include "prog_group.h"
#include "common/global/generic_config.h"

namespace Programmer
{
//----------------------------------------------------------------------------
class GroupConfig
{
public:
  static PortType portType(const Group &group);
  static void writePortType(const Group &group, PortType type);
  static QString portDevice(const Group &group, PortType type);
  static void writePortDevice(const Group &group, PortType type, const QString &device);
  static Port::Description portDescription(const Group &group);
  static void writePortDescription(const Group &group, const Port::Description &pd);
  static QString firmwareDirectory(const Group &group);
  static void writeFirmwareDirectory(const Group &group, const QString &path);
};

//----------------------------------------------------------------------------
BEGIN_DECLARE_CONFIG(Config)
  OnlyProgramNonMask = 0, VerifyAfterProgram, OnlyVerifyProgrammed,
  PowerDownAfterProgramming, TargetSelfPowered, BlankCheckAfterErase,
  PreserveEeprom, ProgramEeprom, RunAfterProgram
END_DECLARE_CONFIG(Config, "programmer")

} // namespace

#endif
