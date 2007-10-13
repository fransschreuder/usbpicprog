/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef COFF_DATA_H
#define COFF_DATA_H

namespace Coff
{
  enum { MAX_NB_IDS = 2 };
  struct Data {
    uint ids[MAX_NB_IDS];
  };
  extern QString findId(uint id);

} // namespace

#endif
