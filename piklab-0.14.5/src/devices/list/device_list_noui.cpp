/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "device_list.h"

#include "devices/pic/pic/pic_group.h"
#include "devices/mem24/mem24/mem24_group.h"

Device::Lister::Lister()
{
  addGroup(new Pic::Group, 0);
  addGroup(new Mem24::Group, 0);
}
