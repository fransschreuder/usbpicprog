/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "misc_gui.h"

#include <qapplication.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qwidgetstack.h>
#include <qobjectlist.h>
#include <qpainter.h>
#include <qheader.h>
#include <qmetaobject.h>
#include <qvariant.h>
#include <qpopupmenu.h>

#include <kcursor.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <ktabbar.h>

#include "dialog.h"
#include "common/common/number.h"
#include "common/common/misc.h"
#include "common/gui/number_gui.h"

//-----------------------------------------------------------------------------
bool BusyCursor::_overridePaused = false;

void BusyCursor::start()
{
  QApplication::setOverrideCursor(KCursor::waitCursor(), true);
}

void BusyCursor::stop()
{
  QApplication::restoreOverrideCursor();
}

void BusyCursor::pause()
{
  _overridePaused = QApplication::overrideCursor();
  stop();
}

void BusyCursor::restore()
{
  if (_overridePaused) start();
}

//-----------------------------------------------------------------------------
void MessageBox::information(const QString &text, Log::ShowMode show)
{
  if ( show==Log::DontShow ) return;
  BusyCursor::pause();
  KMessageBox::information(qApp->mainWidget(), text, QString::null, QString::null, KMessageBox::Notify | KMessageBox::AllowLink);
  BusyCursor::restore();
}

void MessageBox::detailedSorry(const QString &text, const QString &details, Log::ShowMode show)
{
  if ( show==Log::DontShow ) return;
  BusyCursor::pause();
  if ( details.isEmpty() ) KMessageBox::sorry(qApp->mainWidget(), text, QString::null, KMessageBox::Notify | KMessageBox::AllowLink);
  else KMessageBox::detailedSorry(qApp->mainWidget(), text, details, QString::null, KMessageBox::Notify | KMessageBox::AllowLink);
  BusyCursor::restore();
}

bool MessageBox::askContinue(const QString &text, const KGuiItem &buttonContinue, const QString &caption)
{
  ::BusyCursor::pause();
  int res = KMessageBox::warningContinueCancel(qApp->mainWidget(), text, caption, buttonContinue);
  ::BusyCursor::restore();
  return ( res==KMessageBox::Continue );
}

bool MessageBox::questionYesNo(const QString &text, const KGuiItem &yesButton,const KGuiItem &noButton, const QString &caption)
{
  ::BusyCursor::pause();
  int res = KMessageBox::questionYesNo(qApp->mainWidget(), text, caption, yesButton, noButton);
  ::BusyCursor::restore();
  return ( res==KMessageBox::Yes );
}

MessageBox::Result MessageBox::questionYesNoCancel(const QString &text, const KGuiItem &yesButton, const KGuiItem &noButton,
                                                        const QString &caption)
{
  ::BusyCursor::pause();
  int res = KMessageBox::questionYesNoCancel(qApp->mainWidget(), text, caption, yesButton, noButton);
  ::BusyCursor::restore();
  if ( res==KMessageBox::Yes ) return Yes;
  if ( res==KMessageBox::No ) return No;
  return Cancel;
}

void MessageBox::text(const QString &text, Log::ShowMode show)
{
  if ( show==Log::DontShow ) return;
  BusyCursor::pause();
  TextEditorDialog dialog(text, QString::null, false, qApp->mainWidget());
  dialog.exec();
  BusyCursor::restore();
}

//----------------------------------------------------------------------------
PopupButton::PopupButton(QWidget *parent, const char *name)
  : KPushButton(parent, name)
{
  init();
}

PopupButton::PopupButton(const QString &text, QWidget *parent, const char *name)
  : KPushButton(text, parent, name)
{
  init();
}

void PopupButton::init()
{
  _separator = false;
  setFlat(true);
  QPopupMenu *popup = new QPopupMenu(this);
  connect(popup, SIGNAL(activated(int)), SIGNAL(activated(int)));
  setPopup(popup);
}

