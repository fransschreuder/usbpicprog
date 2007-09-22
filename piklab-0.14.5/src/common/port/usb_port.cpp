/***************************************************************************
 * Copyright (C) 2005 Lorenz Mösenlechner & Matthias Kranz                 *
 *                    <icd2linux@hcilab.org>                               *
 * Copyright (C) 2003-2005 Orion Sky Lawlor <olawlor@acm.org>              *
 * Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "usb_port.h"

#include "common/common/misc.h"
#ifdef HAVE_USB
#include <usb.h>
#endif
#include <qdatetime.h>

//-----------------------------------------------------------------------------
class Port::USB::Private
{
public:
#ifdef HAVE_USB
  Private() : _interface(0) {}
  const usb_interface_descriptor *_interface;
#endif
};

bool Port::USB::_initialized = false;

void Port::USB::initialize()
{
  if (_initialized) return;
  _initialized = true;
#ifdef HAVE_USB
  usb_init();
  VersionData vd = VersionData::fromString(LIBUSB_VERSION);
  QString s = QString("libusb %1").arg(vd.pretty());
  if ( vd<VersionData(0, 1, 8) ) qWarning("%s: may be too old (you need at least version 0.1.8)", s.latin1());
#endif
}

bool Port::USB::isAvailable()
{
#ifdef HAVE_USB
  return true;
#else
  return false;
#endif
}

usb_bus *Port::USB::getBusses()
{
  initialize();
#ifdef HAVE_USB
  // refresh libusb structures
  usb_find_busses();
  usb_find_devices();
  return usb_get_busses();
#else
  return 0;
#endif
}

bool Port::USB::findBulk(const struct usb_device *dev)
{
  initialize();
#ifdef HAVE_USB
  int configuration = -1, interface = -1, altsetting = -1, bulk_endpoint = -1;
  // walk through the possible configs, etc.
  qDebug("This device has %d possible configuration(s).", dev->descriptor.bNumConfigurations);
  for (int c=0; c<dev->descriptor.bNumConfigurations; c++) {
    qDebug("Looking at configuration %d...This configuration has %d interfaces.", c, dev->config[c].bNumInterfaces);
    for(int i=0; i<dev->config[c].bNumInterfaces; i++) {
      qDebug("  Looking at interface %d...This interface has %d altsettings.", i, dev->config[c].interface[i].num_altsetting);
      for (int a=0; a < dev->config[c].interface[i].num_altsetting; a++) {
        qDebug("    Looking at altsetting %d...This altsetting has %d endpoints.", a, dev->config[c].interface[i].altsetting[a].bNumEndpoints);
        for (int e=0; e < dev->config[c].interface[i].altsetting[a].bNumEndpoints; e++) {
          QString s;
          s.sprintf("      Endpoint %d: Address %02xh, attributes %02xh ", e, dev->config[c].interface[i].altsetting[a].endpoint[e].bEndpointAddress, dev->config[c].interface[i].altsetting[a].endpoint[e].bmAttributes);
          uchar attribs = (dev->config[c].interface[i].altsetting[a].endpoint[e].bmAttributes & 3);
          switch (attribs) {
          case 0: s += "(Control) "; break;
          case 1: s += "(Isochronous) "; break;
          case 2: s += "(Bulk) ";
            /* Found the correct configuration, interface etc... it has bulk endpoints! */
            configuration=c;
            interface=i;
            altsetting=a;
            break;
          case 3: s += "(Interrupt) "; break;
          default: s += "ERROR! Got an illegal value in endpoint bmAttributes";
          }
          if ( attribs!=0 ) {
            uchar dir = (dev->config[c].interface[i].altsetting[a].endpoint[e].bEndpointAddress & 0x80);
            switch (dir) {
            case 0x00: s += "(Out)";
              // Do nothing in this case
              break;
            case 0x80: s += "(In)";
              if ((dev->config[c].interface[i].altsetting[a].endpoint[e].bmAttributes & 0x03) == 2) /* Make sure it's a *bulk* endpoint */ {
                // Found the correct endpoint to use for bulk transfer; use its ADDRESS
                bulk_endpoint=dev->config[c].interface[i].altsetting[a].endpoint[e].bEndpointAddress;
              }
              break;
            default: s += "ERROR! Got an illegal value in endpoint bEndpointAddress";
            }
          }
          qDebug("%s", s.latin1());
        }
      }
    }
  }
  if (bulk_endpoint<0) {
    qDebug("No valid interface found!");
    return false;
  }
