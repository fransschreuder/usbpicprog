/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "toolchain_config_widget.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qgroupbox.h>
#include <qtabwidget.h>
#include <kiconloader.h>

#include "tools/list/compile_config.h"
#include "common/gui/purl_gui.h"
#include "common/global/process.h"
#include "common/gui/container.h"

//----------------------------------------------------------------------------
ToolchainConfigWidget::ToolchainConfigWidget(const Tool::Group &group, QWidget *parent)
  : GridConfigWidget(parent, "toolchain_config_widget"),
    _group(group), _dirty(false), _outputType(0),
    _data(Tool::Category::Nb_Types), _devicesData(group.nbCheckDevices())
{
  _config = group.createConfig(0);
}

ToolchainConfigWidget::~ToolchainConfigWidget()
{
  delete _config;
}

void ToolchainConfigWidget::init()
{
  Container *container = new Container(this, Container::Sunken);
  _topLayout->addMultiCellWidget(container, 0,0, 0,3);
  container->setColStretch(3, 1);

  uint row = 0;
  QLabel *label = new QLabel(i18n(Compile::DirectoryType(Compile::DirectoryType::Executable).label()) + ":", container);
  container->addWidget(label, row, 0);
  _dirs[Compile::DirectoryType::Executable] = new PURL::DirectoryWidget(container);
  connect(_dirs[Compile::DirectoryType::Executable], SIGNAL(changed()), SLOT(forceDetect()));
  container->addMultiCellWidget(_dirs[Compile::DirectoryType::Executable], row,row, 1,3);
  row++;

  label = new QLabel(i18n("Executable Type:"), container);
  container->addWidget(label, row, 0);
  _execType = new QComboBox(container);
  FOR_EACH(Tool::ExecutableType, type) _execType->insertItem(i18n(type.label()));
  connect(_execType, SIGNAL(activated(int)), SLOT(forceDetect()));
  container->addMultiCellWidget(_execType, row,row, 1,2);
  row++;

  uint nbOutputTypes = 0;
  FOR_EACH(Tool::OutputType, type)
    if ( _group.hasOutputType(type) ) nbOutputTypes++;
  if ( nbOutputTypes>1 ) {
    label = new QLabel(i18n("Output Object Type:"), container);
    container->addWidget(label, row, 0);
    _outputType = new KeyComboBox(container);
    FOR_EACH(Tool::OutputType, type)
      if ( _group.hasOutputType(type) ) _outputType->appendItem(type.key(), i18n(type.label()));
    connect(_outputType, SIGNAL(activated()), SLOT(forceDetect()));
    container->addWidget(_outputType, row, 1);
    row++;
  }

  addCustomExecutableOptions(container);

  FOR_EACH(Tool::Category, k) {
    const Tool::Base *base = _group.base(k);
    if ( base==0 ) continue;
    label = new QLabel(i18n(k.label()), container);
    container->addWidget(label, row, 0);
    _data[k].label = new QLabel(container);
    container->addWidget(_data[k].label, row, 1);
    _data[k].button = new KPushButton(KGuiItem(QString::null, "viewmag"), container);
    connect(_data[k].button, SIGNAL(clicked()), SLOT(showDetails()));
    container->addWidget(_data[k].button, row, 2);
    row++;
  }

  label = new QLabel(i18n("Device detection:"), container);
  container->addWidget(label, row, 0);
  _devicesLabel = new QLabel(container);
  container->addWidget(_devicesLabel, row, 1);
  KPushButton *button = new KPushButton(KGuiItem(QString::null, "viewmag"), container);
  connect(button, SIGNAL(clicked()), SLOT(showDeviceDetails()));
  container->addWidget(button, row, 2);
  row++;

  row = _topLayout->numRows();
  FOR_EACH(Compile::DirectoryType, type) {
    if ( type==Compile::DirectoryType::Executable ) continue;
    if ( !_group.hasDirectory(type) ) _dirs[type] = 0;
    else {
      label = new QLabel(i18n(type.label()) + ":", this);
      _topLayout->addWidget(label, row, 0);
      _dirs[type] = new PURL::DirectoryWidget(this);
      _topLayout->addMultiCellWidget(_dirs[type], row,row, 1,3);
      row++;
    }
  }

  _topLayout->setRowStretch(row, 1);
  _topLayout->setColStretch(3, 1);
}

