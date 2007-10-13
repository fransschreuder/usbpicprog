/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TBL_BOOTLOADER_UI_H
#define TBL8BOOTLOADER_UI_H

#include <kcombobox.h>
#include <knuminput.h>

#include "progs/bootloader/gui/bootloader_ui.h"

//-----------------------------------------------------------------------------
namespace TinyBootloader
{
class ConfigWidget: public GridConfigWidget
{
Q_OBJECT
public:
  ConfigWidget(QWidget *parent);
  virtual void loadConfig();
  virtual void saveConfig();

private:
  KIntNumInput *_timeout, *_retries;
  KComboBox    *_type, *_speed;
};

//----------------------------------------------------------------------------
class GroupUI : public ::Bootloader::GroupUI
{
public:
  virtual ::ConfigWidget *createSubConfigWidget(QWidget *parent) const { return new ConfigWidget(parent); }
  virtual bool hasAdvancedDialog() const { return false; }
  virtual ::Programmer::AdvancedDialog *createAdvancedDialog(::Programmer::Base &, QWidget *) const { return 0; }
};

} // namespace

#endif
