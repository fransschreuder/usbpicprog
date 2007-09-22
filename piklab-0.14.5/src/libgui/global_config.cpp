/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "global_config.h"

#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>

#include "progs/list/prog_list.h"
#include "tools/list/tool_list.h"

//-----------------------------------------------------------------------------
PURL::Url GlobalConfig::openedProject()
{
  return PURL::Url(readEntry("project", QString::null));
}
void GlobalConfig::writeOpenedProject(const PURL::Url &p)
{
  writeEntry("project", p.filepath(false));
}

PURL::UrlList GlobalConfig::openedFiles()
{
  PURL::UrlList files;
  uint i = 0;
  for (;;) {
    QString file = readEntry(QString("file%1").arg(i), QString::null);
    if ( file.isEmpty() ) break;
    files += PURL::Url(file);
    i++;
  }
  return files;
}
void GlobalConfig::writeOpenedFiles(const PURL::UrlList &files)
{
  for (uint i=0; i<=files.count(); i++) {
    QString s = (i==files.count() ? QString::null : files[i].filepath(false));
    writeEntry(QString("file%1").arg(i), s);
  }
}

void GlobalConfig::writeProgrammerGroup(const Programmer::Group &group)
{
  writeEntry("programmer", group.name());
}
const Programmer::Group &GlobalConfig::programmerGroup()
{
  QString s = readEntry("programmer");
  for (uint i=0; i<Programmer::lister().nbGroups(); i++) {
    const Programmer::Group &group = Programmer::lister().group(i);
    if ( s==group.name() ) return group;
  }
  return Programmer::lister().group(0);
}

void GlobalConfig::writeDebugLevel(Log::DebugLevel level)
{
  writeEntry("log_debug_level", level);
}
Log::DebugLevel GlobalConfig::debugLevel()
{
  uint level = readUIntEntry("log_debug_level", Log::Nb_DebugLevels);
  if ( level>Log::Nb_DebugLevels ) return Log::Nb_DebugLevels;
  return Log::DebugLevel(level);
}

void GlobalConfig::writeLogOutputType(Log::OutputType type)
{
  writeEntry("log_output_type", type);
}
Log::OutputType GlobalConfig::logOutputType()
{
  uint output = readUIntEntry("log_output_type", Log::GuiOnly);
  if ( output>=Log::Nb_OutputTypes ) return Log::GuiOnly;
  return Log::OutputType(output);
}

void GlobalConfig::writeProgramAfterBuild(bool program)
{
  writeEntry("program_after_build", program);
}
bool GlobalConfig::programAfterBuild()
{
  return readBoolEntry("program_after_build", false);
}

void GlobalConfig::writeUserIdSetToChecksum(bool set)
{
  writeEntry("user_id_set_to_checksum", set);
}
bool GlobalConfig::isUserIdSetToChecksum()
{
  return readBoolEntry("user_id_set_to_checksum", false);
}

void GlobalConfig::writeShowLineNumbers(bool show)
{
  writeEntry("show_line_numbers", show);
}
bool GlobalConfig::showLineNumbers()
{
  return readBoolEntry("show_line_numbers", false);
}

//-----------------------------------------------------------------------------
void DeviceChooser::Config::writeListType(Dialog::ListType type)
{
  writeEntry("list_type", type);
}
DeviceChooser::Dialog::ListType DeviceChooser::Config::listType()
{
  uint t = readUIntEntry("list_type", Dialog::FamilyTree);
  t = QMIN(t, Dialog::Nb_ListTypes-1);
  return Dialog::ListType(t);
}

void DeviceChooser::Config::writeProgrammerGroup(const Programmer::Group *group)
{
  writeEntry("programmer", group ? group->name() : QString::null);
}
const Programmer::Group *DeviceChooser::Config::programmerGroup()
{
  QString name = readEntry("programmer", QString::null);
  return Programmer::lister().group(name);
}

void DeviceChooser::Config::writeToolGroup(const Tool::Group *group)
{
  writeEntry("tool", group ? group->name() : QString::null);
}
const Tool::Group *DeviceChooser::Config::toolGroup()
{
  QString name = readEntry("tool", QString::null);
  return Tool::lister().group(name);
}

void DeviceChooser::Config::writeMemoryTechnology(Device::MemoryTechnology tech)
{
  writeEntry("memory_technology", tech==Device::Nb_MemoryTechnologies ? QString::null : QString(Device::MEMORY_TECHNOLOGY_DATA[tech].name));
}
Device::MemoryTechnology DeviceChooser::Config::memoryTechnology()
{
  QString name = readEntry("memory_technology", QString::null);
  for (uint i=0; i<Device::Nb_MemoryTechnologies; i++)
    if ( name==Device::MEMORY_TECHNOLOGY_DATA[i].name ) return Device::MemoryTechnology(i);
  return Device::Nb_MemoryTechnologies;
}
