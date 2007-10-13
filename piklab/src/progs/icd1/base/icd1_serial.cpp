/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "icd1_serial.h"

#include <qdatetime.h>
#include "common/global/global.h"
#include "common/common/misc.h"

//-----------------------------------------------------------------------------
Icd1::SerialPort::SerialPort(const QString &device, Log::Base &log)
  : Port::Serial(device, NeedDrain | NeedFlush, log)
{}

bool Icd1::SerialPort::open()
{
  if ( !Port::Serial::open() ) return false;
  return setMode(NoInputFlag, ByteSize8 | IgnoreControlLines | EnableReceiver, S57600, 0);
}

bool Icd1::SerialPort::reset()
{
  // reset
  setPinOn(RTS, false, Port::PositiveLogic);
  setPinOn(DTR, false, Port::PositiveLogic);
  Port::msleep(10);
  // remove reset
  setPinOn(RTS, true, Port::PositiveLogic);
  setPinOn(DTR, true, Port::PositiveLogic);
  Port::msleep(10);
  return true;
}

bool Icd1::SerialPort::synchronize()
{
  if ( !setPinOn(RTS, false, Port::PositiveLogic) ) return false;
  QTime time;
  time.start();
  for (;;) {
    bool bit;
    if ( !readPin(CTS, Port::PositiveLogic, bit) ) return false;
    if ( !bit) break;
    if ( uint(time.elapsed())>3000 ) { // 3 seconds
      log(Log::Error, i18n("Timeout synchronizing."));
      return false;
    }
  }
  return setPinOn(RTS, true, Port::PositiveLogic);
}
  
bool Icd1::SerialPort::sendCommand(uint cmd)
{
  Q_ASSERT( cmd<=0xFFFF );
  synchronize();
  char c[7] = "$XXXX\r";
  QString cs = toHex(cmd, 4);
  log(Log::ExtraDebug, QString("Send command: %1").arg(toPrintable(cs, PrintAlphaNum)));
  c[1] = cs[0].latin1();
  c[2] = cs[1].latin1();
  c[3] = cs[2].latin1();
  c[4] = cs[3].latin1();
  return send(c, 7);
}

bool Icd1::SerialPort::receiveByte(char &byte, bool synchronizeBefore, uint timeout)
{
  if ( synchronizeBefore && !synchronize() ) return false;
  return receiveChar(byte, timeout);
}
