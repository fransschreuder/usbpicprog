/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "prog_list.h"

namespace Programmer
{
  Lister *_lister = 0;
}

const Programmer::Lister &Programmer::lister()
{
  if ( _lister==0 ) _lister = new Lister;
  return *_lister;
}
