/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit2_bootloader_prog.h"

//-----------------------------------------------------------------------------
Pickit2Bootloader::ProgrammerBase::ProgrammerBase(const Programmer::Group &group, const Pic::Data *data)
  : Bootloader::ProgrammerBase(group, data, "pickit2_bootloader_programmer_base")
{}
