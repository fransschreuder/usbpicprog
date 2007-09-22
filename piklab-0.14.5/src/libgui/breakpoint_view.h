/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BREAKPOINT_VIEW_H
#define BREAKPOINT_VIEW_H

#include "progs/manager/breakpoint.h"
#include "common/gui/list_view.h"

namespace Breakpoint
{
//----------------------------------------------------------------------------
class ListViewItem : public KListViewItem
{
public:
  ListViewItem(ListView *parent, const Data &data);
  const Data &data() const { return _data; }

private:
  Data _data;
};

//----------------------------------------------------------------------------
class View : public QWidget, public GenericView
{
Q_OBJECT
public:
  View(QWidget *parent);
  virtual void updateView();
  const Data *currentData() const { return _currentData; }

private slots:
  void itemClicked(QListViewItem *item);
  void contextMenu(QListViewItem *item, const QPoint &pos, int col);

private:
  ListView   *_listview;
  const Data *_currentData;
};

} // namespace

#endif
