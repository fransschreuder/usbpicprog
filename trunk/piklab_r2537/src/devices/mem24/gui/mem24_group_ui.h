/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MEM24_GROUP_UI_H
#define MEM24_GROUP_UI_H

#include "devices/gui/device_group_ui.h"

namespace Mem24
{
class GroupUI : public Device::GroupUI
{
public:
  virtual Device::HexView *createHexView(const HexEditor &editor, QWidget *parent) const;
  virtual Register::View *createRegisterView(QWidget *) const { return 0; }
  virtual Device::MemoryEditor *createConfigEditor(Device::Memory &, QWidget *) const { return 0; }
  virtual void fillWatchListContainer(ListContainer *, QValueVector<Register::TypeData> &) const {}
  virtual Register::ListViewItem *createWatchItem(const Register::TypeData &, KListViewItem *) const { return 0; }
};

} // namespace

#endif
