/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "psp_serial.h"

#include "common/global/global.h"
#include "common/common/misc.h"

//-----------------------------------------------------------------------------
Psp::SerialPort::SerialPort(const QString &device, Log::Base &log)
  : Port::Serial(device, NeedFlush, log)
{}

bool Psp::SerialPort::open()
{
  if ( !Port::Serial::open() ) return false;
  return setMode(NoInputFlag, ByteSize8 | IgnoreControlLines | EnableReceiver, S19200, 100);
}

bool Psp::SerialPort::reset()
{
  if ( !setHardwareFlowControl(false) ) return false;
  // reset
  setPinOn(DTR, false, Port::PositiveLogic);
  Port::msleep(250);
  // remove reset
  setPinOn(DTR, true, Port::PositiveLogic);
  for (uint i=0; i<=100; i++) {
    bool bit;
    if ( !readPin(CTS, Port::PositiveLogic, bit) ) return false;
    if ( bit ) break;
    if ( i==100 ) {
      log(Log::Error, i18n("Could not contact Picstart+"));
      return false;
    }
    Port::msleep(1);
  }
  if ( !setHardwareFlowControl(true) ) return false;
  return flush(0);
}

bool Psp::SerialPort::command(uchar c, uint nbBytes, QMemArray<uchar> &a)
{
  log(Log::ExtraDebug, QString("Command %1").arg(toHexLabel(c, 2)));
  if ( !sendChar(c) ) return false;
  return receive(nbBytes, a);
}

bool Psp::SerialPort::checkAck(uchar ec, uchar rc)
{
  if ( ec==rc ) return true;
  log(Log::Error, i18n("Incorrect ack: expecting %1, received %2")
                  .arg(QString(toHex(ec, 2))).arg(QString(toHex(rc, 2))));
  return false;
}

bool Psp::SerialPort::checkEnd(uchar c)
{
  if ( c==0 ) return true;
  log(Log::Error, i18n("Incorrect received data end: expecting 00, received %1")
                  .arg(QString(toHex(c, 2))));
  return false;
}

bool Psp::SerialPort::commandAck(uchar c, uint nbBytes, QMemArray<uchar> *a)
{
  Q_ASSERT( nbBytes>=1 );
  QMemArray<uchar> tmp;
  if ( !command(c, nbBytes, tmp) ) return false;
  if ( !checkAck(c, tmp[0]) ) return false;
  if (a) *a = tmp;
  return true;
}

bool Psp::SerialPort::receiveEnd()
{
  QMemArray<uchar> a;
  if ( !receive(1, a) ) return false;
  if ( !checkEnd(a[0]) ) return false;
  return true;
}

bool Psp::SerialPort::commandAckEnd(uchar c, uint nbBytes, QMemArray<uchar> &a)
{
  Q_ASSERT( nbBytes>=2 );
  if ( !command(c, nbBytes, a) ) return false;
  if ( !checkAck(c, a[0]) ) return false;
  if ( !checkEnd(a[nbBytes-1]) ) return false;
  return true;
}

bool Psp::SerialPort::sendData(uint value, uint nbBytes)
{
  Q_ASSERT( nbBytes==1 || nbBytes==2 );
  Q_ASSERT( value<uint(1 << 8*nbBytes) );
  QMemArray<uchar> a(nbBytes);
  if ( nbBytes==2 ) {
    a[0] = value / 256;
    a[1] = value & 0xFF;
  } else a[0] = value;
  if ( !send(a) ) return false;
  return receive(nbBytes, a);
}
