/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit1_group_ui.h"

#include "progs/gui/prog_config_widget.h"
#include "progs/pickit2/gui/pickit2_group_ui.h"
#include "progs/pickit1/base/pickit1_prog.h"

::Programmer::ConfigWidget *Pickit1::GroupUI::createConfigWidget(QWidget *parent) const
{
  return new ::Programmer::ConfigWidget(static_cast<const ::Programmer::Group &>(group()), parent, "pickit1_config_widget");
}

::Programmer::AdvancedDialog *Pickit1::GroupUI::createAdvancedDialog(::Programmer::Base &base, QWidget *parent) const
{
  return new Pickit::AdvancedDialog(static_cast<Base &>(base), parent, "pickit1_advanced_dialog");
}
