/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef WATCH_VIEW_H
#define WATCH_VIEW_H

class KPopupMenu;
#include "common/common/storage.h"
#include "common/gui/list_view.h"
#include "devices/base/register.h"
namespace Device { class Data; }

namespace Register
{

class WatchView : public ListView, public GenericView
{
Q_OBJECT
public:
  WatchView(QWidget *parent);
  void init(bool force);
  virtual QString tooltip(const QListViewItem &item, int col) const;

public slots:
  virtual void updateView();

private slots:
  void itemClicked(int button, QListViewItem *item, const QPoint &p, int col);
  void contextMenu(QListViewItem *item, const QPoint &p, int col);

private:
  KListViewItem *_root;
  KPopupMenu    *_popup, *_formatPopup;
  QValueVector<Register::TypeData> _ids;
  const Device::Data *_data;
  Number::Base _base;
};

} // namespace

#endif
