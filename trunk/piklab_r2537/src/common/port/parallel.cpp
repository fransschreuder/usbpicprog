/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "parallel.h"

#include "common/global/global.h"
#if defined(HAVE_PPDEV)
#  include <linux/ppdev.h>
#  include <linux/parport.h>
#  include <fcntl.h>
#  include <sys/ioctl.h>
#  include <unistd.h> // needed on some system
#  include <errno.h>
#elif defined(HAVE_PPBUS)
#  include <sys/param.h>
#  include <machine/cpufunc.h>
#  include <dev/ppbus/ppi.h>
#  include <dev/ppbus/ppbconf.h>
#  include <fcntl.h>
#  include <unistd.h>
#  include <errno.h>
#endif
#include "common/common/misc.h"

//-----------------------------------------------------------------------------
QStringList *Port::Parallel::_list = 0;

Port::IODirs Port::Parallel::probe(const QString &device)
{
#if defined(HAVE_PPDEV)
  int fd = ::open(device.latin1(), O_RDWR);
  if ( fd<0) return NoIO;
  if ( ioctl(fd, PPCLAIM)<0 ) {
    ::close(fd) ;
    return In;
  }
  ioctl(fd, PPRELEASE);
  ::close(fd);
  return (In | Out);
#elif defined(HAVE_PPBUS)
  int fd = ::open(device.latin1(), O_RDWR);
  if ( fd<0 ) return NoIO;
  ::close(fd);
  return In | Out; // #### can we detect read-only ?
#else
  return NoIO;
#endif
}

QStringList Port::Parallel::deviceList()
{
  QStringList list;
#if defined(HAVE_PPDEV)
  // standard parport in user space
  for(int i = 0; i<8; ++i) list.append(QString("/dev/parport%1").arg(i));
  // new devfs parport flavour
  for(int i = 0; i<8; ++i) list.append(QString("/dev/parports/%1").arg(i));
#elif defined(HAVE_PPBUS)
  // FreeBSD
  for(int i = 0; i<8; ++i) list.append(QString("/dev/ppi%1").arg(i));
#endif
  return list;
}

const QStringList &Port::Parallel::probedDeviceList()
{
  if ( _list==0 ) {
    QStringList all = deviceList();
    _list = new QStringList;
    for (uint i=0; i<uint(all.count()); i++)
      if( probe(all[i]) & (In | Out) ) _list->append(all[i]);
  }
  return *_list;
}

bool Port::Parallel::isAvailable()
{
#if defined(HAVE_PPDEV) || defined(HAVE_PPBUS)
  return true;
#else
  return false;
#endif
}

//-----------------------------------------------------------------------------
const Port::Parallel::PPinData Port::Parallel::PIN_DATA[Nb_Pins] = {
  { Control, 0x01, Out, "/DS"    }, // !strobe
  { Data,    0x01, Out, "D0"     }, // data 0
  { Data,    0x02, Out, "D1"     }, // data 1
  { Data,    0x04, Out, "D2"     }, // data 2
  { Data,    0x08, Out, "D3"     }, // data 3
  { Data,    0x10, Out, "D4"     }, // data 4
  { Data,    0x20, Out, "D5"     }, // data 5
  { Data,    0x40, Out, "D6"     }, // data 6
  { Data,    0x80, Out, "D7"     }, // data 7
  { Status,  0x40, In,  "/ACK"   }, // !ack
  { Status,  0x80, In,  "BUSY"   }, // busy
  { Status,  0x20, In,  "PAPER"  }, // pout
  { Status,  0x10, In,  "SELin"  }, // select
  { Control, 0x02, Out, "LF"     }, // !feed
  { Status,  0x08, In,  "/ERROR" }, // !error
  { Control, 0x04, Out, "PRIME"  }, // !init
  { Control, 0x08, Out, "SELout" }, // !si
  { Nb_RequestTypes, 0x00, NoIO, "GND" }, // GND
  { Nb_RequestTypes, 0x00, NoIO, "GND" }, // GND
  { Nb_RequestTypes, 0x00, NoIO, "GND" }, // GND
  { Nb_RequestTypes, 0x00, NoIO, "GND" }, // GND
  { Nb_RequestTypes, 0x00, NoIO, "GND" }, // GND
  { Nb_RequestTypes, 0x00, NoIO, "GND" }, // GND
  { Nb_RequestTypes, 0x00, NoIO, "GND" }, // GND
  { Nb_RequestTypes, 0x00, NoIO, "GND" }  // GND
};

