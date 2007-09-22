/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic30_config.h"

#include <qlabel.h>

#include "common/gui/purl_gui.h"

//----------------------------------------------------------------------------
void PIC30::ToolchainConfigWidget::addCustomExecutableOptions(Container *container)
{
  uint row = container->numRows();
  QLabel *label = new QLabel(i18n("Output Object Type:"), container);
  container->addWidget(label, row, 0);
  _outputType = new QComboBox(container);
  for (uint i=0; i<Nb_OutputTypes; i++) _outputType->insertItem(i18n(OUTPUT_TYPE_DATA[i].label));
  connect(_outputType, SIGNAL(activated(int)), SLOT(forceDetect()));
  container->addWidget(_outputType, row, 1);
}

QString PIC30::ToolchainConfigWidget::baseExecutable(Tool::Category category) const
{
  const Base *base = static_cast<const Base *>(_group.base(category));
  return base->baseExecutable(withWine(), OutputType(_outputType->currentItem()));
}

::Process::LineOutput *PIC30::ToolchainConfigWidget::checkExecutableProcess(Tool::Category category) const
{
  PURL::Directory execDir = _dirs[Compile::ExecutableDirectory]->directory();
  const Base *base = static_cast<const Base *>(_group.base(category));
  return base->checkExecutableProcess(execDir, withWine(), OutputType(_outputType->currentItem()));
}

void PIC30::ToolchainConfigWidget::loadConfig()
{
  _outputType->setCurrentItem(static_cast<const Config *>(_config)->outputType());
  ::ToolchainConfigWidget::loadConfig();
}

void PIC30::ToolchainConfigWidget::saveConfig()
{
  static_cast<const Config *>(_config)->setOutputType(OutputType(_outputType->currentItem()));
  ::ToolchainConfigWidget::saveConfig();
}

//----------------------------------------------------------------------------
PIC30::OutputType PIC30::Config::outputType() const
{
  QString s = groupValue("output_type", OUTPUT_TYPE_DATA[ElfOutput].key);
  for (uint i=0; i<Nb_OutputTypes; i++)
    if ( OUTPUT_TYPE_DATA[i].key==s ) return OutputType(i);
  return ElfOutput;
}

void PIC30::Config::setOutputType(OutputType type)
{
  setGroupValue("output_type", OUTPUT_TYPE_DATA[type].key);
  const_cast<Tool::Group *>(_group)->init();
}

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
