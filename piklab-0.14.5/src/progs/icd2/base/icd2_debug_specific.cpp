/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "icd2_debug_specific.h"

#include "devices/pic/base/pic_register.h"
#include "icd2_data.h"

//----------------------------------------------------------------------------
uint Icd2::P16FDebuggerSpecific::addressWREG() const
{
  return FAMILY_DATA[family(device()->name())].wreg;
}

uint Icd2::P16FDebuggerSpecific::addressRegister(uint address) const
{
  QString name = device()->registersData().sfrNames[address];
  if ( name=="FSR" ) return FAMILY_DATA[family(device()->name())].fsr;
  if ( name=="STATUS" ) return FAMILY_DATA[family(device()->name())].status;
  return address;
}

bool Icd2::P16FDebuggerSpecific::reset()
{
  if ( !hardware()->setTargetReset(Pic::ResetHeld) ) return false;
  return init(true);
}

bool Icd2::P16FDebuggerSpecific::setBreakpoint(uint address)
{
  if ( address==Device::INVALID ) address = 0x1FFF; // outside memory range
  uint value = address | 0x8000; // read-only INBUG bit
  return hardware()->writeRegister(0x18E, value, 2);
}

bool Icd2::P16FDebuggerSpecific::readBreakpoint(uint &value)
{
  if ( !hardware()->readRegister(0x18E, value, 2) ) return false;
  value &= 0x1FFF;
  return true;
}

bool Icd2::P16FDebuggerSpecific::beginInit(Device::Array *saved)
{
  if ( !hardware()->setTargetReset(Pic::ResetHeld) ) return false;
  double vdd;
  if ( !hardware()->readVoltage(Pic::TargetVdd, vdd) ) return false;
  log(Log::NormalDebug, QString("  Target Vdd: %1 V").arg(vdd));

  if (saved) {
    if ( !hardware()->readMemory(Pic::MemoryCode, 0, 1, *saved, 0) ) return false; // save first instruction
    if ( (*saved)!=device()->nopInstruction() ) log(Log::Warning, i18n("  According to ICD2 manual, instruction at address 0x0 should be \"nop\"."));
  }

  return true;
}

bool Icd2::P16FDebuggerSpecific::endInit(uint expectedPC)
{
  if ( !hardware()->setTargetReset(Pic::ResetReleased) ) return false;
  if ( !base().waitForTargetMode(Pic::TargetStopped) ) return false;
  uint value;
  if ( !readBreakpoint(value) ) return false;
  if ( value==expectedPC+1 ) {
    // #### happen for some custom icd2 or sometimes when we had to force a halt (?)
    expectedPC = expectedPC+1;
    //log(Log::Information, i18n("Detected custom ICD2"));
  }
  if ( value!=expectedPC ) {
    log(Log::Error, i18n("  PC is not at address %1 (%2)").arg(toHexLabel(expectedPC, 4)).arg(toHexLabel(value, 4)));
    return false;
  }
  if ( !setBreakpoint(0x0000) ) return false;

  if ( !base().update() ) return false;
  if ( base().pc()!=expectedPC ) {
    log(Log::Error, i18n("  PC is not at address %1 (%2)").arg(toHexLabel(expectedPC, 4)).arg(toHexLabel(base().pc(), 4)));
    return false;
  }
  return true;
}