void PopupButton::appendAction(KAction *action)
{
  if ( _separator && popup()->count()!=0 ) popup()->insertSeparator();
  _separator = false;
  action->plug(popup());
}

void PopupButton::appendAction(const QString &label, const QString &icon,
                               QObject *receiver, const char *slot)
{
  appendAction(new KAction(label, icon, 0, receiver, slot, (KActionCollection *)0));
}

int PopupButton::appendItem(const QString &label, const QString &icon, int id)
{
  KIconLoader loader;
  QPixmap pixmap = loader.loadIcon(icon, KIcon::Small);
  return appendItem(label, pixmap, id);
}

int PopupButton::appendItem(const QString &label, const QPixmap &icon, int id)
{
  if ( _separator && popup()->count()!=0 ) popup()->insertSeparator();
  _separator = false;
  return popup()->insertItem(icon, label, id);
}

//-----------------------------------------------------------------------------
Splitter::Splitter(const QValueList<int> &defaultSizes, Orientation o, QWidget *parent, const char *name)
  : QSplitter(o, parent, name), _defaultSizes(defaultSizes)
{
  Q_ASSERT(name);
  setOpaqueResize(true);
  QTimer::singleShot(0, this, SLOT(updateSizes()));
}

Splitter::~Splitter()
{
  GuiConfig gc;
  gc.writeEntry(QString(name()) + "_sizes", sizes());
}

void Splitter::updateSizes()
{
  GuiConfig gc;
  QValueList<int> sizes = gc.readIntListEntry(QString(name()) + "_sizes");
  for (uint i=sizes.count(); i<_defaultSizes.count(); i++) sizes.append(_defaultSizes[i]);
  setSizes(sizes);
}

//-----------------------------------------------------------------------------
TabBar::TabBar(QWidget *parent, const char *name)
  : KTabBar(parent, name), _ignoreWheelEvent(false)
{}

void TabBar::wheelEvent(QWheelEvent *e)
{
  if (_ignoreWheelEvent) QApplication::sendEvent(parent(), e); // #### not sure why ignoring is not enough...
  else KTabBar::wheelEvent(e);
}

TabWidget::TabWidget(QWidget *parent, const char *name)
  : KTabWidget(parent, name)
{
  setTabBar(new TabBar(this));
}

void TabWidget::setIgnoreWheelEvent(bool ignore)
{
  static_cast<TabBar *>(tabBar())->_ignoreWheelEvent = ignore;
}

void TabWidget::wheelEvent(QWheelEvent *e)
{
  if (static_cast<TabBar *>(tabBar())->_ignoreWheelEvent) e->ignore();
  else KTabWidget::wheelEvent(e);
}

void TabWidget::setTabBar(TabBar *tabbar)
{
  KTabWidget::setTabBar(tabbar);
  connect(tabBar(), SIGNAL(contextMenu( int, const QPoint & )), SLOT(contextMenu( int, const QPoint & )));
  connect(tabBar(), SIGNAL(mouseDoubleClick( int )), SLOT(mouseDoubleClick( int )));
  connect(tabBar(), SIGNAL(mouseMiddleClick( int )), SLOT(mouseMiddleClick( int )));
  connect(tabBar(), SIGNAL(initiateDrag( int )), SLOT(initiateDrag( int )));
  connect(tabBar(), SIGNAL(testCanDecode(const QDragMoveEvent *, bool & )), SIGNAL(testCanDecode(const QDragMoveEvent *, bool & )));
  connect(tabBar(), SIGNAL(receivedDropEvent( int, QDropEvent * )), SLOT(receivedDropEvent( int, QDropEvent * )));
  connect(tabBar(), SIGNAL(moveTab( int, int )), SLOT(moveTab( int, int )));
  connect(tabBar(), SIGNAL(closeRequest( int )), SLOT(closeRequest( int )));
  connect(tabBar(), SIGNAL(wheelDelta( int )), SLOT(wheelDelta( int )));
}
