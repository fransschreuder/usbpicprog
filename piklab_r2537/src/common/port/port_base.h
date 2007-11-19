/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PORT_BASE_H
#define PORT_BASE_H

#include "port.h"
#include "common/global/log.h"

namespace Port
{

class Base : public Log::Base
{
public:
  Base(Log::Base &base) : Log::Base(&base) {}
  virtual PortType type() const { return description().type; }
  virtual Description description() const = 0;
  bool open();
  void close();

  virtual void log(Log::LineType kind, const QString &message);
  virtual void log(Log::DebugLevel level, const QString &message);
  void logData(const QString &s);

  enum { DEFAULT_TIMEOUT = 3000 }; // 3s
  bool sendChar(char c, uint timeout = DEFAULT_TIMEOUT) { return send(&c, 1, timeout); }
  bool send(const char *data, uint size, uint timeout = DEFAULT_TIMEOUT);
  bool send(const QMemArray<uchar> &a,  uint timeout = DEFAULT_TIMEOUT) { return send((const char *)a.data(), a.count(), timeout); }
  bool receiveChar(char &c, uint timeout = DEFAULT_TIMEOUT);
  bool receive(uint size, QString &s, uint timeout = DEFAULT_TIMEOUT);
  bool receive(uint size, QMemArray<uchar> &a, uint timeout = DEFAULT_TIMEOUT);

  virtual bool setPinOn(uint pin, bool on, LogicType type);
  virtual bool readPin(uint pin, LogicType type, bool &value);
  virtual QValueVector<PinData> pinData(IODir dir) const;
  virtual bool isGroundPin(uint pin) const;
  virtual uint groundPin() const;
  virtual IODir ioDir(uint pin) const;

protected:
  virtual bool internalOpen() = 0;
  virtual void internalClose() = 0;
  virtual bool internalSend(const char *, uint, uint) { qFatal("Not implemented"); return false; }
  virtual bool internalReceive(uint, char *, uint) { qFatal("Not implemented"); return false; }
  virtual void setSystemError(const QString &message) = 0;

private:
  bool _closing;
};

} // namespace

#endif
