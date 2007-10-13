/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MPC_H
#define MPC_H

#include "tools/base/tool_group.h"
#include "common/global/pfile.h"

namespace MPC
{
//----------------------------------------------------------------------------
class Base : public Tool::Base
{
public:
  virtual QString baseExecutable(bool, Tool::OutputType) const { return "mpcw"; }
  virtual bool checkExecutable() const { return false; }
  virtual QStringList checkExecutableOptions(bool) const { return QStringList(); }
  virtual bool checkExecutableResult(bool, QStringList &) const { return true; }
};

//----------------------------------------------------------------------------
class Group : public Tool::Group
{
public:
  virtual QString name() const { return "mpcw"; }
  virtual QString label() const { return i18n("MPC Compiler"); }
  virtual QString informationText() const;
  virtual Tool::Category checkDevicesCategory() const { return Tool::Category::Nb_Types; }
  virtual Tool::ExecutableType preferedExecutableType() const { return Tool::ExecutableType::Windows; }
  virtual Tool::CompileType compileType() const { return Tool::SingleFile; }
  virtual bool needs(Tool::ActionType, Tool::Category category) const { return ( category==Tool::Category::Compiler ); }
  virtual PURL::FileType implementationType(PURL::ToolType type) const { return (type==PURL::ToolType::Compiler ? PURL::CSource : PURL::Nb_FileTypes); }

private:
  virtual QValueList<const Device::Data *> getSupportedDevices(const QString &s) const;
  virtual Compile::Process *processFactory(const Compile::Data &data) const;
  virtual Compile::Config *configFactory(::Project *project) const;
  virtual Tool::Base *baseFactory(Tool::Category) const;
  virtual Tool::SourceGenerator *sourceGeneratorFactory() const { /*TODO*/ return 0; }
};

} // namespace

#endif
