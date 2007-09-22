/***************************************************************************
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef USBPICPROG_BASELINE_HH
#define USBPICPROG_BASELINE_HH

#include "usbpicprog_16F.h"

namespace Usbpicprog
{
//-----------------------------------------------------------------------------
class Baseline : public pic16
{
public:
  Baseline(::Programmer::Base &base) : pic16(base) {}
  virtual bool gotoMemory(Pic::MemoryRangeType type);
  virtual bool setPowerOn() { return setPowerOnVppFirst(); }
};

//-----------------------------------------------------------------------------
class P12C5XX : public Baseline
{
public:
  P12C5XX(::Programmer::Base &base) : Baseline(base) {}
  virtual bool writeWords(Pic::MemoryRangeType type, const Device::Array &data, uint i);
  virtual bool doEraseRange(Pic::MemoryRangeType) { return false; }
  virtual bool doErase(bool) { return false; }
  virtual uint waitProgTime(Pic::MemoryRangeType) const { return 0; } // unused
};

//-----------------------------------------------------------------------------
class P10F2XX : public Baseline
{
public:
  P10F2XX(::Programmer::Base &base) : Baseline(base) {}
  virtual bool doEraseRange(Pic::MemoryRangeType type);
  virtual bool doErase(bool);
  virtual uint waitProgTime(Pic::MemoryRangeType) const { return 2000; }
  virtual void endProg(Pic::MemoryRangeType) { pulseEngine("k14,w100"); }
};

class P16F57 : public P10F2XX
{
public:
  P16F57(::Programmer::Base &base) : P10F2XX(base) {}
  virtual uint nbWordsCodeProg() const { return 4; }
};

//-----------------------------------------------------------------------------
class P12C67X : public P16F
{
public:
  P12C67X(::Programmer::Base &base) : P16F(base) {}
  virtual bool writeWords(Pic::MemoryRangeType type, const Device::Array &data, uint i);
  virtual uint waitProgTime(Pic::MemoryRangeType) const { return 0; } // unused
  virtual bool doEraseRange(Pic::MemoryRangeType) { return false; }
  virtual bool doErase(bool) { return false; }
};

} // namespace

#endif
