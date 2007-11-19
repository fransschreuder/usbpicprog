/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "custom.h"

#include "devices/list/device_list.h"
#include "devices/base/device_group.h"

QValueList<const Device::Data *> CustomTool::Group::getSupportedDevices(const QString &) const
{
  QValueVector<QString> devices = Device::lister().supportedDevices();
  QValueList<const Device::Data *> list;
  for (uint i=0; i<devices.count(); i++) {
    const Device::Data *data = Device::lister().data(devices[i]);
    list.append(data);
  }
  return list;
}
