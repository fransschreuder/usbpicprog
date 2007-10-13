/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *   Copyright (C) Brian C. Lane                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "direct.h"

#include <iostream>

#include "common/port/parallel.h"
#include "common/port/serial.h"
#include "progs/base/generic_prog.h"
#include "direct_prog_config.h"

using namespace std;

//-----------------------------------------------------------------------------
const Direct::PinData Direct::PIN_DATA[Nb_PinTypes] = {
  { I18N_NOOP("MCLR (Vpp)"),  "0 V", "13 V", "vpp",
    I18N_NOOP("The VPP pin is used to select the high voltage programming mode."),
    Port::Out, false, I18N_NOOP("Check this box to turn voltage on/off for this pin.") },
  { I18N_NOOP("Power (Vdd)"), "0 V", "5 V",  "vdd",
    I18N_NOOP("The VDD pin is used to apply 5V to the programmed device.\nMust be set to GND if your programmer doesn't control the VDD line."),
    Port::Out, true,  I18N_NOOP("Check this box to turn voltage on/off for this pin.") },
  { I18N_NOOP("Clock"),       "0 V", "5 V",  "clock",
    I18N_NOOP("The CLOCK pin is used to synchronize serial data of the DATA IN and DATA OUT pins."),
    Port::Out, false, I18N_NOOP("Check this box to turn voltage on/off for this pin.") },
  { I18N_NOOP("Data Out"),    "0 V", "5 V",  "datao",
    I18N_NOOP("The DATA OUT pin is used to send data to the programmed device."),
    Port::Out, false, I18N_NOOP("Check this box to turn voltage on/off for this pin.") },
  { I18N_NOOP("Data In"),     "0 V", "5 V",  "datai",
    I18N_NOOP("The DATA IN pin is used to receive data from the programmed device."),
    Port::In,  false, I18N_NOOP("This pin is driven by the programmed device.\nWithout device, it must follow the \"Data out\" pin (when powered on).") },
  { I18N_NOOP("Data R/W"),    "Data in", "Data out", "drw",
    I18N_NOOP("The DATA RW pin selects the direction of data buffer.\nMust be set to GND if your programmer does not use bi-directionnal buffer."),
    Port::Out, true,  I18N_NOOP("Check this box to change DATA buffer direction.") }
};

//-----------------------------------------------------------------------------
namespace Direct
{
class SerialPort : public Port::Serial
{
public:
  SerialPort(const QString &device, Log::Base &base)
    : Serial(device, NeedBreak, base) {}

  bool open() {
    if ( !Port::Serial::open() ) return false;
    if ( !setMode(IgnoreBreak | IgnoreParity, ByteSize8 | EnableReceiver | IgnoreControlLines, S9600, 0) ) return false;
    // set up lines for "idle state" ???
    return true;
  }
};
} // namespace

//-----------------------------------------------------------------------------
Direct::Hardware::Hardware(::Programmer::Base &base, Port::Base *port, const HardwareData &data)
  : ::Programmer::PicHardware(base, port, data.name), _data(data.data)
{}

bool Direct::Hardware::internalConnectHardware()
{
  if ( !_port->open() ) return false;

  // keep the safe state of rw (read)...
  setPin(DataOut, Low);
  setPin(Clock, Low);
  setPin(Vpp, Off);
  setPin(Vdd, Off);
  setRead();

  return true;
}

void Direct::Hardware::setPin(PinType type, bool on)
{
  int pin = _data.pins[type];
  if ( isGroundPin(pin) ) return;
  uint p = (pin<0 ? -pin : pin)-1;
  //log(Log::ExtraDebug, QString("Hardware::setPin %1 %2: %3 %4").arg(PIN_DATA[type].label).arg(pin).arg(on).arg(_data.clockDelay));
  _port->setPinOn(p, on, (pin<0 ? Port::NegativeLogic : Port::PositiveLogic));
  if ( type==Clock ) Port::usleep(_data.clockDelay);
}

bool Direct::Hardware::readBit()
{
  int pin = _data.pins[DataIn];
  Q_ASSERT( pin!=0 );
  uint p = (pin<0 ? -pin : pin)-1;
  bool on;
  _port->readPin(p, (pin<0 ? Port::NegativeLogic : Port::PositiveLogic), on);
  //log(Log::ExtraDebug, QString("Hardware::read DataIn %2: %3").arg(pin).arg(on));
  return on;
}

uint Direct::Hardware::nbPins(Port::IODir dir) const
{
  return _port->pinData(dir).count();
}

QString Direct::Hardware::pinLabelForIndex(Port::IODir dir, uint i) const
{
  Port::PinData pd = _port->pinData(dir)[i];
  return QString("%1 (%2)").arg(pd.pin+1).arg(pd.label);
}

