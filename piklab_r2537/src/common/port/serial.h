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
#ifndef SERIAL_H
#define SERIAL_H

#include "common/global/global.h"
#ifdef Q_OS_UNIX
#  include <termios.h>
#elif defined(Q_OS_WIN)
#  include <io.h>
#endif
#include "port_base.h"

namespace Port
{

class Serial : public Base
{
public:
  enum Speed { S0 = 0, S50, S75, S110, S134, S150, S200, S300, S600, S1200,
               S1800, S2400, S4800, S9600, S19200, S38400, S57600, S115200,
               Nb_Speeds };
  static const uint SPEED_VALUES[Nb_Speeds];
  struct SpeedData {
    bool supported;
    uint flag;
  };
  static const SpeedData SPEED_DATA[Nb_Speeds];

  enum Property { NoProperty = 0, NeedDrain = 1, NeedFlush = 2, NeedBreak = 4,
                  Blocking = 8 };
  Q_DECLARE_FLAGS(Properties, Property)

  Serial(const QString &device, Properties properties, Log::Base &base);
  virtual ~Serial() { close(); }
  virtual Description description() const { return Description(PortType::Serial, _device); }

  static const QStringList &probedDeviceList();
  static IODirs probe(const QString &device);
  static bool isAvailable() { return true; }

  enum InputFlag { NoInputFlag = 0, IgnoreBreak = 1, IgnoreParity = 2 };
  Q_DECLARE_FLAGS(InputFlags, InputFlag)
  enum ControlFlag { NoControlFlag = 0, ByteSize8 = 1, HardwareFlowControl = 2,
                     EnableReceiver = 4, IgnoreControlLines = 8 };
  Q_DECLARE_FLAGS(ControlFlags, ControlFlag)
  bool setMode(InputFlags inputFlags, ControlFlags controlFlags, Speed speed, uint readTimeout); // in ms
  bool drain(uint timeout);
  bool flush(uint timeout);
  bool doBreak(uint duration); // in ms
  bool setHardwareFlowControl(bool on);

  enum Pin { DCD = 0, RX, TX, DTR, SG, DSR, RTS, CTS, RI, Nb_Pins };
  struct SPinData {
    IODir dir;
    const char *label;
  };
  static const SPinData PIN_DATA[Nb_Pins];
  virtual bool setPinOn(uint pin, bool on, LogicType type);
  virtual bool readPin(uint pin, LogicType type, bool &value);
  virtual QValueVector<PinData> pinData(IODir dir) const;
  virtual bool isGroundPin(uint pin) const;
  virtual uint groundPin() const { return SG; }
  virtual IODir ioDir(uint pin) const;

private:
  QString    _device;
  Properties _properties;
#if defined(Q_OS_UNIX)
  typedef int Handle;
  typedef termios Parameters;
#elif defined(Q_OS_WIN)
  typedef HANDLE Handle;
  struct Parameters {
    DCB             dcb;
    COMMTIMEOUTS    comtmo;
  };
#endif
  Handle  _fd;
  Parameters _oldParameters;

  bool setParameters(const Parameters &parameters);
  bool getParameters(Parameters &parameters);
  virtual bool internalOpen();
  virtual void internalClose();
  virtual bool internalSend(const char *data, uint size, uint timeout);
  virtual bool internalReceive(uint size, char *data, uint timeout);
  virtual void setSystemError(const QString &message);
  bool internalSetPinOn(Pin pin, bool on);
  bool internalReadPin(Pin pin, LogicType type, bool &value);

  static const Handle INVALID_HANDLE;
  static Handle openHandle(const QString &device, IODirs dirs);
  static void closeHandle(Handle handle);
  static QStringList *_list;
  static QStringList deviceList();
  static void setHardwareFlowControl(Parameters &parameters, bool on);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Serial::Properties)
Q_DECLARE_OPERATORS_FOR_FLAGS(Serial::InputFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(Serial::ControlFlags)

} // namespace

#endif
