/***************************************************************************
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DIRECT_18_HH
#define DIRECT_18_HH

#include "direct_pic.h"

namespace Direct
{

class P18 : public Pic8DeviceSpecific
{
public:
  P18(::Programmer::Base &base) : Pic8DeviceSpecific(base) {}
  virtual bool setPowerOn() { return setPowerOnVddFirst(); }

protected:
  bool pulse(const char *&cmd, uint value, uint &res, bool &ok);
  virtual void send_word(uint d);
  virtual uint get_word();
  virtual uint get_byte();
  virtual void send_cmd(uint d) { send_bits(d, 4); }
  virtual void send_bits(uint d, uint nbBits);
};

} //namespace

#endif
