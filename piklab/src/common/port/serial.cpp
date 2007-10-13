/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "serial.h"

#ifdef Q_OS_UNIX
#  include <stdio.h>
#  include <fcntl.h>
#  include <sys/time.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/ioctl.h>
#  include <errno.h>
#  include <unistd.h> // needed on some system
#endif
#include <qdatetime.h>

//-----------------------------------------------------------------------------
QStringList *Port::Serial::_list = 0;
#if defined(Q_OS_UNIX)
const Port::Serial::Handle Port::Serial::INVALID_HANDLE = -1;
#elif defined(Q_OS_WIN)
const Port::Serial::Handle Port::Serial::INVALID_HANDLE = INVALID_HANDLE_VALUE;
#endif

Port::Serial::Handle Port::Serial::openHandle(const QString &device, IODirs dirs)
{
#if defined(Q_OS_UNIX)
  // open non blocking: avoid missing DCD (comment from xwisp2)
  int mode = O_NOCTTY | O_NONBLOCK;
  if ( dirs & In ) {
    if ( dirs & Out ) mode |= O_RDWR;
    else mode |= O_RDONLY;
  } else mode |= O_WRONLY;
  return ::open(device.latin1(), mode);
#elif defined(Q_OS_WIN)
  int mode = 0;
  if ( dirs & In ) mode |= GENERIC_READ;
  if ( dirs & Out ) mode |= GENERIC_WRITE;
  return CreateFileA(device.latin1(), mode, 0, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
#endif
}

void Port::Serial::closeHandle(Handle handle)
{
#if defined(Q_OS_UNIX)
  ::close(handle);
#elif defined(Q_OS_WIN)
  CloseHandle(handle);
#endif
}

Port::IODirs Port::Serial::probe(const QString &device)
{
  Handle handle = openHandle(device, In);
  if ( handle==INVALID_HANDLE ) return NoIO;
  closeHandle(handle);
  handle = openHandle(device, In | Out);
  if ( handle==INVALID_HANDLE ) return In;
  closeHandle(handle);
  return (In | Out);
}

QStringList Port::Serial::deviceList()
{
  QStringList list;
#if defined(Q_OS_UNIX)
  // standard serport in user space
  for (uint i=0; i<8; i++) list.append(QString("/dev/ttyS%1").arg(i));
  // new devfs serport flavour
  for (uint i=0; i<8; i++) list.append(QString("/dev/tts/%1").arg(i));
  // standard USB serport in user space
  for (uint i=0; i<8; i++) list.append(QString("/dev/ttyUSB%1").arg(i));
  // new devfs USB serport flavour
  for (uint i=0; i<8; i++) list.append(QString("/dev/usb/tts/%1").arg(i));
  // FreeBSD
  for (uint i=0; i<8; i++) list.append(QString("/dev/ttyd%1").arg(i));
  // Slackware 11 devfs USB Serial port support.
  for (uint i=0; i<8; i++) list.append(QString("/dev/tts/USB%1").arg(i));
  // MacOSX devfs USB Serial port support.
  list.append("/dev/tty.usbserial");
#elif defined(Q_OS_WIN)
  for (uint i=1; i<10; i++) list.append(QString("COM%1").arg(i));
#endif
  return list;
}

const QStringList &Port::Serial::probedDeviceList()
{
  if ( _list==0 ) {
    QStringList all = deviceList();
    _list = new QStringList;
    for (uint i=0; i<uint(all.count()); i++)
      if( probe(all[i]) & (In | Out) ) _list->append(all[i]);
  }
  return *_list;
}

//-----------------------------------------------------------------------------
const uint Port::Serial::SPEED_VALUES[Nb_Speeds] = {
  0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400,
  57600, 115200
};
const Port::Serial::SpeedData Port::Serial::SPEED_DATA[Nb_Speeds] = {
#if defined(Q_OS_UNIX)
  { true, B0     }, { true, B50     }, { true, B75    }, { true, B110   },
  { true, B134   }, { true, B150    }, { true, B200   }, { true, B300   },
  { true, B600   }, { true, B1200   }, { true, B1800  }, { true, B2400  },
  { true, B4800  }, { true, B9600   }, { true, B19200 }, { true, B38400 },
  { true, B57600 }, { true, B115200 }
#elif defined(Q_OS_WIN)
  { false, 0        }, { false, 0         }, { false, 0        }, { true, CBR_110   },
  { false, 0        }, { false, 0         }, { false, 0        }, { true, CBR_300   },
  { true,  CBR_600  }, { true, CBR_1200   }, { false, 0        }, { true, CBR_2400  },
  { true, CBR_4800  }, { true, CBR_9600   }, { true, CBR_19200 }, { true, CBR_38400 },
  { true, CBR_57600 }, { true, CBR_115200 }
#endif
};

const Port::Serial::SPinData Port::Serial::PIN_DATA[Nb_Pins] = {
  { In,  "DCD" }, { In,  "RX" }, { Out, "TX"  }, { Out, "DTR" },
  { NoIO,   "GND" }, { In, "DSR" }, { Out, "RTS" }, { In,  "CTS" },
  { Out,  "RI" }
};

QValueVector<Port::PinData> Port::Serial::pinData(IODir dir) const
{
  QValueVector<PinData> v;
  for (uint i=0; i<Nb_Pins; i++) {
    if ( PIN_DATA[i].dir!=dir ) continue;
    PinData pd = { i, PIN_DATA[i].label };
    v.append(pd);
  }
  return v;
}
bool Port::Serial::isGroundPin(uint pin) const
{
  return ( PIN_DATA[pin].label=="GND" );
}

Port::IODir Port::Serial::ioDir(uint pin) const
{
  return PIN_DATA[pin].dir;
}

Port::Serial::Serial(const QString &device, Properties properties, Log::Base &base)
  : Base(base), _device(device), _properties(properties), _fd(INVALID_HANDLE)
{}

bool Port::Serial::getParameters(Parameters &parameters)
{
  if ( _fd==INVALID_HANDLE ) return false;
#if defined(Q_OS_UNIX)
  if ( tcgetattr(_fd, &parameters)<0 ) {
#elif defined(Q_OS_WIN)
  if ( !GetCommState(_fd, &parameters.dcb) || !GetCommTimeouts(_fd, &parameters.comtmo) ) {
#endif
    setSystemError(i18n("Could not get file descriptor parameters"));
    return false;
  }
  return true;
}

bool Port::Serial::setParameters(const Parameters &parameters)
{
  if ( _fd==INVALID_HANDLE ) return false;
#if defined(Q_OS_UNIX)
  if ( tcsetattr(_fd, TCSANOW, &parameters)<0 ) {
#elif defined(Q_OS_WIN)
  Parameters tmp = parameters;
  if ( !SetCommState(_fd, &tmp.dcb) || !SetCommTimeouts(_fd, &tmp.comtmo) ) {
#endif
    setSystemError(i18n("Could not set file descriptor parameters"));
    return false;
  }
  return true;
}

bool Port::Serial::internalOpen()
{
  _fd = openHandle(_device, In | Out);
  if ( _fd==INVALID_HANDLE ) {
    setSystemError(i18n("Could not open device \"%1\" read-write").arg(_device));
    return false;
  }
  if ( !getParameters(_oldParameters) ) return false; // save configuration
#if defined(Q_OS_UNIX)
  if ( _properties & Blocking ) {
    int flags = fcntl(_fd, F_GETFL, 0);
    if ( fcntl(_fd, F_SETFL, flags & ~O_NONBLOCK)<0 ) {
      setSystemError(i18n("Could not modify file descriptor flags"));
      return false;
    }
  }
#endif
  return flush(DEFAULT_TIMEOUT);
}

void Port::Serial::internalClose()
{
  if ( _fd==INVALID_HANDLE ) return;
  if ( _properties & NeedFlush ) flush(0);
  if ( _properties & NeedBreak ) doBreak(1);
  setParameters(_oldParameters);
  closeHandle(_fd);
  _fd = INVALID_HANDLE;
}

bool Port::Serial::internalSend(const char *data, uint size, uint timeout)
{
  if ( _fd==INVALID_HANDLE ) return false;
  QTime time;
  time.start();
  for (uint todo=size; todo!=0; ) {
#if defined(Q_OS_UNIX)
    int res = write(_fd, data+size-todo, todo);
    if ( res<0 && errno!=EAGAIN ) {
#elif defined(Q_OS_WIN)
    DWORD res = 0;
    if ( WriteFile(_fd, data+size-todo, todo, &res, NULL)==0 ) {
#endif
      setSystemError(i18n("Error sending data"));
      return false;
    }
    if ( res>0 ) todo -= res;
    else {
      if ( uint(time.elapsed())>timeout ) {
        log(Log::Error, i18n("Timeout sending data (%1/%2 bytes sent).").arg(size-todo).arg(size));
        return false;
      }
      msleep(1);
    }
  }
  if ( (_properties & NeedDrain) && !drain(timeout) ) return false;
  return true;
}

bool Port::Serial::internalReceive(uint size, char *data, uint timeout)
{
  if ( _fd==INVALID_HANDLE ) return false;
  QTime time;
  time.start();
  for(uint todo=size; todo!=0; ) {
#if defined(Q_OS_UNIX)
    // this help reduce CPU usage. It also prevents blocking if the serial cable is disconnected
    fd_set rfd;
    FD_ZERO(&rfd);
    FD_SET(_fd, &rfd);
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout%1000)*1000;
    int res = select(_fd+1, &rfd, 0, 0, &tv);
    if ( res<0 ) {
      setSystemError(i18n("Error receiving data"));
      return false;
    }
    if ( res==0 ) {
      log(Log::Error, i18n("Timeout waiting for data."));
      return false;
    }
    res = read(_fd, data+size-todo, todo);
    if ( res<0 && errno!=EAGAIN ) {
#elif defined(Q_OS_WIN)
    DWORD res = 0;
    if ( ReadFile(_fd, data+size-todo, todo, &res, NULL)==0 ) {
#endif
      setSystemError(i18n("Error receiving data"));
      return false;
    }
    if ( res>0 ) todo -= res;
    else {
      if ( uint(time.elapsed())>timeout ) {
        log(Log::Error, i18n("Timeout receiving data (%1/%2 bytes received).").arg(size-todo).arg(size));
        return false;
      }
      msleep(1);
    }
  }
  return true;
}

bool Port::Serial::drain(uint timeout)
{
  if ( _fd==INVALID_HANDLE ) return false;
#if defined(Q_OS_UNIX)
  // tcdrain will block if the serial cable is disconnected
  // so we first check for data in output buffer...
  QTime time;
  time.start();
  for (;;) {
    int nb;
    if ( ioctl(_fd, TIOCOUTQ, &nb)==-1 ) {
      setSystemError(i18n("Error checking for data in output buffer"));
      return false;
    }
    if ( nb==0 ) break;
    if ( uint(time.elapsed())>timeout ) {
      _fd = INVALID_HANDLE; // otherwise close blocks...
      log(Log::Error, i18n("Timeout sending data (%1 bytes left).").arg(nb));
      return false;
    }
  }
  if ( tcdrain(_fd)<0 ) {
    setSystemError(i18n("Error while draining"));
    return false;
  }
#endif
  return true;
}

bool Port::Serial::flush(uint timeout)
{
  if ( _fd==INVALID_HANDLE ) return false;
  if ( (_properties & NeedDrain) && !drain(timeout) ) return false;
#if defined(Q_OS_UNIX)
  if ( tcflush(_fd, TCIFLUSH)<0 ) {
#elif defined(Q_OS_WIN)
  if ( FlushFileBuffers(_fd)==0 || PurgeComm(_fd, PURGE_TXABORT)==0
       || PurgeComm(_fd, PURGE_RXABORT)==0 || PurgeComm(_fd, PURGE_TXCLEAR)==0
       || PurgeComm(_fd, PURGE_RXCLEAR)==0 ) {
#endif
    setSystemError(i18n("Could not flush device"));
    return false;
  }
  return true;
}

bool Port::Serial::internalSetPinOn(Pin pin, bool on)
{
#if defined(Q_OS_UNIX)
  int bit = 0;
  switch (pin) {
    case TX:  return ( ioctl(_fd, on ? TIOCSBRK : TIOCCBRK, 0)>=0 );
    case DTR: bit = TIOCM_DTR; break;
    case RTS: bit = TIOCM_RTS; break;
    case RI:  bit = TIOCM_RI; break;
    default:  Q_ASSERT(false); return false;
  }
  return ( ioctl(_fd, on ? TIOCMBIS : TIOCMBIC, &bit)>=0 );
#elif defined(Q_OS_WIN)
  DWORD func = 0;
  switch (pin) {
    case TX:  func = (on ? SETBREAK : CLRBREAK); break;
    case DTR: func = (on ? SETDTR : CLRDTR); break;
    case RTS: func = (on ? SETRTS : CLRRTS); break;
    case RI:  // #### not possible with Win32 API ??
    default:  Q_ASSERT(false); return false;
  }
  return ( EscapeCommFunction(_fd, func)!=0 );
#endif
}

bool Port::Serial::setPinOn(uint pin, bool on, LogicType type)
{
  if ( _fd==INVALID_HANDLE ) return false;
  if ( type==NegativeLogic ) on = !on;
  Q_ASSERT( pin<Nb_Pins );
  Q_ASSERT( PIN_DATA[pin].dir==Out );
  if ( !internalSetPinOn(Pin(pin), on) ) {
    setSystemError(i18n("Error setting bit %1 of serial port to %2").arg(PIN_DATA[pin].label).arg(on));
    return false;
  }
  return true;
}

bool Port::Serial::internalReadPin(Pin pin, LogicType type, bool &value)
{
#if defined(Q_OS_UNIX)
  int bits;
  if ( ioctl(_fd, TIOCMGET, &bits)<0 ) return false;
  int mask = 0;
  switch (pin) {
    case DCD: mask = TIOCM_CD;  break;
    case RX : mask = TIOCM_SR;  break;
    case DSR: mask = TIOCM_DSR; break;
    case CTS: mask = TIOCM_CTS; break;
    default:  Q_ASSERT(false); return false;
  }
  value = ((type==NegativeLogic ? ~bits : bits) & mask);
  return true;
#elif defined(Q_OS_WIN)
  DWORD status;
  if ( GetCommModemStatus(_fd, &status)==0 ) return false;
  switch (pin) {
    case DCD: value = (status & MS_RLSD_ON); break;
    case DSR: value = (status & MS_DSR_ON); break;
    case CTS: value = (status & MS_CTS_ON); break;
    case RX:  // not possible with Win32 API ??
    default:  Q_ASSERT(false); return false;
  }
  if ( type==NegativeLogic) value = !value;
  return true;
#endif
}

  bool Port::Serial::readPin(uint pin, LogicType type, bool &value)
{
  if ( _fd==INVALID_HANDLE ) return false;
  Q_ASSERT( pin<Nb_Pins );
  Q_ASSERT( PIN_DATA[pin].dir==In );
  if ( !internalReadPin(Pin(pin), type, value) ) {
    setSystemError(i18n("Error reading serial pin %1").arg(PIN_DATA[pin].label));
    return false;
  }
  return true;
}

bool Port::Serial::setMode(InputFlags inputFlags, ControlFlags controlFlags, Speed speed, uint readTimeout)
{
  Q_ASSERT (SPEED_DATA[speed].supported );
  if ( !flush(0) ) return false;
  Parameters parameters;
  if ( !getParameters(parameters) ) return false;
#if defined(Q_OS_UNIX)
  cfsetispeed(&parameters, SPEED_DATA[speed].flag);
  cfsetospeed(&parameters, SPEED_DATA[speed].flag);
  parameters.c_cflag &= ~(CSIZE|PARENB|PARODD|HUPCL|CSTOPB);
  int cflags = 0;
  if ( controlFlags & ByteSize8 ) cflags |= CS8;
  if ( controlFlags & HardwareFlowControl ) cflags |= CRTSCTS;
  if ( controlFlags & EnableReceiver ) cflags |= CREAD;
  if ( controlFlags & IgnoreControlLines ) cflags |= CLOCAL;
  parameters.c_cflag |= cflags;
  parameters.c_iflag &= ~(ISTRIP|INPCK|IGNCR|INLCR|ICRNL|IXOFF|IXON);
  int iflags = 0;
  if ( inputFlags & IgnoreBreak ) iflags |= IGNBRK;
  if ( inputFlags & IgnoreParity ) iflags |= IGNPAR;
  parameters.c_iflag |= iflags;
  parameters.c_oflag &= ~OPOST;
  parameters.c_lflag &= ~(ICANON|ECHO|ECHONL|ISIG|IEXTEN|TOSTOP);
  parameters.c_cc[VMIN]  = 0;               // wait for 1 char or timeout
  parameters.c_cc[VTIME] = readTimeout/100; // wait in deciseconds
#elif defined(Q_OS_WIN)
  if ( controlFlags & EnableReceiver ) ; // #### not sure what to do
  if ( controlFlags & IgnoreControlLines ) ; // #### not sure what to do
  setHardwareFlowControl(parameters, (controlFlags & HardwareFlowControl));
  if ( inputFlags & IgnoreParity ) parameters.dcb.Parity = NOPARITY;
  parameters.dcb.StopBits = ONESTOPBIT;
  if ( controlFlags & ByteSize8 ) parameters.dcb.ByteSize = 8;
  parameters.dcb.BaudRate = SPEED_DATA[speed].flag;
  parameters.comtmo.ReadIntervalTimeout = MAXDWORD;
  parameters.comtmo.ReadTotalTimeoutMultiplier = MAXDWORD;
  parameters.comtmo.ReadTotalTimeoutConstant = readTimeout;
  parameters.comtmo.WriteTotalTimeoutMultiplier = 1;
  parameters.comtmo.WriteTotalTimeoutConstant = readTimeout;
#endif
  return setParameters(parameters);
  // need flush ??
}

bool Port::Serial::doBreak(uint duration)
{
  if ( !flush(0) ) return false;
#if defined(Q_OS_UNIX)
  if ( ioctl(_fd, TIOCSBRK)<0 ) {
#elif defined(Q_OS_WIN)
  if ( SetCommBreak(_fd)==0 ) {
#endif
    setSystemError(i18n("Error setting up break"));
    return false;
  }
  msleep(duration);
#if defined(Q_OS_UNIX)
  if ( ioctl(_fd, TIOCCBRK) ) {
#elif defined(Q_OS_WIN)
  if ( ClearCommBreak(_fd)==0 ) {
#endif
    setSystemError(i18n("Error clearing up break"));
    return false;
  }
  msleep(1);
  return flush(0);
}

void Port::Serial::setHardwareFlowControl(Parameters &parameters, bool on)
{
#if defined(Q_OS_UNIX)
  if (on) parameters.c_cflag |= CRTSCTS;
  else parameters.c_cflag &= ~CRTSCTS;
  parameters.c_cc[VMIN]  = 0; // #### needed ?
  parameters.c_cc[VTIME] = 0; // #### needed ?
#elif defined(Q_OS_WIN)
  // #### not sure about that
  parameters.dcb.fParity = on;
  parameters.dcb.fOutxCtsFlow = on;
  parameters.dcb.fOutxDsrFlow = on;
  parameters.dcb.fDtrControl = (on ? DTR_CONTROL_ENABLE : DTR_CONTROL_DISABLE);
  parameters.dcb.fDsrSensitivity = on;
  parameters.dcb.fTXContinueOnXoff = on;
  parameters.dcb.fOutX = on;
  parameters.dcb.fInX = on;
  parameters.dcb.fNull = on;
  parameters.dcb.fRtsControl = (on ? RTS_CONTROL_ENABLE : RTS_CONTROL_DISABLE);
  parameters.dcb.fAbortOnError = on;
#endif
}

bool Port::Serial::setHardwareFlowControl(bool on)
{
  Parameters parameters;
  if ( !getParameters(parameters) ) return false;
  setHardwareFlowControl(parameters, on);
  return setParameters(parameters);
}

void Port::Serial::setSystemError(const QString &message)
{
#if defined(Q_OS_UNIX)
  log(Log::Error, message + QString(" (errno=%1)").arg(strerror(errno)));
#elif defined(Q_OS_WIN)
  LPVOID lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
  log(Log::Error, message + QString(" (last error=%1 %2)").arg(GetLastError()).arg((const char *)(LPCTSTR)lpMsgBuf));
  LocalFree(lpMsgBuf);
#endif
}
