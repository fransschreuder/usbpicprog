/***************************************************************************
 * Copyright (C) 2005 Lorenz M�senlechner & Matthias Kranz                 *
 *                    <icd2linux@hcilab.org>                               *
 * Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "icd2_usb.h"

#include "microchip.h"
#include "common/common/misc.h"

//------------------------------------------------------------------------------
const Icd2::USBPort::SequenceData Icd2::USBPort::SEQUENCE_DATA[Nb_SequenceTypes] = {
  { 0x00 }, // receive
  { 0x01 }, // send
  { 0x02 }, // connect
  { 0x00 }  // poll
};

Icd2::USBPort::USBPort(uint deviceId, Log::Base &log)
  : Port::USB(log, Microchip::VENDOR_ID, deviceId, 1, 0), _dataSend(false)
{}

bool Icd2::USBPort::doSequence(SequenceType type, char *data, uint size)
{
  QByteArray tx(0x12);
  for (uint i=0; i<uint(tx.count()); i++) tx[i] = 0x00;
  tx[0x00] = SEQUENCE_DATA[type].type;
  tx[0x01] = _seqnum;
  tx[0x02] = size & 0xFF;
  tx[0x03] = (size >> 8) & 0xFF;
  tx[0x0A] = _ctype & 0xFF;
  tx[0x0B] = (_ctype >> 8) & 0xFF;
  if ( !write(0x01, tx.data(), tx.size()) ) return false;

  switch (type) {
    case Connect: break;
    case Receive:
      if ( !read(0x82, data, size, 0) ) return false;
      break;
    case Poll:
      if ( !read(0x82, data, size, &_poll) ) return false;
      break;
    case Send:
      if ( !write(0x01, data, size) ) return false;
      break;
    case Nb_SequenceTypes: Q_ASSERT(false); break;
  }

  QByteArray rx(0x08);
  if ( !read(0x81, rx.data(), rx.size(), 0) ) return false;
  //Q_ASSERT( rx[0]==tx[1] );

  _seqnum++;
  if ( _seqnum==0xFF ) _seqnum = 0xC0;
  return true;
}

bool Icd2::USBPort::connectDevice(const QString &mode)
{
  _seqnum = 0xC1; // preset seqnum

  _ctype = 0x00;
  if ( !doSequence(Connect, 0, 0) ) return false;
  if ( !send("Z", 1) ) return false;
  QString s;
  if ( !receive(4, s) ) return false;

  _ctype = 0x00;
  if ( !doSequence(Connect, 0, 0) ) return false;
  for (uint i=0; true; i++) {
    if ( i==10 ) {
      log(Log::LineType::Error, i18n("Problem connecting ICD2: please try again after unplug-replug."));
      return false;
    }
    _ctype = 0x02;
    QByteArray a = toAscii(mode);
    if ( !doSequence(Send, a.data(), a.count()) ) return false;
    char c;
    _ctype = 0x02;
    if ( !doSequence(Receive, &c, 1) ) return false;
    if ( c==mode.lower()[0] ) break;
  }

  return true;
}

bool Icd2::USBPort::internalReceive(uint size, char *data, uint)
{
  if (_dataSend) {
    //_ctype = qMin(0x65, qRound(4.8 * size)); // timing ?? (1.6 for my ICD2)
    _ctype = 0xC9;
  } else _ctype = 0xC9;
  bool ok = doSequence(Receive, data, size);
  if (_dataSend) _dataSend = false;
  return ok;
}

bool Icd2::USBPort::internalSend(const char *data, uint size, uint)
{
  if (_dataSend) {
    //_ctype = qMin(0x65, qRound(4.8 * size)); // timing ?? (1.6 for my ICD2)
    _ctype = 0xC9;
  } else _ctype = 0xC9;
  bool ok = doSequence(Send, (char *)data, size);
  if (_dataSend) _dataSend = false;
  return ok;
}

bool Icd2::USBPort::poll(uint size, QString &s)
{
  QMemArray<uchar> a;
  if ( !poll(size, a) ) return false;
  s.fill(0, size);
  for (uint i=0; i<size; i++) s[i] = a[i];
  return true;
}

bool Icd2::USBPort::poll(uint size, QMemArray<uchar> &a)
{
  a.resize(size);
  for (;;) {
    _ctype = 0x65;//0x01;
    if ( !doSequence(Poll, (char *)a.data(), size) ) return false;
    if (_poll) break;
  }
  //log(Log::DebugLevel::Max, QString("Receiced: \"%1\"").arg(toPrintable((const char *)a.data(), size)));
  return true;
}

bool Icd2::USBPort::dataSend(const char *data, uint size)
{
  _dataSend = true;
  return Port::USB::send(data, size);
}

bool Icd2::USBPort::dataReceive(uint size, QString &s)
{
  _dataSend = true;
  return Port::USB::receive(size, s);
}

//------------------------------------------------------------------------------
Icd2::USBHardware::USBHardware(::Programmer::Base &base)
  : Hardware(base, new USBPort(ID_CLIENT, base))
{}

bool Icd2::USBHardware::internalConnect(const QString &mode)
{
  // load control messages for USB device if needed
  log(Log::DebugLevel::Extra, QString("need firmware ? %1").arg(USBPort::findDevice(Microchip::VENDOR_ID, ID_FIRMWARE)!=0));
  if ( Port::USB::findDevice(Microchip::VENDOR_ID, ID_FIRMWARE) ) {
    USBPort port(ID_FIRMWARE, *this);
    if ( !port.open() ) return false;
    uint i = 0;
    while ( CONTROL_MESSAGE_DATA[i].bytes!=0 ) {
      if ( !port.sendControlMessage(CONTROL_MESSAGE_DATA[i]) ) return false;
      i++;
    }
    port.close();
    for (uint i=0; i<10; i++) {
      log(Log::DebugLevel::Extra, QString("client here ? %1").arg(USBPort::findDevice(Microchip::VENDOR_ID, ID_CLIENT)!=0));
      if ( Port::USB::findDevice(Microchip::VENDOR_ID, ID_CLIENT) ) break;
      Port::msleep(1000);
    }
  }

  if ( !_port->open() ) return false;
  return static_cast<USBPort *>(_port)->connectDevice(mode);
}
