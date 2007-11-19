/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "prog_specific.h"

#include "generic_prog.h"

//-----------------------------------------------------------------------------
Programmer::DeviceSpecific::DeviceSpecific(Programmer::Base &base)
 : Log::Base(&base), _base(base)
{}

//-----------------------------------------------------------------------------
Programmer::Hardware::Hardware(Programmer::Base &base, Port::Base *port, const QString &name)
  : Log::Base(&base), _port(port), _name(name), _base(base)
{}

Programmer::Hardware::~Hardware()
{
  delete _port;
}

bool Programmer::Hardware::connectHardware()
{
  if (_port) _port->close();
  return internalConnectHardware();
}

void Programmer::Hardware::disconnectHardware()
{
  if (_port) _port->close();
  internalDisconnectHardware();
}

bool Programmer::Hardware::rawWrite(const QString &data)
{
  Q_ASSERT(_port);
  QByteArray a = toAscii(data);
  return _port->send(a.data(), a.count());
}

bool Programmer::Hardware::rawRead(uint size, QString &data)
{
  Q_ASSERT(_port);
  return _port->receive(size, data);
}
