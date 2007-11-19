/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "device_list.h"

#include <qregexp.h>

#include "devices/pic/pic/pic_group.h"
#include "devices/mem24/mem24/mem24_group.h"

const Device::AutoData Device::AUTO_DATA = { "*", I18N_NOOP("<auto>") };

const Device::Data *Device::Lister::data(const QString &device) const
{
  for (ConstIterator it=begin(); it!=end(); it++) {
    const Data *data = static_cast<const Data *>(it.data()->deviceData(device).data);
    if (data) return data;
  }
  return 0;
}

QString Device::Lister::checkName(const QString &device) const
{
  if ( device==AUTO_DATA.name ) return device;
  if ( isSupported(device) ) return device;
  if ( device.startsWith("p") ) // compat mode for PiKdev
    return checkName(device.mid(1));
  return "16F871"; // default...
}

namespace Device
{
  Lister _lister;
  const Lister &lister() { return _lister; }
}
