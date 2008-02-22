/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DEBUG_CONFIG_CENTER_H
#define DEBUG_CONFIG_CENTER_H

#include <qcheckbox.h>

#include "common/gui/config_widget.h"
#include "progs/base/debug_config.h"

namespace Debugger
{

BEGIN_DECLARE_CONFIG_WIDGET(Config, OptionsConfigWidget)
  virtual QString header() const { return i18n("Debugging Options"); }
  virtual QString title() const { return i18n("Debugging Options"); }
  virtual QPixmap pixmap() const;
END_DECLARE_CONFIG_WIDGET

} // namespace

#endif
