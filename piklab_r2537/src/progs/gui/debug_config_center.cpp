/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "debug_config_center.h"

#include <kiconloader.h>

QPixmap Debugger::OptionsConfigWidget::pixmap() const
{
  KIconLoader loader;
  return loader.loadIcon("piklab_config_debugger", KIcon::Toolbar, KIcon::SizeMedium);
}
