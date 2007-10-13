/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BOOTLOADER_PROG_H
#define BOOTLOADER_PROG_H

#include "common/global/generic_config.h"
#include "devices/pic/pic/pic_prog.h"
#include "bootloader.h"

namespace Bootloader
{
//-----------------------------------------------------------------------------
class ProgrammerBase : public Programmer::PicBase
{
Q_OBJECT
public:
  ProgrammerBase(const Programmer::Group &group, const Pic::Data *data, const char *name);

protected:
  Hardware &hardware() { return static_cast<Hardware &>(*_hardware); }
};

//-----------------------------------------------------------------------------
class Group : public ::Programmer::PicGroup
{
public:
  virtual bool checkConnection(const ::Programmer::HardwareDescription &hd) const;
};

} // namespace

#endif
