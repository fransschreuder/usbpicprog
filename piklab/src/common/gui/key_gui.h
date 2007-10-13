/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KEY_GUI_H
#define KEY_GUI_H

#include <qcombobox.h>
#include <qwidgetstack.h>

#include "common/gui/misc_gui.h"
#include "common/common/misc.h"

//-----------------------------------------------------------------------------
template <class Type>
class KeyWidget
{
public:
  typedef QMapConstIterator<QString, int> ConstIterator;

  KeyWidget() {}
  ConstIterator begin() const { return _keys.begin(); }
  ConstIterator end() const { return _keys.end(); }
  uint count() const { return _keys.count(); }
  void appendItem(const QString &key, Type type) {
    CRASH_ASSERT( !_keys.contains(key) );
    _keys[key] = append(type);
  }
  QString currentItem() const {
    QMap<QString, int>::const_iterator it;
    for (it=_keys.begin(); it!=_keys.end(); it++)
      if ( it.data()==currentId() ) return it.key();
    return QString::null;
  }
  void setCurrentItem(const QString &key) {
    if ( _keys.contains(key) ) setCurrentId(_keys[key]);
  }
  bool contains(const QString &key) const { return _keys.contains(key); }
  Type item(const QString &key) const {
    CRASH_ASSERT( _keys.contains(key) );
    return get(_keys[key]);
  }
  Type item(ConstIterator it) const {
    CRASH_ASSERT( it!=end() );
    return get(it.data());
  }

protected:
  virtual int append(Type type) = 0;
  virtual int currentId() const = 0;
  virtual void setCurrentId(int id) = 0;
  virtual Type get(int id) const = 0;

  QMap<QString, int> _keys;
};

//-----------------------------------------------------------------------------
class KeyComboBox : public QWidget, public KeyWidget<QString>
{
Q_OBJECT
public:
  KeyComboBox(QWidget *parent = 0, const char *name = 0);
  void clear();
  void fixMinimumWidth();

signals:
  void activated();

private:
  virtual int append(QString label) { _combo->insertItem(label); return _combo->count()-1; }
  virtual int currentId() const { return _combo->currentItem(); }
  virtual void setCurrentId(int id) { _combo->setCurrentItem(id); }
  virtual QString get(int id) const { return _combo->text(id); }

  QComboBox *_combo;
};

//-----------------------------------------------------------------------------
class KeyWidgetStack : public QFrame, public KeyWidget<QWidget *>
{
Q_OBJECT
public:
  KeyWidgetStack(QWidget *parent = 0, const char *name = 0);

private:
  virtual int append(QWidget *widget) { return _stack->addWidget(widget); }
  virtual int currentId() const { return _stack->id(_stack->visibleWidget()); }
  virtual void setCurrentId(int id) { _stack->raiseWidget(id); }
  virtual QWidget *get(int id) const { return _stack->widget(id); }

  QWidgetStack *_stack;
};

//-----------------------------------------------------------------------------
class KeyPopupButton : public PopupButton, public KeyWidget<QString>
{
Q_OBJECT
public:
  KeyPopupButton(QWidget *parent = 0, const char *name = 0);
  void appendItem(const QString &key, QString label) { KeyWidget<QString>::appendItem(key, label); }

signals:
  void activated(const QString &name);

private slots:
  void activatedSlot(int id);

private:
  virtual int append(QString label) { return PopupButton::appendItem(label, QPixmap()); }
  virtual QString get(int) const { return QString::null; }

  // disabled
  QString currentItem() const;
  void setCurrentItem(const QString &key);
  virtual int currentId() const { return 0; }
  virtual void setCurrentId(int) {}
};

#endif
