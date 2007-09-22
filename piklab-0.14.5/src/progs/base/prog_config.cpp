/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "prog_config.h"

//----------------------------------------------------------------------------
Port::Type Programmer::GroupConfig::portType()
{
  QString s = readEntry("port_group");
  Port::Type supported = Port::Nb_Types;
  for (uint i=0; i<Port::Nb_Types; i++) {
    if ( _group.isPortSupported(Port::Type(i)) ) {
      if ( s==Port::DATA[i].key ) return Port::Type(i);
      supported = Port::Type(i);
    }
  }
  return supported;
}
void Programmer::GroupConfig::writePortType(Port::Type type)
{
  if ( type==Port::Nb_Types ) return;
  writeEntry("port_group", QString(Port::DATA[type].key));
}

QString Programmer::GroupConfig::portDevice(Port::Type portType)
{
  QString device = readEntry(QString(Port::DATA[portType].key) + "_port_device" , QString::null);
  if ( device.isNull() ) {
    QStringList list = Port::probedDeviceList(portType);
    if ( list.isEmpty() ) return QString::null;
    return list[0];
  }
  return device;
}
void Programmer::GroupConfig::writePortDevice(Port::Type type, const QString &device)
{
  if ( type==Port::Nb_Types ) return;
  writeEntry(QString(Port::DATA[type].key) + "_port_device", device);
}

Port::Description Programmer::GroupConfig::portDescription()
{
  Port::Type type = portType();
  return Port::Description(type, portDevice(type));
}
void Programmer::GroupConfig::writePortDescription(const Port::Description &dp)
{
  writePortType(dp.type);
  writePortDevice(dp.type, dp.device);
}

QString Programmer::GroupConfig::firmwareDirectory()
{
  return readEntry("firmware_directory", QString::null);
}
void Programmer::GroupConfig::writeFirmwareDirectory(const QString &path)
{
  writeEntry("firmware_directory", path);
}

//----------------------------------------------------------------------------
bool Programmer::Config::onlyProgramNonMask()
{
  return readBoolEntry("only_program_non_mask", true);
}
void Programmer::Config::writeOnlyProgramNonMask(bool only)
{
  writeEntry("only_program_non_mask", only);
}

bool Programmer::Config::onlyVerifyProgrammed()
{
  return readBoolEntry("only_verify_programmed", true);
}
void Programmer::Config::writeOnlyVerifyProgrammed(bool only)
{
  writeEntry("only_verify_programmed", only);
}

bool Programmer::Config::powerDownAfterProgramming()
{
  return readBoolEntry("power_down_after_programming", true);
}
void Programmer::Config::writePowerDownAfterProgramming(bool powerDown)
{
  writeEntry("power_down_after_programming", powerDown);
}

bool Programmer::Config::isTargetSelfPowered()
{
  return readBoolEntry("target_self_powered", false);
}
void Programmer::Config::writeTargetSelfPowered(bool selfPowered)
{
  writeEntry("target_self_powered", selfPowered);
}

void Programmer::Config::writeBlankCheckAfterErase(bool check)
{
  writeEntry("blank_check_after_erase", check);
}
bool Programmer::Config::blankCheckAfterErase()
{
  return readBoolEntry("blank_check_after_erase", false);
}

void Programmer::Config::writePreserveEeprom(bool preserve)
{
  writeEntry("preserve_eeprom", preserve);
}
bool Programmer::Config::preserveEeprom()
{
  return readBoolEntry("preserve_eeprom", false);
}
