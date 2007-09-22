/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "register.h"

//----------------------------------------------------------------------------
namespace Register
{
  List *_list = 0;
}
Register::List &Register::list()
{
  if ( _list==0 ) _list = new List;
  return *_list;
}

//----------------------------------------------------------------------------
Register::TypeData::TypeData(uint address, uint nbChars)
  : _address(address), _nbChars(nbChars)
{
  Q_ASSERT( address!=Device::INVALID && nbChars!=0 );
}
Register::TypeData::TypeData(const QString &name, uint nbChars)
  : _address(Device::INVALID), _nbChars(nbChars), _name(name)
{
  Q_ASSERT( !name.isEmpty() && nbChars!=0 );
}
Register::TypeData::TypeData(const QString &name, uint address, uint nbChars)
  : _address(address), _nbChars(nbChars), _name(name)
{
  Q_ASSERT( address!=Device::INVALID && nbChars!=0 && !name.isEmpty() );
}

Register::Type Register::TypeData::type() const
{
  if ( _address==Device::INVALID ) {
    if ( _name.isEmpty() ) return Invalid;
    return Special;
  }
  if ( _name.isEmpty() ) return Regular;
  return Combined;
}

QString Register::TypeData::toString() const
{
  return QString("%1 %2 %3").arg(_address).arg(_nbChars).arg(_name);
}

Register::TypeData Register::TypeData::fromString(const QString &s)
{
  QStringList list = QStringList::split(" ", s);
  if ( list.count()<2 || list.count()>3 ) return TypeData();
  bool ok;
  uint address = list[0].toUInt(&ok);
  if ( !ok ) return TypeData();
  uint nbChars = list[1].toUInt(&ok);
  if ( !ok || nbChars==0 || (nbChars%2)!=0 ) return TypeData();
  QString name;
  if ( list.count()==3 ) name = list[2];
  if ( address==Device::INVALID ) {
    if ( name.isEmpty() ) return TypeData();
    return TypeData(name, nbChars);
  }
  if ( name.isEmpty() ) return TypeData(address, nbChars);
  return TypeData(name, address, nbChars);
}

//----------------------------------------------------------------------------
void Register::List::init()
{
  _regulars.clear();
  _specials.clear();
  _watched.clear();
  _portDatas.clear();
  delayedChanged();
}

void Register::List::setWatched(const TypeData &data, bool watched)
{
  if (watched) {
    if ( _watched.contains(data) ) return;
    _watched.append(data);
  } else _watched.remove(data);
  delayedChanged();
}

void Register::List::clearWatched()
{
  _watched.clear();
  delayedChanged();
}

void Register::List::setValue(const TypeData &data, uint value)
{
  if ( data.address()==Device::INVALID ) {
    _specials[data.name()].old = _specials[data.name()].current;
    _specials[data.name()].current = value;
  } else {
    Q_ASSERT( (data.nbChars()%2)==0 );
    uint nb = data.nbChars()/2;
    for (uint i=0; i<nb; i++) {
      uint address = data.address() + i;
      _regulars[address].old = _regulars[address].current;
      _regulars[address].current = value & 0xFF;
      value <<= 8;
    }
  }
  delayedChanged();
}

void Register::List::setPortData(uint index, const QMap<uint, Device::PortBitData> &data)
{
  _portDatas[index].old = _portDatas[index].current;
  _portDatas[index].current = data;
  delayedChanged();
}

uint Register::List::value(const TypeData &data) const
{
  if ( data.address()==Device::INVALID ) {
    if ( !_specials.contains(data.name()) ) return Device::INVALID;
    return _specials[data.name()].current;
  }
  Q_ASSERT( (data.nbChars()%2)==0 );
  uint nb = data.nbChars()/2;
  uint value = 0;
  for (int i=nb-1; i>=0; i--) {
    value <<= 8;
    uint v = _regulars[data.address() + i].current;
    if ( v==Device::INVALID ) return Device::INVALID;
    value += v;
  }
  return value;
}

uint Register::List::oldValue(const TypeData &data) const
{
  if ( data.address()==Device::INVALID ) {
    if ( !_specials.contains(data.name()) ) return Device::INVALID;
    return _specials[data.name()].old;
  }
  Q_ASSERT( (data.nbChars()%2)==0 );
  uint nb = data.nbChars()/2;
  uint value = 0;
  for (int i=nb-1; i>=0; i--) {
    value <<= 8;
    uint v = _regulars[data.address() + i].old;
    if ( v==Device::INVALID ) return Device::INVALID;
    value += v;
  }
  return value;
}
