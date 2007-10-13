/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GPSIM_H
#define GPSIM_H

#include "common/common/misc.h"
#include "progs/base/prog_specific.h"
#include "common/global/process.h"
#include "common/global/purl.h"

namespace GPSim
{
//-----------------------------------------------------------------------------
class Process : public ::Process::LineOutput, public Log::Base
{
Q_OBJECT
public:
  Process(Log::Base *base);

private slots:
  void stdoutDataReceivedSlot();

private:
  bool _ready;

  virtual void addStdoutLine(const QString &line);

  friend class ProcessManager;
};

//-----------------------------------------------------------------------------
class ProcessManager : public Log::Base
{
public:
  ProcessManager(Log::Base *base);
  Process &process() { return _process; }
  bool isReady() const { return _process._ready; }
  bool start();
  bool runSynchronously();
  bool sendCommand(const QString &cmd, bool synchronous);
  bool sendSignal(uint n, bool synchronous);
  bool getVersion(VersionData &version);

private:
  Process _process;
};

//-----------------------------------------------------------------------------
class Hardware : public Programmer::Hardware
{
public:
  Hardware(::Programmer::Base &base) : Programmer::Hardware(base, 0, QString::null), _manager(0) {}
  virtual ~Hardware();
  bool isReady() const { return _manager->isReady(); }
  bool execute(const QString &command, bool synchronous, QStringList *output = 0);
  bool signal(uint n, bool synchronous, QStringList *output = 0);
  const VersionData &version() const { return _version; }

private:
  ProcessManager *_manager;
  VersionData _version;

  virtual bool internalConnectHardware();
  virtual void internalDisconnectHardware();
};

//-----------------------------------------------------------------------------
class DeviceSpecific : public ::Programmer::DeviceSpecific
{
public:
  DeviceSpecific(::Programmer::Base &base) : ::Programmer::DeviceSpecific(base) {}
  virtual bool setPowerOff() { return false; }
  virtual bool setPowerOn() { return false; }
  virtual bool setTargetPowerOn(bool) { return true; }
};

} // namespace

#endif
