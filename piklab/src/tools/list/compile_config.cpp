/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "compile_config.h"

#include "common/global/generic_config.h"
#include "devices/list/device_list.h"
#include "libgui/project.h"
#include "tool_list.h"

Compile::Config::Config(Project *project)
  : _group(0), _project(project)
{}

QStringList Compile::Config::includeDirs(Tool::Category category, const QString &prefix, const QString &suffix, const QString &separator) const
{
  QStringList list;
  QStringList raw = rawIncludeDirs(category);
  for (uint i=0; i<raw.size(); i++) {
    if ( separator.isEmpty() ) list.append(prefix + raw[i] + suffix);
    else if ( i==0 ) list.append(prefix + raw[i]);
    else list[0] += separator + raw[i];
  }
  if ( !separator.isEmpty() && list.count()!=0 ) list[0] += suffix;
  return list;
}

HexBuffer::Format Compile::Config::hexFormat() const
{
  QString s = value(Tool::Category::Linker, "format", HexBuffer::FORMATS[HexBuffer::IHX32]);
  for (uint i=0; i<HexBuffer::Nb_Formats; i++)
    if ( s==HexBuffer::FORMATS[i] ) return HexBuffer::Format(i);
  return HexBuffer::Nb_Formats;
}
void Compile::Config::setHexFormat(HexBuffer::Format f)
{
  QString s = (f==HexBuffer::Nb_Formats ? 0 : HexBuffer::FORMATS[f]);
  setValue(Tool::Category::Linker, "format", s);
}

QString Compile::Config::device(const Project *project)
{
  QString device = globalValue(project, "device", QString::null);
  return Device::lister().checkName(device);
}

const Tool::Group &Compile::Config::toolGroup(const Project *project)
{
  QString s = globalValue(project, "tool", QString::null);
  const Tool::Group *group = Tool::lister().group(s);
  if ( group==0 ) return *Tool::lister().begin().data();
  return *group;
}

QStringList Compile::Config::customOptions(Tool::Category category) const
{
  return QStringList::split(' ', rawCustomOptions(category));
}

QStringList Compile::Config::customLibraries(Tool::Category category) const
{
  return QStringList::split(' ', rawCustomLibraries(category));
}

void Compile::Config::setValue(Tool::Category category, const QString &key, const QString &value)
{
  Q_ASSERT( category!=Tool::Category::Nb_Types );
  Q_ASSERT( _project || _group );
  if (_project) setValue(*_project, category.key(), key, value);
  else setValue(_group->name(), category.key(), key, value);
}
QString Compile::Config::value(Tool::Category category, const QString &key, const QString &defaultValue) const
{
  Q_ASSERT( category!=Tool::Category::Nb_Types );
  Q_ASSERT( _project || _group );
  if (_project) return value(*_project, category.key(), key, defaultValue);
  return value(_group->name(), category.key(), key, defaultValue);
}
void Compile::Config::setListValues(Tool::Category category, const QString &key, const QStringList &values)
{
  Q_ASSERT( category!=Tool::Category::Nb_Types );
  Q_ASSERT( _project || _group );
  if (_project) setListValues(*_project, category.key(), key, values);
  else setListValues(_group->name(), category.key(), key, values);
}
QStringList Compile::Config::listValues(Tool::Category category, const QString &key, const QStringList &defaultValues) const
{
  Q_ASSERT( category!=Tool::Category::Nb_Types );
  Q_ASSERT( _project || _group );
  if (_project) return listValues(*_project, category.key(), key, defaultValues);
  return listValues(_group->name(), category.key(), key, defaultValues);
}
bool Compile::Config::boolValue(Tool::Category category, const QString &key, bool defaultValue) const
{
  QString s = value(category, key, QString::null);
  if ( s.isNull() ) return defaultValue;
  return !( s=="false" || s=="0" );
}
uint Compile::Config::uintValue(Tool::Category category, const QString &key, uint defaultValue) const
{
  bool ok;
  uint i = value(category, key, QString::null).toUInt(&ok);
  if ( !ok ) return defaultValue;
  return i;
}

