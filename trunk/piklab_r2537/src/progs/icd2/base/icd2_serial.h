/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ICD2_SERIAL_H
#define ICD2_SERIAL_H

#include "icd2.h"
#include "common/port/serial.h"

namespace Icd2
{

//-----------------------------------------------------------------------------
class SerialPort : public Port::Serial
{
public:
  SerialPort(const QString &portDevice, Log::Base &log);
  bool open(Speed speed);
};

//-----------------------------------------------------------------------------
class SerialHardware : public Hardware
{
public:
  SerialHardware(::Programmer::Base &base, const QString &portDevice);

private:
  bool setFastSpeed();
  bool reset();
  virtual bool internalConnect(const QString &mode);
};

} // namespace

#endif
