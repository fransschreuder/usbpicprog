/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "port.h"

#if defined(Q_WS_WIN)
#  include <sys/timeb.h>
#else
#  include <sys/time.h>
#endif

#include "common/global/global.h"
#include "serial.h"
#include "parallel.h"
#include "usb_port.h"

//-----------------------------------------------------------------------------
void getTime(int &sec, int &usec)
{
#if defined (Q_OS_WIN)
  struct _timeb tb;
  _ftime (&tb);
  sec  = tb.time;
  usec = tb.millitm * 1000 + 500;
#else
  struct timeval tv;
  gettimeofday(&tv, 0);
  usec = tv.tv_usec;
  sec = tv.tv_sec;
#endif
}

void Port::msleep(uint ms)
{
  usleep(ms*1000);
}

// from Brian C Lane's code
// works better than usleep
void Port::usleep(uint us)
{
  if ( us==0 ) return;
  int tsec, tusec;
  getTime(tsec, tusec);
  int usec = (tusec + us) % 1000000;
  int sec = tsec + (tusec + us) / 1000000;
  for (;;) {
    getTime(tsec, tusec);
    if ( tsec>sec ) return;
    if ( tsec==sec && tusec>usec ) return;
  }
}

//-----------------------------------------------------------------------------
const Port::Data Port::DATA[Nb_Types] = {
  { I18N_NOOP("Serial Port"),   "serial",   true  },
  { I18N_NOOP("Parallel Port"), "parallel", true  },
  { I18N_NOOP("USB Port"),      "usb",      false }
};

const char * const Port::IO_DIR_NAMES[3] = { "no_io", "in", "out" };

QStringList Port::probedDeviceList(Type type)
{
  if ( !isAvailable(type) ) return QStringList();
  switch (type) {
    case SerialType:   return Serial::probedDeviceList();
    case ParallelType: return Parallel::probedDeviceList();
    case USBType:      return USB::probedDeviceList();
    case Nb_Types:     break;
  }
  return QStringList();
}

bool Port::isAvailable(Type type)
{
  switch (type) {
    case SerialType:   return Serial::isAvailable();
    case ParallelType: return Parallel::isAvailable();
    case USBType:      return USB::isAvailable();
    case Nb_Types:     break;
  }
  return false;
}

Port::Type Port::findType(const QString &portDevice)
{
  for (uint i=0; i<Nb_Types; i++) {
    if ( !DATA[i].withDevice ) continue;
    if ( probedDeviceList(Type(i)).contains(portDevice) ) return Type(i);
  }
  return Nb_Types;
}