QString Compile::Config::globalValue(const Project *project, const QString &key, const QString &defaultValue)
{
  if (project) return value(*project, "general", key, defaultValue);
  return value("general", QString::null, key, defaultValue);
}
void Compile::Config::setGlobalValue(Project *project, const QString &key, const QString &value)
{
  if (project) setValue(*project, "general", key, value);
  else setValue("general", QString::null, key, value);
}

QString Compile::Config::value(const Project &project, const QString &subGroup, const QString &key, const QString &defaultValue)
{
  return  project.value(subGroup, key, defaultValue);
}
void Compile::Config::setValue(Project &project, const QString &subGroup, const QString &key, const QString &value)
{
  project.setValue(subGroup, key, value);
}
QStringList Compile::Config::listValues(const Project &project, const QString &subGroup, const QString &key, const QStringList &defaultValues)
{
  return project.listValues(subGroup, key, defaultValues);
}
void Compile::Config::setListValues(Project &project, const QString &subGroup, const QString &key, const QStringList &values)
{
  project.setListValues(subGroup, key, values);
}

QString Compile::Config::value(const QString &group, const QString &subGroup, const QString &key, const QString &defaultValue)
{
  QString grp = (subGroup.isEmpty() ? group : group + '-' + subGroup);
  GenericConfig gc(grp);
  return gc.readEntry(key, defaultValue);
}
void Compile::Config::setValue(const QString &group, const QString &subGroup, const QString &key, const QString &value)
{
  QString grp = (subGroup.isEmpty() ? group : group + '-' + subGroup);
  GenericConfig gc(grp);
  gc.writeEntry(key, value);
}
QStringList Compile::Config::listValues(const QString &group, const QString &subGroup, const QString &key, const QStringList &defaultValues)
{
  QString grp = (subGroup.isEmpty() ? group : group + '-' + subGroup);
  GenericConfig gc(grp);
  return gc.readListEntry(key, defaultValues);
}
void Compile::Config::setListValues(const QString &group, const QString &subGroup, const QString &key, const QStringList &values)
{
  QString grp = (subGroup.isEmpty() ? group : group + '-' + subGroup);
  GenericConfig gc(grp);
  gc.writeEntry(key, values);
}

QString Compile::Config::groupValue(const QString &key, const QString &defaultValue) const
{
  Q_ASSERT(_group);
  return value(_group->name(), QString::null, key, defaultValue);
}

void Compile::Config::setGroupValue(const QString &key, const QString &value)
{
  Q_ASSERT(_group);
  setValue(_group->name(), QString::null, key, value);
}

PURL::Directory Compile::Config::directory(DirectoryType type) const
{
  QString def;
  if ( type!=DirectoryType::Executable )
    def = _group->autodetectDirectory(type, directory(DirectoryType::Executable), withWine()).path();
  return PURL::Directory(groupValue(type.key() + QString("_path"), def));
}
void Compile::Config::setDirectory(DirectoryType type, const PURL::Directory &dir)
{
  setGroupValue(type.key() + QString("_path"), dir.path());
  if ( type==DirectoryType::Executable ) const_cast<Tool::Group *>(_group)->init();
}

bool Compile::Config::withWine() const
{
  QString def = (_group->preferedExecutableType()==Tool::ExecutableType::Unix ? "false" : "true");
  return ( groupValue("with_wine", def)=="true" );
}
void Compile::Config::setWithWine(bool withWine)
{
  setGroupValue("with_wine", withWine ? "true" : "false");
  const_cast<Tool::Group *>(_group)->init();
}

Tool::OutputType Compile::Config::outputType() const
{
  QString s = groupValue("output_type", Tool::OutputType(Tool::OutputType::Coff).key());
  return Tool::OutputType::fromKey(s);
}

void Compile::Config::setOutputType(Tool::OutputType type)
{
  setGroupValue("output_type", type.key());
  const_cast<Tool::Group *>(_group)->init();
}
