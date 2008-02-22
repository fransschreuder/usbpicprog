/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "key_gui.h"

//-----------------------------------------------------------------------------
KeyComboBox::KeyComboBox(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  _combo = new QComboBox(this);
  QHBoxLayout *top = new QHBoxLayout(this);
  top->addWidget(_combo);
  connect(_combo, SIGNAL(activated(int)), SIGNAL(activated()));
}

void KeyComboBox::clear()
{
  _keys.clear();
  _combo->clear();
}

void KeyComboBox::fixMinimumWidth()
{
  setMinimumWidth(_combo->sizeHint().width());
}

//-----------------------------------------------------------------------------
KeyWidgetStack::KeyWidgetStack(QWidget *parent, const char *name)
  : QFrame(parent, name)
{
  _stack = new QWidgetStack(this);
  QHBoxLayout *top = new QHBoxLayout(this);
  top->addWidget(_stack);
}

//-----------------------------------------------------------------------------
KeyPopupButton::KeyPopupButton(QWidget *parent, const char *name)
  : PopupButton(parent, name)
{
  connect(this, SIGNAL(activated(int)), SLOT(activatedSlot(int)));
}

void KeyPopupButton::activatedSlot(int id)
{
  QMap<QString, int>::const_iterator it;
  for (it=_keys.begin(); it!=_keys.end(); ++it)
    if ( it.data()==id ) emit activated(it.key());
}
