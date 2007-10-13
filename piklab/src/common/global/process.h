/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROCESS_H
#define PROCESS_H

#include <signal.h>
#include <qstringlist.h>
#include <qobject.h>
#include <qtimer.h>
class KProcess;

#include "global.h"
namespace PURL { class Directory; }

namespace Process
{
enum State { Stopped, StartFailed, Running, Exited, Timedout };
class Base;
enum RunAction { NoRunAction = 0, Start = 1 };
Q_DECLARE_FLAGS(RunActions, RunAction)
Q_DECLARE_OPERATORS_FOR_FLAGS(RunActions)
extern State runSynchronously(Base &process, RunActions actions, uint timeout); // in ms (0 == no timeout)

//----------------------------------------------------------------------------
class Base : public QObject
{
Q_OBJECT
public:
  Base(QObject *parent, const char *name);
  virtual ~Base();
  void setup(const QString &executable, const QStringList &options, bool withWine);
  QStringList arguments() const;
  void setWorkingDirectory(const PURL::Directory &dir);
  virtual bool start(uint timeout); // in ms (0 == no timeout)
  QString prettyCommand() const { return arguments().join(" "); }
  void writeToStdin(const QString &s);
  bool signal(int n);
  bool isRunning() const;
  State state() const { return _state; }
  int exitCode() const;

signals:
  void requestSynchronousStop();
  void done(int code);
  void timeout();
  void stdoutDataReceived();
  void stderrDataReceived();

protected slots:
  void exited();
  void timeoutSlot();
  virtual void receivedStdout(KProcess*, char *buffer, int len) = 0;
  virtual void receivedStderr(KProcess*, char *buffer, int len) = 0;

  friend State runSynchronously(Base &, RunActions, uint);

protected:
  State     _state;
  KProcess *_process;
  QTimer   *_timer;
  QString   _stdout, _stderr;

  static bool isFilteredLine(const QString &line);
};

//----------------------------------------------------------------------------
class StringOutput : public Base
{
Q_OBJECT
public:
  StringOutput(QObject *parent = 0, const char *name = 0) : Base(parent, name) {}
  QString sout() const { return _stdout; }
  QString serr() const { return _stderr; }

private slots:
  virtual void receivedStdout(KProcess *, char *buffer, int len);
  virtual void receivedStderr(KProcess *, char *buffer, int len);
};

//----------------------------------------------------------------------------
class LineBase : public Base
{
Q_OBJECT
public:
  LineBase(QObject *parent = 0, const char *name = 0) : Base(parent, name) {}

private slots:
  virtual void receivedStdout(KProcess *, char *buffer, int len);
  virtual void receivedStderr(KProcess *, char *buffer, int len);

private:
  virtual void addStdoutLine(const QString &line) = 0;
  virtual void addStderrLine(const QString &line) = 0;
};

//----------------------------------------------------------------------------
class LineOutput : public LineBase
{
Q_OBJECT
public:
  LineOutput(QObject *parent = 0, const char *name = 0) : LineBase(parent, name) {}
  virtual bool start(uint timeout);
  QStringList sout() const { return _stdoutLines; }
  QStringList serr() const { return _stderrLines; }

protected:
  QStringList _stdoutLines, _stderrLines;

  virtual void addStdoutLine(const QString &line) { _stdoutLines += line; }
  virtual void addStderrLine(const QString &line) { _stderrLines += line; }
};

//----------------------------------------------------------------------------
class LineSignal : public LineBase
{
Q_OBJECT
public:
  LineSignal(QObject *parent = 0, const char *name = 0) : LineBase(parent, name) {}

signals:
  void logStdoutLine(const QString &line);
  void logStderrLine(const QString &line);

private:
  virtual void addStdoutLine(const QString &line) { emit logStdoutLine(line); }
  virtual void addStderrLine(const QString &line) { emit logStderrLine(line); }
};

} // namespace

#endif
