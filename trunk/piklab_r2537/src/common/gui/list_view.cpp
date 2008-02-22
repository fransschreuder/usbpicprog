/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 1992-2003 Trolltech AS.                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "list_view.h"

#include <qapplication.h>
#include <qpainter.h>
#include <qlineedit.h>
#include <qheader.h>
#include <qmetaobject.h>
#include <qvariant.h>

//----------------------------------------------------------------------------
ListView::ListView(QWidget *parent, const char *name)
  : KListView(parent, name)
{
  QToolTip::remove(this);
  _tooltip = new ListViewToolTip(this);
}

ListView::~ListView()
{
  delete _tooltip;
}

QString ListView::tooltip(const QListViewItem &, int) const
{
  return QString::null;
}

void ListView::clear()
{
  _editItems.clear();
  KListView::clear();
}

bool ListView::eventFilter(QObject *o, QEvent *e)
{
  QValueList<EditListViewItem *>::const_iterator it;
  for (it=_editItems.begin(); it!=_editItems.end(); ++it) {
    for (uint i=0; i<(*it)->_editWidgets.count(); i++) {
      if ( (*it)->_editWidgets[i]==o ) {
        //qDebug("event %i", e->type());
        switch (e->type()) {
          case QEvent::KeyPress: {
            QKeyEvent *ke = static_cast<QKeyEvent *>(e);
            switch (ke->key()) {
              case Key_Enter:
              case Key_Return:
                (*it)->renameDone(true);
                return true;
              case Key_Escape:
                (*it)->removeEditBox();
                return true;
            }
            break;
          }
          case QEvent::FocusOut: {
            //qDebug("focus out %i %i=%i", qApp->focusWidget(), focusWidget(), (*it)->_editWidgets[i]);
            if ( qApp->focusWidget() && focusWidget()==(*it)->_editWidgets[i] ) break;
            //qDebug("ext");
            QCustomEvent *e = new QCustomEvent(9999);
            QApplication::postEvent(o, e);
            return true;
          }
          case 9999:
            (*it)->renameDone(false);
            return true;
          default:
            //qDebug("  ignored");
            break;
        }
      }
    }
  }
  return KListView::eventFilter(o, e);
}

void ListView::stopRenaming(bool force)
{
  QValueList<EditListViewItem *>::const_iterator it;
  for (it=_editItems.begin(); it!=_editItems.end(); ++it)
    if ( (*it)->isRenaming() ) (*it)->renameDone(force);
}

//----------------------------------------------------------------------------
void ListViewToolTip::maybeTip(const QPoint &p)
{
  if ( _listView==0 ) return;
  const QListViewItem* item = _listView->itemAt(p);
  if ( item==0 ) return;
  QRect rect = _listView->itemRect(item);
  if ( !rect.isValid() ) return;
  int col = _listView->header()->sectionAt(p.x());
  QString text = _listView->tooltip(*item, col);
  if ( !text.isEmpty() ) {
    int hpos = _listView->header()->sectionPos(col);
    rect.setLeft(hpos);
    rect.setRight(hpos + _listView->header()->sectionSize(col));
    tip(rect, text);
  }
}

//----------------------------------------------------------------------------
EditListViewItem::EditListViewItem(ListView *list)
  : KListViewItem(list), _renaming(false)
{
  setRenameEnabled(0, true);
  list->_editItems.append(this);
}

EditListViewItem::EditListViewItem(KListViewItem *item)
  : KListViewItem(item), _renaming(false)
{
  setRenameEnabled(0, true);
  static_cast<ListView *>(listView())->_editItems.append(this);
}

EditListViewItem::~EditListViewItem()
{
  if ( listView() ) static_cast<ListView *>(listView())->_editItems.remove(this);
  for (uint i=0; i<_editWidgets.count(); i++) delete _editWidgets[i];
}

void EditListViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align)
{
  if ( column<int(_editWidgets.count()) && _editWidgets[column] )
    p->fillRect(0, 0, width, height(), cg.color(QColorGroup::Background));
  else KListViewItem::paintCell(p, cg, column, width, align);
}

void EditListViewItem::startRename()
{
  if ( !renameEnabled(0) ) return;
  QListView *lv = listView();
  if ( !lv ) return;
  KListViewItem::startRename(0);
  if (renameBox) {
    renameBox->removeEventFilter(lv);
    renameBox->hide();
    lv->removeChild(renameBox);
  }
  _renaming = true;
  _editWidgets.resize(lv->columns());
  for (uint i=0; i<_editWidgets.count(); i++) {
    QRect r = lv->itemRect(this);
    r = QRect(lv->viewportToContents(r.topLeft()), r.size());
    r.setLeft(lv->header()->sectionPos(i));
    r.setWidth(lv->header()->sectionSize(i) - 1);
    if ( i==0 ) r.setLeft(r.left() + lv->itemMargin() + (depth() + (lv->rootIsDecorated() ? 1 : 0)) * lv->treeStepSize() - 1);
    if ( (lv->contentsX() + lv->visibleWidth())<(r.x() + r.width()) )
      lv->scrollBy((r.x() + r.width() ) - ( lv->contentsX() + lv->visibleWidth() ), 0);
    if ( r.width()>lv->visibleWidth() ) r.setWidth(lv->visibleWidth());

    _editWidgets[i] = editWidgetFactory(i);
    if ( _editWidgets[i]==0 ) continue;
    _editWidgets[i]->installEventFilter(lv);
    lv->addChild(_editWidgets[i], r.x(), r.y());
    uint w = QMIN(r.width(), _editWidgets[i]->sizeHint().width());
    _editWidgets[i]->resize(w, r.height());
    lv->viewport()->setFocusProxy(_editWidgets[i]);
    _editWidgets[i]->setFocus();
    _editWidgets[i]->show();
  }
}

void EditListViewItem::removeEditBox()
{
  QListView *lv = listView();
  if ( !lv ) return;
  _renaming = false;
  bool resetFocus = false;
  for (uint i=0; i<_editWidgets.count(); i++) {
    if ( lv->viewport()->focusProxy()==_editWidgets[i] ) resetFocus = true;
    delete _editWidgets[i];
  }
  _editWidgets.clear();
  delete renameBox;
  renameBox = 0;
  if (resetFocus) {
    lv->viewport()->setFocusProxy(lv);
    lv->setFocus();
  }
}

void EditListViewItem::editDone(int col, const QWidget *edit)
{
  if ( edit->metaObject()->findProperty("text", true)!=-1 )
    emit listView()->itemRenamed(this, col, edit->property("text").toString());
  else if ( edit->metaObject()->findProperty("currentText", true)!=-1 )
    emit listView()->itemRenamed(this, col, edit->property("currentText").toString());
}

void EditListViewItem::renameDone(bool force)
{
  QListView *lv = listView();
  if ( !lv || !_renaming ) return;
  _renaming = false;
  for (uint i=0; i<_editWidgets.count(); i++) {
    if ( !force && !alwaysAcceptEdit(i) ) continue;
    emit lv->itemRenamed(this, i);
    if ( _editWidgets[i] ) editDone(i, _editWidgets[i]);
  }
  removeEditBox();
}

int EditListViewItem::width(const QFontMetrics &fm, const QListView *lv, int col) const
{
  int w = KListViewItem::width(fm, lv, col);
  QWidget *edit = editWidgetFactory(col);
  if ( edit==0 ) return w;
  w = QMAX(w, edit->sizeHint().width());
  delete edit;
  return w;
}
