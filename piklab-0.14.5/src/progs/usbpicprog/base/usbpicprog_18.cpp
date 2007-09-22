/***************************************************************************
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "usbpicprog_18.h"

// Commands specific to 18F:
// xhh = send a 8 bits constant (hh must be a 2 digits hex constant)
// Xhhhh =  send a 16 bits constant (hhhh must be a 4 digits hex constant)
bool Usbpicprog::P18::pulse(const char *&cmd, uint value, uint &res, bool &ok)
{
  switch (*cmd) {
    case 'x': {
      uint n;
      sscanf(++cmd, "%02X", &n);
      ++cmd;
      log(Log::MaxDebug, "SEND 1 byte constant : " + toHexLabel(n, 2));
      send_bits(n, 8);
      break;
    }
    case 'X': {
      uint n;
      sscanf(++cmd,"%04X", &n);
      cmd += 3;
      log(Log::MaxDebug, "SEND 2 bytes constant : " + toHexLabel(n, 4));
      send_bits(n, 16);
      break;
    }
    default: return Pic8DeviceSpecific::pulse(cmd, value, res, ok);
  }
  ok = true;
  return true;
}

void Usbpicprog::P18::send_word(uint d)
{
    hardware().setWrite();
    for (uint x = 0; x<16; x++) {
        hardware().setPin(Clock, High);
        if ( d & 0x01 ) hardware().setPin(DataOut, High);
        else hardware().setPin(DataOut, Low);
        Port::usleep(_clockDelay+5);
        hardware().setPin(Clock, Low);
        Port::usleep(_clockDelay+3);
        d >>= 1;                    // Move the data over 1 bit
    }
    hardware().setPin(DataOut, High);
    hardware().setRead();
}

uint Usbpicprog::P18::get_word()
{
    hardware().setRead();
    uint ind = 0;
    send_cmd(9);
    hardware().setPin(DataOut, High);
    for (uint x = 0; x<16; x++) {
        hardware().setPin(Clock, High);
        Port::usleep(_clockDelay+5);
        if ( x>7 && hardware().readBit() ) ind |= (1 << x-8);
        hardware().setPin(Clock, Low);
        Port::usleep(_clockDelay+3);
    }
    send_cmd(9);
    hardware().setPin(DataOut, High);
    for (uint x = 0; x<16; x++) {
        hardware().setPin(Clock, High);
        Port::usleep(_clockDelay+5);
        if ( x>7 && hardware().readBit() ) ind |= (1 << x);
        hardware().setPin(Clock, Low);
        Port::usleep(_clockDelay+3);
    }
    return ind;
}

uint Usbpicprog::P18::get_byte()
{
    hardware().setRead();
    uint ind = 0;
    send_cmd(2);
    hardware().setPin(DataOut, High);
    for (uint x = 0; x<16; x++) {
        hardware().setPin(Clock, High);
        Port::usleep(_clockDelay+5);
        if ( x>7 && hardware().readBit() ) ind |= (1 << x-8);
        hardware().setPin(Clock, Low);
        Port::usleep(_clockDelay+3);
    }
    return ind;
}

void Usbpicprog::P18::send_bits(uint d, uint nbb)
{
    hardware().setWrite();
    for (int x = nbb; x; --x) {
        hardware().setPin(Clock, High);
        if ( d & 0x01 ) hardware().setPin(DataOut, High);
        else hardware().setPin(DataOut, Low);
        Port::usleep(_clockDelay+5);
        hardware().setPin(Clock, Low);
        Port::usleep(_clockDelay+3);
        d >>= 1;  // Move the data over 1 bit
    }
    hardware().setPin(DataOut, High);
    hardware().setRead();
}
