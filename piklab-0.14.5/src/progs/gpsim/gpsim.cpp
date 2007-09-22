/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "gpsim.h"

#include <qregexp.h>
#include "progs/base/generic_prog.h"
#include "progs/base/generic_debug.h"

//-----------------------------------------------------------------------------
GPSim::Process::Process(Log::Base *base)
  : ::Process::LineOutput(0, "gpsim_process"), Log::Base(base), _ready(false)
{
  connect(this, SIGNAL(stdoutDataReceived()), SLOT(stdoutDataReceivedSlot()));
}

void GPSim::Process::stdoutDataReceivedSlot()
{
  if ( _stdout.startsWith("**gpsim> ") || _ready ) {
    log(Log::ExtraDebug, "received console prompt", Log::Delayed);
    _ready = true;
    emit requestSynchronousStop();
  }
}

void GPSim::Process::addStdoutLine(const QString &line)
{
  log(Log::ExtraDebug, "  " + line, Log::Delayed);
  if ( line.startsWith("***ERROR:") ) {
    log(Log::Error, line);
    return;
  }
  QString s = line;
  QString prompt = "**gpsim> ";
  while ( s.startsWith(prompt) ) {
    _ready = true;
    s = s.mid(prompt.length());
  }
  s = s.stripWhiteSpace();
  _stdoutLines += s;
}

//-----------------------------------------------------------------------------
GPSim::ProcessManager::ProcessManager(Log::Base *base)
  : Log::Base(base), _process(base)
{}

bool GPSim::ProcessManager::start()
{
  _process._ready = false;
  _process.setup("gpsim", QStringList("-i"), false);
  if ( !_process.start(0) ) {
    log(Log::Error, i18n("Failed to start \"gpsim\""));
    return false;
  }
  return runSynchronously();
}

bool GPSim::ProcessManager::runSynchronously()
{
  ::Process::State state = ::Process::runSynchronously(_process, ::Process::NoRunAction, 5000);
  if ( !_process.isRunning() ) {
    log(Log::Error, i18n("\"gpsim\" unexpectedly exited"));
    return false;
  }
  if ( state==::Process::Timedout ) {
    log(Log::Error, i18n("Timeout waiting for \"gpsim\""));
    return false;
  }
  return true;
}

bool GPSim::ProcessManager::sendCommand(const QString &cmd, bool synchronous)
{
  _process._ready = false;
  _process._stdoutLines.clear();
  _process._stdout = QString::null;
  _process.writeToStdin(cmd + '\n');
  if (synchronous) return runSynchronously();
  return true;
}

bool GPSim::ProcessManager::sendSignal(uint n, bool synchronous)
{
  _process._ready = false;
  _process._stdoutLines.clear();
  _process._stdout = QString::null;
  if ( !_process.signal(n) ) {
    log(Log::Error, i18n("Error send a signal to the subprocess"));
    return false;
  }
  if (synchronous) return runSynchronously();
  return true;
}

bool GPSim::ProcessManager::getVersion(VersionData &version)
{
  if ( !sendCommand("version", true) ) return false;
  QRegExp reg("\\w*\\s*(\\d+\\.\\d+\\.\\d+).*");
  if ( _process.sout().count()==0 || !reg.exactMatch(_process.sout()[0]) ) {
    version = VersionData();
    return true;
  }
  version = VersionData::fromString(reg.cap(1));
  return true;
}

//-----------------------------------------------------------------------------
GPSim::Hardware::~Hardware()
{
  delete _manager;
}

bool GPSim::Hardware::internalConnectHardware()
{
  delete _manager;
  _manager = new ProcessManager(this);
  _manager->process().setWorkingDirectory(_base.debugger()->directory());
  if ( !_manager->start() ) return false;
  if ( !_manager->getVersion(_version) ) return false;
  if ( !_version.isValid() ) {
    log(Log::Error, i18n("Could not recognize gpsim version"));
    return false;
  }
  return true;
}

void GPSim::Hardware::internalDisconnectHardware()
{
  delete _manager;
  _manager = 0;
}

bool GPSim::Hardware::execute(const QString &command, bool synchronous, QStringList *output)
{
  log(Log::NormalDebug, QString("command: %1").arg(command));
  if (output) output->clear();
  if ( !_manager->sendCommand(command, synchronous) ) return false;
  if (output) *output = _manager->process().sout();
  return true;
}

bool GPSim::Hardware::signal(uint n, bool synchronous, QStringList *output)
{
  log(Log::NormalDebug, QString("signal: %1").arg(n));
  if (output) output->clear();
  if ( !_manager->sendSignal(n, synchronous) ) return false;
  if (output) *output = _manager->process().sout();
  return true;
}
