/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CCSC_H
#define CCSC_H

#include "tools/base/tool_group.h"
#include "common/global/pfile.h"

namespace CCSC
{
//----------------------------------------------------------------------------
class Base : public Tool::Base
{
public:
  virtual QString baseExecutable(bool, Tool::OutputType) const { return "ccsc"; }

private:
  virtual QStringList checkExecutableOptions(bool withWine) const;
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const;
  virtual PURL::Directory checkExecutableWorkingDirectory() const;
};

//----------------------------------------------------------------------------
class Group : public Tool::Group
{
public:
  Group();
  PURL::Url checkExecutableUrl() const { return _checkExecTmp.url(); }
  virtual QString name() const { return "ccsc"; }
  virtual QString label() const { return i18n("CCS Compiler"); }
  virtual QString informationText() const;
  virtual Tool::Category checkDevicesCategory() const { return Tool::Category::Nb_Types; }
  virtual Tool::ExecutableType preferedExecutableType() const { return Tool::ExecutableType::Unix; }
  virtual Tool::CompileType compileType() const { return Tool::SingleFile; }
  virtual bool needs(Tool::ActionType, Tool::Category category) const { return ( category==Tool::Category::Compiler ); }
  virtual PURL::FileType implementationType(PURL::ToolType type) const { return (type==PURL::ToolType::Compiler ? PURL::CSource : PURL::Nb_FileTypes); }

private:
  Log::StringView _sview;
  PURL::TempFile  _checkExecTmp;

  virtual QValueList<const Device::Data *> getSupportedDevices(const QString &s) const;
  virtual Compile::Process *processFactory(const Compile::Data &data) const;
  virtual Compile::Config *configFactory(::Project *project) const;
    virtual Tool::Base *baseFactory(Tool::Category) const;
  virtual Tool::SourceGenerator *sourceGeneratorFactory() const { /*TODO*/ return 0; }
  virtual VersionData getToolchainVersion();
};

} // namespace

#endif
