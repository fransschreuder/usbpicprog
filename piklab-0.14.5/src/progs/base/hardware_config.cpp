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
bool HProgrammer::HardwareData::isEqual(const HardwareData &data) const
{
  return ( data.portType==portType );
}

void HProgrammer::HardwareData::readConfig(GenericConfig &config)
{
  QString s = config.readEntry("port_type");
  for (uint i=0; i<Port::Nb_Types; i++ )
    if ( s==Port::DATA[i].key ) portType = Port::Type(i);
}

void HProgrammer::HardwareData::writeConfig(GenericConfig &config) const
{
  config.writeEntry("port_type", Port::DATA[portType].key);
}

//-----------------------------------------------------------------------------
void HProgrammer::Config::writeCurrentHardware(Port::Type type, const QString &name)
{
  writeEntry(QString("current_hardware_") + Port::DATA[type].key, name);
}

QString HProgrammer::Config::currentHardware(Port::Type type)
{
  QStringList names = hardwareNames(type);
  return readEntry(QString("current_hardware_") + Port::DATA[type].key, names[0]);
}

QStringList HProgrammer::Config::hardwareNames(Port::Type type)
{
  QStringList names = standardHardwareNames(type);
  QStringList customNames = readListEntry("custom_hardware_names", QStringList());
  for (uint i=0; i<uint(customNames.count()); i++) {
    if ( type!=Port::Nb_Types ) {
      HardwareData *hdata = hardwareData(customNames[i]);
      if ( hdata->portType==type ) names += customNames[i];
      delete hdata;
    } else names += customNames[i];
  }
  return names;
}

QString HProgrammer::Config::label(const QString &name) const
{
  const HardwareDataInfo *info = standardHardwareDataInfo(name);
  if ( info==0 ) return QString::null;
  return i18n(info->label);
}

QString HProgrammer::Config::comment(const QString &name) const
{
  const HardwareDataInfo *info = standardHardwareDataInfo(name);
  if ( info==0 || info->comment==0 ) return QString::null;
  return i18n(info->comment);
}

HProgrammer::HardwareData *HProgrammer::Config::hardwareData(const QString &name)
{
  if ( isStandardHardware(name) ) return standardHardwareData(name);
  HardwareData *hdata = createHardwareData();
  GenericConfig config(group() + "_custom_hardware_" + name);
  hdata->readConfig(config);
  return hdata;
}

void HProgrammer::Config::writeCustomHardware(const QString& name, const HardwareData &hdata)
{
  Q_ASSERT( !standardHardwareData(name) );
  QStringList customNames = readListEntry("custom_hardware_names", QStringList());
  if ( !customNames.contains(name) ) {
    customNames += name;
    writeEntry("custom_hardware_names", customNames);
  }
  GenericConfig config(group() + "_custom_hardware_" + name);
  hdata.writeConfig(config);
}

void HProgrammer::Config::deleteCustomHardware(const QString &name)
{
  Q_ASSERT( !standardHardwareData(name) );
  QStringList customNames = readListEntry("custom_hardware_names", QStringList());
  customNames.remove(name);
  writeEntry("custom_hardware_names", customNames);
  GenericConfig::deleteGroup(group() + "_custom_hardware_" + name);
}
