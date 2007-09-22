/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "mem24_group.h"

#include "mem24_memory.h"

Device::Memory *Mem24::Group::createMemory(const Device::Data &data) const
{
  return new Memory(static_cast<const Data &>(data));
}
