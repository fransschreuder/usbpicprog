/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_debug.h"

#include "common/common/misc.h"
#include "devices/pic/base/pic_register.h"
#include "progs/manager/debug_manager.h"

//----------------------------------------------------------------------------
Register::TypeData Debugger::PicBase::registerTypeData(const QString &name) const
{
  const Pic::RegistersData &rdata = device()->registersData();
  Q_ASSERT(rdata.sfrs.contains(name));
  return Register::TypeData(rdata.sfrs[name].address, rdata.nbChars());
}

bool Debugger::PicBase::updatePortStatus(uint index, QMap<uint, Device::PortBitData> &bits)
{
  const Pic::RegistersData &rdata = device()->registersData();
  uint tris = Device::INVALID;
  if ( rdata.hasTris(index) ) {
    tris = Register::list().value(registerTypeData(rdata.trisName(index)));
    Q_ASSERT( tris!=Device::INVALID );
  }
  uint port = Register::list().value(registerTypeData(rdata.portName(index)));
  Q_ASSERT( port!=Device::INVALID );
  uint latch = Device::INVALID;
  if ( rdata.hasLatch(index) ) {
    latch = Register::list().value(registerTypeData(rdata.latchName(index)));
    Q_ASSERT( latch!=Device::INVALID );
  }
  for (uint i=0; i<Device::MAX_NB_PORT_BITS; i++) {
    if ( !rdata.hasPortBit(index, i) ) continue;
    bits[i].state = Device::Unknown;
    bits[i].drivenState = Device::IoUnknown;
    bits[i].drivingState = Device::IoUnknown;
    if ( tris!=Device::INVALID ) {
      bits[i].driving = !(tris & (1 << i));
      if (bits[i].driving) {
        bits[i].drivenState = Device::IoUnknown;
        bits[i].drivingState = (port & (1 << i) ? Device::IoHigh : Device::IoLow);
      } else {
        bits[i].drivenState = (port & (1 << i) ? Device::IoHigh : Device::IoLow);
        if ( latch==Device::INVALID ) bits[i].drivingState = Device::IoUnknown;
        else bits[i].drivingState = (latch & (1 << i) ? Device::IoHigh : Device::IoLow);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------
Debugger::PicBase &Debugger::PicSpecific::base()
{
  return static_cast<PicBase &>(_base);
}
const Debugger::PicBase &Debugger::PicSpecific::base() const
{
  return static_cast<PicBase &>(_base);
}

bool Debugger::PicSpecific::updateStatus()
{
  if ( !Debugger::manager->readRegister(base().pcTypeData()) ) return false;
  if ( !Debugger::manager->readRegister(base().registerTypeData("STATUS")) ) return false;
  if ( !Debugger::manager->readRegister(wregTypeData()) ) return false;
  return true;
}

//----------------------------------------------------------------------------
Register::TypeData Debugger::P16FSpecific::wregTypeData() const
{
  return Register::TypeData("WREG", device().registersData().nbChars());
}

QString Debugger::P16FSpecific::statusString() const
{
  const Pic::RegistersData &rdata = device().registersData();
  uint status = Register::list().value(base().registerTypeData("STATUS"));
  uint bank = (status & 0x20 ? 1 : 0);
  bank += (status & 0x40 ? 2 : 0);
  uint wreg = Register::list().value(wregTypeData());
  return QString("W:%1   %2 %3 %4   PC:%5   Bank:%6")
         .arg(toHexLabel(wreg, rdata.nbChars())).arg(status & 0x4 ? "Z" : "z")
         .arg(status & 0x2 ? "DC" : "dc").arg(status & 0x1 ? "C" : "c")
         .arg(toHexLabel(_base.pc(), device().nbCharsAddress())).arg(bank);
}

//----------------------------------------------------------------------------
bool Debugger::P18FSpecific::updateStatus()
{
  if ( !PicSpecific::updateStatus() ) return false;
  if ( !Debugger::manager->readRegister(base().registerTypeData("BSR")) ) return false;
  return true;
}

Register::TypeData Debugger::P18FSpecific::wregTypeData() const
{
  return base().registerTypeData("WREG");
}

QString Debugger::P18FSpecific::statusString() const
{
  const Pic::RegistersData &rdata = device().registersData();
  uint status = Register::list().value(base().registerTypeData("STATUS"));
  uint bsr = Register::list().value(base().registerTypeData("BSR"));
  uint wreg = Register::list().value(wregTypeData());
  return QString("W:%1   %2 %3 %4 %5 %6  PC:%7   Bank:%8")
         .arg(toHexLabel(wreg, rdata.nbChars())).arg(status & 0x10 ? "N" : "n")
         .arg(status & 0x08 ? "OV" : "ov").arg(status & 0x04 ? "Z" : "z")
         .arg(status & 0x02 ? "DC" : "dc").arg(status & 0x01 ? "C" : "c")
         .arg(toHexLabel(base().pc(), device().nbCharsAddress())).arg(bsr);
}
