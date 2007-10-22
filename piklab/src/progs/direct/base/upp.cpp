#include "upp.h"
#include <usb.h> /* libusb header */
#include <stdio.h>
#include <string.h>


#include "common/global/global.h"
#if defined(HAVE_PPDEV)
#  include <linux/ppdev.h>
#  include <linux/parport.h>
#  include <fcntl.h>
#  include <sys/ioctl.h>
#  include <unistd.h> // needed on some system
#  include <errno.h>
#elif defined(HAVE_PPBUS)
#  include <machine/cpufunc.h>
#  include <dev/ppbus/ppi.h>
#  include <dev/ppbus/ppbconf.h>
#  include <fcntl.h>
#  include <errno.h>
#endif
#include "common/common/misc.h"

QStringList *Port::UPP::_list = 0;

Port::UPP::UPP(const QString &device, Log::Base &base)
  : Base(base), _device(0), _vendorId(vendorId), _productId(productId),
    _config(configuration), _interface(0), _handle(0)
{
  initialize();
}

bool Port::UPP::_initialized = false;

void Port::UPP::initialize()
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

usb_bus *Port::UPP::getBusses()
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

/* max packet size is 64-bytes */



void Port::UPP::bad(const char *why) {
	setSystemError(i18n("Fatal error %s").arg(why));
}

QStringList Port::UPP::deviceList()
{
  QStringList list;
  list.append(QString("Usbpicprog"));

  return list;
}

const  QStringList &Port::UPP::probedDeviceList()
{
  if ( _list==0 ) {
    QStringList all = deviceList();
    _list = new QStringList;
    for (uint i=0; i<uint(all.count()); i++)
      if( probe(all[i]) & (In | Out) ) _list->append(all[i]);
  }
  return *_list;
}

Port::IODirs Port::UPP::probe(const QString &device)
{
	//TODO probe for usbpicprog
	return (In | Out);
}

bool Port::UPP::doBreak(uint duration)
{

  msleep(duration);

  return 0;
}


struct usb_device *Port::UPP::findDevice(uint vendorId, uint productId)
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


bool Port::UPP::setPinOn(uint pin, bool on, LogicType type)
{
  if ( type==NegativeLogic ) on = !on;
  Q_ASSERT( pin<Nb_Pins );
  Q_ASSERT( PIN_DATA[pin].dir==Out );
  if ( !internalSetPinOn(Pin(pin), on) ) {
    setSystemError(i18n("Error setting bit %1 of serial port to %2").arg(PIN_DATA[pin].label).arg(on));
    return false;
  }
  return true;
}

bool Port::UPP::readPin(uint pin, LogicType type, bool &value)
{

  Q_ASSERT( pin<Nb_Pins );
  Q_ASSERT( PIN_DATA[pin].dir==In );
  if ( !internalReadPin(Pin(pin), type, value) ) {
    setSystemError(i18n("Error reading serial pin %1").arg(PIN_DATA[pin].label));
    return false;
  }
  return true;
}

QValueVector<Port::PinData> Port::UPP::pinData(IODir dir) const
{
  QValueVector<PinData> v;
  for (uint i=0; i<Nb_Pins; i++) {
    if ( PIN_DATA[i].dir!=dir ) continue;
    PinData pd = { i, PIN_DATA[i].label };
    v.append(pd);
  }
  return v;
}


bool Port::UPP::isGroundPin(uint pin) const
{
  return ( PIN_DATA[pin].label=="GND" );
}

Port::IODir Port::UPP::ioDir(uint pin) const
{
  return PIN_DATA[pin].dir;
}

const Port::UPP::UPinData Port::UPP::PIN_DATA[Nb_Pins] = {
  { Out,  "VPP" }, { Out,  "VPP_Reset" }, { Out, "DATA_OUT"  }, { In, "DATA_IN" },
  { Out,   "CLOCK" }, { NoIO, "GND" }, { Out, "VDD" }, { Out, "DIR" }
};

/****************** Internal I/O Commands *****************/



bool Port::UPP::internalOpen()
{
#ifdef HAVE_USB
  _device = findDevice(vendorId, productId);
  if ( _device==0 ) {
    log(Log::Error, i18n("Could not find Usbpicprog device (vendor=%1 product=%2).")
             .arg(toLabel(Number::Hex, _vendorId, 4)).arg(toLabel(Number::Hex, _productId, 4)));
    return false;
  }
  _handle = usb_open(_device);
  if ( _handle==0 ) {
    setSystemError(i18n("Error opening Usbpicprog device."));
    return false;
  }
// windows: usb_reset takes about 7-10 seconds to re-enumerate the device...
// BSD : not implemented in libusb...
#  if !defined(Q_OS_BSD4) && !defined(Q_OS_WIN)
  if ( usb_reset(_handle)<0 ) {
    setSystemError(i18n("Error resetting Usbpicprog device."));
    return false;
  }
#  endif
  usb_close(_handle);
  _handle = usb_open(_device);
  if ( _handle==0 ) {
    setSystemError(i18n("Error opening Usbpicprog device."));
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
  //_private->_interface = &(configd.interface[i].altsetting[0]);
  if ( usb_claim_interface(_handle, _interface)<0 ) {
    setSystemError(i18n("Could not claim USB interface %1").arg(_interface));
    return false;
  }
  //log(Log::MaxDebug, QString("alternate setting is %1").arg(_private->_interface->bAlternateSetting));
  log(Log::MaxDebug, QString("USB bcdDevice: %1").arg(toHexLabel(_device->descriptor.bcdDevice, 4)));
  return true;
#else
  log(Log::Error, i18n("USB support disabled"));
  return false;
#endif
}

void Port::UPP::tryToDetachDriver()
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

void Port::UPP::internalClose()
{
  if ( _handle==0 ) return;
#ifdef HAVE_USB
  usb_release_interface(_handle, _interface);
  usb_close(_handle);
  //_private->_interface = 0;
#endif
  _device = 0;
  _handle = 0;
}

bool Port::UPP::internalSend(const unsigned char *data, uint size, uint timeout)
{
 int r = usb_interrupt_write(_handle, endpoint_out, (char *)data, size, timeout);
 if (r!=(int)size)return false;
 return true;
}

bool Port::UPP::internalReceive(uint size, unsigned char *data, uint timeout)
{
   int r = usb_interrupt_read(_handle, endpoint_in, (char*)data, size, timeout);
   if( r != (int)size )return false;
 return true;
}

void Port::UPP::setSystemError(const QString &message)
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

bool Port::UPP::internalSetPinOn(Pin pin, bool on)
{
    unsigned char buffer[reqLen];
    if(on)buffer[0]=(unsigned char)(0x80|pin);
    else buffer[0]=(unsigned char)0x80;
    //printf("Send 0x%X to Usbpicprog\n",(int)buffer[0]);
    return internalSend(buffer,1,timeout);
}

bool Port::UPP::internalReadPin(Pin pin, LogicType type, bool &value)
{
   unsigned char buffer[reqLen];
   bool retval_s,retval_r;
   buffer[0]=(unsigned char)(0xC0|pin);
   retval_s=internalSend(buffer,1,timeout);
   
   retval_r=internalReceive(1, buffer,timeout); 
   if (buffer[0]==(unsigned char)0xc1) value=1;
   else value=0;
   //printf("Read value from Usbicprog: 0x%X ,%i\n",buffer[0],value);
   return retval_s&retval_r;
}

extern int usb_debug;


