/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_GROUP_UI_H
#define PIC_GROUP_UI_H

#include "devices/gui/device_group_ui.h"

namespace Pic
{

class GroupUI : public Device::GroupUI
{
public:
  virtual Device::HexView *createHexView(const HexEditor &editor, QWidget *parent) const;
  virtual Register::View *createRegisterView(QWidget *parent) const;
  virtual Device::MemoryEditor *createConfigEditor(Device::Memory &memory, QWidget *parent) const;
  virtual void fillWatchListContainer(ListContainer *container, QValueVector<Register::TypeData> &ids) const;
  virtual Register::ListViewItem *createWatchItem(const Register::TypeData &data, KListViewItem *parent) const;
};

} // namespace

#endif
