/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROG_SPECIFIC_H
#define PROG_SPECIFIC_H

#include <qfile.h>

#include "common/global/log.h"
#include "common/port/port_base.h"

namespace Programmer
{
class Base;

//-----------------------------------------------------------------------------
class DeviceSpecific : public Log::Base
{
public:
  DeviceSpecific(::Programmer::Base &base);
  virtual bool setTargetPowerOn(bool on) = 0;

protected:
  ::Programmer::Base &_base;

  virtual bool setPowerOff() = 0;
  virtual bool setPowerOn() = 0;
};

//-----------------------------------------------------------------------------
class Hardware : public Log::Base
{
public:
  Hardware(::Programmer::Base &base, Port::Base *port);
  virtual ~Hardware();
  Port::Description portDescription() const { return _port->description(); }
  bool connectHardware();
  bool rawWrite(const QString &data);
  bool rawRead(uint size, QString &data);
  void disconnectHardware();

protected:
  Port::Base       *_port;
  ::Programmer::Base &_base;

  virtual bool internalConnectHardware() = 0;
  virtual void internalDisconnectHardware() {}
};

} // namespace

#endif
