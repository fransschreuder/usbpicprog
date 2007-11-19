/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "list_container.h"

//----------------------------------------------------------------------------
PopupContainer::PopupContainer(const QString &title, QWidget *parent, const char *name)
  : KPopupMenu(parent, name)
{
  if ( !title.isEmpty() ) insertTitle(title);
}

ListContainer *PopupContainer::appendBranch(const QString &title)
{
  PopupContainer *branch = new PopupContainer(title, this);
  insertItem(title, branch);
  return branch;
}

ListContainer *PopupContainer::appendBranch(const QPixmap &pixmap, const QString &title)
{
  PopupContainer *branch = new PopupContainer(title, this);
  insertItem(pixmap, title, branch);
  return branch;
}

void PopupContainer::appendItem(const QPixmap &icon, const QString &label, uint id, ItemState state)
{
  insertItem(icon, label, id);
  switch (state) {
    case Normal:    break;
    case Checked:   setItemChecked(id, true); break;
    case UnChecked: setItemChecked(id, false); break;
    case Disabled:  setItemEnabled(id, false); break;
  }
}

//----------------------------------------------------------------------------
ListViewItemContainer::ListViewItemContainer(const QString &title, KListView *parent)
  : KListViewItem(parent, title), _parent(0), _column(0)
{
  _ids = new QMap<const QListViewItem *, uint>;
}

ListViewItemContainer::ListViewItemContainer(const QString &title, ListViewItemContainer *parent)
  : KListViewItem(parent, title), _parent(parent), _column(0)
{
  _ids = parent->_ids;
}

ListViewItemContainer::~ListViewItemContainer()
{
  if ( _parent==0 ) delete _ids;
}

ListContainer *ListViewItemContainer::appendBranch(const QString &title)
{
  ListViewItemContainer *branch = new ListViewItemContainer(title, this);
  branch->setColumn(_column);
  branch->setSelectable(false);
  // append instead of prepend
  QListViewItem *litem=firstChild();
  while ( litem && litem->nextSibling() ) litem = litem->nextSibling();
  if (litem) branch->moveItem(litem);
  return branch;
}

void ListViewItemContainer::appendItem(const QPixmap &icon, const QString &title, uint id, ItemState state)
{
  QListViewItem *item = 0;
  if ( state==Normal || state==Disabled ) {
    item = new KListViewItem(this);
    item->setText(_column, title);
  } else {
    item = new QCheckListItem(this, title, QCheckListItem::CheckBox);
    static_cast<QCheckListItem *>(item)->setState(state==Checked ? QCheckListItem::On : QCheckListItem::Off);
  }
  item->setPixmap(_column, icon);
  item->setSelectable(state==Normal);
  // append instead of prepend
  QListViewItem *litem=firstChild();
  while ( litem && litem->nextSibling() ) litem = litem->nextSibling();
  if (litem) item->moveItem(litem);
  (*_ids)[item] = id;
}

int ListViewItemContainer::id(const QListViewItem *item) const
{
  return (_ids->contains(item) ? int((*_ids)[item]) : -1);
}
