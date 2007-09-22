/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BOOTLOADER_DATA_H
#define BOOTLOADER_DATA_H

#include "common/port/port.h"

namespace Bootloader
{
  enum Type { TinyBootloader = 0, Pickit2Bootloader, PicdemBootloader, Nb_Types };
  struct TypeData {
    const char *name, *label, *tag;
    Port::Type portType;
  };
  extern const TypeData TYPE_DATA[Nb_Types];

  struct Data {
    uchar ids[Nb_Types];
    bool  supported[Nb_Types];
  };
  extern bool isSupported(const QString &device);
  extern const Data &data(const QString &device);
} // namespace

#endif
