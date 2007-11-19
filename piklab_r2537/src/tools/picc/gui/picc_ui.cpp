/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "picc_ui.h"

//----------------------------------------------------------------------------
PICC::ConfigWidget::ConfigWidget(Project *project)
  : ToolConfigWidget(project, "picc_config_widget")
{}

void PICC::ConfigWidget::initEntries()
{
  if ( _category!=Tool::Category::Linker ) createIncludeDirectoriesEntry();
}

//----------------------------------------------------------------------------
ToolConfigWidget *PICC::GroupUI::configWidgetFactory(Tool::Category category, Project *project) const
{
  if ( category==Tool::Category::Compiler || category==Tool::Category::Assembler || (category==Tool::Category::Linker && project))
    return new PICC::ConfigWidget(project);
  return 0;
}
