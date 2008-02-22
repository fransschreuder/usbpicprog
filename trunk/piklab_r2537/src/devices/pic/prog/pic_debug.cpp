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
  BitValue tris;
  if ( rdata.hasTris(index) ) {
    tris = Register::list().value(registerTypeData(rdata.trisName(index)));
    Q_ASSERT( tris.isInitialized() );
  }
  BitValue port = Register::list().value(registerTypeData(rdata.portName(index)));
  Q_ASSERT( port.isInitialized() );
  BitValue latch;
  if ( rdata.hasLatch(index) ) {
    latch = Register::list().value(registerTypeData(rdata.latchName(index)));
    Q_ASSERT( latch.isInitialized() );
  }
  for (uint i=0; i<Device::MAX_NB_PORT_BITS; i++) {
    if ( !rdata.hasPortBit(index, i) ) continue;
    bits[i].state = Device::Unknown;
    bits[i].drivenState = Device::IoUnknown;
    bits[i].drivingState = Device::IoUnknown;
    if ( tris.isInitialized() ) {
      bits[i].driving = !tris.bit(i);
      if (bits[i].driving) {
        bits[i].drivenState = Device::IoUnknown;
        bits[i].drivingState = (port.bit(i) ? Device::IoHigh : Device::IoLow);
      } else {
        bits[i].drivenState = (port.bit(i) ? Device::IoHigh : Device::IoLow);
        if ( latch.isInitialized() ) bits[i].drivingState = (latch.bit(i) ? Device::IoHigh : Device::IoLow);
        else bits[i].drivingState = Device::IoUnknown;
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
  BitValue status = Register::list().value(base().registerTypeData("STATUS"));
  uint bank = (status.bit(5) ? 1 : 0) + (status.bit(6) ? 2 : 0);
  BitValue wreg = Register::list().value(wregTypeData());
  return QString("W:%1   %2 %3 %4   PC:%5   Bank:%6")
         .arg(toHexLabel(wreg, rdata.nbChars())).arg(status.bit(2) ? "Z" : "z")
         .arg(status.bit(1) ? "DC" : "dc").arg(status.bit(0) ? "C" : "c")
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
  BitValue status = Register::list().value(base().registerTypeData("STATUS"));
  BitValue bsr = Register::list().value(base().registerTypeData("BSR"));
  BitValue wreg = Register::list().value(wregTypeData());
  return QString("W:%1   %2 %3 %4 %5 %6  PC:%7   Bank:%8")
         .arg(toHexLabel(wreg, rdata.nbChars())).arg(status.bit(4) ? "N" : "n")
         .arg(status.bit(3) ? "OV" : "ov").arg(status.bit(2) ? "Z" : "z")
         .arg(status.bit(1) ? "DC" : "dc").arg(status.bit(0) ? "C" : "c")
         .arg(toHexLabel(base().pc(), device().nbCharsAddress())).arg(toLabel(bsr));
}
