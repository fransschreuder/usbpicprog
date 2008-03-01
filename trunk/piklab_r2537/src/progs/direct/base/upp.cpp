/***************************************************************************
 * Copyright (C) 2007 Frans Schreuder <usbpicprog.sf.net>                  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "upp.h"
#include <stdio.h>
#include <string.h>


#include "common/global/global.h"
#include "common/common/misc.h"
#include "common/port/usb_port.h"

Port::UPP::UPP(Log::Base &base):USB( base,vendorId, productId, configuration, interface)
{
  //init( vendorId, productId, configuration, interface);
}



/*Port::IODirs Port::UPP::probe(const QString &device)
{
	//TODO probe for usbpicprog
	return (In | Out);
}*/



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


void Port::UPP::setSystemError(const QString &message)
{
#if defined(Q_OS_UNIX)
  log(Log::LineType::Error, message + QString(" (errno=%1)").arg(strerror(errno)));
#elif defined(Q_OS_WIN)
  LPVOID lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
  log(Log::LineType::Error, message + QString(" (last error=%1 %2)").arg(GetLastError()).arg((const char *)(LPCTSTR)lpMsgBuf));
  LocalFree(lpMsgBuf);
#endif
}
  
bool Port::UPP::internalSetPinOn(Pin pin, bool on)
{
    unsigned char buffer[reqLen];
    if(pin>15)return 0; //Error, does not fit in the 4 bits that describe the pin
    if(on)buffer[0]=(unsigned char)(0x90|pin);
    else buffer[0]=(unsigned char)(0x80|pin);
    //printf("Send 0x%X to Usbpicprog\n",(int)buffer[0]);
    return write(endpoint_out,(const char*)buffer,1);
//    return internalSend(buffer,1,timeout);
}

bool Port::UPP::internalReadPin(Pin pin, LogicType type, bool &value)
{
   unsigned char buffer[reqLen];
   bool retval_s,retval_r;
   if(pin>15)return 0; //Error, does not fit in the 4 bits that describe the pin
   buffer[0]=(unsigned char)(0xC0|pin);
   retval_s=write(endpoint_out,(const char*)buffer,1);

   retval_r=read(endpoint_in, ( char*)buffer,1, NULL);
   
   if (buffer[0]==(unsigned char)(0xD0|pin)) 
   {
   	//printf("read char: 0x%x, translated as 1\n",buffer[0]);
	value=1;
   }
   else 
   {
	//printf("read char: 0x%x, translated as 0\n",buffer[0]);
	value=0;
   }
   //printf("Read value from Usbicprog: 0x%X ,%i\n",buffer[0],value);
   return retval_s&retval_r;
}

//extern int usb_debug;


