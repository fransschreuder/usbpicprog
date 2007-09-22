/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "dialog.h"

#include <qheader.h>
#include <qtimer.h>
#include <qlabel.h>
#include <qwidgetstack.h>
#include <ktextedit.h>

#include "misc_gui.h"

//-----------------------------------------------------------------------------
Dialog::Dialog(QWidget *parent, const char *name, bool modal,
               const QString &caption, int buttonMask, ButtonCode defaultButton,
               bool separator, const QSize &defaultSize)
  : KDialogBase(parent, name, modal, caption, buttonMask, defaultButton, separator),
    _defaultSize(defaultSize)
{
  BusyCursor::start();
  Q_ASSERT(name);
  QWidget *main = new QWidget(this);
  setMainWidget(main);

  QTimer::singleShot(0, this, SLOT(updateSize()));
}

Dialog::Dialog(DialogType type, const QString &caption, int buttonMask, ButtonCode defaultButton,
               QWidget *parent, const char *name, bool modal, bool separator, const QSize &defaultSize)
  : KDialogBase(type, caption, buttonMask, defaultButton, parent, name, modal, separator),
    _defaultSize(defaultSize)
{
  BusyCursor::start();
  Q_ASSERT(name);
  QTimer::singleShot(0, this, SLOT(updateSize()));
}

Dialog::~Dialog()
{
  GuiConfig gc;
  gc.writeEntry(QString(name()) + "_size", size());
}

void Dialog::updateSize()
{
  GuiConfig gc;
  resize(gc.readSizeEntry(QString(name()) + "_size", &_defaultSize));
  BusyCursor::stop();
}

//-----------------------------------------------------------------------------
TreeListDialog::Item::Item(const QString &label, QWidget *page, const QString &title, QListView *listview)
  : KListViewItem(listview, label), _page(page), _title(title)
{}
TreeListDialog::Item::Item(const QString &label, QWidget *page, const QString &title, QListViewItem *item)
  : KListViewItem(item, label), _page(page), _title(title)
{}

TreeListDialog::TreeListDialog(QWidget *parent, const char *name, bool modal,
                               const QString &caption, int buttonMask, ButtonCode defaultButton,
                               bool separator)
  : Dialog(parent, name, modal, caption, buttonMask, defaultButton, separator)
{
  QVBoxLayout *top = new QVBoxLayout(mainWidget(), 0, 10);

  // list view
  QValueList<int> widths;
  widths += 80;
  widths += 500;
  Splitter *splitter = new Splitter(widths, Horizontal, mainWidget(), name);
  top->addWidget(splitter);
  _listView = new KListView(splitter);
  connect(_listView, SIGNAL(currentChanged(QListViewItem *)), SLOT(currentChanged(QListViewItem *)));
  _listView->setAllColumnsShowFocus(true);
  _listView->setRootIsDecorated(true);
  _listView->setSorting(0);
  _listView->addColumn(QString::null);
  _listView->header()->hide();
  _listView->setResizeMode(QListView::LastColumn);

  // pages
  QFrame *frame = new QFrame(splitter);
  QVBoxLayout *vbox = new QVBoxLayout(frame);
  _label = new QLabel(frame);
  vbox->addWidget(_label);
  _stack = new QWidgetStack(frame);
  connect(_stack, SIGNAL(aboutToShow(QWidget *)), SIGNAL(aboutToShowPage(QWidget *)));
  vbox->addWidget(_stack);
  vbox->addStretch(1);
}

QWidget *TreeListDialog::addPage(const QStringList &labels, const QString &title)
{
  Q_ASSERT( !labels.isEmpty() );

  QWidget *page = new QWidget(_stack);
  connect(page, SIGNAL(destroyed(QObject *)), SLOT(pageDestroyed(QObject *)));
  _stack->addWidget(page);

  QListViewItem *item = 0;
  QListViewItemIterator it(_listView);
  for (; it.current(); ++it) {
    if ( it.current()->text(0)==labels[0] ) {
      item = it.current();
      break;
    }
  }
  if ( item==0 ) {
    bool last = ( labels.count()==1 );
    item = new Item(labels[0], (last ? page : 0), (last ? title : 0), _listView);
    item->setOpen(true);
    item->setSelectable(last);
  }
  for (uint i=1; i<labels.count(); i++) {
    QListViewItem *parent = item;
    item = 0;
    QListViewItemIterator iti(parent);
    for (; it.current(); ++it) {
      if ( it.current()->text(0)==labels[i] ) {
        item = it.current();
        break;
      }
    }
    if ( item==0 ) {
      bool last = ( labels.count()==i+1 );
      item = new Item(labels[i], (last ? page : 0), (last ? title : 0), parent);
      item->setOpen(true);
      item->setSelectable(last);
    }
  }

  return page;
}

void TreeListDialog::currentChanged(QListViewItem *lvitem)
{
  if ( lvitem==0 ) return;
  Item *item = static_cast<Item *>(lvitem);
  _listView->ensureItemVisible(item);
  _label->setText(item->_title);
  _stack->raiseWidget(item->_page);
}

void TreeListDialog::showPage(QWidget *page)
{
  QListViewItemIterator it(_listView);
  for (; it.current(); ++it) {
    Item *item = static_cast<Item *>(it.current());
    if ( item->_page==page ) {
      _listView->setCurrentItem(item);
      currentChanged(item);
      break;
    }
  }
}

int TreeListDialog::pageIndex(QWidget *page) const
{
  return _stack->id(page);
}

int TreeListDialog::activePageIndex() const
{
  const Item *item = static_cast<const Item *>(_listView->currentItem());
  if ( item==0 ) return -1;
  return pageIndex(item->_page);
}

void TreeListDialog::pageDestroyed(QObject *object)
{
  QListViewItemIterator it(_listView);
  for (; it.current(); ++it) {
    Item *item = static_cast<Item *>(it.current());
    if ( item->_page!=object ) continue;
    delete item;
    break;
  }
}

//-----------------------------------------------------------------------------
TextEditorDialog::TextEditorDialog(const QString &text, const QString &caption, QWidget *parent)
  : Dialog(parent, "text_editor_dialog", true, caption, Close, Close, false, QSize(200, 100))
{
  KTextEdit *w = new KTextEdit(text, QString::null, this);
  w->setReadOnly(true);
  w->setWordWrap(QTextEdit::NoWrap);
  setMainWidget(w);
}
