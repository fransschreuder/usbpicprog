/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SDCC_H
#define SDCC_H

#include "tools/list/tool_list.h"
#include "common/global/pfile.h"

namespace SDCC
{
//----------------------------------------------------------------------------
  enum Family { P14 = 0, P16, Nb_Families };
  struct FamilyData {
    const char *name;
  };
  extern const FamilyData FAMILY_DATA[Nb_Families];
  extern Family family(const QString &device);

//----------------------------------------------------------------------------
class Base : public Tool::Base
{
public:
  virtual QString baseExecutable(bool) const { return "sdcc"; }

private:
  virtual QStringList checkExecutableOptions(bool) const { return "-v"; }
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const;
};

//----------------------------------------------------------------------------
class Group : public Tool::Group
{
public:
  Group();
  virtual QString name() const { return "sdcc"; }
  virtual QString label() const { return i18n("Small Device C Compiler"); }
  virtual QString downloadText() const;
  virtual const ::Tool::Base *base(Tool::Category category) const;
  virtual uint nbCheckDevices() const { return SDCC::Nb_Families; }
  virtual bool hasDirectory(Compile::DirectoryType type) const { return type==Compile::HeaderDirectory || type==Compile::LinkerScriptDirectory; }
  virtual PURL::FileType linkerScriptType() const { return PURL::Lkr; }
  virtual PURL::Url linkerScript(const ::Project *project, Compile::LinkType type) const { return Tool::lister().group("gputils")->linkerScript(project, type); }
  virtual PURL::Directory autodetectDirectory(Compile::DirectoryType type, const PURL::Directory &execDir, bool withWine) const;
  virtual Tool::ExecutableType preferedExecutableType() const { return Tool::Unix; }
  virtual Tool::CompileType compileType() const { return Tool::SeparateFiles; }
  virtual Tool::Category checkDevicesCategory() const { return Tool::Compiler; }
  virtual QStringList checkDevicesOptions(uint i) const;
  virtual bool needs(Tool::ActionType type, Tool::Category category) const;
  virtual PURL::FileType implementationType(PURL::ToolType type) const;

private:
  PURL::TempFile _checkDevicesTmp;
  virtual Tool::Base *baseFactory(Tool::Category category) const;
  virtual QValueList<const Device::Data *> getSupportedDevices(const QString &s) const;
  virtual Compile::Process *processFactory(const Compile::Data &data) const;
  virtual ToolConfigWidget *configWidgetFactory(Tool::Category category, ::Project *project) const;
  virtual Compile::Config *configFactory(::Project *project) const;
  virtual Tool::SourceGenerator *sourceGeneratorFactory() const;
};

} // namespace

#endif
