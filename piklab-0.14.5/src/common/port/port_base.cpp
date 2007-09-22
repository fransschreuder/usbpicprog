/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "port_base.h"

#include "common/global/global.h"
#include "common/common/misc.h"

bool Port::Base::open()
{
  close();
  resetError();
  _closing = false;
  return internalOpen();
}

void Port::Base::close()
{
  _closing = true;
  internalClose();
}

bool Port::Base::send(const char *data, uint size, uint timeout)
{
  log(Log::LowLevelDebug, QString("Sending: \"%1\"").arg(toPrintable(data, size, PrintAlphaNum)));
  return internalSend(data, size, timeout);
}

bool Port::Base::receive(uint size, QString &s, uint timeout)
{
  QMemArray<uchar> a;
  if ( !receive(size, a, timeout) ) return false;
  s.fill(0, size);
  for (uint i=0; i<size; i++) s[i] = a[i];
  return true;
}

bool Port::Base::receive(uint size, QMemArray<uchar> &a, uint timeout)
{
  a.resize(size);
  bool ok = internalReceive(size, (char *)a.data(), timeout);
  if (ok) log(Log::LowLevelDebug, QString("Received: \"%1\"").arg(toPrintable(a, PrintAlphaNum)));
  return ok;
}

bool Port::Base::receiveChar(char &c, uint timeout)
{
  if ( !internalReceive(1, &c, timeout) ) return false;
  log(Log::LowLevelDebug, QString("Received: \"%1\"").arg(toPrintable(c, PrintAlphaNum)));
  return true;
}

bool Port::Base::setPinOn(uint, bool, LogicType)
{
  qFatal("setPinOn not implemented");
  return false;
}
bool Port::Base::readPin(uint, LogicType, bool &)
{
  qFatal("readPin not implemented");
  return 0;
}
QValueVector<Port::PinData> Port::Base::pinData(IODir) const
{
  qFatal("pinData not implemented");
  return QValueVector<PinData>();
}
bool Port::Base::isGroundPin(uint) const
{
  qFatal("isGroundPin not implemented");
  return false;
}
uint Port::Base::groundPin() const
{
  qFatal("groundPin not implemented");
  return 0;
}
Port::IODir Port::Base::ioDir(uint) const
{
  qFatal("ioType not implemented");
  return NoIO;
}

void Port::Base::log(Log::Type logType, const QString &message)
{
  if ( logType==Log::Error && _closing ) return;
  Log::Base::log(logType, i18n(DATA[description().type].label) + ": " + message);
  if ( logType==Log::Error ) close();
}

void Port::Base::log(Log::DebugLevel level, const QString &message)
{
  Log::Base::log(level, i18n(DATA[description().type].label) + ": " + message);
}

void Port::Base::logData(const QString &)
{
/*
  QString vs;
  for (uint i=0; i<s.length(); i++) {
    char c = s[i];
    switch (c) {
      case '\r': vs += "\\r"; break;
      case '\n': vs += "\\n"; break;
      case '<': vs += "&lt;"; break;
      case '>': vs += "&gt;"; break;
      default: {
        if ( c>=32 && c<=126 ) vs += c;
        else {
          QString tmp;
          tmp.sprintf("\\x%02x", c);
          vs += tmp;
        }
        break;
      }
    }
  }
  qDebug("%s", vs.latin1());
*/
  //log(Log::Debug, vs);
}
