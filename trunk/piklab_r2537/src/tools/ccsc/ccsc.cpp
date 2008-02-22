/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "ccsc.h"

#include <qregexp.h>

#include "ccsc_compile.h"
#include "ccsc_config.h"
#include "devices/pic/pic/pic_memory.h"
#include "devices/list/device_list.h"
#include "devices/base/device_group.h"

//----------------------------------------------------------------------------
QStringList CCSC::Base::checkExecutableOptions(bool withWine) const
{
  QStringList args;
  if (withWine) {
    args += "+STDOUT";
    args += "+FM";
    args += static_cast<const Group *>(_group)->checkExecutableUrl().filename();
  } else args += "+V";
  return args;
}

PURL::Directory CCSC::Base::checkExecutableWorkingDirectory() const
{
  return static_cast<const Group *>(_group)->checkExecutableUrl().directory();
}

bool CCSC::Base::checkExecutableResult(bool withWine, QStringList &lines) const
{
  if (withWine) {
    PURL::Url url = static_cast<const Group *>(_group)->checkExecutableUrl().toExtension("err");
    Log::StringView sview;
    PURL::File file(url, sview);
    if ( !file.openForRead() ) {
      url = static_cast<const Group *>(_group)->checkExecutableUrl().appendExtension("err");
      file.setUrl(url);
      if ( !file.openForRead() ) return false;
    }
    lines = file.readLines();
  }
  return true;
}

//----------------------------------------------------------------------------
CCSC::Group::Group()
  : _checkExecTmp(_sview, ".c")
{
  if ( _checkExecTmp.openForWrite() ) _checkExecTmp.appendText("#include <16f877a.h>\nvoid main(void) {}\n");
  _checkExecTmp.close();
}

QValueList<const Device::Data *> CCSC::Group::getSupportedDevices(const QString &) const
{
  QValueList<const Device::Data *> list;
  QValueVector<QString> devices = Device::lister().group("pic")->supportedDevices();
  for (uint i=0; i<devices.count(); i++) {
    const Device::Data *data = Device::lister().data(devices[i]);
    Pic::Architecture arch = static_cast<const Pic::Data *>(data)->architecture();
    if ( arch==Pic::Architecture::P30F || arch==Pic::Architecture::P33F ) continue;
    list.append(data);
  }
  return list;
}

Compile::Process *CCSC::Group::processFactory(const Compile::Data &data) const
{
  switch (data.category.type()) {
    case Tool::Category::Compiler: return new CCSC::CompileFile;
    default: break;
  }
  Q_ASSERT(false);
  return 0;
}

Compile::Config *CCSC::Group::configFactory(::Project *project) const
{
  return new Config(project);
}

QString CCSC::Group::informationText() const
{
  return i18n("<a href=\"%1\">CCS Compiler</a> is a C compiler distributed by CCS.").arg("http://www.ccsinfo.com/content.php?page=compilers");
}

Tool::Base *CCSC::Group::baseFactory(Tool::Category category) const
{
  if ( category==Tool::Category::Compiler ) return new CCSC::Base;
  return 0;
}

VersionData CCSC::Group::getToolchainVersion()
{
  if ( !Compile::Config::withWine(*this) ) {
    QStringList lines;
    if ( checkExecutable(Tool::Category::Compiler, lines) && lines.count()>=1 && lines[0].contains("3.") ) return VersionData(3, 0, 0);
  }
  return VersionData(4, 0, 0); // default
}
