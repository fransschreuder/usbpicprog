/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICKIT2_BOOTLOADER_DATA_H
#define PICKIT2_BOOTLOADER_DATA_H

namespace Pickit2Bootloader
{
  struct Data {};
  extern bool isSupported(const QString &device);
  extern const Data &data(const QString &device);
} // namespace

#endif
