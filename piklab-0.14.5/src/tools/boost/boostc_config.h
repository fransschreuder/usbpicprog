/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BOOSTC_CONFIG_H
#define BOOSTC_CONFIG_H

#include "tools/list/compile_config.h"
#include "tools/base/tool_config_widget.h"

namespace Boost
{
//----------------------------------------------------------------------------
class Config : public Compile::Config
{
public:
  Config(Project *project) : Compile::Config(project) {}
};

//----------------------------------------------------------------------------
class ConfigWidget : public ToolConfigWidget
{
Q_OBJECT
public:
  ConfigWidget(Project *project);
  virtual void initEntries();
};

} // namespace

#endif