Port::IODir Direct::Hardware::ioTypeForPin(int pin) const
{
  if ( isGroundPin(pin) ) return Port::NoIO;
  uint p = (pin<0 ? -pin : pin)-1;
  return _port->ioDir(p);
}

uint Direct::Hardware::pinForIndex(Port::IODir dir, uint i) const
{
  Q_ASSERT( i<=uint(_port->pinData(dir).count()) );
  if ( i==uint(_port->pinData(dir).count()) ) return _port->groundPin()+1;
  return _port->pinData(dir)[i].pin+1;
}

uint Direct::Hardware::indexForPin(Port::IODir dir, int pin) const
{
  QValueVector<Port::PinData> v = _port->pinData(dir);
  Q_ASSERT( pin!=0 );
  uint p = (pin<0 ? -pin : pin)-1;
  for (uint i=0; i<uint(v.count()); i++)
    if ( v[i].pin==p ) return i;
  Q_ASSERT( isGroundPin(pin) );
  return v.count();
}

bool Direct::Hardware::isGroundPin(int pin) const
{
  Q_ASSERT( pin!=0 );
  uint p = (pin<0 ? -pin : pin)-1;
  return _port->isGroundPin(p);
}

bool Direct::operator !=(const HData &d1, const HData &d2)
{
  for (uint i=0; i<Nb_PinTypes; i++)
    if ( d1.pins[i]!=d2.pins[i] ) return true;
  if ( d1.clockDelay!=d2.clockDelay ) return true;
  return false;
}

//-----------------------------------------------------------------------------
Direct::SerialHardware::SerialHardware(::Programmer::Base &base, const QString &portDevice, const HardwareData &data)
  : Hardware(base, new SerialPort(portDevice, base), data)
{}

Direct::ParallelHardware::ParallelHardware(::Programmer::Base &base, const QString &portDevice, const HardwareData &data)
  : Hardware(base, new Port::Parallel(portDevice, base), data)
{}

