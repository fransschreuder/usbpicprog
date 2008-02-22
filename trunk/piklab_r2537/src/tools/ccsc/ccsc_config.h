/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CCSC_CONFIG_H
#define CCSC_CONFIG_H

#include "tools/list/compile_config.h"

namespace CCSC
{

class Config : public Compile::Config
{
public:
  Config(Project *project) : Compile::Config(project) {}
};

} // namespace

#endif
