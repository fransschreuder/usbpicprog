/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2004 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "tool_config_widget.h"

#include <qtooltip.h>
#include <qwidgetstack.h>
#include <qvgroupbox.h>
#include <klocale.h>

#include "devices/base/hex_buffer.h"
#include "tools/list/compile_config.h"
#include "libgui/project.h"
#include "common/gui/purl_gui.h"
#include "tools/list/compile.h"

const char * const ToolConfigWidget::ARGUMENTS_TYPE_LABELS[Nb_ArgumentsTypes] = {
  I18N_NOOP("Automatic"), I18N_NOOP("Custom")
};

ToolConfigWidget::ToolConfigWidget(Project *project, const char *name)
  : GridConfigWidget(0, name), _group(0), _project(project),
    _customOptions(0), _customLibraries(0), _includeDirs(0), _hexFormat(0),
    _config(0), _tmpConfig(0), _process(0), _tmpProject(0)
{
  Container *container = new Container(this);
  container->setColStretch(2, 1);
  _topLayout->addWidget(container, 0, 0);
  QLabel *label = new QLabel(i18n("Configuration:"), container);
  container->addWidget(label, 0, 0);
  _argumentsType = new KComboBox(container);
  for (uint i=0; i<Nb_ArgumentsTypes; i++)
    _argumentsType->insertItem(i18n(ARGUMENTS_TYPE_LABELS[i]), i);
  connect(_argumentsType, SIGNAL(activated(int)), SLOT(updateArguments()));
  container->addWidget(_argumentsType, 0, 1);
  label = new QLabel(i18n("Arguments:"), container);
  container->addWidget(label, 1, 0);
  _arguments = new KLineEdit(container);
  _arguments->setReadOnly(true);
  container->addMultiCellWidget(_arguments, 1,1, 1,2);
  KPushButton *button = new KPushButton(KGuiItem(QString::null, "help"), container);
  connect(button, SIGNAL(clicked()), SIGNAL(displayHelp()));
  container->addWidget(button, 1, 3);
  _argumentsEditor = new EditListBox(1, container, "arguments_editor", EditListBox::DuplicatesAllowed,
                                     EditListBox::Add | EditListBox::Remove | EditListBox::UpDown | EditListBox::RemoveAll | EditListBox::Reset);
  connect(_argumentsEditor, SIGNAL(changed()), SLOT(updateArguments()));
  connect(_argumentsEditor, SIGNAL(reset()), SLOT(resetCustomArguments()));
  container->addMultiCellWidget(_argumentsEditor, 2,2, 0,3);

  _container = new Container(container);
  _container->setColStretch(2, 1);
  container->addMultiCellWidget(_container, 3,3, 0,3);
  container->setRowStretch(4, 1);

  connect(this, SIGNAL(changed()), SLOT(updateArguments()));
}

void ToolConfigWidget::init(Tool::Category category, const Tool::Group &group)
{
  _category = category;
  _group = &group;
  _config = _group->createConfig(_project);
  _tmpProject = new Project(PURL::Url());
  _tmpConfig = _group->createConfig(_tmpProject);
  Compile::Data data(_category, QValueList<Compile::TodoItem>(), QString::null, _project, Compile::NormalLinking);
  _process = _group->createCompileProcess(data, 0);

  initEntries();
  createCustomOptionsEntry();
}

ToolConfigWidget::~ToolConfigWidget()
{
  delete _process;
  delete _tmpConfig;
  delete _tmpProject;
  delete _config;
}

PURL::DirectoriesWidget *ToolConfigWidget::createDirectoriesEntry(const QString &text)
{
  uint row = container()->numRows();
  PURL::DirectoriesWidget *sdw = new PURL::DirectoriesWidget(text, _project ? _project->directory().path() : QString::null, container());
  connect(sdw, SIGNAL(changed()), SIGNAL(changed()));
  container()->addMultiCellWidget(sdw, row,row, 0,2);
  return sdw;
}

