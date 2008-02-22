/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2004 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "toolchain_config_center.h"

#include <qlabel.h>
#include <qlayout.h>
#include <kiconloader.h>

#include "tools/list/tools_config_widget.h"
#include "tools/list/compile_config.h"
#include "tools/list/tool_list.h"
#include "toolchain_config_widget.h"
#include "tool_group_ui.h"

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
      PURL::FileType type = it.data()->implementationType(family.data().toolType);
      if ( type==PURL::Nb_FileTypes || type.data().sourceFamily!=family ) continue;
      if ( family==PURL::SourceFamily::Asm && it.data()->implementationType(PURL::ToolType::Compiler)!=PURL::Nb_FileTypes ) continue;
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
