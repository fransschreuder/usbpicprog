/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GENERIC_PROG_H
#define GENERIC_PROG_H

#include "common/global/log.h"
#include "common/global/pfile.h"
#include "common/port/port_base.h"
#include "devices/base/generic_device.h"
#include "devices/base/generic_memory.h"
#include "prog_specific.h"
namespace Debugger { class Base; }
namespace Device { class MemoryRange; }

namespace Programmer
{
  enum Mode { NormalMode, BootloadMode };
  enum State { NotConnected = 0, Stopped, Running, Halted };
  class Hardware;
  class Group;
  class DeviceSpecific;
  extern const double UNKNOWN_VOLTAGE;

  enum VerifyAction { NormalVerify = 0, BlankCheckVerify = 1,
                      IgnoreProtectedVerify = 2, OnlyProgrammedVerify = 4 };
  Q_DECLARE_FLAGS(VerifyActions, VerifyAction)
  Q_DECLARE_OPERATORS_FOR_FLAGS(VerifyActions)
  class VerifyData {
  public:
    VerifyData(VerifyActions pactions, const Device::Memory &pmemory) : actions(pactions), memory(pmemory) {}
    VerifyActions actions;
    RangeVector protectedRanges;
    const Device::Memory &memory;
  };

  enum ResultType { Pass = 0, Low, High, Fail, Nb_ResultTypes };
  extern const char * const RESULT_TYPE_LABELS[Nb_ResultTypes+1];

//-----------------------------------------------------------------------------
class Base : public QObject, public Log::Base
{
Q_OBJECT
public:
  Base(const Group &group, const Device::Data *data, const char *name);
  virtual ~Base();

  virtual void log(Log::Type type, const QString &message);
  virtual void log(Log::DebugLevel level, const QString &message);
  void init(bool targetSelfPowered, Hardware *hardware, DeviceSpecific *deviceSpecific);
  const Device::Data *device() const { return _device; }
  const Group &group() const { return _group; }
  Hardware *hardware() { return _hardware; }
  ::Debugger::Base *debugger() { return _debugger; }
  virtual uint maxNbBreakpoints() const { return 1; }
  virtual uint runUpdateWait() const { return 300; }

  bool simpleConnectHardware();
  bool connectHardware();
  void disconnectHardware();
  bool connectDevice();
  bool setTargetPowerOn(bool on);
  bool isTargetPowerOn() const { return _targetPowerOn; }
  void setFirmwareDirectory(const QString &dir) { _firmwareDirectory = dir; }
  const VersionData &firmwareVersion() const { return _firmwareVersion; }
  virtual bool readFirmwareVersion() { return true; }
  bool uploadFirmware(const QString &filepath);
  virtual bool readVoltages() { return true; }
  virtual bool selfTest(bool ask) { Q_UNUSED(ask); return true; }

  bool erase(const Device::MemoryRange &range);
  bool read(Device::Memory &memory, const Device::MemoryRange &range);
  bool program(const Device::Memory &memory, const Device::MemoryRange &range);
  bool verify(const Device::Memory &memory, const Device::MemoryRange &range);
  bool blankCheck(const Device::MemoryRange &range);
  bool run();
  bool stop();
  State state() const { return _state; }
  void setState(State state) { _state = state; }
  bool isConnected() const { return ( _state!=NotConnected ); }
  bool isInDebugging() const { return ( _state==Halted || _state==Running ); }
  bool enterMode(Mode mode);
  bool checkNormalMode();
  bool doConnectDevice();
  virtual void clear();

signals:
  void actionMessage(const QString &message);

protected:
  Hardware           *_hardware;
  DeviceSpecific     *_specific;
  const Device::Data *_device;
  ::Debugger::Base     *_debugger;
  State               _state;
  bool                _targetPowerOn;
  const Group        &_group;
  QString             _firmwareDirectory;
  VersionData         _firmwareVersion;
  Mode                _mode;
  bool                _targetSelfPowered;

  PURL::Directory firmwareDirectory();
  virtual bool setupFirmware() { return true; }
  virtual VersionData minFirmwareVersion() const { return VersionData(); }
  virtual VersionData minRecommendedFirmwareVersion() const { return VersionData(); }
  virtual VersionData maxFirmwareVersion() const { return VersionData(); }
  virtual bool checkFirmwareVersion();
  virtual bool setTarget() { return true; }
  virtual bool internalSetupHardware() { return true; }
  virtual bool verifyDeviceId() = 0;
  enum Operation { Read = 0, Write, Verify, Erase, BlankCheck, Connect, Nb_Operations };
  struct OperationData {
    uint nbSteps;
    const char *message;
  };
  static const OperationData OPERATION_DATA[Nb_Operations];
  virtual uint nbSteps(Operation operation) const;
  void prepare(Operation operation);
  virtual bool doUploadFirmware(PURL::File &) { qFatal("doUploadFirmware not implemented"); return false; }
  virtual bool internalEnterMode(Mode) { qFatal("internalEnterMode not implemented"); return false; }

  virtual bool checkErase() = 0;
  virtual bool internalErase(const Device::MemoryRange &range) = 0;
  bool checkCanRead();
  virtual bool checkRead() = 0;
  virtual bool internalRead(Device::Memory *memory, const Device::MemoryRange &range, const VerifyData *vdata) = 0;
  virtual bool checkProgram(const Device::Memory &memory) = 0;
  virtual bool internalProgram(const Device::Memory &memory, const Device::MemoryRange &range) = 0;
  virtual bool internalRun() { return false; }
  virtual bool internalStop() { return false; }

  void endProgramming();
  bool doErase(const Device::MemoryRange &range);
  bool doRead(Device::Memory &memory, const Device::MemoryRange &range);
  bool doVerify(const Device::Memory &memory, const Device::MemoryRange &range);
  bool doVerify(VerifyAction action, const Device::MemoryRange &range, const Device::Memory *memory);
  bool doBlankCheck(const Device::MemoryRange &range);
  virtual bool doProgram(const Device::Memory &memory, const Device::MemoryRange &range);

  friend class DeviceSpecific;
};

} // namespace

#endif
