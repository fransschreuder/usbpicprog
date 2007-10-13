/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "sdcc_ui.h"

//----------------------------------------------------------------------------
SDCC::CompilerConfigWidget::CompilerConfigWidget(Project *project)
  : ToolConfigWidget(project, "sdcc_compiler_config_widget")
{}

void SDCC::CompilerConfigWidget::initEntries()
{
  createIncludeDirectoriesEntry();
}

//----------------------------------------------------------------------------
SDCC::LinkerConfigWidget::LinkerConfigWidget(Project *project)
  : ToolConfigWidget(project, "sdcc_linker_config_widget")
{}

void SDCC::LinkerConfigWidget::initEntries()
{
  createHexFormatEntry();
  createIncludeDirectoriesEntry();
}

//----------------------------------------------------------------------------
ToolConfigWidget *SDCC::GroupUI::configWidgetFactory(Tool::Category category, Project *project) const
{
  switch (category) {
    case Tool::Category::Compiler: return new SDCC::CompilerConfigWidget(project);
    case Tool::Category::Linker: return new SDCC::LinkerConfigWidget(project);
    default: break;
  }
  return 0;
}
