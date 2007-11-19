/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit2_bootloader_ui.h"

//-----------------------------------------------------------------------------
Pickit2Bootloader::ConfigWidget::ConfigWidget(const::Programmer::Group &group, QWidget *parent)
  : ::Programmer::ConfigWidget(group, parent, "pickit2_bootloader_config_widget")
{}

//-----------------------------------------------------------------------------
::Programmer::ConfigWidget *Pickit2Bootloader::GroupUI::createConfigWidget(QWidget *parent) const
{
  return new ConfigWidget(static_cast<const ::Programmer::Group &>(group()), parent);
}
