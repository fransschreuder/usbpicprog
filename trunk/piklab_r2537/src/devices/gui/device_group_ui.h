/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DEVICE_GROUP_UI_H
#define DEVICE_GROUP_UI_H

#include <qpixmap.h>
class QWidget;
class KPopupMenu;
class KListViewItem;
class KAction;

#include "devices/base/generic_device.h"
#include "devices/base/device_group.h"
#include "devices/base/register.h"
namespace Register { class View; class ListViewItem; }
class HexEditor;
class ListContainer;

namespace Device
{
class Memory;
class HexView;
class MemoryEditor;

class GroupUI : public ::Group::BaseGui
{
public:
  virtual HexView *createHexView(const HexEditor &editor, QWidget *parent) const = 0;
  virtual Register::View *createRegisterView(QWidget *parent) const = 0;
  virtual MemoryEditor *createConfigEditor(Device::Memory &memory, QWidget *parent) const = 0;
  virtual void fillWatchListContainer(ListContainer *container, QValueVector<Register::TypeData> &ids) const = 0;
  virtual Register::ListViewItem *createWatchItem(const Register::TypeData &data, KListViewItem *parent) const = 0;
};

inline const Device::GroupUI &groupui(const Device::Data &data) { return static_cast<const Device::GroupUI &>(*data.group().gui()); }

} // namespace

#endif
