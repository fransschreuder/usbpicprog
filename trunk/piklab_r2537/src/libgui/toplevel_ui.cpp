/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "toplevel_ui.h"

#include <qstyle.h>
#include <kiconloader.h>

#include "progs/list/prog_list.h"
#include "tools/list/tool_list.h"

//----------------------------------------------------------------------------
ProgrammerStatusWidget::ProgrammerStatusWidget(QWidget *parent)
  : KeyPopupButton(parent, "programmer_status_widget")
{
  connect(this, SIGNAL(activated(const QString &)), SLOT(activatedSlot(const QString &)));
  appendAction(i18n("Configure..."), "configure", this, SIGNAL(configure()));
  appendSeparator();
  Programmer::Lister::ConstIterator it;
  for (it=Programmer::lister().begin(); it!=Programmer::lister().end(); ++it)
    appendItem(it.key(), it.data()->label());
}

void ProgrammerStatusWidget::activatedSlot(const QString &name)
{
  emit selected(*Programmer::lister().group(name));
}

//----------------------------------------------------------------------------
ToolStatusWidget::ToolStatusWidget(QWidget *parent)
  : KeyPopupButton(parent, "tool_status_widget")
{
  connect(this, SIGNAL(activated(const QString &)), SLOT(activatedSlot(const QString &)));
  appendAction(i18n("Configure Toolchain..."), "configure", this, SIGNAL(configureToolchain()));
  appendAction(i18n("Configure Compilation..."), "configure", this, SIGNAL(configure()));
  appendSeparator();
  Tool::Lister::ConstIterator it;
  for (it=Tool::lister().begin(); it!=Tool::lister().end(); ++it)
    appendItem(it.key(), it.data()->label());
}

void ToolStatusWidget::activatedSlot(const QString &name)
{
  emit selected(*Tool::lister().group(name));
}

//----------------------------------------------------------------------------
ViewMenuAction::ViewMenuAction(KDockWidget *widget)
  : KToggleAction(widget->tabPageLabel()), _widget(widget)
{}

void ViewMenuAction::slotActivated()
{
  KAction::slotActivated();
  emit activated(_widget);
}

//----------------------------------------------------------------------------
MenuBarButton::MenuBarButton(const QString &icon, QWidget *parent)
  : QToolButton(parent, "menu_bar_button")
{
  QFontMetrics fm(font());
  int h = fm.height() + 2*style().pixelMetric(QStyle::PM_DefaultFrameWidth, this);
  setFixedHeight(h);
  KIconLoader loader;
  setIconSet(loader.loadIconSet(icon, KIcon::Small, fm.height()-2));
  setUsesTextLabel(false);
  setAutoRaise(true);
}

QSize MenuBarButton::sizeHint() const
{
  return QSize(QToolButton::sizeHint().width(), height());
}
