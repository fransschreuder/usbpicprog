/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TOOL_GROUP_UI_H
#define TOOL_GROUP_UI_H

#include "tools/base/tool_group.h"
#include "tools/base/generic_tool.h"
class ToolConfigWidget;
class ToolchainConfigWidget;

namespace Tool
{

class GroupUI : public ::Group::BaseGui
{
public:
  ToolConfigWidget *createConfigWidget(Category category, ::Project *project) const;
  virtual ToolConfigWidget *configWidgetFactory(Category category, ::Project *project) const = 0;
  virtual ToolchainConfigWidget *toolchainConfigWidgetFactory(QWidget *parent) const;
};

inline const GroupUI &groupui(const Base &base) { return static_cast<const GroupUI &>(*base.group().gui()); }

} // namespace

#endif
