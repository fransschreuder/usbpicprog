/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "mpc_ui.h"

//----------------------------------------------------------------------------
MPC::ConfigWidget::ConfigWidget(Project *project)
  : ToolConfigWidget(project, "mpc_config_widget")
{}

//----------------------------------------------------------------------------
ToolConfigWidget *MPC::GroupUI::configWidgetFactory(Tool::Category category, Project *project) const
{
  if ( category==Tool::Category::Compiler ) return new MPC::ConfigWidget(project);
  return 0;
}
