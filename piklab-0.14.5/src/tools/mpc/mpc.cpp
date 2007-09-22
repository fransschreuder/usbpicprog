/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "mpc.h"

#include <qregexp.h>

#include "mpc_compile.h"
#include "mpc_config.h"
#include "devices/pic/pic/pic_memory.h"
#include "devices/list/device_list.h"
#include "devices/base/device_group.h"

//----------------------------------------------------------------------------
QValueList<const Device::Data *> MPC::Group::getSupportedDevices(const QString &) const
{
  QValueList<const Device::Data *> list;
  QValueVector<QString> devices = Device::lister().group("pic")->supportedDevices();
  for (uint i=0; i<devices.count(); i++) {
    const Device::Data *data = Device::lister().data(devices[i]);
    Pic::Architecture arch = static_cast<const Pic::Data *>(data)->architecture();
    if ( arch!=Pic::Arch_16X && arch!=Pic::Arch_17X ) continue;
    list.append(data);
  }
  return list;
}

Compile::Process *MPC::Group::processFactory(const Compile::Data &data) const
{
  switch (data.category) {
    case Tool::Compiler: return new MPC::CompileFile;
    default: break;
  }
  Q_ASSERT(false);
  return 0;
}

ToolConfigWidget *MPC::Group::configWidgetFactory(Tool::Category category, Project *project) const
{
  if ( category==Tool::Compiler ) return new MPC::ConfigWidget(project);
  return 0;
}

Compile::Config *MPC::Group::configFactory(::Project *project) const
{
  return new Config(project);
}

QString MPC::Group::downloadText() const
{
  return i18n("<a href=\"http://www.bytecraft.com/mpccaps.html\">MPC Compiler</a> is a C compiler distributed by Byte Craft.");
}

Tool::Base *MPC::Group::baseFactory(Tool::Category category) const
{
  if ( category==Tool::Compiler ) return new MPC::Base;
  return 0;
}
