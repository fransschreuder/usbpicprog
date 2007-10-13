/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICKIT1_H
#define PICKIT1_H

#include "progs/pickit2/base/pickit.h"
#include "pickit1_data.h"

namespace Pickit1
{
//-----------------------------------------------------------------------------
class Array : public Pickit::Array
{
public:
  Array() : Pickit::Array(8, 'Z', PrintAlphaNum) {}
};

//-----------------------------------------------------------------------------
class USBPort : public Pickit::USBPort
{
public:
  USBPort(Log::Base &log) : Pickit::USBPort(0x0032, log) {}
  virtual Pickit::Array array() const { return Array(); }

private:
  virtual uint readEndPoint() const { return 0x81; }
  virtual uint writeEndPoint() const { return 0x01; }
};

//-----------------------------------------------------------------------------
class Hardware : public Pickit::Hardware
{
public:
  Hardware(::Programmer::Base &base) : Pickit::Hardware(base, new USBPort(base)) {}
};

//----------------------------------------------------------------------------
class Baseline : public Pickit::Baseline
{
public:
  Baseline(::Programmer::Base &base) : Pickit::Baseline(base) {}
  virtual char entryMode() const { return data(device().name()).entryMode; }
  virtual bool init();
  virtual uint nbWrites(Pic::MemoryRangeType) const { return 1; }
  virtual bool incrementPC(uint nb);
};

//----------------------------------------------------------------------------
class P16F : public Pickit::P16F
{
public:
  P16F(::Programmer::Base &base) : Pickit::P16F(base) {}
  virtual char entryMode() const { return data(device().name()).entryMode; }
  virtual bool init();
  virtual uint nbWrites(Pic::MemoryRangeType) const { return 1; }
};

class P16F716 : public P16F
{
public:
  P16F716(::Programmer::Base &base) : P16F(base) {}
  virtual bool init();
};

} // namespace

#endif
