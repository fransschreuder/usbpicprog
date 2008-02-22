/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic30_ui.h"

#include <qlabel.h>

#include "common/gui/purl_gui.h"
#include "tools/pic30/pic30_config.h"

//----------------------------------------------------------------------------
PIC30::CompilerConfigWidget::CompilerConfigWidget(Project *project)
  : ToolConfigWidget(project, "pic30_compiler_config_widget")
{}

void PIC30::CompilerConfigWidget::initEntries()
{
  createIncludeDirectoriesEntry();
}

//----------------------------------------------------------------------------
PIC30::AssemblerConfigWidget::AssemblerConfigWidget(Project *project)
  : ToolConfigWidget(project, "pic30_assembler_config_widget")
{}

void PIC30::AssemblerConfigWidget::initEntries()
{
  createIncludeDirectoriesEntry();
}

//----------------------------------------------------------------------------
PIC30::LinkerConfigWidget::LinkerConfigWidget(Project *project)
  : ToolConfigWidget(project, "pic30_linker_config_widget")
{}

void PIC30::LinkerConfigWidget::initEntries()
{
  createIncludeDirectoriesEntry();
  createCustomLibrariesEntry();
}

//----------------------------------------------------------------------------
ToolConfigWidget *PIC30::GroupUI::configWidgetFactory(Tool::Category category, Project *project) const
{
  if ( category==Tool::Category::Compiler ) return new PIC30::CompilerConfigWidget(project);
  if ( category==Tool::Category::Assembler ) return new PIC30::AssemblerConfigWidget(project);
  if ( category==Tool::Category::Linker ) return new PIC30::LinkerConfigWidget(project);
  return 0;
}
