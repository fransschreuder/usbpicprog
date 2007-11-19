/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PARALLEL_H
#define PARALLEL_H

#include "port_base.h"
#if defined(Q_OS_WIN)
# undef ERROR
#endif

namespace Port
{

class Parallel : public Base
{
public:
  Parallel(const QString &device, Log::Base &base);
  virtual ~Parallel() { close(); }
  virtual Description description() const { return Description(PortType::Parallel, _device); }

  static IODirs probe(const QString &device);
  static const QStringList &probedDeviceList();
  static bool isAvailable();

  enum Pin { DS = 0, D0, D1, D2, D3, D4, D5, D6, D7, ACK, BUSY, PAPER, SELin,
             LF, ERROR, PRIME, SELout, P18, P19, P20, P21, P22, P23, P24, P25,
             Nb_Pins };
  enum RequestType { Control = 0, Status, Data, Nb_RequestTypes };
  struct PPinData {
    RequestType rType;
    uchar       mask;
    IODir       dir;
    const char *label;
  };
  static const PPinData PIN_DATA[Nb_Pins];
  virtual bool setPinOn(uint pin, bool on, LogicType type);
  virtual bool readPin(uint pin, LogicType type, bool &value);
  virtual QValueVector<PinData> pinData(IODir dir) const;
  virtual bool isGroundPin(uint pin) const;
  virtual uint groundPin() const { return P25; }
  virtual IODir ioDir(uint pin) const;

private:
  int     _fd;
  QString _device;
  struct RequestData {
    int read, write;
  };
  static const RequestData REQUEST_DATA[Nb_RequestTypes];
  uchar _images[Nb_RequestTypes];

  static QStringList *_list;
  static QStringList deviceList();

  virtual bool internalOpen();
  virtual void internalClose();
  virtual void setSystemError(const QString &message);
};

} // namespace

#endif