#endif
  return true;
}

QStringList Port::USB::probedDeviceList()
{
  QStringList list;
#ifdef HAVE_USB
  usb_init(); // needed ?
  for (usb_bus *bus=getBusses(); bus; bus=bus->next) {
    for (struct usb_device *dev=bus->devices; dev; dev=dev->next) {
      if ( dev->descriptor.idVendor==0 ) continue; // controller
      list.append(QString("Vendor Id: %1 - Product Id: %2")
                  .arg(toLabel(Number::Hex, dev->descriptor.idVendor, 4)).arg(toLabel(Number::Hex, dev->descriptor.idProduct, 4)));
    }
  }
#endif
  return list;
}

struct usb_device *Port::USB::findDevice(uint vendorId, uint productId)
{
#ifdef HAVE_USB
  for (usb_bus *bus=getBusses(); bus; bus=bus->next) {
    for (struct usb_device *dev=bus->devices; dev; dev=dev->next) {
      if ( dev->descriptor.idVendor==vendorId && dev->descriptor.idProduct==productId )
        return dev;
    }
  }
#else
  Q_UNUSED(vendorId); Q_UNUSED(productId);
  qDebug("USB support disabled");
#endif
  return 0;
}

//-----------------------------------------------------------------------------
const char * const Port::USB::ENDPOINT_MODE_NAMES[Nb_EndpointModes] = {
  "bulk", "interrupt", "control", "isochronous"
};

Port::USB::USB(Log::Base &base)
  : Base(base), _vendorId(0), _productId(0),
    _config(1), _interface(0), _handle(0), _device(0)
{
  _private = new Private;
  initialize();
}

void Port::USB::init(uint vendorId, uint productId, uint config, uint interface)
{
  close();
  _vendorId = vendorId;
  _productId = productId;
  _config = config;
  _interface = interface;
}

Port::USB::~USB()
{
  close();
  delete _private;
}

void Port::USB::setSystemError(const QString &message)
{
#ifdef HAVE_USB
  log(Log::Error, message + QString(" (err=%1).").arg(usb_strerror()));
#else
  log(Log::Error, message);
#endif
}

void Port::USB::tryToDetachDriver()
{
  // try to detach an already existing driver... (linux only)
#if defined(LIBUSB_HAS_GET_DRIVER_NP) && LIBUSB_HAS_GET_DRIVER_NP
  log(Log::ExtraDebug, "find if there is already an installed driver");
  char dname[256] = "";
  if ( usb_get_driver_np(_handle, _interface, dname, 255)<0 ) return;
  log(Log::NormalDebug, QString("  a driver \"%1\" is already installed...").arg(dname));
#  if defined(LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP) && LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
  usb_detach_kernel_driver_np(_handle, _interface);
  log(Log::NormalDebug, "  try to detach it...");
  if ( usb_get_driver_np(_handle, _interface, dname, 255)<0 ) return;
  log(Log::NormalDebug, "  failed to detach it");
#  endif
#endif
}

