/***************************************************************************
 * Copyright (C) 2005 Lorenz Mösenlechner & Matthias Kranz                 *
 *                    <icd2linux@hcilab.org>                               *
 * Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef UPP_H
#define UPP_H

#include "common/global/global.h"
#ifdef Q_OS_UNIX
#  include <termios.h>
#elif defined(Q_OS_WIN)
#  include <io.h>
#endif
//#include "port_base.h"

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
#include "common/port/port_base.h"

struct usb_dev_handle;
struct usb_device;
struct usb_bus;

namespace Port
{

class UPP : public Base
{
public:


 /* enum Property { NoProperty = 0, NeedDrain = 1, NeedFlush = 2, NeedBreak = 4,
                  Blocking = 8 };
  Q_DECLARE_FLAGS(Properties, Property)*/
  UPP(const QString &device, Log::Base &base);

  virtual Description description() const { return Description(USBType, QString::null); }
  static struct usb_device *findDevice(uint vendorId, uint productId);
  virtual ~UPP() { close(); }
  static const QStringList &probedDeviceList();
  static IODirs probe(const QString &device);
  static bool isAvailable() { return true; }

  //enum InputFlag { NoInputFlag = 0, IgnoreBreak = 1, IgnoreParity = 2 };
  //Q_DECLARE_FLAGS(InputFlags, InputFlag)
  //enum ControlFlag { NoControlFlag = 0, ByteSize8 = 1, HardwareFlowControl = 2,
  //                   EnableReceiver = 4, IgnoreControlLines = 8 };
  //Q_DECLARE_FLAGS(ControlFlags, ControlFlag)
  //bool setMode(InputFlags inputFlags, ControlFlags controlFlags, Speed speed, uint readTimeout); // in ms
  //bool drain(uint timeout);
  //bool flush(uint timeout);
  bool doBreak(uint duration); // in ms
  //bool setHardwareFlowControl(bool on);

  enum Pin { VPP=0, VPP_Reset, DATA_OUT, DATA_IN, CLOCK, GND, VDD, DIR, Nb_Pins };
  struct UPinData {
    IODir dir;
    const char *label;
  };
  static const UPinData PIN_DATA[Nb_Pins];
  virtual bool setPinOn(uint pin, bool on, LogicType type);
  virtual bool readPin(uint pin, LogicType type, bool &value);
  virtual QValueVector<PinData> pinData(IODir dir) const;
  virtual bool isGroundPin(uint pin) const;
  virtual uint groundPin() const { return GND; }
  virtual IODir ioDir(uint pin) const;



private:
	/* PICkit USB values */ /* From Microchip firmware */
	const static uint vendorId=0x04d8; // Microchip, Inc
	const static uint productId=0x000E; // Usbpicprog Device Descriptor
	const static int configuration=1; /*  Configuration 1*/
	const static int interface=0;	/* Interface 0 */

	/* change by Xiaofan */
	/* libusb-win32 requires the correct endpoint address
	   including the direction bits. This is the most important
	   differece between libusb-win32 and libusb.
	*/  
	//const static int endpoint=1; /* first endpoint for everything */
	const static int endpoint_in=0x1; /* endpoint 0x81 address for IN */
	const static int endpoint_out=1; /* endpoint 1 address for OUT */

	const static int timeout=20000; /* timeout in ms */
  static bool _initialized;
  static void initialize();
  const static int reqLen=64;
  uint _vendorId, _productId, _config, _interface;
  usb_device * _device;
  usb_dev_handle *_handle;
  void bad(const char *why);
  virtual bool internalOpen();
  virtual void internalClose();
  virtual bool internalSend(const unsigned char *data, uint size, uint timeout);
  virtual bool internalReceive(uint size, unsigned char *data, uint timeout);
  virtual void setSystemError(const QString &message);
  bool internalSetPinOn(Pin pin, bool on);
  bool internalReadPin(Pin pin, LogicType type, bool &value);
//  static Handle openHandle(const QString &device, IODirs dirs);
//  static void closeHandle(Handle handle);
  static QStringList *_list;
  static QStringList deviceList();
  void tryToDetachDriver();
  static usb_bus *getBusses();
  //void send_usb(struct usb_dev_handle * d, int len, const char * src);
  //void recv_usb(struct usb_dev_handle * d, int len,  char * dest);
  //struct usb_dev_handle *upp_open(void);
  //static void setHardwareFlowControl(Parameters &parameters, bool o
};

} // namespace

#endif
