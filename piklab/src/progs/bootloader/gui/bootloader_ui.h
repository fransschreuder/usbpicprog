/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BOOTLOADER_UI_H
#define BOOTLOADER_UI_H

#include <qcheckbox.h>

#include "progs/gui/prog_group_ui.h"
#include "progs/gui/prog_config_widget.h"

namespace Bootloader
{

//----------------------------------------------------------------------------
class GroupUI : public ::Programmer::GroupUI
{
public:
  virtual ::Programmer::ConfigWidget *createConfigWidget(QWidget *parent) const { return new ::Programmer::ConfigWidget(static_cast<const ::Programmer::Group &>(group()), parent, "bootloader_config_widget"); }
  virtual bool hasAdvancedDialog() const { return false; }
  virtual ::Programmer::AdvancedDialog *createAdvancedDialog(::Programmer::Base &, QWidget *) const { return 0; }
  virtual ::ConfigWidget *createSubConfigWidget(QWidget *parent) const = 0;
};

} // namespace

#endif
