/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "device_test.h"

#include "devices/base/device_group.h"
#include "devices/list/device_list.h"

void DeviceTest::checkArguments()
{
  if ( _args->count()==1 ) {
    _device = QString(_args->arg(0)).upper();
    if ( !Device::lister().isSupported(_device) ) qFatal("Specified device \"%s\" not supported.", _device.latin1());
    printf("Testing only %s\n", _device.latin1());
  }
}

bool DeviceTest::execute()
{
  Device::Lister::ConstIterator it;
  for (it=Device::lister().begin(); it!=Device::lister().end(); ++it) {
    Group::Base::ConstIterator git;
    for (git=it.data()->begin(); git!=it.data()->end(); ++git) {
      const Device::Data &data = *git.data().data;
      if ( !_device.isEmpty() && data.name()!=_device ) continue;
      _message = data.name();
      if ( skip(data) ) {
        skipped();
        printf("S");
      } else {
        if ( init(data) ) {
          printf("*");
          execute(data);
        } else printf("S");
        cleanup(data);
      }
      fflush(stdout);
    }
  }
  return true;
}

Piklab::OptionList DeviceTest::optionList() const
{
  Piklab::OptionList optionList;
  const KCmdLineOptions OPTION = { "+[device]", "Only check the specified device", 0 };
  optionList.append(OPTION);
  return optionList;
}
