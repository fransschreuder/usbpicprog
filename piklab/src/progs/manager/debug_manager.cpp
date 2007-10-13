/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "debug_manager.h"

#include <qtimer.h>

#include "coff/base/text_coff.h"
#include "progs/base/generic_prog.h"
#include "progs/base/generic_debug.h"
#include "progs/base/prog_group.h"
#include "devices/base/register.h"

Debugger::Manager *Debugger::manager = 0;

Debugger::Manager::Manager()
  :  QObject(Programmer::manager, "debug_manager"), Log::Base(Programmer::manager), GenericView(Breakpoint::list()),
     _coff(0), _data(0)
{
  connect(&_timer, SIGNAL(timeout()), SLOT(slotTimeout()));
}

Debugger::Manager::~Manager()
{
  delete _coff;
}

bool Debugger::Manager::isActive() const
{
  return (programmer() ? programmer()->state()==Programmer::Halted : false);
}

Debugger::Base *Debugger::Manager::debugger() const
{
  return (programmer() ? programmer()->debugger() : 0);
}

void Debugger::Manager::updateDevice()
{
  const Device::Data *data = deviceData();
  if ( data==_data ) return;
  _data = data;
  Register::list().clearWatched();
  clear();
}

bool Debugger::Manager::checkState(bool *first)
{
  if ( programmer()->state()==Programmer::NotConnected ) {
    if ( !prepareDebugging() ) return false;
    if ( !programmer()->connectHardware() ) return false;
  }
  if ( debugger()->hasError() || programmer()->state()==Programmer::Stopped ) {
    if (first) (*first) = true;
    log(Log::Data, "--------------------------------------------------");
    if ( !programmer()->debugger()->init() ) return false;
  } else if (first) *first = false;
  return true;
}

bool Debugger::Manager::init()
{
  if ( !internalInit() ) return false;
  return update();
}

bool Debugger::Manager::internalInit()
{
  delete _coff;
  _coff = 0;
  if ( !coffUrl().exists() ) return false;
  Log::Base log;
  log.setView(compileView());
  _coff = new Coff::TextObject(*_data, coffUrl(), log);
  if ( hasError() ) {
    delete _coff;
    _coff = 0;
    return false;
  }
  computeBreakpointAddresses();
  return true;
}

void Debugger::Manager::clearBreakpoints()
{
  Breakpoint::list().clear();
}

void Debugger::Manager::freeActiveBreakpoint()
{
  uint nb = 0;
  Breakpoint::Data last;
  for (uint i=0; i<Breakpoint::list().count(); i++) {
    const Breakpoint::Data &data = Breakpoint::list().data(i);
    if ( Breakpoint::list().state(data)!=Breakpoint::Active ) continue;
    nb++;
    last = data;
  }
  uint max = programmerGroup()->maxNbBreakpoints(deviceData());
  Q_ASSERT( nb<=max && max!=0 );
  if ( nb==max ) {
    log(Log::Warning, i18n("The number of active breakpoints is higher than the maximum for the current debugger (%1): disabling the last breakpoint.").arg(max));
    Breakpoint::list().setState(last, Breakpoint::Disabled);
  }
}

BitValue Debugger::Manager::pc() const
{
  return (debugger() ? debugger()->pc() : BitValue());
}

Breakpoint::MarkType Debugger::Manager::breakpointType(const Breakpoint::Data &data) const
{
  if ( _coff==0 ) return Breakpoint::BreakpointDisabled;
  Address address = Breakpoint::list().address(data);
  if ( !address.isValid() ) return Breakpoint::BreakpointInvalid;
  if ( Breakpoint::list().state(data)!=Breakpoint::Active ) return Breakpoint::BreakpointDisabled;
  if ( address==pc() ) {
    if ( programmer()->state()==::Programmer::Halted ) return Breakpoint::BreakpointReached;
    return Breakpoint::ProgramCounterDisabled;
  }
  return Breakpoint::BreakpointActive;
}

bool Debugger::Manager::checkBreakpoint(const Breakpoint::Data &bdata, bool onlyWarn, Address &address)
{
  address = Address();
  if ( _coff==0 ) return true;
  QValueVector<Address> addresses = _coff->addresses(bdata.url, bdata.line);
  if ( addresses.isEmpty() ) {
    QString s = i18n("Breakpoint at non-code line.");
    if (onlyWarn) log(Log::Warning, s);
    else sorry(s);
    return false;
  }
  if ( addresses.count()>1 ) log(Log::Warning, i18n("Breakpoint corresponds to several addresses. Using the first one."));
  address = addresses[0];
  return true;
}

void Debugger::Manager::computeBreakpointAddresses()
{
  if ( programmerGroup()==0 ) return;
  int nb = programmerGroup()->maxNbBreakpoints(deviceData());
  for (int i=Breakpoint::list().count()-1; i>=0; i--) {
    const Breakpoint::Data &data = Breakpoint::list().data(i);
    Address address;
    checkBreakpoint(data, true, address);
    Breakpoint::list().setAddress(data, address);
    if ( _coff==0 ) Breakpoint::list().setState(data, Breakpoint::Unknown);
    else if ( Breakpoint::list().address(data).isValid() && nb>0 ) {
      Breakpoint::list().setState(data, Breakpoint::Active);
      nb--;
    }
  }
}

QValueList<Address> Debugger::Manager::activeBreakpointAddresses() const
{
  QValueList<Address> addresses;
  for (uint i=0; i<Breakpoint::list().count(); i++) {
    const Breakpoint::Data &data = Breakpoint::list().data(i);
    if ( Breakpoint::list().state(data)==Breakpoint::Active ) addresses.append(Breakpoint::list().address(data));
  }
  return addresses;
}

