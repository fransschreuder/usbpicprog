/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_DEBUG_H
#define PIC_DEBUG_H

#include "progs/base/generic_debug.h"
#include "pic_prog.h"
#include "devices/base/register.h"

namespace Debugger
{
class PicBase;

//----------------------------------------------------------------------------
class PicSpecific : public DeviceSpecific
{
public:
  PicSpecific(Debugger::Base &base) : DeviceSpecific(base) {}
  const Pic::Data &device() const { return static_cast<const Pic::Data &>(*_base.device()); }
  PicBase &base();
  const PicBase &base() const;
  virtual bool updateStatus();
  virtual Register::TypeData wregTypeData() const = 0;
};

//----------------------------------------------------------------------------
class P16FSpecific : public PicSpecific
{
public:
  P16FSpecific(Debugger::Base &base) : PicSpecific(base) {}
  virtual QString statusString() const;
  virtual Register::TypeData wregTypeData() const;
};

//----------------------------------------------------------------------------
class P18FSpecific : public PicSpecific
{
public:
  P18FSpecific(Debugger::Base &base) : PicSpecific(base) {}
  virtual QString statusString() const;
  virtual bool updateStatus();
  virtual Register::TypeData wregTypeData() const;
};

//----------------------------------------------------------------------------
class PicBase : public Debugger::Base
{
public:
  PicBase(Programmer::PicBase &base) : Debugger::Base(base) {}
  PicSpecific *deviceSpecific() { return static_cast<PicSpecific *>(_deviceSpecific); }
  const PicSpecific *deviceSpecific() const { return static_cast<const PicSpecific *>(_deviceSpecific); }
  const Pic::Data *device() const { return static_cast<const Pic::Data *>(Debugger::Base::device()); }
  Register::TypeData registerTypeData(const QString &name) const;
  virtual bool updatePortStatus(uint index, QMap<uint, Device::PortBitData> &bits);
};

} // namespace

#endif
