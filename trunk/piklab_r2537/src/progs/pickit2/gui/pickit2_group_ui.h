/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICKIT2_GROUP_UI_H
#define PICKIT2_GROUP_UI_H

#include "devices/pic/gui/pic_prog_group_ui.h"
#include "progs/pickit2/base/pickit_prog.h"

//----------------------------------------------------------------------------
namespace Pickit
{
class AdvancedDialog : public ::Programmer::PicAdvancedDialog
{
Q_OBJECT
public:
  AdvancedDialog(Base &base, QWidget *parent, const char *name);
  Base &base() { return static_cast<Base &>(_base); }

public slots:
  virtual void regenerateCalibration();
};
} // namespace

//----------------------------------------------------------------------------
namespace Pickit2
{
class GroupUI : public ::Programmer::GroupUI
{
public:
  virtual ::Programmer::ConfigWidget *createConfigWidget(QWidget *parent) const;
  virtual bool hasAdvancedDialog() const { return true; }
  virtual ::Programmer::AdvancedDialog *createAdvancedDialog(::Programmer::Base &base, QWidget *parent) const;
};
} // namespace

#endif
