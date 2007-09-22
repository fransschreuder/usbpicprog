/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef LIST_VIEW_H
#define LIST_VIEW_H

#include <qtooltip.h>
#include <qvaluevector.h>
#define private public
#define protected public
#include <klistview.h>
#undef private
#undef protected

//-----------------------------------------------------------------------------
class EditListViewItem;
class ListViewToolTip;

class ListView : public KListView
{
Q_OBJECT
public:
  ListView(QWidget *parent = 0, const char *name = 0);
  virtual ~ListView();
  virtual void clear();
  void stopRenaming(bool force);
  virtual QString tooltip(const QListViewItem &item, int column) const;

public slots:
  void cancelRenaming() { stopRenaming(false); }
  void finishRenaming() { stopRenaming(true); }

protected:
  virtual bool eventFilter(QObject *o, QEvent *e);

private:
  ListViewToolTip *_tooltip;
  QValueList<EditListViewItem *> _editItems;

  friend class EditListViewItem;
};

//-----------------------------------------------------------------------------
class ListViewToolTip : public QToolTip
{
public:
  ListViewToolTip(ListView *parent)
    : QToolTip(parent->viewport()), _listView(parent) {}

protected:
  virtual void maybeTip(const QPoint &p);

private:
  ListView *_listView;
};

//-----------------------------------------------------------------------------
class EditListViewItem : public KListViewItem
{
public:
  EditListViewItem(ListView *list);
  EditListViewItem(KListViewItem *item);
  virtual ~EditListViewItem();
  void startRename();
  bool isRenaming() const { return _renaming; }

protected:
  virtual QWidget *editWidgetFactory(int col) const = 0;
  virtual bool alwaysAcceptEdit(int col) const = 0;
  virtual int width(const QFontMetrics &fm, const QListView *lv, int c) const;
  virtual void editDone(int col, const QWidget *editWidget);
  virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);

private:
  bool _renaming;
  QValueVector<QWidget *> _editWidgets;

  virtual void activate() { startRename(); }
  virtual void startRename(int) { startRename(); }
  virtual void okRename(int) { renameDone(true); }
  virtual void cancelRename(int) { renameDone(false); }
  void renameDone(bool force);
  void removeEditBox();

  friend class ListView;
};

#endif
