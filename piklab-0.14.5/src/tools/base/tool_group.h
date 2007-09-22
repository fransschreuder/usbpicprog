/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TOOL_GROUP_H
#define TOOL_GROUP_H

#include <qstringlist.h>
#include <klocale.h>

#include "common/global/purl.h"
#include "common/common/group.h"
#include "generic_tool.h"
#include "coff/base/disassembler.h"
class ToolConfigWidget;
class ToolchainConfigWidget;
namespace Compile { class Manager; }

namespace Tool
{
enum CompileType { SeparateFiles, AllFiles, SingleFile };

//-----------------------------------------------------------------------------
class SourceGenerator
{
public:
  SourceGenerator() {}
  virtual ~SourceGenerator() {}
  virtual SourceLine::List configLines(PURL::ToolType type, const Device::Memory &memory, bool &ok) const = 0;
  SourceLine::List templateSourceFile(PURL::ToolType type, const Device::Data &data, bool &ok) const;
  virtual SourceLine::List sourceFileContent(PURL::ToolType type, const Device::Data &data, bool &ok) const = 0;
  virtual SourceLine::List includeLines(PURL::ToolType type, const Device::Data &data) const = 0;
};

//-----------------------------------------------------------------------------
class Group : public ::Group::Base
{
public:
  Group();
  virtual void init();
  virtual const ::Tool::Base *base(Category category) const { return _bases[category]; }
  virtual QString downloadText() const = 0;
  virtual ExecutableType preferedExecutableType() const = 0;
  virtual bool hasDirectory(Compile::DirectoryType) const { return false; }
  virtual PURL::FileType linkerScriptType() const { return PURL::Nb_FileTypes; }
  virtual PURL::Directory autodetectDirectory(Compile::DirectoryType type, const PURL::Directory &execDir, bool withWine) const;
  bool withWine() const;
  virtual uint nbCheckDevices() const { return 1; }
  bool hasCheckDevicesError() const { return _checkDevicesError; }
  virtual Tool::Category checkDevicesCategory() const = 0;
  virtual QStringList checkDevicesOptions(uint) const { return QStringList(); }
  ::Process::LineOutput *checkDevicesProcess(uint i, const PURL::Directory &execDir, bool withWine) const;
  virtual QValueList<const Device::Data *> getSupportedDevices(const QString &s) const = 0;
  virtual CompileType compileType() const = 0;
  virtual bool needs(ActionType type, Category category) const = 0;
  Compile::Process *createCompileProcess(const Compile::Data &data, Compile::Manager *manager) const;
  ToolConfigWidget *createConfigWidget(Category category, ::Project *project) const;
  Compile::Config *createConfig(::Project *project) const;
  bool hasCustomLinkerScript(const ::Project *project) const;
  virtual PURL::Url linkerScript(const ::Project *project, Compile::LinkType type) const;
  virtual PURL::FileType implementationType(PURL::ToolType type) const = 0;
  virtual Compile::Process *processFactory(const Compile::Data &data) const = 0;
  virtual ToolConfigWidget *configWidgetFactory(Category category, ::Project *project) const = 0;
  virtual ToolchainConfigWidget *toolchainConfigWidgetFactory(QWidget *parent) const;
  const SourceGenerator *sourceGenerator() const { return _sourceGenerator; }
  bool check(const QString &device = QString::null) const;
  const VersionData &version() const { return _version; }

protected:
  virtual void initSupported();
  virtual Tool::Base *baseFactory(Category category) const = 0;
  virtual QString defaultLinkerScriptFilename(Compile::LinkType type, const QString &device) const;
  virtual Compile::Config *configFactory(::Project *project) const = 0;
  virtual SourceGenerator *sourceGeneratorFactory() const = 0;
  bool checkExecutable(Tool::Category category, QStringList &lines);
  virtual VersionData getToolchainVersion() { return VersionData(); }

private:
  SourceGenerator *_sourceGenerator;
  ::Tool::Base    *_bases[Nb_Categories];
  bool             _checkDevicesError;
  VersionData      _version;
};

} // namespace

#endif
