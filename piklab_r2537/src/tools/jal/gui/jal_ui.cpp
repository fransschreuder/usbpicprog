/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "jal_ui.h"

//----------------------------------------------------------------------------
JAL::ConfigWidget::ConfigWidget(Project *project)
  : ToolConfigWidget(project, "jal_config_widget")
{}

//----------------------------------------------------------------------------
ToolConfigWidget *JAL::GroupUI::configWidgetFactory(Tool::Category category, Project *project) const
{
  if ( category==Tool::Category::Compiler ) return new ConfigWidget(project);
  return 0;
}
