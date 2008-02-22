/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "tool_group_ui.h"

#include "tool_config_widget.h"
#include "toolchain_config_widget.h"

ToolchainConfigWidget *Tool::GroupUI::toolchainConfigWidgetFactory(QWidget *parent) const
{
  return new ToolchainConfigWidget(static_cast<const Group &>(group()), parent);
}

ToolConfigWidget *Tool::GroupUI::createConfigWidget(Category category, ::Project *project) const
{
  ToolConfigWidget *cw = configWidgetFactory(category, project);
  if (cw) cw->init(category, static_cast<const Group &>(group()));
  return cw;
}
