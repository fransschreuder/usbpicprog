/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef WATCH_VIEW_H
#define WATCH_VIEW_H

#include "common/gui/list_container.h"
#include "common/common/storage.h"
#include "common/gui/list_view.h"
#include "devices/base/register.h"
namespace Device { class Data; }

namespace Register
{

//-----------------------------------------------------------------------------
class BaseListView : public ListView
{
Q_OBJECT
public:
  BaseListView(QWidget *parent);
  virtual void init(const Device::Data *data) = 0;
  virtual void updateView() = 0;

private slots:
  virtual void itemClicked(int button, QListViewItem *item, const QPoint &p, int col) = 0;
  virtual void contextMenu(QListViewItem *item, const QPoint &p, int col) = 0;

protected:
  ListViewItemContainer *_root;
  QValueVector<Register::TypeData> _ids;
};

//-----------------------------------------------------------------------------
class RegisterListView : public BaseListView
{
Q_OBJECT
public:
  RegisterListView(QWidget *parent);
  virtual void init(const Device::Data *data);
  virtual void updateView();

private slots:
  virtual void itemClicked(int button, QListViewItem *item, const QPoint &p, int col);
  virtual void contextMenu(QListViewItem *, const QPoint &, int) {}
};

//-----------------------------------------------------------------------------
class WatchedListView : public BaseListView
{
Q_OBJECT
public:
  WatchedListView(QWidget *parent);
  virtual QString tooltip(const QListViewItem &item, int col) const;
  virtual void init(const Device::Data *data);
  virtual void updateView();

private slots:
  virtual void itemClicked(int button, QListViewItem *item, const QPoint &p, int col);
  virtual void contextMenu(QListViewItem *item, const QPoint &p, int col);

private:
  PopupContainer *_popup;
  KPopupMenu *_formatPopup;
  NumberBase _base;

  static KPopupMenu *appendFormatMenu(KPopupMenu *parent, uint offset);
};

//-----------------------------------------------------------------------------
class WatchView : public QWidget, public GenericView
{
Q_OBJECT
public:
  WatchView(QWidget *parent);
  void init(bool force);

public slots:
  virtual void updateView();

private:
  WatchedListView *_watchedListView;
  RegisterListView *_registerListView;
  const Device::Data *_data;
};

} // namespace

#endif
