/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "icd2_debug.h"

#include "common/global/pfile.h"
#include "progs/base/prog_config.h"
#include "devices/pic/pic/pic_debug.h"
#include "devices/pic/base/pic_register.h"
#include "devices/pic/pic/pic_group.h"
#include "icd2_data.h"
#include "icd2_debug_specific.h"

//-----------------------------------------------------------------------------
Icd2::DebuggerSpecific *Icd2::Debugger::specific() { return static_cast<Icd2::DebuggerSpecific *>(_specific); }

bool Icd2::Debugger::waitForTargetMode(Pic::TargetMode mode)
{
  Pic::TargetMode rmode;
  for (uint i=0; i<20; i++) {
    if ( !programmer().getTargetMode(rmode) ) return false;
    if ( rmode==mode ) return true;
    Port::msleep(200);
  }
  log(Log::Error, QString("Timeout waiting for mode: %1 (target is in mode: %2).")
                  .arg(i18n(Pic::TARGET_MODE_LABELS[mode])).arg(i18n(Pic::TARGET_MODE_LABELS[rmode])));
  return false;
}

bool Icd2::Debugger::init(bool last)
{
  _initLast = last;
  return ::Debugger::PicBase::init();
}

bool Icd2::Debugger::internalInit()
{
  return specific()->init(_initLast);
}

bool Icd2::Debugger::updateState()
{
  Pic::TargetMode mode;
  if ( !programmer().getTargetMode(mode) ) return false;
  switch (mode) {
    case Pic::TargetStopped: _programmer.setState(::Programmer::Halted); break;
    case Pic::TargetRunning: _programmer.setState(::Programmer::Running); break;
    case Pic::TargetInProgramming: _programmer.setState(::Programmer::Stopped); break;
    case Pic::Nb_TargetModes: Q_ASSERT(false); break;
  }
  return true;
}

bool Icd2::Debugger::setBreakpoints(const QValueList<uint> &addresses)
{
  if ( addresses.count()==0 ) return specific()->setBreakpoint(Device::INVALID);
  for (uint i=0; i<uint(addresses.count()); i++) {
    log(Log::NormalDebug, QString("Set breakpoint at %1").arg(toHexLabel(addresses[i], device()->nbCharsAddress())));
    if ( !specific()->setBreakpoint(addresses[i]) ) return false;
  }
  return true;
}

bool Icd2::Debugger::internalRun()
{
  return hardware()->resumeRun();
}

bool Icd2::Debugger::hardHalt()
{
  log(Log::Warning, i18n("Failed to halt target: try a reset."));
  return reset();
}

bool Icd2::Debugger::softHalt(bool &success)
{
  if ( !hardware()->haltRun() ) return false;
  success = waitForTargetMode(Pic::TargetStopped);
  return true;
}

bool Icd2::Debugger::internalReset()
{
  return specific()->reset();
}

bool Icd2::Debugger::internalStep()
{
  return hardware()->step();
}

bool Icd2::Debugger::readRegister(const Register::TypeData &data, uint &value)
{
  if ( data.type()==Register::Special ) {
    if ( data.name()=="WREG" ) return hardware()->readRegister(specific()->addressWREG(), value, 1);
    if ( data.name()=="PC" ) { value = hardware()->getProgramCounter() & specific()->maskPC(); return !hasError(); }
    Q_ASSERT(false);
    return true;
  }
  QString name = device()->registersData().sfrNames[data.address()];
  if ( name=="WREG" ) return hardware()->readRegister(specific()->addressWREG(), value, 1);
  if ( name=="PCL" ) { value = (hardware()->getProgramCounter() & specific()->maskPC()) && 0xFF; return !hasError(); }
  if ( name=="PCLATH" ) { value = (hardware()->getProgramCounter() & specific()->maskPC()) >> 8; return !hasError(); }
  return hardware()->readRegister(specific()->addressRegister(data.address()), value, 1);
}

