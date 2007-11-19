/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "group.h"

//-----------------------------------------------------------------------------
const Group::Support::Data Group::Support::DATA[Nb_Types] = {
  { "not_supported", I18N_NOOP("Not Supported") },
  { "untested", I18N_NOOP("Untested") },
  { "tested", I18N_NOOP("Tested") }
};

Group::Base::Base()
  : _gui(0), _initialized(false)
{}

Group::Base::ConstIterator Group::Base::begin() const
{
  const_cast<Base &>(*this).checkInitSupported();
  return _devices.begin();
}

Group::Base::ConstIterator Group::Base::end() const
{
  const_cast<Base &>(*this).checkInitSupported();
  return _devices.end();
}

void Group::Base::addDevice(const QString &name, const Device::Data *data, Support support)
{
  _devices[name].data = data;
  _devices[name].support = support;
}

Group::Base::Data Group::Base::deviceData(const QString &device) const
{
  const_cast<Base &>(*this).checkInitSupported();
  return _devices[device];
}

QValueVector<QString> Group::Base::supportedDevices() const
{
  const_cast<Base &>(*this).checkInitSupported();
  QValueVector<QString> names;
  for (ConstIterator it=begin(); it!=end(); ++it) names.append(it.key());
  return names;
}

uint Group::Base::count() const
{
  const_cast<Base &>(*this).checkInitSupported();
  return _devices.count();
}

void Group::Base::init()
{
  _initialized = false;
}

void Group::Base::checkInitSupported()
{
  if (_initialized) return;
  _initialized = true;
  _devices.clear();
  initSupported();
}
