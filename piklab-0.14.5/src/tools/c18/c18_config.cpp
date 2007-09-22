/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "c18_config.h"

#include <qlabel.h>
#include <klocale.h>

//----------------------------------------------------------------------------
const char * const C18::Config::WARNING_LEVEL_LABELS[Nb_WarningLevels] = {
  I18N_NOOP("All messages"), I18N_NOOP("Warning and errors"), I18N_NOOP("Errors only")
};

//----------------------------------------------------------------------------
C18::CompilerConfigWidget::CompilerConfigWidget(Project *project)
  : ToolConfigWidget(project, "c18_compiler_config_widget")
{}

void C18::CompilerConfigWidget::initEntries()
{
  uint row = container()->numRows();
  QLabel *label = new QLabel(i18n("Warning level:"), container());
  container()->addWidget(label, row, 0);
  _warningLevel = new QComboBox(container());
  connect(_warningLevel, SIGNAL(activated(int)), SIGNAL(changed()));
  for (uint i=0; i<Config::Nb_WarningLevels; i++)
    _warningLevel->insertItem(i18n(Config::WARNING_LEVEL_LABELS[i]));
  container()->addWidget(_warningLevel, row, 1);
  createIncludeDirectoriesEntry();
}

void C18::CompilerConfigWidget::loadConfig(const Compile::Config &config)
{
  ToolConfigWidget::loadConfig(config);
  _warningLevel->setCurrentItem(static_cast<const Config &>(config).warningLevel());
}

void C18::CompilerConfigWidget::saveConfig(Compile::Config &config) const
{
  ToolConfigWidget::saveConfig(config);
  static_cast<Config &>(config).setWarningLevel(_warningLevel->currentItem());
}

//----------------------------------------------------------------------------
C18::LinkerConfigWidget::LinkerConfigWidget(Project *project)
  : ToolConfigWidget(project, "c18_linker_config_widget")
{}

void C18::LinkerConfigWidget::initEntries()
{
  createCustomLibrariesEntry();
  createHexFormatEntry();
}
