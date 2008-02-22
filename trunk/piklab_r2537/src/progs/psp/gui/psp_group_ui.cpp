/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "psp_group_ui.h"

#include "progs/gui/prog_config_widget.h"
#include "devices/pic/gui/pic_prog_group_ui.h"

::Programmer::ConfigWidget *Psp::GroupUI::createConfigWidget(QWidget *parent) const
{
  return new ::Programmer::ConfigWidget(static_cast<const ::Programmer::Group &>(group()), parent, "psp_config_widget");
}

::Programmer::AdvancedDialog *Psp::GroupUI::createAdvancedDialog(::Programmer::Base &base, QWidget *parent) const
{
  return new ::Programmer::PicAdvancedDialog(static_cast< ::Programmer::PicBase &>(base), parent, "psp_advanced_dialog");
}
