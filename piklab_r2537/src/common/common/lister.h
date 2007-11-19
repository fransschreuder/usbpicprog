/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef LISTER_H
#define LISTER_H

#include "group.h"

namespace Group
{
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
