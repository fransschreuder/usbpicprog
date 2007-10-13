/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_PROG_GROUP_UI_H
#define PIC_PROG_GROUP_UI_H

#include "progs/gui/prog_group_ui.h"
#include "devices/pic/pic/pic_prog.h"

namespace Programmer
{
class PicAdvancedDialog : public ::Programmer::AdvancedDialog
{
Q_OBJECT
public:
  PicAdvancedDialog(PicBase &base, QWidget *parent, const char *name);
  virtual void updateDisplay();

private:
  QLabel *_voltages[Pic::Nb_VoltageTypes];
  PicBase &base() { return static_cast<PicBase &>(_base); }
};

} // namespace

#endif
