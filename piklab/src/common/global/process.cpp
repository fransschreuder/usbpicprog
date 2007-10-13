/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "process.h"

#include <qdatetime.h>

#if defined(NO_KDE)
#  include "common/nokde/nokde_kprocess.h"
#else
#  include <kprocess.h>
#endif
#include "purl.h"
#include "common/common/synchronous.h"

//----------------------------------------------------------------------------
Process::State Process::runSynchronously(Base &process, RunActions actions, uint timeout)
{
  Synchronous sync(timeout);
  QObject::connect(&process, SIGNAL(done(int)), &sync, SLOT(done()));
  QObject::connect(&process, SIGNAL(requestSynchronousStop()), &sync, SLOT(done()));
  if ( (actions & Start) && !process.start(0) ) return process.state();
  Q_ASSERT( process.isRunning() );
  if ( !sync.enterLoop() ) process.timeoutSlot();
  return process.state();
}

//----------------------------------------------------------------------------
Process::Base::Base(QObject *parent, const char *name)
  : QObject(parent, name), _state(Stopped)
{
  _process = new KProcess(this);
  connect(_process, SIGNAL(processExited(KProcess *)), SLOT(exited()));
  connect(_process, SIGNAL(receivedStdout(KProcess *, char *, int)), SLOT(receivedStdout(KProcess*, char *, int)));
  connect(_process, SIGNAL(receivedStderr(KProcess *, char *, int)), SLOT(receivedStderr(KProcess*, char *, int)));
  _timer = new QTimer(this);
  connect(_timer, SIGNAL(timeout()), SLOT(timeoutSlot()));
}

Process::Base::~Base()
{
  _process->kill();
}

QStringList Process::Base::arguments() const
{
  if ( _process==0 ) return QStringList();
#if defined(NO_KDE)
  return _process->args();
#else
  QStringList list;
  const QValueList<QCString> &args = _process->args();
  for (uint i=0; i<args.count(); i++) list.append(args[i]);
  return list;
#endif
}

void Process::Base::setup(const QString &executable, const QStringList &options, bool withWine)
{
  _state = Stopped;
  _timer->stop();
  _process->clearArguments();
  if (withWine) {
    _process->setEnvironment("WINEDEBUG", "-all");
    *_process << QString("wine");
  }
  *_process << executable;
  *_process << options;
}

bool Process::Base::start(uint timeout)
{
  _state = Stopped;
  _timer->stop();
  _stdout = QString::null;
  _stderr = QString::null;
#if defined(NO_KDE)
  if ( !_process->start() ) {
#else
  if ( !_process->start(KProcess::NotifyOnExit, KProcess::All) ) {
#endif
    _state = StartFailed;
    return false;
  }
  _state = Running;
  if (timeout) _timer->start(timeout);
  return true;
}

void Process::Base::timeoutSlot()
{
  _state = Timedout;
  _process->kill();
  emit timeout();
}

int Process::Base::exitCode() const
{
  return _process->exitStatus();
}

void Process::Base::exited()
{
  _state = Exited;
  _timer->stop();
  emit done(exitCode());
}

bool Process::Base::isRunning() const
{
  return _process->isRunning();
}

void Process::Base::writeToStdin(const QString &s)
{
  QCString cs = s.latin1();
  _process->writeStdin(cs.data(), cs.length());
}

bool Process::Base::signal(int n)
{
  return _process->kill(n);
}

void Process::Base::setWorkingDirectory(const PURL::Directory &dir)
{
  _process->setWorkingDirectory(dir.path());
}

bool Process::Base::isFilteredLine(const QString &line)
{
  // "wine" returns all those "libGL warning" that mess up the output...
  return line.startsWith("libGL warning");
}

//----------------------------------------------------------------------------
void Process::StringOutput::receivedStdout(KProcess*, char *data, int len)
{
  _stdout += QString::fromLatin1(data, len);
  emit stdoutDataReceived();
}

void Process::StringOutput::receivedStderr(KProcess*, char *data, int len)
{
  _stderr += QString::fromLatin1(data, len);
  emit stderrDataReceived();
}

//----------------------------------------------------------------------------
void Process::LineBase::receivedStdout(KProcess*, char *data, int len)
{
  for (uint i=0; i<uint(len); i++) {
    if ( data[i]=='\r' ) continue;
    if ( data[i]=='\n' ) {
      if ( !isFilteredLine(_stdout) ) addStdoutLine(_stdout);
      _stdout = QString::null;
    } else _stdout += QString::fromLatin1(data + i, 1);
  }
  if ( !_process->isRunning() && !isFilteredLine(_stdout) ) addStdoutLine(_stdout);
  emit stdoutDataReceived();
}

void Process::LineBase::receivedStderr(KProcess*, char *data, int len)
{
  for (uint i=0; i<uint(len); i++) {
    if ( data[i]=='\r' ) continue;
    if ( data[i]=='\n' ) {
      if ( !isFilteredLine(_stderr) ) addStderrLine(_stderr);
      _stderr = QString::null;
    } else _stderr += QString::fromLatin1(data + i, 1);
  }
  if ( !_process->isRunning() && !isFilteredLine(_stderr) ) addStderrLine(_stderr);
  emit stderrDataReceived();
}

//----------------------------------------------------------------------------
bool Process::LineOutput::start(uint timeout)
{
  _stdoutLines.clear();
  _stderrLines.clear();
  return Process::LineBase::start(timeout);
}

