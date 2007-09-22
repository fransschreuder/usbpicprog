/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "group.h"

#include "devices/base/generic_device.h"

//-----------------------------------------------------------------------------
const char * const Group::SUPPORT_TYPE_NAMES[Nb_SupportTypes] = {
  "not_supported", "untested", "tested"
};

Group::Base::Base()
  : _gui(0), _lister(0), _initialized(false)
{}

uint Group::Base::index() const
{
  return _lister->groupIndex(name());
}

void Group::Base::addDevice(const DeviceData &data)
{
  _devices[data.data->name()] = data;
}

const Device::Data *Group::Base::deviceData(const QString &device) const
{
  const_cast<Base &>(*this).checkInitSupported();
  if ( !_devices.contains(device) ) return 0;
  return _devices[device].data;
}

bool Group::Base::isSupported(const QString &device) const
{
  const_cast<Base &>(*this).checkInitSupported();
  return _devices.contains(device);
}

Group::SupportType Group::Base::supportType(const QString &device) const
{
  const_cast<Base &>(*this).checkInitSupported();
  if ( !_devices.contains(device) ) return NotSupported;
  return _devices[device].supportType;
}

QValueVector<QString> Group::Base::supportedDevices() const
{
  const_cast<Base &>(*this).checkInitSupported();
  QValueVector<QString> names(_devices.count());
  QMap<QString, DeviceData>::const_iterator it;
  uint i = 0;
  for (it=_devices.begin(); it!=_devices.end(); ++it) {
    names[i] = it.key();
    i++;
  }
  return names;
}

void Group::Base::init()
{
  _devices.clear();
  _initialized = false;
}

void Group::Base::checkInitSupported()
{
  if (_initialized) return;
  _initialized = true;
  initSupported();
}

//-----------------------------------------------------------------------------
void Group::ListerBase::addGroup(Base *group, BaseGui *gui)
{
  Q_ASSERT(group);
  group->_lister = this;
  group->_gui = gui;
  if (gui) gui->_group = group;
  group->init();
  _groups.append(group);
}

const Group::Base *Group::ListerBase::group(const QString &name) const
{
  for (uint i=0; i<nbGroups(); i++)
    if ( _groups[i]->name()==name ) return _groups[i];
  return 0;
}

uint Group::ListerBase::groupIndex(const QString &name) const
{
  for (uint i=0; i<nbGroups(); i++)
    if ( _groups[i]->name()==name ) return i;
  return nbGroups();
}

bool Group::ListerBase::isSupported(const QString &device) const
{
  for (uint i=0; i<nbGroups(); i++)
    if ( _groups[i]->isSupported(device) ) return true;
  return false;
}

QValueVector<QString> Group::ListerBase::supportedDevices() const
{
  QMap<QString, bool> map;
  for (uint i=0; i<nbGroups(); i++) {
    QValueVector<QString> gnames = _groups[i]->supportedDevices();
    for (uint k=0; k<uint(gnames.count()); k++) map[gnames[k]] = true;
  }
  QValueVector<QString> names(map.count());
  QMap<QString, bool>::const_iterator it;
  uint i = 0;
  for(it=map.begin(); it!=map.end(); ++it) {
    names[i] = it.key();
    i++;
  }
  return names;
}
