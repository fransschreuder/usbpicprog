/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "gputils_config.h"

#include <qlabel.h>
#include <klocale.h>

//----------------------------------------------------------------------------
const char * const GPUtils::Config::WARNING_LEVEL_LABELS[Nb_WarningLevels] = {
  I18N_NOOP("All messages"), I18N_NOOP("Warning and errors"), I18N_NOOP("Errors only")
};

uint GPUtils::Config::gpasmWarningLevel() const
{
  return QMIN(warningLevel(Tool::Assembler), uint(Nb_WarningLevels));
}

//----------------------------------------------------------------------------
GPUtils::AssemblerConfigWidget::AssemblerConfigWidget(Project *project)
  : ToolConfigWidget(project, "gputils_assembler_config_widget")
{}

void GPUtils::AssemblerConfigWidget::initEntries()
{
  uint row = container()->numRows();
  QLabel *label = new QLabel(i18n("Warning level:"), container());
  container()->addWidget(label, row, 0);
  _gpasmWarning = new QComboBox(container());
  connect(_gpasmWarning, SIGNAL(activated(int)), SIGNAL(changed()));
  for (uint i=0; i<GPUtils::Config::Nb_WarningLevels; i++)
    _gpasmWarning->insertItem(i18n(GPUtils::Config::WARNING_LEVEL_LABELS[i]));
  _gpasmWarning->insertItem(i18n("as in LIST directive"));
  container()->addWidget(_gpasmWarning, row, 1);
  createIncludeDirectoriesEntry();
  if ( _project==0 ) createHexFormatEntry();
}

void GPUtils::AssemblerConfigWidget::loadConfig(const Compile::Config &config)
{
  ToolConfigWidget::loadConfig(config);
  _gpasmWarning->setCurrentItem(static_cast<const Config &>(config).gpasmWarningLevel());
}

void GPUtils::AssemblerConfigWidget::saveConfig(Compile::Config &config) const
{
  ToolConfigWidget::saveConfig(config);
  static_cast<Config &>(config).setGPAsmWarningLevel(_gpasmWarning->currentItem());
}

//----------------------------------------------------------------------------
GPUtils::ProjectLinkerConfigWidget::ProjectLinkerConfigWidget(Project &project)
  : ToolConfigWidget(&project, "gputils_project_linker_config_widget")
{}

void GPUtils::ProjectLinkerConfigWidget::initEntries()
{
  createHexFormatEntry();
  createIncludeDirectoriesEntry();
}