bool Icd2::Debugger::writeRegister(const Register::TypeData &data, uint value)
{
  if ( data.type()==Register::Special ) {
    if ( data.name()=="WREG" ) return hardware()->writeRegister(specific()->addressWREG(), value, 1);
    Q_ASSERT(false);
    return true;
  }
  QString name = device()->registersData().sfrNames[data.address()];
  if ( name=="WREG" ) return hardware()->writeRegister(specific()->addressWREG(), value, 1);
  return hardware()->writeRegister(specific()->addressRegister(data.address()), value, 1);
}

//-----------------------------------------------------------------------------
Icd2::DebugProgrammer::DebugProgrammer(const ::Programmer::Group &group, const Pic::Data *data)
  : Icd2::ProgrammerBase(group, data, "icd2_programmer")
{}

void Icd2::DebugProgrammer::clear()
{
  Icd2::ProgrammerBase::clear();
  _debugExecutiveVersion.clear();
}

bool Icd2::DebugProgrammer::internalSetupHardware()
{
  if ( _specific==0 ) return true;
  const FamilyData &fdata = FAMILY_DATA[family(device()->name())];

  // find debug executive file
  ::Programmer::GroupConfig config(group());
  PURL::Directory dir(config.firmwareDirectory());
  if ( !dir.probablyExists() ) {
    log(Log::Error, i18n("Firmware directory not configured."));
    return false;
  }
  uint reservedBank = 0;
  QString filename;
  if ( device()->is18Family() ) {
    Debugger *debug = static_cast<Debugger *>(debugger());
    reservedBank = static_cast<const P18FDebuggerSpecific *>(debug->specific())->reservedBank();
    filename = QString("de18F_BANK%1.hex").arg(QString(toString(Number::Dec, reservedBank, 2)));
  } else filename = QString("de%1.hex").arg(fdata.debugExec);
  PURL::Url url = dir.findMatchingFilename(filename);
  log(Log::NormalDebug, QString("  Debug executive file: %1").arg(url.pretty()));
  if ( !url.probablyExists() ) {
    log(Log::Error, i18n("Could not find debug executive file \"%1\".").arg(url.pretty()));
    return false;
  }
  // upload hex file
  PURL::File file(url, 0);
  if ( !file.openForRead() ) {
    log(Log::Error, i18n("Could not open firmware file \"%1\".").arg(url.pretty()));
    return false;
  }
  QStringList errors;
  HexBuffer hbuffer;
  if ( !hbuffer.load(file.stream(), errors) ) {
    log(Log::Error, i18n("Could not read debug executive file \"%1\": %2.").arg(url.pretty()).arg(errors[0]));
    return false;
  }
  uint nbWords = device()->nbWords(Pic::MemoryCode);
  uint offset = nbWords - 0x100;
  if ( fdata.debugExecOffset!=0 && fdata.debugExecOffset!=offset )
    for (uint i=0; i<0x100; i++) hbuffer.set(offset+i, hbuffer[fdata.debugExecOffset+i]);
  Pic::Memory::WarningTypes warningTypes;
  QStringList warnings;
  QMap<uint, bool> inRange;
  Pic::Memory memory(*device());
  memory.fromHexBuffer(Pic::MemoryCode, hbuffer, warningTypes, warnings, inRange);
  _deArray = memory.arrayForWriting(Pic::MemoryCode);
  if ( device()->is18Family() ) {
    // that's a bit ugly but it cannot be guessed for 18F2455 family...
    uint size;
    switch (reservedBank) {
      case 0:  size = 0x0E0; break;
      case 12:
      case 14: size = 0x140; break;
      default: size = 0x120; break;
    }
    _deStart = nbWords - size;
    _deEnd = nbWords - 1;
    for (uint i=0; i<=size; i++) {
      uint v = memory.word(Pic::MemoryCode, i);
      memory.setWord(Pic::MemoryCode, i, Device::INVALID);
      memory.setWord(Pic::MemoryCode, _deStart+i, v);
    }
    _deArray = memory.arrayForWriting(Pic::MemoryCode);
  } else {
    _deStart = specific()->findNonMaskStart(Pic::MemoryCode, _deArray);
    _deEnd = specific()->findNonMaskEnd(Pic::MemoryCode, _deArray);
  }
  log(Log::ExtraDebug, QString("debug executive: \"%1\" %2:%3").arg(url.pretty()).arg(toHexLabel(_deStart, 4)).arg(toHexLabel(_deEnd, 4)));

  return Icd2::ProgrammerBase::internalSetupHardware();
}

