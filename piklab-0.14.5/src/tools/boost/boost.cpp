/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "boost.h"

#include "boostc_compile.h"
#include "boostc_config.h"
#include "devices/pic/pic/pic_memory.h"
#include "devices/list/device_list.h"
#include "devices/base/device_group.h"

//----------------------------------------------------------------------------
bool Boost::Linker::checkExecutableResult(bool, QStringList &lines) const
{
  return ( lines.count()>0 && lines[0].startsWith("BoostLink ") );
}

PURL::Directory Boost::Linker::checkExecutableWorkingDirectory() const
{
  return static_cast<const Group *>(_group)->checkExecutableUrl().directory();
}

QStringList Boost::Linker16::checkExecutableOptions(bool) const
{
  // #### otherwise may stall...
  QStringList args;
  args += "-t PIC16F873";
  args += static_cast<const Group *>(_group)->checkExecutableUrl().toExtension("obj").filename();
  return args;
}

QStringList Boost::Linker18::checkExecutableOptions(bool) const
{
  // #### otherwise may stall...
  QStringList args;
  args += "-t PIC18F452";
  args += static_cast<const Group *>(_group)->checkExecutableUrl().toExtension("obj").filename();
  return args;
}

//----------------------------------------------------------------------------
QStringList Boost::Compiler::checkExecutableOptions(bool) const
{
  // #### otherwise may stall...
  return static_cast<const Group *>(_group)->checkExecutableUrl().filename();
}

PURL::Directory Boost::Compiler::checkExecutableWorkingDirectory() const
{
  return static_cast<const Group *>(_group)->checkExecutableUrl().directory();
}

//----------------------------------------------------------------------------
Boost::Group::Group(const QString &extension, const QString &text)
  : _checkExecTmp(0, extension)
{
  if ( _checkExecTmp.openForWrite() ) _checkExecTmp.appendText(text);
  _checkExecTmp.close();
}

PURL::Directory Boost::Group::autodetectDirectory(Compile::DirectoryType type, const PURL::Directory &execDir, bool) const
{
  if ( type==Compile::LibraryDirectory ) return execDir.path(false) + "Lib";
  return PURL::Directory();
}

Compile::Process *Boost::Group::processFactory(const Compile::Data &data) const
{
  switch (data.category) {
    case Tool::Compiler: return new Boost::CompileFile;
    case Tool::Linker: return new Boost::Link;
    default: break;
  }
  Q_ASSERT(false);
  return 0;
}

ToolConfigWidget *Boost::Group::configWidgetFactory(Tool::Category category, Project *project) const
{
  if ( category==Tool::Compiler || category==Tool::Linker ) return new Boost::ConfigWidget(project);
  return 0;
}

Compile::Config *Boost::Group::configFactory(::Project *project) const
{
  return new Boost::Config(project);
}

QValueList<const Device::Data *> Boost::Group::getSupportedDevices(const QString &) const
{
  QValueList<const Device::Data *> list;
  QValueVector<QString> devices = Device::lister().group("pic")->supportedDevices();
  for (uint i=0; i<devices.count(); i++) {
    const Device::Data *data = Device::lister().data(devices[i]);
    if ( supportedArchitecture(static_cast<const Pic::Data *>(data)->architecture()) ) list.append(data);
  }
  return list;
}
