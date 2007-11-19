/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GPUTILS_CONFIG_GENERATOR_CHECK_H
#define GPUTILS_CONFIG_GENERATOR_CHECK_H

#include "piklab-test/base/generator_check.h"

class GPUtilsConfigGeneratorCheck : public ConfigGeneratorCheck
{
public:
  GPUtilsConfigGeneratorCheck() : ConfigGeneratorCheck(new GPUtilsGeneratorCheckHelper) {}
};

#endif
