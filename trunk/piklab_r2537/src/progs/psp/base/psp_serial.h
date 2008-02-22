/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PSP_SERIAL_H
#define PSP_SERIAL_H

#include "common/port/serial.h"

namespace Psp
{

class SerialPort : public Port::Serial
{
public:
  SerialPort(const QString &portDevice, Log::Base &log);
  bool open();
  bool reset();
  bool command(uchar c, uint nbBytes, QMemArray<uchar> &a);
  bool commandAck(uchar c, uint nbBytes = 1, QMemArray<uchar> *a = 0);
  bool commandAckEnd(uchar c, uint nbBytes, QMemArray<uchar> &a);
  bool receiveEnd();
  bool sendData(uint value, uint nbBytes);

private:
  bool checkAck(uchar ec, uchar rc);
  bool checkEnd(uchar c);
};

} // namespace

#endif