bool Port::USB::internalOpen()
{
#ifdef HAVE_USB
  _device = findDevice(_vendorId, _productId);
  if ( _device==0 ) {
    log(Log::Error, i18n("Could not find USB device (vendor=%1 product=%2).")
             .arg(toLabel(Number::Hex, _vendorId, 4)).arg(toLabel(Number::Hex, _productId, 4)));
    return false;
  }
  _handle = usb_open(_device);
  if ( _handle==0 ) {
    setSystemError(i18n("Error opening USB device."));
    return false;
  }
// windows: usb_reset takes about 7-10 seconds to re-enumerate the device...
// BSD : not implemented in libusb...
#  if !defined(Q_OS_BSD4) && !defined(Q_OS_WIN)
  if ( usb_reset(_handle)<0 ) {
    setSystemError(i18n("Error resetting USB device."));
    return false;
  }
#  endif
  usb_close(_handle);
  _handle = usb_open(_device);
  if ( _handle==0 ) {
    setSystemError(i18n("Error opening USB device."));
    return false;
  }
  tryToDetachDriver();
  uint i = 0;
  for (; i<_device->descriptor.bNumConfigurations; i++)
    if ( _config==_device->config[i].bConfigurationValue ) break;
  if ( i==_device->descriptor.bNumConfigurations ) {
    uint old = _config;
    i = 0;
    _config = _device->config[i].bConfigurationValue;
    log(Log::Warning, i18n("Configuration %1 not present: using %2").arg(old).arg(_config));
  }
  const usb_config_descriptor &configd = _device->config[i];
  if ( usb_set_configuration(_handle, _config)<0 ) {
    setSystemError(i18n("Error setting USB configuration %1.").arg(_config));
    return false;
  }
  for (i=0; i<configd.bNumInterfaces; i++)
    if ( _interface==configd.interface[i].altsetting[0].bInterfaceNumber ) break;
  if ( i==configd.bNumInterfaces ) {
    uint old = _interface;
    i = 0;
    _interface = configd.interface[i].altsetting[0].bInterfaceNumber;
    log(Log::Warning, i18n("Interface %1 not present: using %2").arg(old).arg(_interface));
  }
  _private->_interface = &(configd.interface[i].altsetting[0]);
  if ( usb_claim_interface(_handle, _interface)<0 ) {
    setSystemError(i18n("Could not claim USB interface %1").arg(_interface));
    return false;
  }
  log(Log::MaxDebug, QString("alternate setting is %1").arg(_private->_interface->bAlternateSetting));
  log(Log::MaxDebug, QString("USB bcdDevice: %1").arg(toHexLabel(_device->descriptor.bcdDevice, 4)));
  return true;
#else
  log(Log::Error, i18n("USB support disabled"));
  return false;
#endif
}

void Port::USB::internalClose()
{
  if ( _handle==0 ) return;
#ifdef HAVE_USB
  usb_release_interface(_handle, _interface);
  usb_close(_handle);
  _private->_interface = 0;
#endif
  _device = 0;
  _handle = 0;
}

bool Port::USB::sendControlMessage(const ControlMessageData &data)
{
  if ( hasError() ) return false;
#ifdef HAVE_USB
  QString s = data.bytes;
  uint length = strlen(data.bytes) / 2;
  QByteArray ba(length);
  for (uint i=0; i<length; i++)
    ba[i] = fromString(Number::Hex, s.mid(2*i, 2), 0);
  int res = usb_control_msg(_handle, data.type, data.request, data.value, 0, ba.data(), length, 1000); // 1s
  if ( res<0 ) {
    setSystemError(i18n("Error sending control message to USB port."));
    return false;
  }
#endif
  return true;
}

uint timeout(uint size)
{
  return qMax(size*5, uint(1000)); // 5ms per byte or 1s
}


Port::USB::EndpointMode Port::USB::endpointMode(uint ep) const
{
#ifdef HAVE_USB
  uint index = ep & USB_ENDPOINT_ADDRESS_MASK;
  Q_ASSERT(_private->_interface);
  const usb_endpoint_descriptor *ued = _private->_interface->endpoint + index;
  Q_ASSERT(ued);
  switch (ued->bmAttributes & USB_ENDPOINT_TYPE_MASK) {
    case USB_ENDPOINT_TYPE_BULK: return Bulk;
    case USB_ENDPOINT_TYPE_INTERRUPT: return Interrupt;
    case USB_ENDPOINT_TYPE_ISOCHRONOUS: return Isochronous;
    case USB_ENDPOINT_TYPE_CONTROL: return Control;
    default: break;
  }
#endif
  Q_ASSERT(false);
  return Nb_EndpointModes;
}

