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

#include "common/global/global.h"
#include <qstringlist.h>
#include <qmap.h>
namespace Device { class Data; }

namespace Group
{
//-----------------------------------------------------------------------------
class ListerBase;
class BaseGui;
enum SupportType { NotSupported = 0, Untested, Tested, Nb_SupportTypes };
extern const char * const SUPPORT_TYPE_NAMES[Nb_SupportTypes];
struct DeviceData
{
  SupportType supportType;
  const Device::Data *data;
};

//-----------------------------------------------------------------------------
class Base
{
public:
  Base();
  virtual ~Base() {}
  uint index() const;
  virtual QString name() const = 0;
  virtual QString label() const = 0;
  virtual bool isSupported(const QString &device) const;
  virtual SupportType supportType(const QString &device) const;
  const Device::Data *deviceData(const QString &device) const;
  const QMap<QString, DeviceData> &devices() const { return _devices; }
  QValueVector<QString> supportedDevices() const;
  virtual void init();
  const BaseGui *gui() const { return _gui; }
  void checkInitSupported();

protected:
  virtual void addDevice(const DeviceData &data);
  virtual void initSupported() = 0;

private:
  const BaseGui    *_gui;
  const ListerBase *_lister;
  bool              _initialized;
  QMap<QString, DeviceData> _devices;

  friend class ListerBase;
};

class BaseGui
{
public:
  BaseGui() : _group(0) {}
  virtual ~BaseGui() {}
  const Base &group() const { return *_group; }

private:
  const Base *_group;

  friend class ListerBase;
};

class ListerBase
{
public:
  virtual ~ListerBase() {}
  uint nbGroups() const { return _groups.count(); }
  const Base &group(uint i) const { return *_groups[i]; }
  const Base *group(const QString &name) const;
  uint groupIndex(const QString &name) const;
  QValueVector<QString> supportedDevices() const;
  bool isSupported(const QString &device) const;

protected:
  void addGroup(Base *group, BaseGui *gui);

private:
  QValueVector<const Base *> _groups;
};

template <class GroupType>
class Lister : public ListerBase
{
public:
  const GroupType &group(uint i) const { return static_cast<const GroupType &>(ListerBase::group(i)); }
  const GroupType *group(const QString &name) const { return static_cast<const GroupType *>(ListerBase::group(name)); }
};

} // namespace

#endif
