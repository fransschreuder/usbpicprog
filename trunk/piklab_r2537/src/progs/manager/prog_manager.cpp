/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "prog_manager.h"

#include "progs/base/generic_prog.h"
#include "progs/base/prog_group.h"
#include "progs/base/generic_debug.h"
#include "progs/base/prog_config.h"
#include "debug_manager.h"

//----------------------------------------------------------------------------
Programmer::Manager *Programmer::manager = 0;

Programmer::Manager::Manager(QObject *parent)
  : QObject(parent, "programmer_manager"), _programmer(0)
{}

Programmer::Manager::~Manager()
{
  delete _programmer;
}

void Programmer::Manager::clear()
{
  delete _programmer;
  _programmer = 0;
}

void Programmer::Manager::createProgrammer(const Device::Data *data, const HardwareDescription &hd)
{
  if ( _programmer && _programmer->device()==data && !hasError() ) return;
  delete _programmer;
  _programmer = group().createProgrammer(isTargetSelfPowered(), data, hd);
  _programmer->Log::Base::setParent(this);
  connect(_programmer, SIGNAL(actionMessage(const QString &)), SIGNAL(actionMessage(const QString &)));
  connect(&_programmer->progressMonitor(), SIGNAL(setLabel(const QString &)), SIGNAL(actionMessage(const QString &)));
  connect(&_programmer->progressMonitor(), SIGNAL(setTotalProgress(uint)), SIGNAL(setTotalProgress(uint)));
  connect(&_programmer->progressMonitor(), SIGNAL(setProgress(uint)), SIGNAL(setProgress(uint)));
  connect(&_programmer->progressMonitor(), SIGNAL(showProgress(bool)), SIGNAL(showProgress(bool)));
}

bool Programmer::Manager::initProgramming(bool debugging)
{
  if ( !internalInitProgramming(debugging) ) return false;
  setState(Programming);
  return true;
}

bool Programmer::Manager::internalInitProgramming(bool)
{
  if ( device()==0 ) {
    sorry(i18n("You need to specify the device for programming."));
    return false;
  }
  if ( !group().isSupported(device()->name()) ) {
    if ( group().isSoftware() && group().supportedDevices().isEmpty() )
      sorry(i18n("Could not detect supported devices for \"%1\". Please check installation.").arg(group().label()));
    else sorry(i18n("The current programmer \"%1\" does not support device \"%2\".").arg(group().label()).arg(device()->name()));
    return false;
  }
  createProgrammer(device());
  return true;
}

void Programmer::Manager::endProgramming()
{
  ::Debugger::manager->update(true);
  setState(Idle);
}

bool Programmer::Manager::program(const Device::Memory &memory, const Device::MemoryRange &range)
{
  if ( !initProgramming(false) ) return false;
  bool ok = _programmer->program(memory, range);
  endProgramming();
  if ( ok && !group().isDebugger() && readConfigEntry(Config::RunAfterProgram).toBool() ) return run();
  return ok;
}

bool Programmer::Manager::verify(const Device::Memory &memory, const Device::MemoryRange &range)
{
  if ( !initProgramming(false) ) return false;
  bool ok = _programmer->verify(memory, range);
  endProgramming();
  return ok;
}

bool Programmer::Manager::read(Device::Memory &memory, const Device::MemoryRange &range)
{
  if ( !initProgramming(false) ) return false;
  bool ok = _programmer->read(memory, range);
  endProgramming();
  return ok;
}

bool Programmer::Manager::erase(const Device::MemoryRange &range)
{
  if ( !initProgramming(false) ) return false;
  bool ok = _programmer->erase(range);
  endProgramming();
  return ok;
}

bool Programmer::Manager::blankCheck(const Device::MemoryRange &range)
{
  if ( !initProgramming(false) ) return false;
  bool ok = _programmer->blankCheck(range);
  endProgramming();
  return ok;
}

bool Programmer::Manager::connectDevice()
{
  if ( !initProgramming(false) ) return false;
  _programmer->disconnectHardware();
  bool ok = ::Debugger::manager->prepareDebugging();
  if (ok) ok = _programmer->connectDevice();
  if ( ok && group().isSoftware() && group().isDebugger() ) {
    ok = _programmer->debugger()->init();
    if (ok) ::Debugger::manager->update(true);
  }
  endProgramming();
  return ok;
}

bool Programmer::Manager::setDevicePower(bool on)
{
  if ( !initProgramming(false) ) return false;
  bool ok = true;
  if ( _programmer->isTargetSelfPowered() )
    sorry(i18n("Cannot toggle target power since target is self-powered."), QString::null);
  else {
    emit actionMessage(i18n("Toggle Device Power..."));
    ok = _programmer->setTargetPowerOn(on);
  }
  endProgramming();
  return ok;
}

bool Programmer::Manager::disconnectDevice()
{
  ::Debugger::manager->clear();
  emit actionMessage(i18n("Disconnecting..."));
  bool debugger = group().isDebugger();
  _programmer->setTargetPowerOn(false);
  _programmer->disconnectHardware();
  endProgramming();
  if (debugger) log(Log::LineType::Information, i18n("Stopped."));
  return true;
}

bool Programmer::Manager::run()
{
  bool debugger = group().isDebugger();
  if ( !initProgramming(debugger) ) return false;
  bool ok = (debugger ? ::Debugger::manager->run() : _programmer->run());
  setState(Idle);
  return ok;
}

bool Programmer::Manager::halt()
{
  bool debugger = group().isDebugger();
  bool ok = (debugger ? ::Debugger::manager->halt() : _programmer->stop());
  if (debugger) setState(Idle);
  else {
    endProgramming();
    log(Log::LineType::Information, i18n("Stopped."));
  }
  return ok;
}

void Programmer::Manager::stop()
{
  if (_programmer) _programmer->disconnectHardware();
}

bool Programmer::Manager::restart()
{
  bool ok;
  if (group().isDebugger()) {
    if ( _programmer==0 || _programmer->state()==::Programmer::NotConnected
         || _programmer->state()==::Programmer::Stopped ) return step();
    if ( !initProgramming(true) ) return false;
    ok = ::Debugger::manager->reset();
    log(Log::LineType::Information, i18n("Reset.<Translators: please translate the past form of the verb>", "Reset."));
    setState(Idle);
  } else {
    log(Log::LineType::Information, i18n("Restarting..."));
    ok = _programmer->stop();
    Port::msleep(200);
    if (ok) ok = _programmer->run();
    endProgramming();
  }
  return ok;
}

bool Programmer::Manager::step()
{
  if ( !initProgramming(true) ) return false;
  bool ok = ::Debugger::manager->step();
  setState(Idle);
  return ok;
}

bool Programmer::Manager::isTargetSelfPowered() const
{
  if ( group().targetPowerMode()==TargetPowerModeFromConfig ) return readConfigEntry(Config::TargetSelfPowered).toBool();
  return ( group().targetPowerMode()==TargetSelfPowered );
}