void ToolConfigWidget::createCustomOptionsEntry()
{
  uint row = container()->numRows();
  QLabel *label = new QLabel(i18n("Custom options:"), container());
  container()->addWidget(label, row, 0);
  _customOptions = new QLineEdit(container());
  connect(_customOptions, SIGNAL(textChanged(const QString &)), SIGNAL(changed()));
  container()->addMultiCellWidget(_customOptions, row,row, 1,2);
}

void ToolConfigWidget::createCustomLibrariesEntry()
{
  uint row = container()->numRows();
  QLabel *label = new QLabel(i18n("Custom libraries:"), container());
  container()->addWidget(label, row, 0);
  _customLibraries = new QLineEdit(container());
  connect(_customLibraries, SIGNAL(textChanged(const QString &)), SIGNAL(changed()));
  QToolTip::add(_customLibraries, i18n("<qt>This values will be placed after the linked objects.</qt>")) ;
  container()->addMultiCellWidget(_customLibraries, row,row, 1,2);
}

void ToolConfigWidget::createHexFormatEntry()
{
  uint row = container()->numRows();
  QLabel *label = new QLabel(i18n("Hex file format:"), container());
  container()->addWidget(label, row, 0);
  _hexFormat = new QComboBox(container());
  connect(_hexFormat, SIGNAL(activated(int)), SIGNAL(changed()));
  for (uint i=0; i<HexBuffer::Nb_Formats; i++)
    _hexFormat->insertItem(HexBuffer::FORMATS[i]);
  _hexFormat->insertItem(i18n("as in LIST directive"));
  container()->addWidget(_hexFormat, row, 1);
}

void ToolConfigWidget::loadConfig()
{
  loadConfig(*_config);
  if ( _config->customArguments(_category).isEmpty() ) resetCustomArguments();
  else updateArguments();
}

void ToolConfigWidget::resetCustomArguments()
{
  _argumentsEditor->setTexts(arguments(AutomaticArguments));
  updateArguments();
}

void ToolConfigWidget::loadConfig(const Compile::Config &config)
{
  _argumentsType->setCurrentItem(config.hasCustomArguments(_category) ? CustomArguments : AutomaticArguments);
  _argumentsEditor->setTexts(config.customArguments(_category));
  if (_includeDirs) _includeDirs->setDirectories(config.rawIncludeDirs(_category)) ;
  if (_customOptions) _customOptions->setText(config.rawCustomOptions(_category));
  if (_customLibraries) _customLibraries->setText(config.rawCustomLibraries(_category));
  if (_hexFormat) _hexFormat->setCurrentItem(config.hexFormat());
}

void ToolConfigWidget::saveConfig(Compile::Config &config) const
{
  config.setHasCustomArguments(_category, _argumentsType->currentItem()==CustomArguments);
  config.setCustomArguments(_category, _argumentsEditor->texts());
  if (_includeDirs) config.setRawIncludeDirs(_category, _includeDirs->directories());
  if (_customOptions) config.setRawCustomOptions(_category, _customOptions->text());
  if (_customLibraries) config.setRawCustomLibraries(_category, _customLibraries->text());
  if (_hexFormat) config.setHexFormat(HexBuffer::Format(_hexFormat->currentItem()));
}

QStringList ToolConfigWidget::arguments(ArgumentsType type) const
{
  if ( _tmpConfig==0 ) return QStringList();
  saveConfig(*_tmpConfig);
  if ( type==AutomaticArguments ) return _process->genericArguments(*_tmpConfig);
  return _argumentsEditor->texts();
}

void ToolConfigWidget::updateArguments()
{
  ArgumentsType type = ArgumentsType(_argumentsType->currentItem());
  if ( type==AutomaticArguments ) {
    _argumentsEditor->hide();
    _container->show();
  } else {
    _argumentsEditor->show();
    _container->hide();
  }
  _arguments->setText(arguments(type).join(" "));
}
