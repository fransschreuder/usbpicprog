/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2004 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "tools_config_widget.h"

#include <qwidgetstack.h>
#include <qlabel.h>

#include "tool_list.h"
#include "tools/base/tool_config_widget.h"
#include "compile_config.h"
#include "compile.h"

//----------------------------------------------------------------------------
HelpDialog::HelpDialog(QWidget *parent)
  : Dialog(parent, "help_dialog", false, i18n("Help Dialog"), Close, Close, false)
{
  QGridLayout *top = new QGridLayout(mainWidget(), 1, 1, 10, 10);
  uint k = 0;
  for (; Compile::ARGUMENT_DATA[k].key; k++) {
    QLabel *label = new QLabel(Compile::ARGUMENT_DATA[k].key, mainWidget());
    top->addWidget(label, k, 0);
    label = new QLabel(i18n(Compile::ARGUMENT_DATA[k].description), mainWidget());
    top->addWidget(label, k, 1);
  }
  top->setColStretch(2, 1);
  top->setRowStretch(k, 1);
}

//----------------------------------------------------------------------------
ToolsConfigWidget::ToolsConfigWidget(Project *project, QWidget *parent)
  : GridConfigWidget(parent, "tools_config_widget"), _project(project),
    _stacks(Tool::Nb_Categories), _helpDialog(0)
{
  QLabel *label = new QLabel(i18n("Toolchain:"), this);
  _topLayout->addWidget(label, 0, 0);
  _tool = new QComboBox(this);
  for (uint i=0; i<Tool::lister().nbGroups(); i++)
    _tool->insertItem(Tool::lister().group(i).label());
  connect(_tool, SIGNAL(activated(int)), SLOT(toolChanged()));
  _topLayout->addWidget(_tool, 0, 1);

  _tabWidget = new QTabWidget(this);
  _topLayout->addMultiCellWidget(_tabWidget, 1,1, 0,2);
  for (uint k=0; k<Tool::Nb_Categories; k++) {
    _stacks[k] = new QWidgetStack(_tabWidget);
    for (uint i=0; i<Tool::lister().nbGroups(); i++) {
      ToolConfigWidget *cw = Tool::lister().group(i).createConfigWidget(Tool::Category(k), project);
      if (cw) {
        _stacks[k]->addWidget(cw, i);
        connect(cw, SIGNAL(displayHelp()), SLOT(displayHelp()));
      }
    }
  }

  _topLayout->setColStretch(2, 1);
}

void ToolsConfigWidget::toolChanged()
{
  for (uint k=0; k<Tool::Nb_Categories; k++) {
    _tabWidget->removePage(_stacks[k]);
    _stacks[k]->hide();
    if ( _stacks[k]->widget(_tool->currentItem()) ) {
      _stacks[k]->raiseWidget(_tool->currentItem());
      _stacks[k]->show();
      _tabWidget->addTab(_stacks[k], i18n(Tool::CATEGORY_DATA[k].title));
    }
  }
  _tabWidget->setCurrentPage(0);
}

void ToolsConfigWidget::loadConfig()
{
  const Tool::Group &group = Compile::Config::toolGroup(_project);
  _tool->setCurrentItem(Tool::lister().groupIndex(group.name()));
  toolChanged();
  for (uint k=0; k<Tool::Nb_Categories; k++) {
    for (uint i=0; i<Tool::lister().nbGroups(); i++) {
      if ( _stacks[k]->widget(i)==0 ) continue;
      static_cast<ConfigWidget *>(_stacks[k]->widget(i))->loadConfig();
    }
  }
}

void ToolsConfigWidget::saveConfig()
{
  uint i = _tool->currentItem();
  Compile::Config::setToolGroup(_project, Tool::lister().group(i));
  for (uint k=0; k<Tool::Nb_Categories; k++) {
    if ( _stacks[k]->widget(i)==0 ) continue;
    static_cast<ConfigWidget *>(_stacks[k]->widget(i))->saveConfig();
  }
}

void ToolsConfigWidget::displayHelp()
{
  if ( _helpDialog.isNull() ) _helpDialog = new HelpDialog(this);
  _helpDialog->show();
}
