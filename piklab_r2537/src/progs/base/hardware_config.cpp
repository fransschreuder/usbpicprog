/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "hardware_config.h"

//-----------------------------------------------------------------------------
bool Hardware::Data::isEqual(const Data &data) const
{
  return ( data.portType==portType );
}

void Hardware::Data::readConfig(GenericConfig &config)
{
  portType = config.readEnumEntry<PortType>("port_type", PortType::Serial);
}

void Hardware::Data::writeConfig(GenericConfig &config) const
{
  config.writeEnumEntry<PortType>("port_type", portType);
}

//-----------------------------------------------------------------------------
void Hardware::Config::writeCurrentHardware(PortType type, const QString &name)
{
  writeEntry(QString("current_hardware_") + type.key(), name);
}

QString Hardware::Config::currentHardware(PortType type)
{
  QStringList names = hardwareNames(type);
  return readEntry(QString("current_hardware_") + type.key(), names[0]);
}

QString Hardware::Config::label(const QString &name) const
{
  const DataInfo *info = standardHardwareDataInfo(name);
  if ( info==0 ) return QString::null;
  return i18n(info->label);
}

QString Hardware::Config::comment(const QString &name) const
{
  const DataInfo *info = standardHardwareDataInfo(name);
  if ( info==0 || info->comment==0 ) return QString::null;
  return i18n(info->comment);
}

void Hardware::Config::writeCustomHardware(const QString& name, const Hardware::Data &hdata)
{
  Q_ASSERT( !isStandardHardware(name) );
  QStringList customNames = readListEntry("custom_hardware_names", QStringList());
  if ( !customNames.contains(name) ) {
    customNames += name;
    writeEntry("custom_hardware_names", customNames);
  }
  GenericConfig config(group() + "_custom_hardware_" + name);
  hdata.writeConfig(config);
}

void Hardware::Config::deleteCustomHardware(const QString &name)
{
  Q_ASSERT( !isStandardHardware(name) );
  QStringList customNames = readListEntry("custom_hardware_names", QStringList());
  customNames.remove(name);
  writeEntry("custom_hardware_names", customNames);
  GenericConfig::deleteGroup(group() + "_custom_hardware_" + name);
}

Hardware::Data *Hardware::Config::hardwareData(const QString &name) const
{
  if ( isStandardHardware(name) ) return standardHardwareData(name);
  Hardware::Data *hdata = createHardwareData();
  hdata->name = name;
  GenericConfig config(group() + "_custom_hardware_" + name);
  hdata->readConfig(config);
  return hdata;
}

QStringList Hardware::Config::hardwareNames(PortType type)
{
  QStringList names = standardHardwareNames(type);
  QStringList customNames = readListEntry("custom_hardware_names", QStringList());
  for (uint i=0; i<uint(customNames.count()); i++) {
    if ( type!=PortType::Nb_Types ) {
      Hardware::Data *hdata = hardwareData(customNames[i]);
      if ( hdata->portType==type ) names += customNames[i];
      delete hdata;
    } else names += customNames[i];
  }
  return names;
}
