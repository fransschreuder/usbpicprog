/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "progs/bootloader/base/bootloader_data.h"

const Bootloader::TypeData Bootloader::TYPE_DATA[Nb_Types] = {
  { "tbl_bootloader",     I18N_NOOP("Tiny Bootloader"),    "tbl",     Port::SerialType },
  { "pickit2_bootloader", I18N_NOOP("Pickit2 Bootloader"), "pickit2", Port::USBType    },
  { "picdem_bootloader",  I18N_NOOP("Picdem Bootloader"),  "picdem",  Port::USBType    }
};