Port::IODir Port::USB::endpointDir(uint ep) const
{
#ifdef HAVE_USB
  switch (ep & USB_ENDPOINT_DIR_MASK) {
    case USB_ENDPOINT_IN: return In;
    case USB_ENDPOINT_OUT: return Out;
    default: break;
  }
#endif
  Q_ASSERT(false);
  return NoIO;
}

bool Port::USB::write(uint ep, const char *data, uint size)
{
  if ( hasError() ) return false;
#ifdef HAVE_USB
  IODir dir = endpointDir(ep);
  EndpointMode mode = endpointMode(ep);
  log(Log::LowLevelDebug, QString("write to endpoint %1 (%2 - %3) %4 chars: \"%5\"")
                          .arg(toHexLabel(ep, 2)).arg(ENDPOINT_MODE_NAMES[mode]).arg(IO_DIR_NAMES[dir]).arg(size).arg(toPrintable(data, size, PrintEscapeAll)));
  Q_ASSERT( dir==Out );
  QTime time;
  time.start();
  int todo = size;
  for (;;) {
    int res = 0;
    //qDebug("write ep=%i todo=%i/%i", ep, todo, size);
    if ( mode==Interrupt ) res = usb_interrupt_write(_handle, ep, (char *)data + size - todo, todo, timeout(todo));
    else res = usb_bulk_write(_handle, ep, (char *)data + size - todo, todo, timeout(todo));
    //qDebug("res: %i", res);
    if ( res==todo ) break;
    if ( uint(time.elapsed())>3000 ) { // 3 s
      if ( res<0 ) setSystemError(i18n("Error sending data (ep=%1 res=%2)").arg(toHexLabel(ep, 2)).arg(res));
      else log(Log::Error, i18n("Timeout: only some data sent (%1/%2 bytes).").arg(size-todo).arg(size));
      return false;
    }
    if ( res==0 ) log(Log::NormalDebug, i18n("Nothing sent: retrying..."));
    if ( res>0 ) todo -= res;
    msleep(100);
  }
#else
  Q_UNUSED(ep); Q_UNUSED(data); Q_UNUSED(size);
#endif
  return true;
}

bool Port::USB::read(uint ep, char *data, uint size, bool *poll)
{
  if ( hasError() ) return false;
#ifdef HAVE_USB
  IODir dir = endpointDir(ep);
  EndpointMode mode = endpointMode(ep);
  log(Log::LowLevelDebug, QString("read from endpoint %1 (%2 - %3) %4 chars")
                          .arg(toHexLabel(ep, 2)).arg(ENDPOINT_MODE_NAMES[mode]).arg(IO_DIR_NAMES[dir]).arg(size));
  Q_ASSERT( dir==In );
  QTime time;
  time.start();
  int todo = size;
  for (;;) {
    int res = 0;
    //qDebug("read ep=%i size=%i", ep, todo);
    if ( mode==Interrupt ) res = usb_interrupt_read(_handle, ep, data + size - todo, todo, timeout(todo));
    else res = usb_bulk_read(_handle, ep, data + size - todo, todo, timeout(todo));
    //qDebug("res: %i", res);
    if ( res==todo ) break;
    if ( uint(time.elapsed())>3000 ) { // 3 s: seems to help icd2 in some case (?)
      if ( res<0 ) setSystemError(i18n("Error receiving data (ep=%1 res=%2)").arg(toHexLabel(ep, 2)).arg(res));
      else log(Log::Error, i18n("Timeout: only some data received (%1/%2 bytes).").arg(size-todo).arg(size));
      return false;
    }
    if ( res==0 ) {
      if (poll) {
        *poll = false;
        return true;
      } else log(Log::NormalDebug, i18n("Nothing received: retrying..."));
    }
    if ( res>0 ) todo -= res;
    msleep(100);
  }
  if (poll) *poll = true;
#else
  Q_UNUSED(ep); Q_UNUSED(data); Q_UNUSED(size);
#endif
  return true;
}
