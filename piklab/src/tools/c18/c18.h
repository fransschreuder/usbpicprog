/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef C18_H
#define C18_H

#include "tools/base/tool_group.h"

namespace C18
{
//----------------------------------------------------------------------------
class Compiler : public Tool::Base
{
public:
  virtual QString baseExecutable(bool, Tool::OutputType) const { return "mcc18"; }

private:
  virtual QStringList checkExecutableOptions(bool) const { return "-v"; }
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const;
};

class Linker : public Tool::Base
{
public:
  virtual QString baseExecutable(bool, Tool::OutputType) const { return "mplink"; }

private:
  virtual QStringList checkExecutableOptions(bool) const { return "/v"; }
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const;
};

//----------------------------------------------------------------------------
class Group : public Tool::Group
{
public:
  virtual QString name() const { return "c18"; }
  virtual QString label() const { return i18n("C18 Compiler"); }
  virtual QString informationText() const;
  virtual Tool::Category checkDevicesCategory() const { return Tool::Category::Nb_Types; }
  virtual bool hasDirectory(Compile::DirectoryType type) const { return ( type!=Compile::DirectoryType::Source ); }
  virtual PURL::FileType linkerScriptType() const { return PURL::Lkr; }
  virtual Tool::ExecutableType preferedExecutableType() const { return Tool::ExecutableType::Windows; }
  virtual Tool::CompileType compileType() const { return Tool::SeparateFiles; }
  virtual PURL::Directory autodetectDirectory(Compile::DirectoryType type, const PURL::Directory &execDir, bool withWine) const;
  virtual bool needs(Tool::ActionType, Tool::Category category) const { return ( category==Tool::Category::Linker ); }
  virtual PURL::FileType implementationType(PURL::ToolType type) const { return (type==PURL::ToolType::Compiler ? PURL::CSource : PURL::Nb_FileTypes); }

private:
  virtual QValueList<const Device::Data *> getSupportedDevices(const QString &s) const;
  virtual Compile::Process *processFactory(const Compile::Data &data) const;
  virtual Compile::Config *configFactory(::Project *project) const;
  virtual Tool::Base *baseFactory(Tool::Category category) const;
  virtual Tool::SourceGenerator *sourceGeneratorFactory() const { /*TODO*/ return 0; }
};

} // namespace

#endif
