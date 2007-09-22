/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "storage.h"

#include <qtimer.h>

//----------------------------------------------------------------------------
void GenericStorage::delayedChanged()
{
  if (_dirty) return;
  _dirty = true;
  QTimer::singleShot(0, this, SLOT(changedSlot()));
}

void GenericStorage::changedSlot()
{
  _dirty = false;
  emit changed();
}

//----------------------------------------------------------------------------
void GenericViewProxy::addStorage(GenericStorage &storage)
{
  connect(&storage, SIGNAL(changed()), SLOT(changed()));
}

void GenericViewProxy::changed()
{
  _view.updateView();
}

GenericView::GenericView(GenericStorage &storage)
{
  _proxy = new GenericViewProxy(*this);
  _proxy->addStorage(storage);
}
