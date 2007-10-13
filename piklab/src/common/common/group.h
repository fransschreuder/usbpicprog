/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GROUP_H
#define GROUP_H

#include <qstringlist.h>
#include <qmap.h>

#include "common/global/global.h"
namespace Device { class Data; }

namespace Group
{
//-----------------------------------------------------------------------------
class BaseGui;
enum SupportType { NotSupported = 0, Untested, Tested, Nb_SupportTypes };
extern const char * const SUPPORT_TYPE_NAMES[Nb_SupportTypes];

//-----------------------------------------------------------------------------
class Base
{
public:
  class Data {
  public:
    Data() : data(0), supportType(NotSupported) {}
    const Device::Data *data;
    SupportType supportType;
  };
  typedef QMap<QString, Data>::ConstIterator ConstIterator;

  Base();
  virtual ~Base() {}
  virtual QString name() const = 0;
  virtual QString label() const = 0;
  ConstIterator begin() const;
  ConstIterator end() const;
  Data deviceData(const QString &device) const;
  bool isSupported(const QString &device) const { return deviceData(device).supportType!=NotSupported; }
  QValueVector<QString> supportedDevices() const;
  uint count() const;
  const BaseGui *gui() const { return _gui; }
  void checkInitSupported();

protected:
  virtual void init();
  virtual void addDevice(const QString &name, const Device::Data *data, SupportType type);
  virtual void initSupported() = 0;

  QMap<QString, Data> _devices;

private:
  const BaseGui *_gui;
  bool _initialized;

  template <class GroupType> friend class Lister;
};

class BaseGui
{
public:
  BaseGui() : _group(0) {}
  virtual ~BaseGui() {}
  const Base &group() const { return *_group; }

private:
  const Base *_group;

  template <class GroupType> friend class Lister;
};

//-----------------------------------------------------------------------------
template <class GroupType>
class Lister
{
public:
  typedef typename QMap<QString, const GroupType *>::ConstIterator ConstIterator;
  ConstIterator begin() const { return ConstIterator(_groups.begin()); }
  ConstIterator end() const { return ConstIterator(_groups.end()); }

  virtual ~Lister() {
    for (ConstIterator it=begin(); it!=end(); ++it) delete it.data();
  }

  QValueVector<QString> supportedDevices() const {
    QValueVector<QString> names;
    for (ConstIterator it=begin(); it!=end(); ++it) {
      QValueVector<QString> gnames = it.data()->supportedDevices();
      for (uint k=0; k<uint(gnames.count()); k++) names.append(gnames[k]);
    }
    return names;
  }

  uint count() const {
    uint nb = 0;
    for (ConstIterator it=begin(); it!=end(); ++it) nb += it.data()->count();
    return nb;
  }

  bool isSupported(const QString &device) const {
    for (ConstIterator it=begin(); it!=end(); ++it)
      if ( it.data()->isSupported(device) ) return true;
    return false;
  }

  const GroupType *group(const QString &name) const {
    if ( _groups.contains(name) ) return _groups[name];
    return 0;
  }

protected:
  void addGroup(GroupType *group, BaseGui *gui) {
    Q_ASSERT(group);
    group->_gui = gui;
    if (gui) gui->_group = group;
    group->init();
    Q_ASSERT( !_groups.contains(group->name()) );
    _groups.insert(group->name(), group);
  }

private:
  QMap<QString, const GroupType *> _groups;
};

} // namespace

#endif
