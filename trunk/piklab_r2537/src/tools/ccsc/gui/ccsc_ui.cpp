/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "ccsc_ui.h"

//----------------------------------------------------------------------------
CCSC::ConfigWidget::ConfigWidget(Project *project)
  : ToolConfigWidget(project, "ccsc_config_widget")
{}

void CCSC::ConfigWidget::initEntries()
{
  if ( _category!=Tool::Category::Linker ) createIncludeDirectoriesEntry();
}

//----------------------------------------------------------------------------
ToolConfigWidget *CCSC::GroupUI::configWidgetFactory(Tool::Category category, Project *project) const
{
  if ( category==Tool::Category::Compiler ) return new CCSC::ConfigWidget(project);
  return 0;
}
