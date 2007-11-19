/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ICD2_DEBUG_SPECIFIC_H
#define ICD2_DEBUG_SPECIFIC_H

#include "icd2_debug.h"

namespace Icd2
{
//-----------------------------------------------------------------------------
class DebuggerSpecific : public ::Debugger::Specific
{
public:
  DebuggerSpecific(::Debugger::Base &base) : ::Debugger::Specific(base) {}
  Debugger &base() { return static_cast<Debugger &>(_base); }
  const Debugger &base() const { return static_cast<const Debugger &>(_base); }
  const Pic::Data *device() const { return base().device(); }
  Hardware *hardware() { return base().programmer().hardware(); }
  DebugProgrammer &programmer() { return base().programmer(); }
  ::Debugger::PicSpecific *deviceSpecific() { return base().deviceSpecific(); }
  virtual Address addressWREG() const = 0;
  virtual BitValue maskPC() const = 0;
  virtual Address addressRegister(Address address) const = 0;
  virtual bool setBreakpoint(Address address) = 0;
  virtual bool readBreakpoint(BitValue &value) = 0;
  virtual bool init(bool last) = 0;
  virtual bool reset() = 0;
};

//-----------------------------------------------------------------------------
class P16FDebuggerSpecific : public DebuggerSpecific
{
public:
  P16FDebuggerSpecific(::Debugger::Base &base) : DebuggerSpecific(base) {}
  virtual Address addressBreakpointRegister() const { return 0x18E; }
  virtual BitValue writeMaskBreakpointRegister() const { return 0x8000; }
  virtual BitValue readMaskBreakpointRegister() const { return 0x1FFF; }
  virtual Address addressWREG() const;
  virtual BitValue maskPC() const { return 0x1FFF; }
  virtual Address addressRegister(Address address) const;
  virtual bool setBreakpoint(Address address);
  virtual bool readBreakpoint(BitValue &value);
  virtual bool reset();

protected:
  bool beginInit(Device::Array *saved);
  bool endInit(BitValue expectedPC);
};

class P16F872DebuggerSpecific : public P16FDebuggerSpecific
{
public:
  P16F872DebuggerSpecific(::Debugger::Base &base) : P16FDebuggerSpecific(base) {}
  virtual bool init(bool last);
};

class P16F87XDebuggerSpecific : public P16FDebuggerSpecific
{
public:
  P16F87XDebuggerSpecific(::Debugger::Base &base) : P16FDebuggerSpecific(base) {}
  virtual bool init(bool last);
};

class P16F7X7DebuggerSpecific : public P16FDebuggerSpecific
{
public:
  P16F7X7DebuggerSpecific(::Debugger::Base &base) : P16FDebuggerSpecific(base) {}
  virtual bool init(bool last);
};

//-----------------------------------------------------------------------------
class P18FDebuggerSpecific : public DebuggerSpecific
{
public:
  P18FDebuggerSpecific(::Debugger::Base &base);
  virtual Address addressWREG() const;
  virtual BitValue maskPC() const { return 0xFFFF; }
  virtual Address addressRegister(Address address) const;
  virtual bool setBreakpoint(Address address);
  virtual bool readBreakpoint(BitValue &value);
  virtual bool init(bool last);
  virtual bool reset();
  uint reservedBank() const { return _reservedBank; }

private:
  uint _reservedBank; // bank where are the debugging sfrs
  uint reservedRegisterOffset() const { return reservedBank() << 8; }
};

} // namespace

#endif
