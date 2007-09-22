/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DEVICE_GROUP_H
#define DEVICE_GROUP_H

#include <qcolor.h>

#include "generic_device.h"
#include "common/common/group.h"
#include "common/common/streamer.h"
namespace Debugger { class DeviceSpecific; class Base; }

namespace Device
{
class Memory;

//----------------------------------------------------------------------------
class MemoryRange {
public:
  MemoryRange() {}
  virtual ~MemoryRange() {}
  virtual bool all() const { return true; }
};

//----------------------------------------------------------------------------
class GroupBase : public ::Group::Base
{
public:
  virtual Memory *createMemory(const Data &data) const = 0;

protected:
  virtual void addDevice(const ::Group::DeviceData &data) {
    const_cast<Data *>(data.data)->_group = this;
    ::Group::Base::addDevice(data);
  }
};

template <class DataType>
class Group : public GroupBase, public DataStreamer<DataType>
{
protected:
  virtual void initSupported() {
    QValueList<DataType *> list = fromCppString(dataStream(), dataSize());
    for (uint i=0; i<uint(list.count()); i++) {
      ::Group::DeviceData data;
      data.supportType = ::Group::Tested;
      data.data = list[i];
      addDevice(data);
    }
  }
  virtual uint dataSize() const = 0;
  virtual const char *dataStream() const = 0;
};

//----------------------------------------------------------------------------
extern QColor statusColor(Device::Status status);
extern QString htmlInfo(const Data &data, const QString &deviceHref, const QString &additionnalHtml,
                        const QString &imagePrefix, QMimeSourceFactory *msf);
extern QPixmap vddGraph(const QString &xLabel, const QString &yLabel, const QValueVector<RangeBox> &boxes);
extern const Package *barPackage(const char *name, const Data &data);
extern QPixmap pinsGraph(const Package &package);

} // namespace

#endif
