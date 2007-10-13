/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef REGISTER_VIEW_H
#define REGISTER_VIEW_H

#include <qframe.h>

#include "devices/base/register.h"
#include "common/gui/number_gui.h"
#include "common/gui/list_view.h"

namespace Register
{
enum { PortBitRtti = 1000, RegisterRtti = 1001 };

//-----------------------------------------------------------------------------
class PortBitListViewItem : public KListViewItem
{
public:
  PortBitListViewItem(uint address, uint bit, KListViewItem *parent);
  virtual int rtti() const { return PortBitRtti; }
  void updateView();
  QString tooltip(int column) const;

private:
  uint _index, _bit;

  virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);
};

//-----------------------------------------------------------------------------
class ListViewItem : public EditListViewItem
{
public:
  ListViewItem(const TypeData &data, KListViewItem *item);
  virtual int rtti() const { return RegisterRtti; }
  virtual void updateView();
  const TypeData &data() const { return _data; }
  QString tooltip(int column) const;
  Number::Base base() const { return _base; }
  void setBase(Number::Base base);
  virtual QString label() const = 0;

protected:
  TypeData     _data;
  QValueList<PortBitListViewItem *> _items;
  Number::Base _base;

  virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);
  virtual QWidget *editWidgetFactory(int col) const;
  virtual bool alwaysAcceptEdit(int) const { return false; }
  virtual void editDone(int col, const QWidget *editWidget);
  virtual uint nbCharsAddress() const = 0;
  virtual QString text() const;
  virtual void activate() {}
};

//-----------------------------------------------------------------------------
class LineEdit : public Number::LineEdit
{
Q_OBJECT
public:
  LineEdit(QWidget *parent, const char *name = 0);
  void setValue(Number::Base base, BitValue value, uint nbChars);
  BitValue value() const { return _value; }

signals:
  void modified();

protected:
  virtual void keyPressEvent(QKeyEvent *e);

private slots:
  void updateText();
  void returnPressedSlot();

private:
  Number::Base _base;
  BitValue     _value;
  uint         _nbChars;
};

//-----------------------------------------------------------------------------
class View : public QFrame, public GenericView
{
Q_OBJECT
public:
  View(QWidget *parent, const char *name);

signals:
  void readSignal(uint address);
  void writeSignal(uint address, uint value);
  void setWatchedSignal(uint address, bool watched);
};

} // namespace

#endif
