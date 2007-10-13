/***************************************************************************
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "direct_pic.h"

#include "common/common/misc.h"

//-----------------------------------------------------------------------------
Direct::PulseEngine::PulseEngine(::Programmer::Base &base)
  : _base(base)
{
  _clockDelay = 0;
}

BitValue Direct::PulseEngine::pulseEngine(const QString &cmd, BitValue value)
{
  _base.log(Log::ExtraDebug, QString("pulse engine: %1").arg(cmd));
  QByteArray a = toAscii(cmd);
  BitValue res = 0;
  for (const char *ptr=a.data(); (ptr-a.data())<int(a.count()); ++ptr)
    if ( !pulse(ptr, value, res) ) qFatal("pulse engine: invalid command '%c'", *ptr);
  return res;
}

// Electric signals level commands:
// UPPER case means signal High - lower case means Low
//  Example: "cPCcp" means "clock-low, power on, clock high, clock low, power off".
// c/C = clock
// d/D = data
// p/P = power (vdd)
// Higher level commands:
// wnnnnn = wait nnnnn microseconds (nnnnn is decimal)
// , = a shorthand for w1
// ; or space = NOP (can be used as separator - useful in debug mode)
bool Direct::PulseEngine::pulse(const char * &cmd, BitValue, BitValue &)
{
  switch (*cmd) {
    case ';':
    case ' ':
      _base.log(Log::MaxDebug, "NOP");
      break;
    case 'c':
      _base.log(Log::MaxDebug, "CLOCK L");
      hardware().setPin(Clock, Low);
      break;
    case 'C':
      _base.log(Log::MaxDebug, "CLOCK H");
      hardware().setPin(Clock, High);
      break;
    case 'd':
      _base.log(Log::MaxDebug, "DATA L");
      hardware().setPin(DataOut, Low);
      break;
    case 'D':
      _base.log(Log::MaxDebug, "DATA H");
      hardware().setPin(DataOut, High);
      break;
    case 'p':
      _base.log(Log::MaxDebug, "VDD Off");
      hardware().setPin(Vdd, Off);
      Port::usleep(10000);
      break;
    case 'P':
      _base.log(Log::MaxDebug, "VDD On");
      hardware().setPin(Vdd, On);
      // wait 10ms for ALL devices because some programmers (or target board) have capacitors to charge
      Port::usleep(10000);
      break;
    case 'w': {
      uint n = 0;
      for(; *(cmd+1) && isdigit((int)*(cmd+1)); ++cmd)
        n = (n * 10) + *(cmd+1) - '0';
      _base.log(Log::MaxDebug, "WAIT " + QString::number(n) + " micro-sec.");
      Port::usleep(n);
      break;
    }
    case ',':
      _base.log(Log::MaxDebug, "WAIT 1 micro-sec");
      Port::usleep(1);
      break;
    default: return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
Direct::PicDeviceSpecific::PicDeviceSpecific(::Programmer::Base &base)
  : ::Programmer::PicDeviceSpecific(base), PulseEngine(base)
{}

// Electric signals level commands:
// UPPER case means signal High - lower case means Low
// b/B = burn (vpp)
bool Direct::PicDeviceSpecific::pulse(const char *&cmd, BitValue value, BitValue &res)
{
  switch (*cmd) {
    case 'b':
      log(Log::MaxDebug, "VPP Off");
      hardware().setPin(Vpp, Off);
      Port::usleep(10); // 10us
      break;
    case 'B':
      log(Log::MaxDebug, "VPP On");
      hardware().setPin(Vpp, On);
      Port::usleep(100000); // 100ms
      break;
    default: return PulseEngine::pulse(cmd, value, res);
  }
  return true;
}

bool Direct::PicDeviceSpecific::setPowerOnVddFirst()
{
  setPowerOff();
  if ( hardware().isGroundPin(Vdd) ) { // some programmer cannot change Vdd independently
    pulseEngine("B");  // charge capacitor to have high Vdd
    pulseEngine("bB"); // turn off Vpp briefly, Vpp on
  } else pulseEngine("PB");
  return true;
}

bool Direct::PicDeviceSpecific::setPowerOnVppFirst()
{
  setPowerOff();
  pulseEngine("BP");
  return true;
}

bool Direct::PicDeviceSpecific::setPowerOff()
{
  pulseEngine("cdpb"); // turn off Vpp after Vdd to prevent running device
  pulseEngine("w50000"); // to be on the safe size with multiple on/off
  return true;
}

bool Direct::PicDeviceSpecific::setTargetPowerOn(bool on)
{
  pulseEngine(on ? "P" : "p");
  return true;
}

//-----------------------------------------------------------------------------
// Higher level commands:
// S = send data word
// R = read data word
// r = read byte
// knn = send 6 or 4 bits command nn to PIC (nn is decimal)
bool Direct::Pic8DeviceSpecific::pulse(const char *&cmd, BitValue value, BitValue &res)
{
  switch (*cmd) {
    case 'S':
      log(Log::MaxDebug, "SEND " + toHexLabel(value, 4));
      send_word(value);
      break;
    case 'R':
      log(Log::MaxDebug, "Read Word");
      res = get_word();
      break;
    case 'r':
      log(Log::MaxDebug, "Read Byte");
      res = get_byte();
      break;
    case 'k': {
      uint n = 0;
      for(; *(cmd+1) && isdigit((int)*(cmd+1)); ++cmd)
        n = (n * 10) + *(cmd+1) - '0';
      log(Log::MaxDebug, "SEND cmd " + QString::number(n));
      send_cmd(n);
      break;
    }
    default: return PicDeviceSpecific::pulse(cmd, value, res);
  }
  return true;
}