void ToolchainConfigWidget::loadConfig()
{
  _execType->blockSignals(true);
  _execType->setCurrentItem(_config->withWine() ? Tool::ExecutableType::Windows : Tool::ExecutableType::Unix);
  _execType->blockSignals(false);
  if (_outputType) {
    _outputType->blockSignals(true);
    _outputType->setCurrentItem(_config->outputType().key());
    _outputType->blockSignals(false);
  }
  FOR_EACH(Compile::DirectoryType, type) {
    if ( _dirs[type]==0 ) continue;
    _dirs[type]->blockSignals(true);
    _dirs[type]->setDirectory(_config->directory(type));
    _dirs[type]->blockSignals(false);
  }
  _dirty = true;
}

void ToolchainConfigWidget::saveConfig()
{
  _config->setWithWine(withWine());
  if (_outputType) _config->setOutputType(outputType());
  FOR_EACH(Compile::DirectoryType, type)
    if ( _dirs[type] ) _config->setDirectory(type, _dirs[type]->directory());
}

void ToolchainConfigWidget::forceDetect()
{
  _dirty = true;
  detect();
}

void ToolchainConfigWidget::checkExecutableDone()
{
  for(uint i=0; i<_data.count(); i++) {
    if ( _data[i].process!=sender() ) continue;
    if ( _data[i].process->state()==::Process::Timedout ) {
      _data[i].label->setText(i18n("Timeout"));
      return;
    }
    _data[i].checkLines = _data[i].process->sout() + _data[i].process->serr();
    const Tool::Base *base = _group.base(Tool::Category(i));
    QString exec = base->baseExecutable(withWine(), outputType());
    if ( base->checkExecutableResult(withWine(), _data[i].checkLines) ) _data[i].label->setText(i18n("\"%1\" found").arg(exec));
    else _data[i].label->setText(i18n("\"%1\" not recognized").arg(exec));
    break;
  }
}

void ToolchainConfigWidget::checkDevicesDone()
{
  for(uint i=0; i<_devicesData.count(); i++) {
    if ( _devicesData[i].process!=sender() ) continue;
    if ( _devicesData[i].process->state()==::Process::Timedout ) {
      _devicesLabel->setText(i18n("Timeout"));
      return;
    }
    _devicesData[i].checkLines = _devicesData[i].process->sout() + _devicesData[i].process->serr();
    _devicesData[i].done = true;
    break;
  }
  QValueList<const Device::Data *> list;
  for(uint i=0; i<_devicesData.count(); i++) {
    if ( !_devicesData[i].done ) return;
    list += _group.getSupportedDevices(_devicesData[i].checkLines.join("\n"));
  }
  _devicesLabel->setText(i18n("Detected (%1)").arg(list.count()));
}

bool ToolchainConfigWidget::withWine() const
{
  return ( _execType->currentItem()==Tool::ExecutableType::Windows );
}

Tool::OutputType ToolchainConfigWidget::outputType() const
{
  if ( _outputType==0 ) return _group.outputType();
  return Tool::OutputType::fromKey(_outputType->currentItem());
}

QString ToolchainConfigWidget::baseExecutable(Tool::Category category) const
{
  return _group.base(category)->baseExecutable(withWine(), outputType());
}

::Process::LineOutput *ToolchainConfigWidget::checkExecutableProcess(Tool::Category category) const
{
  PURL::Directory execDir = _dirs[Compile::DirectoryType::Executable]->directory();
  return _group.base(category)->checkExecutableProcess(execDir, withWine(), outputType());
}

::Process::LineOutput *ToolchainConfigWidget::checkDevicesProcess(uint i) const
{
  PURL::Directory execDir = _dirs[Compile::DirectoryType::Executable]->directory();
  return _group.checkDevicesProcess(i, execDir, withWine());
}

