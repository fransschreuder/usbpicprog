/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2004 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "config_center.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qgroupbox.h>
#include <qtabwidget.h>
#include <qtimer.h>
#include <kiconloader.h>
#include <klocale.h>
#include <klistview.h>

#include "global_config.h"
#include "device_gui.h"
#include "tools/list/tools_config_widget.h"
#include "progs/gui/prog_config_center.h"
#include "progs/gui/debug_config_center.h"
#include "tools/list/compile_config.h"

//----------------------------------------------------------------------------
GlobalConfigWidget::GlobalConfigWidget()
{
  uint row = _topLayout->numRows();

  _showDebug = new QComboBox(this);
  FOR_EACH(Log::DebugLevel, level) _showDebug->insertItem(i18n(level.label()));
  _topLayout->addWidget(_showDebug, row, 0);
  row++;
}

void GlobalConfigWidget::loadConfig()
{
  BaseGlobalConfigWidget::loadConfig();
  GlobalConfig config;
  _showDebug->setCurrentItem(config.debugLevel().type());
}

void GlobalConfigWidget::saveConfig()
{
  BaseGlobalConfigWidget::saveConfig();
  GlobalConfig config;
  config.writeDebugLevel(Log::DebugLevel::Type(_showDebug->currentItem()));
}

QPixmap GlobalConfigWidget::pixmap() const
{
  KIconLoader loader;
  return loader.loadIcon("configure", KIcon::Toolbar, KIcon::SizeMedium);
}

//----------------------------------------------------------------------------
StandaloneConfigWidget::StandaloneConfigWidget()
  : ConfigWidget(0, "standalone_config_widget")
{
  QGridLayout *top = new QGridLayout(this, 1, 1, 10, 10);

  QLabel *label = new QLabel(i18n("Device:"), this);
  top->addWidget(label, 0, 0);
  _device = new DeviceChooser::Button(true, this);
  top->addWidget(_device, 0, 1);

  _tools = new ToolsConfigWidget(0, this);
  top->addMultiCellWidget(_tools, 1,1, 0,2);

  top->setRowStretch(1, 1);
  top->setColStretch(2, 1);
}

void StandaloneConfigWidget::loadConfig()
{
  _device->setDevice(Compile::Config::device(0));
  _tools->loadConfig();
}

void StandaloneConfigWidget::saveConfig()
{
  Compile::Config::setDevice(0, _device->device());
  _tools->saveConfig();
}

QPixmap StandaloneConfigWidget::pixmap() const
{
  KIconLoader loader;
  return loader.loadIcon("configure", KIcon::Toolbar, KIcon::SizeMedium);
}

//----------------------------------------------------------------------------
ConfigWidget *ConfigCenter::factory(Type type)
{
  switch (type) {
    case General:      return new GlobalConfigWidget;
    case ProgSelect:   return new Programmer::SelectConfigWidget;
    case ProgOptions:  return new Programmer::OptionsConfigWidget;
    case DebugOptions: return new Debugger::OptionsConfigWidget;
    case Standalone:   return new StandaloneConfigWidget;
    case Nb_Types:     break;
  }
  Q_ASSERT(false);
  return 0;
}

ConfigCenter::ConfigCenter(Type showType, QWidget *parent)
  : Dialog(IconList, i18n("Configure Piklab"), Ok|Cancel, Cancel,
           parent, "configure_piklab_dialog", true, false)
{
  for (uint i=0; i<Nb_Types; i++) {
    _configWidgets[i] = factory(Type(i));
    _configWidgets[i]->loadConfig();
    _pages[i] = addPage(_configWidgets[i]->title(), _configWidgets[i]->header(), _configWidgets[i]->pixmap());
    QVBoxLayout *vbox = new QVBoxLayout(_pages[i]);
    _configWidgets[i]->reparent(_pages[i], QPoint(0,0));
    vbox->addWidget(_configWidgets[i]);
  }
  showPage(showType);
}

void ConfigCenter::slotApply()
{
  for (uint i=0; i<Nb_Types; i++)
    _configWidgets[i]->saveConfig();
}

void ConfigCenter::slotOk()
{
  slotApply();
  accept();
}
