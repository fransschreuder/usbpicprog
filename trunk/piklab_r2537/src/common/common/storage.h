/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef STORAGE_H
#define STORAGE_H

#include "common/global/global.h"
#include <qobject.h>

//-----------------------------------------------------------------------------
template <class Type>
class Fifo
{
public:
  Fifo() {}
  void clear() { _list.clear(); }
  uint count() const { return _list.count(); }
  void put(Type type) { _list.append(type); }
  Type get() {
    Type t = _list.first();
    _list.pop_front();
    return t;
  }

private:
  QValueList<Type> _list;
};

//----------------------------------------------------------------------------
class GenericStorage : public QObject
{
Q_OBJECT
public:
  GenericStorage(QObject *parent = 0, const char *name = 0) : QObject(parent, name), _dirty(false) {}

signals:
  void changed();

protected:
  // emit changed() only after a return to the GUI loop and only one time
  void delayedChanged();

private slots:
  void changedSlot();

private:
  bool _dirty;
};

//----------------------------------------------------------------------------
class GenericView;

class GenericViewProxy : public QObject
{
Q_OBJECT
public:
  GenericViewProxy(GenericView &view) : _view(view) {}
  void addStorage(GenericStorage &storage);

private slots:
  void changed();

private:
  GenericView &_view;
};

class GenericView
{
public:
  GenericView(GenericStorage &storage);
  virtual ~GenericView() { delete _proxy; }
  virtual void updateView() = 0;

private:
  GenericViewProxy *_proxy;

  friend class GenericViewProxy;
};

#endif