void Debugger::Manager::clear()
{
  if ( programmer() ) programmer()->clear();
  delete _coff;
  _coff = 0;
  computeBreakpointAddresses();
  update();
}

bool Debugger::Manager::update()
{
  _timer.stop();
  _readRegisters.clear();
  updateView(true);
  if ( !updateRegisters() ) return false;
  QString s;
  if ( debugger() ) s = debugger()->statusString();
  emit statusChanged(s);
  return true;
}

bool Debugger::Manager::updateRegister(const Register::TypeData &data)
{
  // read related registers
  const Device::RegistersData *rdata = deviceData()->registersData();
  Q_ASSERT(rdata);
  QValueList<Register::TypeData> related = rdata->relatedRegisters(data);
  for (uint k=0; k<uint(related.count()); k++)
    if ( !readRegister(related[k]) ) return false;
  // read port status
  if ( data.type()==Register::Regular ) {
    int index = rdata->portIndex(data.address());
    if ( index!=-1 ) {
      QMap<uint, Device::PortBitData> data;
      if ( !debugger()->updatePortStatus(index, data) ) return false;
      Register::list().setPortData(index, data);
    }
  }
  return true;
}

bool Debugger::Manager::updateRegisters()
{
  if ( !isActive() ) return true;
  QValueList<Register::TypeData> watched = Register::list().watched();
  for (uint i=0; i<uint(watched.count()); i++)
    if ( !updateRegister(watched[i]) ) return false;
  return true;
}

bool Debugger::Manager::run()
{
  if ( !checkState() ) return false;
  if ( !debugger()->setBreakpoints(activeBreakpointAddresses()) ) return false;
  if ( !debugger()->run() ) return false;
  log(Log::Information, i18n("Running..."));
  if ( !update() ) return false;
  _timer.start(programmer()->runUpdateWait());
  return true;
}

bool Debugger::Manager::halt()
{
  _timer.stop();
  if ( !debugger()->halt() ) return false;
  return update();
}

bool Debugger::Manager::reset()
{
  _timer.stop();
  log(Log::Data, "--------------------------------------------------");
  if ( !debugger()->reset() ) return false;
  return update();
}

bool Debugger::Manager::step()
{
  bool first;
  if ( !checkState(&first) ) return false;
  if ( !debugger()->setBreakpoints(activeBreakpointAddresses()) ) return false;
  if ( !first && !debugger()->step() ) return false;
  return update();
}

void Debugger::Manager::slotTimeout()
{
  _timer.stop();
  if ( !debugger()->update() ) return;
  if ( programmer()->state()==Programmer::Running ) {
    _timer.start(programmer()->runUpdateWait());
    return;
  }
  log(Log::Information, i18n("Reached breakpoint."));
  update();
  emit targetStateChanged();
}

void Debugger::Manager::setRegisterWatched(const Register::TypeData &data, bool watched)
{
  if (watched) {
    if ( Register::list().isWatched(data) ) return;
    Register::list().setWatched(data, true);
    if ( isActive() ) updateRegister(data);
  } else Register::list().setWatched(data, false);
}

bool Debugger::Manager::readRegister(const Register::TypeData &data)
{
  Q_ASSERT( data.type()==Register::Regular || data.type()==Register::Special );
  if ( _readRegisters.contains(data) ) return true;
  BitValue value;
  if ( !debugger()->readRegister(data, value) ) return false;
  Register::list().setValue(data, value);
  _readRegisters.append(data);
  return true;
}

bool Debugger::Manager::writeRegister(const Register::TypeData &data, BitValue value)
{
  Q_ASSERT( data.type()==Register::Regular || data.type()==Register::Special );
  if ( !debugger()->writeRegister(data, value) ) return false;
  _readRegisters.clear();
  if ( !updateRegister(data) ) return false;
  emit statusChanged(debugger()->statusString());
  return true;
}

bool Debugger::Manager::readAllRegisters()
{
  const Device::RegistersData *rdata = _data->registersData();
  for (uint i=0; i<rdata->nbRegisters(); i++) {
    Register::TypeData rtd(rdata->addressFromIndex(i), rdata->nbChars());
    if ( !updateRegister(rtd) ) return false;
  }
  return true;
}

void Debugger::Manager::stopWatchAll()
{
  Register::list().clearWatched();
}

bool Debugger::Manager::prepareDebugging()
{
  if ( programmerGroup()->isSoftware() && programmerGroup()->isDebugger() ) {
    PURL::Url curl = coffUrl();
    if ( curl.isEmpty() ) {
      log(Log::Error, i18n("Cannot start debugging session without input file (not specified)."));
      return false;
    }
    PURL::Url first;
    uint i = 0;
    for (; i<Programmer::Nb_InputFileTypes; i++) {
      PURL::FileType type = Programmer::INPUT_FILE_TYPE_DATA[i];
      if ( !programmerGroup()->isInputFileTypeSupported(type) ) continue;
      PURL::Url url = curl.toFileType(type);
      if ( first.isEmpty() ) first = url;
      if ( !url.exists() ) continue;
      debugger()->setupInput(type, url.directory().path(), url.filename());
      break;
    }
    if ( i==Programmer::Nb_InputFileTypes ) {
      log(Log::Error, i18n("Cannot start debugging session without input file (%1).").arg(first.pretty()));
      return false;
    }
  }
  return true;
}
