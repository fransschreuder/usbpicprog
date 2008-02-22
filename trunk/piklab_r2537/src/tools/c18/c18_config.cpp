/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "c18_config.h"

const char * const C18::Config::WARNING_LEVEL_LABELS[Nb_WarningLevels] = {
  I18N_NOOP("All messages"), I18N_NOOP("Warning and errors"), I18N_NOOP("Errors only")
};
