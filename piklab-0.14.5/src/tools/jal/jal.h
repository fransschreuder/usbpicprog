/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef JAL_H
#define JAL_H

#include "tools/base/tool_group.h"

namespace JAL
{
//----------------------------------------------------------------------------
class Base : public Tool::Base
{
public:
  virtual QString baseExecutable(bool) const { return "jal"; }

private:
  virtual QStringList checkExecutableOptions(bool) const { return QStringList(); }
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const;
};

//----------------------------------------------------------------------------
class Group : public Tool::Group
{
public:
  virtual QString name() const { return "jal"; }
  virtual QString label() const { return i18n("JAL"); }
  virtual QString downloadText() const;
  virtual Tool::Category checkDevicesCategory() const { return Tool::Nb_Categories; }
  virtual Tool::ExecutableType preferedExecutableType() const { return Tool::Unix; }
  virtual Tool::CompileType compileType() const { return Tool::AllFiles; }
  virtual bool needs(Tool::ActionType, Tool::Category) const { return false; }
  virtual PURL::FileType implementationType(PURL::ToolType type) const { return (type==PURL::Compiler ? PURL::JalSource : PURL::Nb_FileTypes); }

private:
  virtual Tool::Base *baseFactory(Tool::Category c) const;
  virtual QValueList<const Device::Data *> getSupportedDevices(const QString &s) const;
  virtual Compile::Process *processFactory(const Compile::Data &data) const;
  virtual ToolConfigWidget *configWidgetFactory(Tool::Category category, ::Project *project) const;
  virtual Compile::Config *configFactory(::Project *project) const;
  virtual Tool::SourceGenerator *sourceGeneratorFactory() const;
};

} // namespace

#endif
