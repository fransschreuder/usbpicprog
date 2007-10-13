/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "cc5x.h"

#include <qregexp.h>

#include "cc5x_compile.h"
#include "cc5x_config.h"
#include "devices/pic/pic/pic_memory.h"
#include "devices/list/device_list.h"
#include "devices/base/device_group.h"

//----------------------------------------------------------------------------
bool CC5X::Base::checkExecutableResult(bool, QStringList &lines) const
{
  return ( lines.count()>0 && lines[0].startsWith("CC5X") );
}

//----------------------------------------------------------------------------
QValueList<const Device::Data *> CC5X::Group::getSupportedDevices(const QString &) const
{
  QValueList<const Device::Data *> list;
  QValueVector<QString> devices = Device::lister().group("pic")->supportedDevices();
  for (uint i=0; i<devices.count(); i++) {
    const Pic::Data *data = static_cast<const Pic::Data *>(Device::lister().data(devices[i]));
    if ( data->architecture()!=Pic::Architecture::P10X && data->architecture()!=Pic::Architecture::P16X ) continue;
    list.append(data);
  }
  return list;
}

Compile::Process *CC5X::Group::processFactory(const Compile::Data &data) const
{
  switch (data.category) {
    case Tool::Category::Compiler: return new CC5X::CompileFile;
    default: break;
  }
  Q_ASSERT(false);
  return 0;
}

Compile::Config *CC5X::Group::configFactory(::Project *project) const
{
  return new Config(project);
}

QString CC5X::Group::informationText() const
{
  return i18n("<a href=\"%1\">CC5X</a> is a C compiler distributed by B Knudsen Data.").arg("http://www.bknd.com/cc5x/index.shtml");
}

Tool::Base *CC5X::Group::baseFactory(Tool::Category category) const
{
  if ( category==Tool::Category::Compiler ) return new CC5X::Base;
  return 0;
}
