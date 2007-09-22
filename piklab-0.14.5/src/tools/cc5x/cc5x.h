/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CC5X_H
#define CC5X_H

#include "tools/base/tool_group.h"
#include "common/global/pfile.h"

namespace CC5X
{
//----------------------------------------------------------------------------
class Base : public Tool::Base
{
public:
  virtual QString baseExecutable(bool) const { return "cc5x"; }

private:
  virtual QStringList checkExecutableOptions(bool) const { return QStringList(); }
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const;
};

//----------------------------------------------------------------------------
class Group : public Tool::Group
{
public:
  virtual QString name() const { return "cc5x"; }
  virtual QString label() const { return i18n("CC5X Compiler"); }
  virtual QString downloadText() const;
  virtual Tool::Category checkDevicesCategory() const { return Tool::Nb_Categories; }
  virtual Tool::ExecutableType preferedExecutableType() const { return Tool::Windows; }
  virtual Tool::CompileType compileType() const { return Tool::SingleFile; }
  virtual bool needs(Tool::ActionType, Tool::Category category) const { return ( category==Tool::Compiler ); }
  virtual PURL::FileType implementationType(PURL::ToolType type) const { return (type==PURL::Compiler ? PURL::CSource : PURL::Nb_FileTypes); }

private:
  virtual QValueList<const Device::Data *> getSupportedDevices(const QString &s) const;
  virtual Compile::Process *processFactory(const Compile::Data &data) const;
  virtual Compile::Config *configFactory(::Project *project) const;
  virtual ToolConfigWidget *configWidgetFactory(Tool::Category category, ::Project *project) const;
  virtual Tool::Base *baseFactory(Tool::Category) const;
  virtual Tool::SourceGenerator *sourceGeneratorFactory() const { /*TODO*/ return 0; }
};

} // namespace

#endif
