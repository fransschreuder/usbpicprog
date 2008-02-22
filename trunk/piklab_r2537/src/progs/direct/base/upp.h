/***************************************************************************
 * Copyright (C) 2007 Frans Schreuder <usbpicprog.sf.net>                  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef UPP_H
#define UPP_H

#include "common/global/global.h"

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
#include "common/port/usb_port.h"


namespace Port
{

class UPP : public USB
{
public:

       UPP( Log::Base &base);
       virtual ~UPP() { }

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

        const static uint vendorId=0x04d8; // Microchip, Inc
	const static uint productId=0x000E; // Usbpicprog Device Descriptor
	const static int configuration=1; /*  Configuration 1*/
	const static int interface=0;	/* Interface 0 */

	const static int endpoint_in=0x81; /* endpoint 0x81 address for IN */
	const static int endpoint_out=1; /* endpoint 1 address for OUT */

        const static int reqLen=64;	//size of the receive / transmit buffer in the PIC
        bool internalSetPinOn(Pin pin, bool on);
        bool internalReadPin(Pin pin, LogicType type, bool &value);
  	virtual void setSystemError(const QString &message);

};

} // namespace

#endif
