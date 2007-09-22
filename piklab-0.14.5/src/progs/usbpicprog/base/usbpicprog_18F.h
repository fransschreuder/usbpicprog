/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef USBPICPROG_18F_H
#define USBPICPROG_18F_H

#include "usbpicprog_18.h"

namespace Usbpicprog
{
//-----------------------------------------------------------------------------
class P18F : public P18
{
public:
  P18F(::Programmer::Base &base) : P18(base) {}
  virtual bool doRead(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata);
  virtual bool doWrite(Pic::MemoryRangeType, const Device::Array &data, bool force);
  virtual bool doEraseCommand(uint cmd1, uint cmd2);
  virtual bool doEraseRange(Pic::MemoryRangeType type);
  virtual bool doErase(bool isProtected);

  bool skipMaskWords(Pic::MemoryRangeType type, const Device::Array &data,
                     uint &i, uint nb, bool force);
  void setPointer(Pic::MemoryRangeType type, uint offset);
  void setCodePointer(uint address);
  enum Type { Code, Eeprom, Erase };
  virtual void program(Type type);
  virtual uint programHighTime(Type type) const { return (type==Code ? 1000 : 5000); }
  virtual uint programLowTime() { return 5; }
  virtual void configureSinglePanel() {}
  virtual void unlockEeprom();
  virtual void directAccess(Pic::MemoryRangeType type);
};

//-----------------------------------------------------------------------------
class P18F1220 : public P18F
{
public:
  P18F1220(::Programmer::Base &base) : P18F(base) {}
  virtual void program(Type type);
};

//-----------------------------------------------------------------------------
class P18F242 : public P18F
{
public:
  P18F242(::Programmer::Base &base) : P18F(base) {}
  virtual void configureSinglePanel();
};

//-----------------------------------------------------------------------------
class P18F2539 : public P18F242
{
public:
  P18F2539(::Programmer::Base &base) : P18F242(base) {}
  virtual bool doErase(bool isProtected);
  virtual bool doEraseCommand(uint cmd1, uint cmd2);
};

//-----------------------------------------------------------------------------
class P18F2439 : public P18F2539
{
public:
  P18F2439(::Programmer::Base &base) : P18F2539(base) {}
  virtual bool doEraseRange(Pic::MemoryRangeType type);
};

//-----------------------------------------------------------------------------
class P18F2221 : public P18F
{
public:
  P18F2221(::Programmer::Base &base) : P18F(base) {}
  virtual bool doEraseRange(Pic::MemoryRangeType type);
  virtual bool doErase(bool isProtected);
  virtual uint programLowTime() { return 100; }
  virtual void unlockEeprom() {}
};

//-----------------------------------------------------------------------------
class P18F6527 : public P18F2221
{
public:
  P18F6527(::Programmer::Base &base) : P18F2221(base) {}
  virtual bool doErase(bool isProtected);
};

//-----------------------------------------------------------------------------
class P18F6310 : public P18F
{
public:
  P18F6310(::Programmer::Base &base) : P18F(base) {}
  virtual bool canEraseRange(Pic::MemoryRangeType) const { return false; }
  virtual uint programHighTime(Type type) const { return (type==Code ? 2000 : 30000); }
  virtual uint programLowTime() { return 120; }
  virtual bool doEraseRange(Pic::MemoryRangeType) { return false; }
  virtual bool doErase(bool isProtected);
};

} // namespace

#endif
