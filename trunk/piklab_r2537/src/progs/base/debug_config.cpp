/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "debug_config.h"

const Debugger::Config::Data Debugger::Config::DATA[Nb_Types] = {
  { "only_stop_on_source_line", I18N_NOOP("Only stop stepping on source line."), QVariant(true, 0) },
  { "only_stop_on_project_source_line", I18N_NOOP("Only stop stepping on project source line."), QVariant(true, 0) }
};
