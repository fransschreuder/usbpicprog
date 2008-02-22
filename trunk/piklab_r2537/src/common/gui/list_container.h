/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef LIST_CONTAINER_H
#define LIST_CONTAINER_H

#include <kpopupmenu.h>
#include <klistview.h>

//----------------------------------------------------------------------------
class ListContainer
{
public:
  virtual ~ListContainer() {}
  virtual ListContainer *appendBranch(const QString &title) = 0;
  enum ItemState { Normal, Checked, UnChecked, Disabled };
  void appendItem(const QString &label, uint id, ItemState state) { appendItem(QPixmap(), label, id, state); }
  virtual void appendItem(const QPixmap &icon, const QString &label, uint id, ItemState state) = 0;
};

//----------------------------------------------------------------------------
class PopupContainer : public KPopupMenu, public ListContainer
{
Q_OBJECT
public:
  PopupContainer(const QString &title, QWidget *parent = 0, const char *name = 0);
  virtual ListContainer *appendBranch(const QString &title);
  virtual ListContainer *appendBranch(const QPixmap &icon, const QString &title);
  virtual void appendItem(const QPixmap &icon, const QString &label, uint id, ItemState state);
};

//----------------------------------------------------------------------------
class ListViewItemContainer : public KListViewItem, public ListContainer
{
public:
  ListViewItemContainer(const QString &title, KListView *parent);
  ListViewItemContainer(const QString &title, ListViewItemContainer *parent);
  virtual ~ListViewItemContainer();
  void setColumn(uint column) { _column = column; }
  virtual ListContainer *appendBranch(const QString &title);
  virtual void appendItem(const QPixmap &icon, const QString &label, uint id, ItemState state);
  int id(const QListViewItem* item) const; // -1 if not known

private:
  ListViewItemContainer *_parent;
  uint _column;
  QMap<const QListViewItem *, uint> *_ids;
};

#endif
