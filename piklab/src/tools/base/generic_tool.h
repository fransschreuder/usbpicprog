/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GENERIC_TOOL_H
#define GENERIC_TOOL_H

#include "common/global/purl.h"
#include "common/common/key_enum.h"
namespace Device { class Data; class Memory; }
namespace Process { class LineOutput; }
class Project;

namespace Tool
{
//----------------------------------------------------------------------------
  struct CategoryData {
    const char *key, *label, *title;
  };
  BEGIN_DECLARE_ENUM(Category)
    Compiler = 0, Assembler, Linker, BinToHex
  END_DECLARE_ENUM(Category, CategoryData)

  BEGIN_DECLARE_ENUM(ExecutableType)
    Unix = 0, Windows
  END_DECLARE_ENUM(ExecutableType, EnumData)
} // namespace

namespace Compile
{
  enum LinkType { NormalLinking, Icd2Linking };
  class Process;
  class Config;

  class TodoItem {
  public:
    TodoItem() {}
    TodoItem(const PURL::Url &_url, bool _generated) : url(_url), generated(_generated) {}
    PURL::Url url;
    bool generated;
  };

  class Data {
  public:
    Data() {}
    Data(Tool::Category c, const QValueList<TodoItem> &i, const QString &d, const Project *p, LinkType lt)
      : category(c), items(i), device(d), project(p), linkType(lt) {}
    Tool::Category category;
    QValueList<TodoItem> items;
    QString  device;
    const Project *project;
    LinkType linkType;
  };

  BEGIN_DECLARE_ENUM(DirectoryType)
    Executable = 0, Header, LinkerScript, Library, Source
  END_DECLARE_ENUM(DirectoryType, EnumData)

} // namespace

namespace Tool
{
  enum ActionType { StandaloneFile, Project };
  class Group;

  struct OutputTypeData {
    const char     *label, *key;
    PURL::FileType  type;
  };
  BEGIN_DECLARE_ENUM(OutputType)
    Coff = 0, Elf
  END_DECLARE_ENUM(OutputType, OutputTypeData)

//-----------------------------------------------------------------------------
class Base
{
public:
  virtual ~Base() {}
  const Group &group() const { return *_group; }
  virtual QString baseExecutable(bool withWine, OutputType type) const = 0;
  PURL::Directory executableDirectory() const;
  virtual bool checkExecutable() const { return true; }
  virtual QStringList checkExecutableOptions(bool withWine) const = 0;
  virtual PURL::Directory checkExecutableWorkingDirectory() const { return PURL::Directory(); }
  virtual ::Process::LineOutput *checkExecutableProcess(const PURL::Directory &dir, bool withWine, OutputType type) const;
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const = 0;

protected:
  Category     _category;
  const Group *_group;

  friend class Group;
};

} // namespace

#endif
