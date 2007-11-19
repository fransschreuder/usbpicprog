/***************************************************************************
 * Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit2v2_group_ui.h"

#include "progs/gui/prog_config_widget.h"
#include "progs/pickit2v2/base/pickit2v2_prog.h"

::Programmer::ConfigWidget *Pickit2V2::GroupUI::createConfigWidget(QWidget *parent) const
{
  return new ::Programmer::ConfigWidget(static_cast<const ::Programmer::Group &>(group()), parent, "pickit2v2_config_widget");
}

::Programmer::AdvancedDialog *Pickit2V2::GroupUI::createAdvancedDialog(::Programmer::Base &base, QWidget *parent) const
{
  return new ::Programmer::AdvancedDialog(static_cast<Base &>(base), parent, "pickit2v2_advanced_dialog");
}
