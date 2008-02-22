/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICDEM_BOOTLOADER_UI_H
#define PICDEM_BOOTLOADER_UI_H

#include "progs/bootloader/gui/bootloader_ui.h"

namespace PicdemBootloader
{
//----------------------------------------------------------------------------
class ConfigWidget: public ::Programmer::ConfigWidget
{
Q_OBJECT
public:
  ConfigWidget(const ::Programmer::Group &group, QWidget *parent);
  virtual void loadConfig();
  virtual void saveConfig();

private:
  HexWordEditor *_vendorId, *_productId;
};

//----------------------------------------------------------------------------
class GroupUI : public ::Bootloader::GroupUI
{
public:
  virtual ::Programmer::ConfigWidget *createConfigWidget(QWidget *parent) const;
};
} // namespace

#endif
