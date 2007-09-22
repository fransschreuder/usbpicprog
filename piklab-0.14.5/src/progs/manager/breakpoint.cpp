/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "breakpoint.h"

#include "coff/base/coff.h"

//----------------------------------------------------------------------------
namespace Breakpoint
{
  List *_list = 0;
}
Breakpoint::List &Breakpoint::list()
{
  if ( _list==0 ) _list = new List;
  return *_list;
}

void Breakpoint::List::append(const Data &data)
{
  Q_ASSERT( !contains(data) );
  StateData sdata;
  sdata.data = data;
  _list.append(sdata);
  delayedChanged();
}

void Breakpoint::List::remove(const Data &data)
{
  Q_ASSERT( contains(data) );
  _list.remove(find(data));
  delayedChanged();
}

void Breakpoint::List::clear()
{
  _list.clear();
  delayedChanged();
}

QValueList<Breakpoint::List::StateData>::iterator Breakpoint::List::find(const Data &data)
{
  QValueList<StateData>::iterator it;
  for (it=_list.begin(); it!=_list.end(); ++it)
    if ( (*it).data==data ) return it;
  return _list.end();
}

QValueList<Breakpoint::List::StateData>::const_iterator Breakpoint::List::find(const Data &data) const
{
  QValueList<StateData>::const_iterator it;
  for (it=_list.begin(); it!=_list.end(); ++it)
    if ( (*it).data==data ) return it;
  return _list.end();
}

void Breakpoint::List::setState(const Data &data, State state)
{
  Q_ASSERT( contains(data) );
  (*find(data)).state = state;
  delayedChanged();
}

void Breakpoint::List::setAddress(const Data &data, uint address)
{
  Q_ASSERT( contains(data) );
  (*find(data)).address = address;
  delayedChanged();
}
