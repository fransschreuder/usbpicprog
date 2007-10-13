/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MISC_CHECK_H
#define MISC_CHECK_H

#include "piklab-test/base/main_test.h"

class MiscCheck : public Test
{
Q_OBJECT
public:
  MiscCheck() {}

protected:
  virtual bool execute();
};

#endif
