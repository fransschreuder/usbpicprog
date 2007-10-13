/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "prog_list.h"

#include "progs/direct/base/direct_prog.h"
#include "progs/icd2/base/icd2_prog.h"
#include "progs/icd2/base/icd2_debug.h"
#include "progs/icd1/base/icd1_prog.h"
#include "progs/pickit2/base/pickit2_prog.h"
#include "progs/pickit1/base/pickit1_prog.h"
#include "progs/psp/base/psp_prog.h"
#include "progs/gpsim/base/gpsim_debug.h"
#include "progs/picdem_bootloader/base/picdem_bootloader_prog.h"
#include "progs/pickit2_bootloader/base/pickit2_bootloader_prog.h"
#include "progs/tbl_bootloader/base/tbl_bootloader_prog.h"
#include "progs/pickit2v2/base/pickit2v2_prog.h"

Programmer::Lister::Lister()
{
  addGroup(new Direct::DGroup, 0);
  addGroup(new Icd2::ProgrammerGroup, 0);
  addGroup(new Icd2::DebuggerGroup, 0);
  addGroup(new Icd1::Group, 0);
  addGroup(new Pickit2::Group, 0);
  //addGroup(new Pickit2V2::Group, 0);
  addGroup(new Pickit1::Group, 0);
  addGroup(new Psp::Group, 0);
  addGroup(new GPSim::Group, 0);
  addGroup(new PicdemBootloader::Group, 0);
  addGroup(new Pickit2Bootloader::Group, 0);
  addGroup(new TinyBootloader::Group, 0);
}