Pic::Memory Icd2::DebugProgrammer::toDebugMemory(const Pic::Memory &mem, bool withDebugExecutive)
{
  Pic::Memory memory = mem;
  memory.setDebugOn(true);
  if ( memory.hasWatchdogTimerOn() ) {
    log(Log::Warning, i18n("Disabling watchdog timer for debugging"));
    memory.setWatchdogTimerOn(false);
  }
  if ( memory.isProtected(Pic::Protection::ProgramProtected, Pic::MemoryCode) ) {
    log(Log::Warning, i18n("Disabling code program protection for debugging"));
    memory.setProtection(false, Pic::Protection::ProgramProtected, Pic::MemoryCode);
  }
  if ( memory.isProtected(Pic::Protection::WriteProtected, Pic::MemoryCode) ) {
    log(Log::Warning, i18n("Disabling code write protection for debugging"));
    memory.setProtection(false, Pic::Protection::WriteProtected, Pic::MemoryCode);
  }
  if ( memory.isProtected(Pic::Protection::ReadProtected, Pic::MemoryCode) ) {
    log(Log::Warning, i18n("Disabling code read protection for debugging"));
    memory.setProtection(false, Pic::Protection::ReadProtected, Pic::MemoryCode);
  }
  uint address = _deStart * device()->addressIncrement(Pic::MemoryCode);
  Device::Array data = device()->gotoInstruction(address, false);
  for (uint i=0; i<data.count(); i++) memory.setWord(Pic::MemoryDebugVector, i, data[i]);
  if ( device()->is18Family() )
    memory.setWord(Pic::MemoryDebugVector, data.count(), 0xFF00); // ??
  if (withDebugExecutive) {
    bool ok = true;
    for (uint i=_deStart; i<=_deEnd; i++) {
      if ( memory.word(Pic::MemoryCode, i)!=Device::INVALID ) ok = false;
      memory.setWord(Pic::MemoryCode, i, _deArray[i]);
    }
    if ( !ok ) log(Log::Warning, i18n("Memory area for debug executive was not empty. Overwrite it and continue anyway..."));
  }
  return memory;
}

bool Icd2::DebugProgrammer::writeDebugExecutive()
{
  log(Log::Information, i18n("  Write debug executive"));
  uint nbWords = _deEnd - _deStart + 1;
  if ( !hardware()->writeMemory(Pic::MemoryCode, _deArray, _deStart, nbWords) ) return false;
  log(Log::Information, i18n("  Verify debug executive"));
  Device::Array data;
  if ( !hardware()->readMemory(Pic::MemoryCode, _deStart, nbWords, data, 0) ) return false;
  for (uint i=0; i<nbWords; i++) {
    if ( _deArray[_deStart+i]==data[i] ) continue;
    uint inc = device()->addressIncrement(Pic::MemoryCode);
    uint address = device()->range(Pic::MemoryCode).start + inc * (_deStart + i);
    log(Log::Error, i18n("Device memory doesn't match debug executive (at address %1: reading %2 and expecting %3).")
                    .arg(toHexLabel(address, device()->nbCharsAddress()))
                    .arg(toHexLabel(data[i], device()->nbCharsWord(Pic::MemoryCode)))
                    .arg(toHexLabel(_deArray[_deStart+i], device()->nbCharsWord(Pic::MemoryCode))));
    return false;
  }
  return true;
}

bool Icd2::DebugProgrammer::doProgram(const Device::Memory &memory, const Device::MemoryRange &range)
{
  if ( !checkProgram(memory) ) return false;
  if ( !doConnectDevice() ) return false;
   // probably needed for all devices that don't have a "erase and write" mode
  if ( range.all() && FAMILY_DATA[family(device()->name())].debugExec==QString("16F7X7") ) {
     Pic::Memory dmemory(*device());
     dmemory.setWord(Pic::MemoryCode, 0, 0x0028);
     dmemory.setWord(Pic::MemoryCode, 1, 0x0030);
     log(Log::Information, i18n("Programming device for debugging test..."));
     if ( !internalProgram(dmemory, range) ) return false;
     if ( !static_cast<Debugger *>(_debugger)->init(false) ) return false;
     log(Log::Information, i18n("Debugging test successful"));
  }
  log(Log::Information, i18n("Programming device memory..."));
  if ( !internalProgram(memory, range) ) return false;
  log(Log::Information, i18n("Programming successful"));
  return static_cast<Debugger *>(_debugger)->init(true);
}

