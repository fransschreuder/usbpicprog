/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "icd2_serial.h"

#include "common/global/global.h"
#include "common/common/misc.h"

//-----------------------------------------------------------------------------
Icd2::SerialPort::SerialPort(const QString &device, Log::Base &log)
  : Port::Serial(device, NeedDrain | NeedFlush, log)
{}

bool Icd2::SerialPort::open(Speed speed)
{
  if ( !Port::Serial::open() ) return false;
  return setMode(NoInputFlag, ByteSize8 | EnableReceiver | HardwareFlowControl, speed, 0);
}

//-----------------------------------------------------------------------------
Icd2::SerialHardware::SerialHardware(::Programmer::Base &base, const QString &portDevice)
  : Hardware(base, new SerialPort(portDevice, base))
{}

bool Icd2::SerialHardware::internalConnect(const QString &mode)
{
  if ( !static_cast<SerialPort *>(_port)->open(Port::Serial::S19200) ) return false;
  if ( !reset() ) return false;
  if ( !_port->send("Z", 1) ) return false;
  QString s;
  if ( !_port->receive(4, s) ) return false;
  if ( !reset() ) return false;
  QByteArray a = toAscii(mode);
  if ( !_port->send(a.data(), a.count()) ) return false;
  if ( !_port->receive(1, s) ) return false;
  if ( s.upper()!=mode ) {
    log(Log::Error, i18n("Failed to set port mode to '%1'.").arg(mode));
    return false;
  }
  //log(Log::Debug, "set fast speed");
  //if ( !setFastSpeed() ) return false;
  return true;
}

bool Icd2::SerialHardware::reset()
{
  static_cast<Port::Serial *>(_port)->setPinOn(Port::Serial::DTR, false, Port::PositiveLogic); // Trigger DTR to reset icd2
  Port::msleep(10);
  static_cast<Port::Serial *>(_port)->setPinOn(Port::Serial::DTR, true, Port::PositiveLogic); // remove reset
  Port::msleep(10);
  return true;
}

bool Icd2::SerialHardware::setFastSpeed()
{
  if ( !command("4D", 0) ) return false; // go faster
  static_cast<SerialPort *>(_port)->open(Port::Serial::S57600);
  Port::msleep(100); // ...we do need to delay here
  return !hasError();
}
