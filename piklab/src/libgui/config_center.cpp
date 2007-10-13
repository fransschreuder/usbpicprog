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
#include "prog_config_widget.h"
#include "tools/list/compile_config.h"
#include "tools/list/tool_list.h"
#include "tools/gui/toolchain_config_widget.h"
#include "tools/gui/tool_group_ui.h"

//----------------------------------------------------------------------------
GeneralConfigWidget::GeneralConfigWidget()
  : GridConfigWidget(0, "general_config_widget")
{
  _autoRebuildModified = new QCheckBox(i18n("Automatically rebuild project before programming if it is modified."), this);
  _topLayout->addMultiCellWidget(_autoRebuildModified, 0,0, 0,1);

  _programAfterBuild = new QCheckBox(i18n("Program device after successful build."), this);
  _topLayout->addMultiCellWidget(_programAfterBuild, 1,1, 0,1);

  _showDebug = new QComboBox(this);
  _showDebug->insertItem(i18n(Log::DEBUG_DATA[Log::Nb_DebugLevels].label));
  for (uint i=0; i<Log::Nb_DebugLevels; i++)
    _showDebug->insertItem(i18n(Log::DEBUG_DATA[i].label));
  _topLayout->addWidget(_showDebug, 2, 0);

  _userIdSetToChecksum = new QCheckBox(i18n("Set User Ids to unprotected checksum (if User Ids are empty)."), this);
  _topLayout->addMultiCellWidget(_userIdSetToChecksum, 3,3, 0,1);
}

void GeneralConfigWidget::loadConfig()
{
  GlobalConfig config;
  _autoRebuildModified->setChecked(config.autoRebuildModified());
  _programAfterBuild->setChecked(config.programAfterBuild());
  Log::DebugLevel level = config.debugLevel();
  if ( level!=Log::Nb_DebugLevels ) _showDebug->setCurrentItem(level+1);
  _userIdSetToChecksum->setChecked(config.isUserIdSetToChecksum());
}

void GeneralConfigWidget::saveConfig()
{
  GlobalConfig config;
  config.writeAutoRebuildModified(_autoRebuildModified->isChecked());
  config.writeProgramAfterBuild(_programAfterBuild->isChecked());
  uint i = _showDebug->currentItem();
  config.writeDebugLevel(i==0 ? Log::Nb_DebugLevels : Log::DebugLevel(i-1));
  config.writeUserIdSetToChecksum(_userIdSetToChecksum->isChecked());
}

QPixmap GeneralConfigWidget::pixmap() const
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
    case General:     return new GeneralConfigWidget;
    case ProgSelect:  return new Programmer::SelectConfigWidget;
    case ProgOptions: return new Programmer::OptionsConfigWidget;
    case Standalone:  return new StandaloneConfigWidget;
    case Nb_Types:    break;
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

//----------------------------------------------------------------------------
ToolchainsConfigCenter::ToolchainsConfigCenter(const Tool::Group &sgroup, QWidget *parent)
  : TreeListDialog(parent, "configure_toolchains_dialog", true,
                   i18n("Configure Toolchains"), Ok|User1|User2|Cancel, Cancel, false)
{
  setButtonGuiItem(User1, KStdGuiItem::reset());
  setButtonGuiItem(User2, KGuiItem(i18n("Update"), "reload"));

  _titleBox->addStretch(1);
  _infoButton = new KPushButton(KGuiItem(QString::null, "viewmag"), _frame);
  connect(_infoButton, SIGNAL(clicked()), SLOT(showInformationDialog()));
  _titleBox->addWidget(_infoButton);

  QWidget *current = 0;
  FOR_EACH(PURL::SourceFamily, family) {
    Tool::Lister::ConstIterator it;
    for (it=Tool::lister().begin(); it!=Tool::lister().end(); ++it) {
      PURL::FileType type = it.data()->implementationType(family.data()->toolType);
      if ( type==PURL::Nb_FileTypes || PURL::DATA[type].sourceFamily!=type ) continue;
      if ( family==PURL::ToolType::Assembler
           && it.data()->implementationType(PURL::ToolType::Compiler)!=PURL::Nb_FileTypes ) continue;
      QStringList names = i18n(family.label());
      names += it.data()->label();
      QWidget *page = addPage(names);
      QVBoxLayout *vbox = new QVBoxLayout(page);
      ToolchainConfigWidget *tcw = static_cast<const ::Tool::GroupUI *>(it.data()->gui())->toolchainConfigWidgetFactory(page);
      tcw->init();
      tcw->loadConfig();
      vbox->addWidget(tcw);
      _pages[page] = tcw;
      if ( it.key()==sgroup.name() ) current = page;
    }
  }
  showPage(current);
  aboutToShowPageSlot(current);
  connect(this, SIGNAL(aboutToShowPage(QWidget *)), SLOT(aboutToShowPageSlot(QWidget *)));
}

void ToolchainsConfigCenter::aboutToShowPageSlot(QWidget *page)
{
  if ( !_pages.contains(page) ) _infoButton->hide();
  else {
    _infoButton->show();
    QTimer::singleShot(0, _pages[page], SLOT(detect()));
  }
}

void ToolchainsConfigCenter::slotApply()
{
  QMap<QWidget *, ToolchainConfigWidget *>::iterator it;
  for (it=_pages.begin(); it!=_pages.end(); ++it) it.data()->saveConfig();
}

void ToolchainsConfigCenter::slotOk()
{
  slotApply();
  accept();
}

ToolchainConfigWidget *ToolchainsConfigCenter::current() const
{
  int i = activePageIndex();
  if ( i==-1 ) return 0;
  QMap<QWidget *, ToolchainConfigWidget *>::const_iterator it;
  for (it=_pages.begin(); it!=_pages.end(); ++it)
    if ( pageIndex(it.key())==i ) return it.data();
  Q_ASSERT(false);
  return 0;
}

void ToolchainsConfigCenter::slotUser1()
{
  ToolchainConfigWidget *tcw = current();
  if (tcw) {
    tcw->loadConfig();
    tcw->forceDetect();
  }
}

void ToolchainsConfigCenter::slotUser2()
{
  ToolchainConfigWidget *tcw = current();
  if (tcw) tcw->forceDetect();
}

void ToolchainsConfigCenter::showInformationDialog()
{
  ToolchainConfigWidget *tcw = current();
  Q_ASSERT(tcw);
  TextEditorDialog dialog(tcw->group().informationText(), tcw->group().label(), true, this);
  dialog.exec();
}
