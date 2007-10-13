/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SDCC_CONFIG_H
#define SDCC_CONFIG_H

#include "tools/gputils/gputils_config.h"

namespace SDCC
{
//----------------------------------------------------------------------------
class Config : public GPUtils::Config
{
public:
  Config(Project *project) : GPUtils::Config(project) {}
};

} // namespace

#endif
