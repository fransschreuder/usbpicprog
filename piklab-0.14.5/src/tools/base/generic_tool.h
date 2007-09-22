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

#include <qstringlist.h>
#include <qmap.h>
#include <klocale.h>

#include "common/global/purl.h"
namespace Device { class Data; class Memory; }
namespace Process { class LineOutput; }
class Project;

namespace Tool
{
//----------------------------------------------------------------------------
  enum Category { Compiler = 0, Assembler, Linker, BinToHex, Preprocessor, Nb_Categories };
  struct CategoryData {
    const char *name, *label, *title;
  };
  extern const CategoryData CATEGORY_DATA[Nb_Categories];

  enum ExecutableType { Unix = 0, Windows, Nb_ExecutableTypes };
  struct ExecutableTypeData {
    const char *label;
  };
  extern const ExecutableTypeData EXECUTABLE_TYPE_DATA[Nb_ExecutableTypes];
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

  enum DirectoryType { ExecutableDirectory = 0, HeaderDirectory, LinkerScriptDirectory, LibraryDirectory, SourceDirectory, Nb_DirectoryTypes };
  struct DirectoryTypeData {
    const char *key, *label;
  };
  extern const DirectoryTypeData DIRECTORY_TYPE_DATA[Nb_DirectoryTypes];

} // namespace

namespace Tool
{
  enum ActionType { StandaloneFile, Project };
  class Group;

//-----------------------------------------------------------------------------
class Base
{
public:
  virtual ~Base() {}
  virtual QString baseExecutable(bool withWine) const = 0;
  PURL::Directory executableDirectory() const;
  virtual bool checkExecutable() const { return true; }
  virtual QStringList checkExecutableOptions(bool withWine) const = 0;
  virtual PURL::Directory checkExecutableWorkingDirectory() const { return PURL::Directory(); }
  virtual ::Process::LineOutput *checkExecutableProcess(const PURL::Directory &dir, bool withWine) const;
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const = 0;

protected:
  Category     _category;
  const Group *_group;

  friend class Group;
};

} // namespace

#endif