bool Icd2::P16F872DebuggerSpecific::init(bool)
{
  Device::Array saved;
  if ( !beginInit(&saved) ) return false;

  // this seems to be needed
  if ( !hardware()->setTargetReset(Pic::ResetReleased) ) return false;
  Pic::TargetMode mode;
  if ( !programmer().getTargetMode(mode) ) return false;

  if ( !hardware()->setTargetReset(Pic::ResetHeld) ) return false;
  if ( !hardware()->setWriteMode(Pic::EraseWriteMode) ) return false;
  log(Log::NormalDebug, "  Write \"goto 0x0\" at reset vector and run target.");
  Device::Array data = device()->gotoInstruction(0x0000, false); // loop at reset vector
  if ( !hardware()->writeMemory(Pic::MemoryCode, data, 0, data.count()) ) return false;
  if ( !hardware()->setTargetReset(Pic::ResetReleased) ) return false; // run device
  if ( !base().waitForTargetMode(Pic::TargetRunning) ) return false;
  log(Log::NormalDebug, "  Try to halt target.");
  if ( !hardware()->haltRun() ) return false;
  if ( !base().waitForTargetMode(Pic::TargetStopped) ) return false;
  if ( !programmer().readDebugExecutiveVersion() ) return false;
  log(Log::NormalDebug, "  Set breakpoint at reset vector.");
  if ( !setBreakpoint(0x0000) ) return false;
  if ( !hardware()->setTargetReset(Pic::ResetHeld) ) return false;
  if ( !base().waitForTargetMode(Pic::TargetInProgramming) ) return false;
  log(Log::NormalDebug, "  Restore instruction at reset vector and run target.");
  if ( !hardware()->writeMemory(Pic::MemoryCode, saved, 0, saved.count()) ) return false; // restore instruction
  if ( !hardware()->setWriteMode(Pic::WriteOnlyMode) ) return false;

  return endInit(0x0001);
}

bool Icd2::P16F87XDebuggerSpecific::init(bool)
{
  Device::Array saved;
  if ( !beginInit(&saved) ) return false;

  if ( !hardware()->setTargetReset(Pic::ResetHeld) ) return false;
  if ( !hardware()->setWriteMode(Pic::EraseWriteMode) ) return false;
  log(Log::NormalDebug, "  Write \"goto 0x0\" at reset vector and run target.");
  Device::Array data = device()->gotoInstruction(0x0000, false); // loop at reset vector
  if ( !hardware()->writeMemory(Pic::MemoryCode, data, 0, data.count()) ) return false;
  if ( !hardware()->setTargetReset(Pic::ResetReleased) ) return false; // run device
  Pic::TargetMode mode;
  if ( !programmer().getTargetMode(mode) ) return false;
  if ( mode==Pic::TargetRunning && !hardware()->haltRun() ) return false;
  if ( !base().waitForTargetMode(Pic::TargetStopped) ) return false;
  if ( !programmer().readDebugExecutiveVersion() ) return false;
  log(Log::NormalDebug, "  Set breakpoint at reset vector.");
  if ( !setBreakpoint(0x0000) ) return false;
  if ( !hardware()->setTargetReset(Pic::ResetHeld) ) return false;
  if ( !base().waitForTargetMode(Pic::TargetInProgramming) ) return false;
  log(Log::NormalDebug, "  Restore instruction at reset vector and run target.");
  if ( !hardware()->writeMemory(Pic::MemoryCode, saved, 0, saved.count()) ) return false; // restore instruction
  if ( !hardware()->setWriteMode(Pic::WriteOnlyMode) ) return false;

  return endInit(0x0001);
}

bool Icd2::P16F7X7DebuggerSpecific::init(bool last)
{
  Device::Array saved;
  if ( !beginInit(last ? &saved : 0) ) return false;

  log(Log::NormalDebug, "  Run target.");
  if ( !hardware()->setTargetReset(Pic::ResetReleased) ) return false;
  if ( !base().waitForTargetMode(Pic::TargetStopped) ) return false;
  if ( !programmer().readDebugExecutiveVersion() ) return false;

  uint value;
  if ( !readBreakpoint(value) ) return false;
  uint expectedPC = (last ? 0x0001 : 0x0000);
  if ( value==expectedPC+1 ) {
    expectedPC = expectedPC+1;
    log(Log::NormalDebug, "Probably detected custom ICD2");
  }
  if ( value!=expectedPC )
    log(Log::NormalDebug, i18n("  PC is not at address %1 (%2)").arg(toHexLabel(expectedPC, 4)).arg(toHexLabel(value, 4)));
  if ( !setBreakpoint(0x0000) ) return false;

  if ( !base().update() ) return false;
  // #### not sure if there is a better way to get initial values (we are stopped here...)
  Register::list().setValue(base().pcTypeData(), value);
  Register::list().setValue(base().registerTypeData("STATUS"), 0x0);
  Register::list().setValue(deviceSpecific()->wregTypeData(), 0x0);

  return true;
}

