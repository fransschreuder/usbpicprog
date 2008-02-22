/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DEBUG_CONFIG_H
#define DEBUG_CONFIG_H

#include "common/global/generic_config.h"
#include "common/common/key_enum.h"

namespace Debugger
{

BEGIN_DECLARE_CONFIG(Config)
  OnlyStopOnSourceLine, OnlyStopOnProjectSourceLine
END_DECLARE_CONFIG(Config, "debugger")

} // namespace

#endif
