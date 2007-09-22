/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "usbpicprog_prog.h"

#include "devices/base/device_group.h"
#include "devices/list/device_list.h"
#include "progs/base/prog_config.h"
#include "usbpicprog_prog_config.h"
#include "usbpicprog_mem24.h"
//#include "usbpicprog_30.h"

QString Usbpicprog::DGroup::statusLabel() const
{
  ::Programmer::GroupConfig config(*this);
  return label() + " (" + i18n(Port::DATA[config.portType()].label) + ")";
}

void Usbpicprog::DGroup::initSupported()
{
  Group::initSupported();
  //  const QMap<QString, ::Group::DeviceData> &pics = Device::lister().group("pic")->devices();
  QMap<QString, ::Group::DeviceData>::const_iterator pit;
  //  for (pit=pics.begin(); pit!=pics.end(); ++pit) {
  //    ::Group::DeviceData data = pit.data();
  //    if ( static_cast<const Pic::Data *>(data.data)->architecture()!=Pic::Arch_30X ) continue;
  //    data.supportType = ::Group::Untested;
  //    addDevice(data);
  //  }
  const QMap<QString, ::Group::DeviceData> &mem24s = Device::lister().group("mem24")->devices();
  for (pit=mem24s.begin(); pit!=mem24s.end(); ++pit) addDevice(pit.data());
}

::Programmer::Base *Usbpicprog::DGroup::createBase(const Device::Data *data) const
{
  if ( data->group().name()=="pic" ) return new PicBase(*this, static_cast<const Pic::Data *>(data));
  return new Mem24Base(*this, static_cast<const Mem24::Data *>(data));
}

::Programmer::Hardware *Usbpicprog::DGroup::createHardware(::Programmer::Base &base, const Port::Description &pd) const
{
  Config config;
  QString current = config.currentHardware(pd.type);
  HardwareData *hdata = static_cast<HardwareData *>(config.hardwareData(current));
  HData data = hdata->data;
  delete hdata;
  if ( pd.type==Port::SerialType ) return new SerialHardware(base, pd.device, data);
  return new ParallelHardware(base, pd.device, data);
}

::Programmer::DeviceSpecific *Usbpicprog::DGroup::createDeviceSpecific(::Programmer::Base &base) const
{
  if ( base.device()->group().name()=="pic" ) {
    //    if ( static_cast<const Pic::Data *>(base.device())->architecture()==Pic::Arch_30X ) return new Pic30(base);
    return Group::createDeviceSpecific(base);
  }
  return new Mem24DeviceSpecific(base);
}