void ToolchainConfigWidget::detect()
{
  if ( !_dirty ) return;
  FOR_EACH(Tool::Category, k) {
    if ( _data[k].label==0 ) continue;
    if ( !_group.base(k)->checkExecutable() ) _data[k].label->setText(i18n("Unknown"));
    else {
      delete _data[k].process;
      _data[k].checkLines.clear();
      _data[k].process = checkExecutableProcess(k);
      _data[k].command = _data[k].process->prettyCommand();
      connect(_data[k].process, SIGNAL(done(int)), SLOT(checkExecutableDone()));
      connect(_data[k].process, SIGNAL(timeout()), SLOT(checkExecutableDone()));
      QString exec = baseExecutable(k);
      if ( !_data[k].process->start(10000) ) _data[k].label->setText(i18n("\"%1\" not found").arg(exec));
      else _data[k].label->setText(i18n("Detecting \"%1\"...").arg(exec));
    }
  }
  if ( _group.checkDevicesCategory()==Tool::Category::Nb_Types ) {
    QValueVector<QString> supported = _group.supportedDevices();
    _devicesLabel->setText(i18n("Hardcoded (%1)").arg(supported.count()));
  } else {
    for (uint i=0; i<_devicesData.count(); i++) {
      delete _devicesData[i].process;
      _devicesData[i].process = checkDevicesProcess(i);
      _devicesData[i].command = _devicesData[i].process->prettyCommand();
      connect(_devicesData[i].process, SIGNAL(done(int)), SLOT(checkDevicesDone()));
      connect(_devicesData[i].process, SIGNAL(timeout()), SLOT(checkDevicesDone()));
      _devicesData[i].done = false;
      _devicesData[i].checkLines.clear();
      if ( !_devicesData[i].process->start(10000) ) _devicesLabel->setText(i18n("Failed"));
      else _devicesLabel->setText(i18n("Detecting ..."));
    }
  }
  FOR_EACH(Compile::DirectoryType, type) {
    if ( _dirs[type]==0 || type==Compile::DirectoryType::Executable ) continue;
    PURL::Directory execDir = _dirs[Compile::DirectoryType::Executable]->directory();
    PURL::Directory dir = _group.autodetectDirectory(type, execDir, withWine());
    if ( !dir.isEmpty() ) _dirs[type]->setDirectory(dir);
  }
  _dirty = false;
}

void ToolchainConfigWidget::showDetails()
{
  FOR_EACH(Tool::Category, k) {
    if ( sender()!=_data[k].button ) continue;
    QString s;
    const Tool::Base *base = _group.base(k);
    if ( base->checkExecutable() ) {
      s += i18n("<qt><b>Command for executable detection:</b><br>%1<br>").arg(_data[k].command);
      s += i18n("<b>Version string:</b><br>%1<br></qt>").arg(_data[k].checkLines.join("<br>"));
    } else s += i18n("This tool cannot be automatically detected.");
    MessageBox::text(s, Log::Show);
    break;
  }
}

void ToolchainConfigWidget::showDeviceDetails()
{
  QString s;
  if ( _group.checkDevicesCategory()==Tool::Category::Nb_Types ) {
    s += "<qt>";
    QValueVector<QString> supported = _group.supportedDevices();
    for (uint i=0; i<supported.count(); i++) {
      if ( i!=0 ) {
        if ( (i%10)==0 ) s += "<br>";
        s += " ";
      }
      s += supported[i];
    }
    s += "</qt>";
  } else {
    uint nb = _group.nbCheckDevices();
    for (uint i=0; i<nb; i++) {
      if ( nb==1 ) s += i18n("<qt><b>Command for devices detection:</b><br>%1<br>").arg(_devicesData[i].command);
      else s += i18n("<qt><b>Command #%1 for devices detection:</b><br>%2<br>").arg(i+1).arg(_devicesData[i].command);
      QString ss = _devicesData[i].checkLines.join("<br>");
      if ( nb==1 ) s += i18n("<b>Device string:</b><br>%1<br>").arg(ss);
      else s += i18n("<b>Device string #%1:</b><br>%2<br>").arg(i+1).arg(ss);
    }
  }
  MessageBox::text(s, Log::Show);
}
