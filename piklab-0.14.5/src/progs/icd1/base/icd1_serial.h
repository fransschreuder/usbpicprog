/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ICD1_SERIAL_H
#define ICD1_SERIAL_H

#include "common/port/serial.h"

namespace Icd1
{

class SerialPort : public Port::Serial
{
public:
  SerialPort(const QString &portDevice, Log::Base &log);
  bool open();
  bool reset();
  bool synchronize();
  bool sendCommand(uint cmd);
  bool receiveByte(char &byte, bool synchronizeBefore, uint timeout = DEFAULT_TIMEOUT);
};

} // namespace

#endif
