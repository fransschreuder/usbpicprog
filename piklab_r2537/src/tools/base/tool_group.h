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

#include "common/global/purl.h"
#include "common/common/group.h"
#include "common/common/version_data.h"
#include "generic_tool.h"
#include "coff/base/disassembler.h"
namespace Compile { class Manager; }

namespace Tool
{
enum CompileType { SeparateFiles, AllFiles, SingleFile };
class SourceGenerator;

//-----------------------------------------------------------------------------
class Group : public ::Group::Base
{
public:
  static const char *CUSTOM_NAME;
  Group();
  bool isCustom() const { return ( name()==CUSTOM_NAME ); }
  virtual QString comment() const { return QString::null; }
  virtual void init();
  virtual const ::Tool::Base *base(Category category) const { return _bases[category]; }
  virtual QString informationText() const = 0;
  virtual ExecutableType preferedExecutableType() const = 0;
  virtual bool hasDirectory(Compile::DirectoryType) const { return false; }
  virtual PURL::FileType linkerScriptType() const { return PURL::Nb_FileTypes; }
  virtual PURL::Directory autodetectDirectory(Compile::DirectoryType type, const PURL::Directory &execDir, bool withWine) const;
  virtual bool hasOutputType(Tool::OutputType type) const { return ( type==Tool::OutputType::Coff ); }
  virtual uint nbCheckDevices() const { return 1; }
  bool hasCheckDevicesError() const { return _checkDevicesError; }
  virtual Tool::Category checkDevicesCategory() const = 0;
  virtual QStringList checkDevicesOptions(uint) const { return QStringList(); }
  ::Process::LineOutput *checkDevicesProcess(uint i, const PURL::Directory &execDir, bool withWine) const;
  virtual QValueList<const Device::Data *> getSupportedDevices(const QString &s) const = 0;
  virtual CompileType compileType() const = 0;
  virtual bool needs(ActionType type, Category category) const = 0;
  Compile::Process *createCompileProcess(const Compile::Data &data, Compile::Manager *manager) const;
  Compile::Config *createConfig(::Project *project) const;
  bool hasCustomLinkerScript(const ::Project *project) const;
  virtual PURL::Url linkerScript(const ::Project *project, Compile::LinkType type) const;
  virtual PURL::FileType implementationType(PURL::ToolType type) const = 0;
  virtual Compile::Process *processFactory(const Compile::Data &data) const = 0;
  const SourceGenerator *sourceGenerator() const { return _sourceGenerator; }
  bool check(const QString &device, Log::Generic *log) const;
  const VersionData &version() const { return _version; }
  virtual bool generateDebugInformation(const QString &device) const { Q_UNUSED(device); return true; }

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
  QMap<Category, ::Tool::Base *> _bases;
  bool             _checkDevicesError;
  VersionData      _version;
};

} // namespace

#endif
