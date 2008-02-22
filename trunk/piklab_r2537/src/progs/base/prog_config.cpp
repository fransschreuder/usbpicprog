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
PortType Programmer::GroupConfig::portType(const Group &group)
{
  GenericConfig config(group.name());
  PortType ctype = config.readEnumEntry<PortType>("port_group");
  if ( group.isPortSupported(ctype) ) return ctype;
  FOR_EACH(PortType, type) if ( group.isPortSupported(type) ) return type;
  return PortType::Nb_Types;
}
void Programmer::GroupConfig::writePortType(const Group &group, PortType type)
{
  if ( type==PortType::Nb_Types ) return;
  GenericConfig config(group.name());
  config.writeEnumEntry<PortType>("port_group", type);
}

QString Programmer::GroupConfig::portDevice(const Group &group, PortType portType)
{
  GenericConfig config(group.name());
  QString device = config.readEntry(QString(portType.key()) + "_port_device" , QString::null);
  if ( device.isNull() ) {
    QStringList list = Port::probedDeviceList(portType);
    if ( list.isEmpty() ) return QString::null;
    return list[0];
  }
  return device;
}
void Programmer::GroupConfig::writePortDevice(const Group &group, PortType type, const QString &device)
{
  if ( type==PortType::Nb_Types ) return;
  GenericConfig config(group.name());
  config.writeEntry(QString(type.key()) + "_port_device", device);
}

Port::Description Programmer::GroupConfig::portDescription(const Group &group)
{
  PortType type = portType(group);
  return Port::Description(type, portDevice(group, type));
}
void Programmer::GroupConfig::writePortDescription(const Group &group, const Port::Description &dp)
{
  writePortType(group, dp.type);
  writePortDevice(group, dp.type, dp.device);
}

QString Programmer::GroupConfig::firmwareDirectory(const Group &group)
{
  GenericConfig config(group.name());
  return config.readEntry("firmware_directory", QString::null);
}
void Programmer::GroupConfig::writeFirmwareDirectory(const Group &group, const QString &path)
{
  GenericConfig config(group.name());
  config.writeEntry("firmware_directory", path);
}

//----------------------------------------------------------------------------
const Programmer::Config::Data Programmer::Config::DATA[Nb_Types] = {
  { "only_program_non_mask", I18N_NOOP("Only program what is needed (faster)."), QVariant(true, 0) },
  { "verify_after_program", I18N_NOOP("Verify device memory after programming."), QVariant(true, 0) },
  { "only_verify_programmed", I18N_NOOP("Only verify programmed words in code memory (faster)."), QVariant(true, 0) },
  { "power_down_after_programming", I18N_NOOP("Power down target after programming."), QVariant(true, 0) },
  { "target_self_powered", I18N_NOOP("Target is self-powered (when possible)."), QVariant(true, 0) },
  { "blank_check_after_erase", I18N_NOOP("Blank check after erase."), QVariant(false, 0) },
  { "preserve_eeprom", I18N_NOOP("Preserve data EEPROM when programming."), QVariant(false, 0) },
  { "program_eeprom", I18N_NOOP("Program data EEPROM."), QVariant(true, 0) },
  { "run_after_program", I18N_NOOP("Run device after successful programming."), QVariant(false, 0) }
};