QValueVector<Port::PinData> Port::Parallel::pinData(IODir dir) const
{
  QValueVector<PinData> v;
  for (uint i=0; i<Nb_Pins; i++) {
    if ( PIN_DATA[i].dir!=dir ) continue;
    PinData pd = { i, PIN_DATA[i].label };
    v.append(pd);
  }
  return v;
}

bool Port::Parallel::isGroundPin(uint pin) const
{
  return ( PIN_DATA[pin].label==QString("GND") );
}

Port::IODir Port::Parallel::ioDir(uint pin) const
{
  return PIN_DATA[pin].dir;
}

const Port::Parallel::RequestData Port::Parallel::REQUEST_DATA[Nb_RequestTypes] = {
#if defined(HAVE_PPDEV)
  { PPRCONTROL, PPWCONTROL },
  { PPRSTATUS,  0          },
  { PPRDATA,    PPWDATA    }
#elif defined(HAVE_PPBUS)
  { PPIGCTRL,    PPISCTRL    },
  { PPIGSTATUS,  0           },
  { PPIGDATA,    PPISDATA    }
#else
  { 0, 0 },
  { 0, 0 },
  { 0, 0 }
#endif
};

Port::Parallel::Parallel(const QString &device, Log::Base &base)
  : Base(base), _fd(-1), _device(device)
{
  for (uint i=0; i<Nb_RequestTypes; i++) _images[i] = 0;
}

bool Port::Parallel::internalOpen()
{
#if defined(HAVE_PPDEV)
  _fd = ::open(_device.latin1(), O_RDWR);
  if ( _fd<0 ) {
    setSystemError(i18n("Could not open device \"%1\"").arg(_device));
    return false;
  }
  if ( ioctl(_fd, PPCLAIM)<0 ) {
    setSystemError(i18n("Could not claim device \"%1\": check it is read/write enabled").arg(_device));
    return false;
  }
#elif defined(HAVE_PPBUS)
  _fd = ::open(_device.latin1(), O_RDWR);
  if ( _fd<0 ) {
    setSystemError(i18n("Could not open device \"%1\"").arg(_device));
    return false;
  }
#endif
  return true;
}

void Port::Parallel::internalClose()
{
  if ( _fd<0 ) return;
#if defined(HAVE_PPDEV)
  ioctl(_fd, PPRELEASE);
  ::close(_fd);
#elif defined(HAVE_PPBUS)
  ::close(_fd);
#endif
  _fd = -1;
}

bool Port::Parallel::setPinOn(uint pin, bool on, LogicType type)
{
  if ( _fd<0 ) return false;
#if defined(HAVE_PPDEV) || defined(HAVE_PPBUS)
  Q_ASSERT( pin<Nb_Pins );
  Q_ASSERT( PIN_DATA[pin].dir==Out );
  RequestType rtype = PIN_DATA[pin].rType;
  Q_ASSERT( rtype!=Nb_RequestTypes );
  uchar mask = PIN_DATA[pin].mask;
  uchar c = (XOR(type==NegativeLogic, on) ? _images[rtype] | mask : _images[rtype] &  ~mask);
  int request = REQUEST_DATA[rtype].write;
  Q_ASSERT( request!=0 );
  if ( ioctl(_fd, request, &c)<0 ) {
    setSystemError(i18n("Error setting pin %1 to %2").arg(PIN_DATA[pin].label).arg(on));
    return false;
  }
  _images[rtype] = c;
#endif
  return true;
}

bool Port::Parallel::readPin(uint pin, LogicType type, bool &value)
{
  if ( _fd<0 ) return false;
#if defined(HAVE_PPDEV) || defined(HAVE_PPBUS)
  Q_ASSERT( pin<Nb_Pins );
  Q_ASSERT( PIN_DATA[pin].dir==In );
  RequestType rtype = PIN_DATA[pin].rType;
  Q_ASSERT( rtype!=Nb_RequestTypes );
  int request = REQUEST_DATA[rtype].read;
  Q_ASSERT( request!=0 );
  uchar c = 0;
  if ( ioctl(_fd, request, &c)<0 ) {
    setSystemError(i18n("Error reading bit on pin %1").arg(PIN_DATA[pin].label));
    return false;
  }
  _images[rtype] = c;
  value = (type==NegativeLogic ? ~c : c ) & PIN_DATA[pin].mask;
#endif
  return true;
}

void Port::Parallel::setSystemError(const QString &message)
{
#if defined(HAVE_PPDEV) || defined(HAVE_PPBUS)
  log(Log::LineType::Error, message + QString(" (errno=%1).").arg(strerror(errno)));
#else
  Q_UNUSED(message);
#endif
}
