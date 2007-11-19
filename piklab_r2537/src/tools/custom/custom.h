/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CUSTOM_TOOL_H
#define CUSTOM_TOOL_H

#include "tools/base/tool_group.h"

namespace CustomTool
{

class Group : public Tool::Group
{
public:
  virtual QString name() const { return CUSTOM_NAME; }
  virtual QString label() const { return i18n("Custom"); }
  virtual QString informationText() const { return QString::null; }
  virtual Tool::Category checkDevicesCategory() const { return Tool::Category::Nb_Types; }
  virtual Tool::ExecutableType preferedExecutableType() const { return Tool::ExecutableType::Unix; }
  virtual Tool::CompileType compileType() const { return Tool::AllFiles; }
  virtual bool needs(Tool::ActionType, Tool::Category) const { return false; }
  virtual PURL::FileType implementationType(PURL::ToolType) const { return PURL::Nb_FileTypes; }

protected:
  virtual Tool::Base *baseFactory(Tool::Category) const { return 0; }
  virtual QValueList<const Device::Data *> getSupportedDevices(const QString &s) const;
  virtual Compile::Process *processFactory(const Compile::Data &) const { return 0; }
  virtual Compile::Config *configFactory(::Project *) const { return 0; }
  virtual Tool::SourceGenerator *sourceGeneratorFactory() const { return 0; }
};

} // namespace

#endif
