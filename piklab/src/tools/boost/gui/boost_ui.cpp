/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "boost_ui.h"

//----------------------------------------------------------------------------
Boost::ConfigWidget::ConfigWidget(Project *project)
  : ToolConfigWidget(project, "boost_config_widget")
{}

void Boost::ConfigWidget::initEntries()
{
  createIncludeDirectoriesEntry();
}

//----------------------------------------------------------------------------
ToolConfigWidget *Boost::GroupUI::configWidgetFactory(Tool::Category category, Project *project) const
{
  if ( category==Tool::Category::Compiler || category==Tool::Category::Linker ) return new Boost::ConfigWidget(project);
  return 0;
}