//----------------------------------------------------------------------------
Icd2::P18FDebuggerSpecific::P18FDebuggerSpecific(::Debugger::Base &base)
  : DebuggerSpecific(base)
{
  const Pic::RegistersData &rdata = device()->registersData();
  // find last used bank (but not #15)
  _reservedBank = rdata.nbBanks - 1; // #14
  for (uint i=1; i<rdata.nbBanks-1; i++) {
    if ( rdata.isBankUsed(i) ) continue;
    _reservedBank = i - 1;
    break;
  }
  // check it's not a bank for CAN
  for (; _reservedBank>0; _reservedBank--)
    if ( !rdata.bankHasSfrs(_reservedBank) ) break;
  // also take care of USB RAM
  if ( (device()->architecture()==Pic::Arch_18F || device()->architecture()==Pic::Arch_18J) && device()->hasUSB() ) {
    if ( _reservedBank==7 ) _reservedBank = 3; // 18F2455 family: 4 USB RAM banks
    // 18F87J50 family ?
  }
}

uint Icd2::P18FDebuggerSpecific::addressWREG()const
{
  return reservedRegisterOffset() | 0x0FF;
}

uint Icd2::P18FDebuggerSpecific::addressRegister(uint address) const
{
  QString name = device()->registersData().sfrNames[address];
  if ( name=="PCLATU" ) return reservedRegisterOffset() | 0x0F4;
  if ( name=="PCLATH" ) return reservedRegisterOffset() | 0x0F5;
  if ( name=="FSR0H" ) return reservedRegisterOffset() | 0x0FB;
  if ( name=="FSR0L" ) return reservedRegisterOffset() | 0x0FC;
  if ( name=="BSR" ) return reservedRegisterOffset() | 0x0FD;
  if ( name=="STATUS" ) return reservedRegisterOffset() | 0x0FE;
  return address;
}

bool Icd2::P18FDebuggerSpecific::setBreakpoint(uint address)
{
  uint value = (address==Device::INVALID ? 0x0FFFFF00 : address << 15); // ??
  return hardware()->writeRegister(0xFB6, value, 4);
}

bool Icd2::P18FDebuggerSpecific::readBreakpoint(uint &value)
{
  return hardware()->readRegister(0xFB6, value, 4) >> 15;
}

bool Icd2::P18FDebuggerSpecific::reset()
{
  if ( !hardware()->writeRegister(0xFB5, 0x00, 1) ) return false; // #### ??
  if ( !hardware()->writeRegister(0xFB5, 0x01, 1) ) return false; // #### ??
  if ( !hardware()->command("2D", 0) ) return false; // reset
  if ( !hardware()->writeRegister(0xFB5, 0x00, 1) ) return false; // #### ??
  if ( !base().update() ) return false;
  uint expectedPC = 0x0000;
  if ( base().pc()==0x0001) {
    expectedPC = 0x0001;
    log(Log::Information, i18n("Detected custom ICD2"));
  }
  if ( base().pc()!=expectedPC ) {
    log(Log::Error, i18n("  PC is not at address %1 (%2)").arg(toHexLabel(expectedPC, 4)).arg(toHexLabel(base().pc(), 4)));
    return false;
  }
  return true;
}

bool Icd2::P18FDebuggerSpecific::init(bool)
{
  if ( !hardware()->setTargetReset(Pic::ResetReleased) ) return false;
  if ( !base().waitForTargetMode(Pic::TargetStopped) ) return false;
  return reset();
}
