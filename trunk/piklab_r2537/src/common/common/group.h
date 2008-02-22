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
#include "key_enum.h"
namespace Device { class Data; }

namespace Group
{
//-----------------------------------------------------------------------------
class BaseGui;
BEGIN_DECLARE_ENUM(Support)
  None = 0, Untested, Tested
END_DECLARE_ENUM_STD(Support)

//-----------------------------------------------------------------------------
class Base
{
public:
  class Data {
  public:
    Data() : data(0), support(Support::None) {}
    const Device::Data *data;
    Support support;
  };
  typedef QMap<QString, Data>::ConstIterator ConstIterator;

  Base();
  virtual ~Base() {}
  virtual QString name() const = 0;
  virtual QString label() const = 0;
  ConstIterator begin() const;
  ConstIterator end() const;
  Data deviceData(const QString &device) const;
  bool isSupported(const QString &device) const { return deviceData(device).support!=Support::None; }
  QValueVector<QString> supportedDevices() const;
  uint count() const;
  const BaseGui *gui() const { return _gui; }
  void checkInitSupported();

protected:
  virtual void init();
  virtual void addDevice(const QString &name, const Device::Data *data, Support support);
  virtual void initSupported() = 0;

  QMap<QString, Data> _devices;

private:
  const BaseGui *_gui;
  bool _initialized;

  template <class GroupType> friend class Lister;
};

//-----------------------------------------------------------------------------
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

} // namespace

#endif