bool Icd2::DebugProgrammer::programAll(const Pic::Memory &mem)
{
  Pic::Memory memory = toDebugMemory(mem, false);
  if ( !programAndVerifyRange(Pic::MemoryCode, memory) ) return false;
  if ( !writeDebugExecutive() ) return false;
  if ( !programAndVerifyRange(Pic::MemoryDebugVector, memory) ) return false;
  if ( !programAndVerifyRange(Pic::MemoryEeprom, memory) ) return false;
  if ( !programAndVerifyRange(Pic::MemoryUserId, memory) ) return false;
  if ( device()->is18Family() ) {
    if ( !hardware()->command("0C00", 0) ) return false; // #### ??
    QString com = "42" + toHex(0xFB5, 8) + toHex(1, 8); // write RSBUG (?)
    if ( !hardware()->command(com, 0) ) return false;
    if ( !hardware()->command("0C01", 0) ) return false; // #### ??
  }
  if ( !programAndVerifyRange(Pic::MemoryConfig, memory) ) return false;
  return true;
}

bool Icd2::DebugProgrammer::internalRead(Device::Memory *mem, const Device::MemoryRange &range, const ::Programmer::VerifyData *vd)
{
  if ( vd==0 || (vd->actions & ::Programmer::BlankCheckVerify) )
    return Icd2::ProgrammerBase::internalRead(mem, range, 0);
  Pic::Memory memory = toDebugMemory(static_cast<const Pic::Memory &>(vd->memory), true);
  ::Programmer::VerifyData vdata(vd->actions, memory);
  if ( !Icd2::ProgrammerBase::internalRead(0, range, &vdata) ) return false;
  if ( range.all() && !readRange(Pic::MemoryDebugVector, 0, &vdata) ) return false;
  return true;
}

bool Icd2::DebugProgrammer::readDebugExecutiveVersion()
{
  if ( !hardware()->getDebugExecVersion(_debugExecutiveVersion) ) return false;
  log(Log::Information, i18n("  Debug executive version: %1").arg(_debugExecutiveVersion.pretty()));
  return true;
}

//----------------------------------------------------------------------------
::Group::SupportType Icd2::DebuggerGroup::supportType(const QString &device) const
{
  if ( !isSupported(device) ) return ::Group::NotSupported;
  return ::Group::SupportType(data(device).debugSupportType);
}

void Icd2::DebuggerGroup::addDevice(const ::Group::DeviceData &data)
{
  if ( FAMILY_DATA[family(data.data->name())].debugExec==0 ) return;
  Group::addDevice(data);
}

::Debugger::Specific *Icd2::DebuggerGroup::createDebuggerSpecific(::Debugger::Base &base) const
{
  const Pic::Data *data = static_cast< ::Debugger::PicBase &>(base).device();
  if ( data==0 ) return 0;
  QString debugExec = FAMILY_DATA[family(data->name())].debugExec;
  switch (data->architecture()) {
    case Pic::Arch_16X:
      if ( debugExec=="16F872" ) return new P16F872DebuggerSpecific(base);
      if ( debugExec=="16F7X7" ) return new P16F7X7DebuggerSpecific(base);
      return new P16F87XDebuggerSpecific(base);
    case Pic::Arch_17X:
    case Pic::Arch_18C:
    case Pic::Arch_18F:
    case Pic::Arch_18J: return new P18FDebuggerSpecific(base);
    case Pic::Arch_10X:
    case Pic::Arch_24F:
    case Pic::Arch_24H:
    case Pic::Arch_30X:
    case Pic::Nb_Architectures: break;
  }
  Q_ASSERT(false);
  return 0;
}