/*
//-----------------------------------------------------------------------------
int Direct::Hardware::hardware_test()
{
    char t[80] ;

    cout << endl << "Please execute this Direct::Hardware test without any PIC"
    " connected to your programmer" <<endl << endl ;

    do
    {
        cout << "Ready for tests ? (y/n) " ;
        cin >> t ;
    } while(t[0] != 'y' && t[0] != 'n') ;
    if( t[0] == 'n') return -1 ;


    // stage 1 - hard initialization
    // setpins("parallel","/dev/parport0", -5, -4, 3, 2, 10) ;

    //Default RW line not used...
    HData data = { { 3, 5, 7, 4, 8, 0 }, 0 };
    Log::ConsoleView view;
    Direct::Programmer programmer;
    programmer.setView(&view);
    SerialHardware hw(programmer, "/dev/ttyS0", data);
    if ( !hw.connectHardware() ) {
        cout << "Direct::Hardware initialization error" <<endl ;
        return 1 ;
    }


    //++Mirko!!
    //By Ciri 11/3/2004...
    //From here to the end of function i haven't modified nothing...
    //--Mirko!!

    // stage 2 - all lines off
    hw.setPin(Vpp, Off);
    hw.setPin(Vdd, Off);
    hw.setPin(Clock, Low);
    hw.setPin(DataOut, Low);
    cout << "All the following lines must be 0V : " << endl <<
    "16F84 pin 13 (data out)"<<endl <<
    "16F84 pin 12 (clock)"<<endl <<
    "16F84 pin 14 (VDD=power)"<<endl <<
    "16F84 pin 4  (VPP=prog)"<<endl ;
    do
    {
        cout << "OK ? (y/n) " ;
        cin >> t ;
    } while(t[0] != 'y' && t[0] != 'n') ;

    if( t[0] == 'n') return 2 ;

    // stage 3 - data out check
    hw.setPin(DataOut, High);
    cout << "16F84 pin 13 (data out) must be 5V"<<endl ;
    do
    {
        cout << "OK ? (y/n) " ;
        cin >> t ;
    } while(t[0] != 'y' && t[0] != 'n') ;
    hw.setPin(DataOut, Low);

    if( t[0] == 'n') return 3 ;

    // stage 4 - clock check
    hw.setPin(Clock, High);
    cout << "16F84 pin 12 (clock) must be 5V"<<endl ;
    do
    {
        cout << "OK ? (y/n) " ;
        cin >> t ;
    } while(t[0] != 'y' && t[0] != 'n') ;
    hw.setPin(Clock, Low);

    if( t[0] == 'n') return 4 ;

    // stage 5 - VDD check
    hw.setPin(Vdd, On);
    cout << "16F84 pin 14 (power) must be 5V"<<endl ;
    do
    {
        cout << "OK ? (y/n) " ;
        cin >> t ;
    } while(t[0] != 'y' && t[0] != 'n') ;
    hw.setPin(Vdd, Off) ;

    if( t[0] == 'n') return 5 ;

    // stage 6 - VPP check
    hw.setPin(Vpp, On);
    cout << "16F84 pin 4 (VDD) must be between 13V and 14V"<<endl ;
    do
    {
        cout << "OK ? (y/n) " ;
        cin >> t ;
    } while(t[0] != 'y' && t[0] != 'n') ;
    hw.setPin(Vpp , Off);

    if( t[0] == 'n') return 6 ;

    // stage 7 - test input data
    // set data out hi, because bi-directionnal
    // on pin 13 uses the open collector capability of 7407
    hw.setPin(DataOut, High);
    int in = hw.readBit();
    if( !in )
    {
        cout << "DataIn error (16F84 pin 13) : must be 5V and is not" << endl ;
        return 7 ;
    }
    cout << "Please set 16F84 pin 13 (DataIn) low " <<
    "(connect it to 16F84 pin 5 - GND)" << endl ;
    do
    {
        cout << "Done ? (y/n) " ;
        cin >> t ;
    } while(t[0] != 'y' && t[0] != 'n') ;

    in = hw.readBit();
    if(in )
    {
        cout << "DataIn error (pin 13) : must be 0V and is not" << endl ;
        return 7 ;
    }

    cout << "Congratulations! - Direct::Hardware is OK." <<endl ;
    return 0 ;
}

// this is my specific speed test
int Direct::Hardware::timing_test()
{
    char t[80] ;
    int cmd ;
    long loop = 50000000 ;

    cout << endl << "Please execute this Direct::Hardware test without any PIC"
    " connected to your programmer" <<endl << endl ;

    do
    {
        cout << "Ready for tests ? (y/n) " ;
        cin >> t ;
    } while(t[0] != 'y' && t[0] != 'n') ;
    if( t[0] == 'n') return -1 ;


    // stage 1 - hard initialization
    //Default Line RW not used...
    HData data = { { -5, -4, 3, 2, 10, 25 }, 0 };
    Log::ConsoleView view;
    Log::Manager manager;
    manager.setView(&view);
    ParallelHardware hw("/dev/parport0", manager, data);
    if ( !hw.connectHardware() ) {
        cout << "Direct::Hardware initialization error" <<endl ;
        return 1 ;
    }

    //++Mirko!!
    //By Ciri 11/3/2004...
    //From here to the end of function i have modified nothing...
    //--Mirko!!

    // stage 2 - all lines off
    hw.setPin(Vpp, Off);
    hw.setPin(Vdd, Off);
    hw.setPin(Clock, Low);
    hw.setPin(DataOut, Low);

    // stage 3 - choose test
    cout << "Remember : " << endl <<
    "16F84 pin 5  is GND"<<endl <<
    "16F84 pin 13 is data-out"<<endl <<
    "16F84 pin 12 is clock"<<endl ;

    cout << loop << " periods test .... " << endl ;

    cout << "1 - Maximum speed clock " << endl <<
    "2 - 100us half period "<<endl <<
    "3 - load data 0x2AAA (programmer->chip)"<<endl <<
    "4 - end"<<endl ;
    do
    {
        cout << "CMD (1-4)>> " ;
        cin >> cmd ;
    } while(cmd < 1 || cmd > 4) ;

    if(cmd == 4) return 2 ;
    else if ( cmd == 1)
    {
        for(long j=0 ; j < loop ; ++j)
        {
            hw.setPin(Clock, Low);
            hw.setPin(Clock, High);
        }
    }

    else if ( cmd == 2)
    {
        for(long j=0 ; j < loop ; ++j)
        {
            hw.setPin(Clock, Low);
            Port::usleep(100);
            hw.setPin(Clock, High);
            Port::usleep(100);
        }
    }

    else if ( cmd == 3) {
        for (long j=0; j<loop; ++j)
        {
            int d = 0x2AAA ;
            d &= 0x3FFF ;               // mask unused msb
            d <<= 1;                // insert start bit

            for (uint x = 0; x<16; x++) {
                hw.setPin(Clock, High);
                if ( d & 0x01 ) hw.setPin(DataOut, High);
                else hw.setPin(DataOut, Low);
                hw.setPin(Clock, Low);
                d >>= 1;
            }
            hw.setPin(DataOut, High);
        }
    }

    return 0;
}
*/
