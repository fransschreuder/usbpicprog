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
#ifndef USB_PORT_H
#define USB_PORT_H

#include "port_base.h"
#if defined(Q_OS_WIN)
#undef interface
#endif
struct usb_dev_handle;
struct usb_device;
struct usb_bus;

namespace Port
{

class USB : public Base
{
public:
  USB(Log::Base &base, uint vendorId, uint productId, uint config, uint interface);
  virtual ~USB();
  virtual Description description() const { return Description(PortType::USB, QString::null); }

  static struct usb_device *findDevice(uint vendorId, uint productId);
  static bool isAvailable();
  static QStringList probedDeviceList();

  struct ControlMessageData {
    int type, request, value;
    const char *bytes;
  };
  bool sendControlMessage(const ControlMessageData &data);
  enum EndpointMode { Bulk = 0, Interrupt, Control, Isochronous, Nb_EndpointModes };
  static const char * const ENDPOINT_MODE_NAMES[Nb_EndpointModes];
  EndpointMode endpointMode(uint ep) const;
  IODir endpointDir(uint ep) const;

protected:
  bool write(uint endPoint, const char *data, uint size);
  bool read(uint endPoint, char *data, uint size, bool *poll = 0);

private:
  class Private;
  Private        *_private;
  uint _vendorId, _productId, _config, _interface;
  usb_dev_handle *_handle;
  usb_device     *_device;

  virtual bool internalOpen();
  virtual void internalClose();
  virtual void setSystemError(const QString &message);
  void tryToDetachDriver();

  static bool _initialized;
  static void initialize();
  static usb_bus *getBusses();
  static bool findBulk(const struct usb_device *device);
};

} // namespace

#endif
