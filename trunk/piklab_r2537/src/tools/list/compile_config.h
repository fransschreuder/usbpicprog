/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef COMPILE_CONFIG_H
#define COMPILE_CONFIG_H

#include "common/global/purl.h"
#include "devices/base/hex_buffer.h"
#include "tools/base/tool_group.h"
class Project;

//----------------------------------------------------------------------------
namespace Compile
{
class Config
{
public:
  Config(Project *project) : _group(0), _project(project) {}

  bool hasCustomArguments(Tool::Category category) const { return boolValue(category, "has_custom_arguments", false); }
  void setHasCustomArguments(Tool::Category category, bool custom) { setValue(category, "has_custom_arguments", custom); }
  QStringList customArguments(Tool::Category category) const { return listValues(category, "custom_arguments", QStringList()); }
  void setCustomArguments(Tool::Category category, const QStringList &args) { return setListValues(category, "custom_arguments", args); }
  QStringList rawIncludeDirs(Tool::Category category) const { return listValues(category, "includes", "$(SRCPATH)"); }
  QStringList includeDirs(Tool::Category category, const QString &prefix, const QString &suffix = QString::null, const QString &separator = QString::null) const;
  void setRawIncludeDirs(Tool::Category category, const QStringList &dirs) { setListValues(category, "includes", dirs); }
  QString rawCustomOptions(Tool::Category category) const { return value(category, "custom_options", QString::null); }
  QStringList customOptions(Tool::Category category) const;
  void setRawCustomOptions(Tool::Category category, const QString &value) { setValue(category, "custom_options", value); }
  QString rawCustomLibraries(Tool::Category category) const { return value(category, "custom_libraries", QString::null); }
  QStringList customLibraries(Tool::Category category) const;
  void setRawCustomLibraries(Tool::Category category, const QString &value) { setValue(category, "custom_libraries", value); }
  uint warningLevel(Tool::Category category) const { return uintValue(category, "warning_level", 0); }
  void setWarningLevel(Tool::Category category, uint level) { setValue(category, "warning_level", QString::number(level)); }
  HexBuffer::Format hexFormat() const;
  void setHexFormat(HexBuffer::Format format);

  static const Tool::Group &toolGroup(const Project *project);
  static void setToolGroup(Project *project, const Tool::Group &group) { setGlobalValue(project, "tool", group.name()); }
  static QString device(const Project *project);
  static void setDevice(Project *project, const QString &device) { setGlobalValue(project, "device", device); }
  static QStringList customCommands(Project *project) { return globalListValues(project, "custom_shell_commands", QStringList()); }
  static void setCustomCommands(Project *project, const QStringList &commands) { setGlobalListValues(project, "custom_shell_commands", commands); }
  static QString globalValue(const Project *project, const QString &key, const QString &defaultValue);
  static void setGlobalValue(Project *project, const QString &key, const QString &value);
  static QStringList globalListValues(const Project *project, const QString &key, const QStringList &defaultValues);
  static void setGlobalListValues(Project *project, const QString &key, const QStringList &value);

  static PURL::Directory directory(const Tool::Group &group, DirectoryType type);
  static void setDirectory(const Tool::Group &group, DirectoryType type, const PURL::Directory &dir);
  static bool withWine(const Tool::Group &group);
  static void setWithWine(const Tool::Group &group, bool withWine);
  static Tool::OutputType outputType(const Tool::Group &group);
  static void setOutputType(const Tool::Group &group, Tool::OutputType type);

protected:
  const Tool::Group *_group;
  Project *_project;

  void setValue(Tool::Category category, const QString &key, const QString &value);
  QString value(Tool::Category category, const QString &key, const QString &defaultValue) const;
  void setListValues(Tool::Category category, const QString &key, const QStringList &values);
  QStringList listValues(Tool::Category category, const QString &key, const QStringList &defaultValues) const;
  void setValue(Tool::Category category, const QString &key, bool value) { setValue(category, key, QString(value ? "true" : "false")); }
  bool boolValue(Tool::Category category, const QString &key, bool defaultValue) const;
  void setValue(Tool::Category category, const QString &key, uint value) { setValue(category, key, QString::number(value)); }
  uint uintValue(Tool::Category category, const QString &key, uint defaultValue) const;

  static QString value(const QString &group, const QString &subGroup, const QString &key, const QString &defaultValue);
  static void setValue(const QString &group, const QString &subGroup, const QString &key, const QString &value);
  static QStringList listValues(const QString &group, const QString &subGroup, const QString &key, const QStringList &defaultValues);
  static void setListValues(const QString &group, const QString &subGroup, const QString &key, const QStringList &values);

  friend class Tool::Group;
};

} // namespace

#endif
