/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "generic_prog.h"

#include <qdir.h>

#include "common/global/global.h"
#include "prog_group.h"
#include "prog_config.h"
#include "devices/base/device_group.h"
#include "generic_debug.h"
#include "hardware_config.h"

//-----------------------------------------------------------------------------
const double Programmer::UNKNOWN_VOLTAGE = -1.0;

const char * const Programmer::RESULT_TYPE_LABELS[Nb_ResultTypes+1] = {
  I18N_NOOP("Pass"),
  I18N_NOOP("Low"),
  I18N_NOOP("High"),
  I18N_NOOP("Fail"),
  I18N_NOOP("---")
};

const Programmer::Base::OperationData Programmer::Base::OPERATION_DATA[Nb_Operations] = {
  {  0, I18N_NOOP("Reading...")        },
  { 10, I18N_NOOP("Programming...")    },
  {  0, I18N_NOOP("Reading...")        },
  { 10, I18N_NOOP("Erasing...")        },
  {  0, I18N_NOOP("Blank Checking...") },
  { 10, I18N_NOOP("Connecting...")     }
};

//-----------------------------------------------------------------------------
Programmer::Base::Base(const Group &group, const Device::Data *device, const char *)
  : _hardware(0), _specific(0), _device(device), _debugger(group.createDebugger(*this)),
    _state(NotConnected), _targetPowerOn(false), _group(group)
{}

void Programmer::Base::init(bool targetSelfPowered, Hardware *hardware, DeviceSpecific *ds)
{
  clear();
  _targetSelfPowered = targetSelfPowered;
  _hardware = hardware;
  _specific = ds;
}

Programmer::Base::~Base()
{
  delete _debugger;
  delete _specific;
  delete _hardware;
}

void Programmer::Base::clear()
{
  _firmwareVersion.clear();
  _mode = NormalMode;
  resetError();
}

bool Programmer::Base::simpleConnectHardware()
{
  Q_ASSERT(_hardware);
  disconnectHardware();
  clear();
  if (_device) {
    QString label = _group.label();
    if ( group().isSoftware() )
      log(Log::Information, i18n("Connecting %1 with device %2...").arg(label).arg(_device->name()));
    else {
      if ( !_hardware->name().isEmpty() ) label += "[" + _hardware->name() + "]";
      Port::Description pd = _hardware->portDescription();
      QString s = i18n(Port::DATA[pd.type].label);
      if (Port::DATA[pd.type].withDevice) s += " (" + pd.device + ")";
      log(Log::Information, i18n("Connecting %1 on %2 with device %3...").arg(label).arg(s).arg(_device->name()));
    }
  }
  return _hardware->connectHardware();
}

bool Programmer::Base::connectHardware()
{
  log(Log::ExtraDebug, "connect hardware");
  if ( !simpleConnectHardware() ) return false;
  if ( !group().isSoftware() ) {
    if ( !readFirmwareVersion() ) return false;
    if ( _specific==0 ) return true;
    if ( _mode==BootloadMode ) return true;
    if ( !setupFirmware() ) return false;
    if ( !checkFirmwareVersion() ) return false;
    if ( !setTargetPowerOn(false) ) return false;
    if ( !setTarget() ) return false;
    log(Log::Information, i18n("  Set target self powered: %1").arg(_targetSelfPowered ? "true" : "false"));
    if ( !setTargetPowerOn(!_targetSelfPowered) ) return false;
    if ( !internalSetupHardware() ) return false;
    if ( !readVoltages() ) return false;
    if ( !selfTest(true) ) return false;
  }
  if ( hasError() ) return false;
  log(Log::Information, i18n("Connected."));
  _state = Stopped;
  addProgress(OPERATION_DATA[Connect].nbSteps);
  return true;
}

void Programmer::Base::disconnectHardware()
{
  log(Log::ExtraDebug, "disconnect hardware");
  clear();
  _hardware->disconnectHardware();
  _state = NotConnected;
}

PURL::Directory Programmer::Base::firmwareDirectory()
{
  if ( _firmwareDirectory.isEmpty() ) {
    ::Programmer::GroupConfig config(group());
    _firmwareDirectory = config.firmwareDirectory();
  }
  PURL::Directory dir(_firmwareDirectory);
  if ( !dir.exists() ) {
    log(Log::Error, i18n("Firmware directory is not configured or does not exist."));
    return PURL::Directory();
  }
  return dir;
}

bool Programmer::Base::checkFirmwareVersion()
{
  if ( _mode==BootloadMode ) log(Log::Information, i18n("Programmer is in bootload mode."));
  if ( !_firmwareVersion.isValid() ) return true;
  log(Log::Information, i18n("Firmware version is %1").arg(_firmwareVersion.pretty()));
  VersionData vd(_firmwareVersion.majorNum(), _firmwareVersion.minorNum(), 0); // ignore minor
  VersionData tmp = maxFirmwareVersion();
  if ( tmp.isValid() && VersionData(tmp.majorNum(), tmp.minorNum(), 0)<vd ) {
    log(Log::Warning, i18n("The firmware version (%1) is higher than the version tested with piklab (%2).\n"
                           "You may experience problems.")
                      .arg(_firmwareVersion.pretty()).arg(tmp.pretty()));
    return true;
  }
  tmp = minFirmwareVersion();
  if ( tmp.isValid() && vd<VersionData(tmp.majorNum(), tmp.minorNum(), 0) ) {
    log(Log::Warning, i18n("The firmware version (%1) is lower than the version tested with piklab (%2).\n"
                           "You may experience problems.")
                      .arg(_firmwareVersion.pretty()).arg(tmp.pretty()));
    return true;
  }
  tmp = minRecommendedFirmwareVersion();
  if ( tmp.isValid() && vd<tmp )
    log(Log::Warning, i18n("The firmware version (%1) is lower than the recommended version (%2).\n"
                           "It is recommended to upgrade the firmware.")
                      .arg(_firmwareVersion.pretty()).arg(tmp.pretty()));
  return true;
}

bool Programmer::Base::enterMode(Mode mode)
{
  log(Log::NormalDebug, mode==BootloadMode ? "  Enter bootload mode" : "  Enter normal mode");
  if ( _mode==mode ) {
    log(Log::NormalDebug, "  Already in requested mode.");
    return true;
  }
  if ( !internalEnterMode(mode) ) return false;
  return ( _mode==mode );
}

bool Programmer::Base::checkNormalMode()
{
  if ( _mode==BootloadMode ) return enterMode(NormalMode);
  return true;
}

void Programmer::Base::log(Log::Type type, const QString &message)
{
  if ( type==Log::Error ) _state = NotConnected;
  Log::Base::log(type, message);
}

void Programmer::Base::log(Log::DebugLevel level, const QString &message)
{
  Log::Base::log(level, message);
}

uint Programmer::Base::nbSteps(Operation operation) const
{
  uint length = OPERATION_DATA[Connect].nbSteps;
  if ( operation==Connect ) return length;
  length += OPERATION_DATA[operation].nbSteps;
  return length;
}

void Programmer::Base::prepare(Operation operation)
{
  emit actionMessage(i18n(OPERATION_DATA[operation].message));
  uint nb = nbSteps(operation);
  setTotalSteps(nb);
}

bool Programmer::Base::setTargetPowerOn(bool on)
{
  _targetPowerOn = on;
  return _specific->setTargetPowerOn(on);
}

bool Programmer::Base::connectDevice()
{
  prepare(Connect);
  bool ok = doConnectDevice();
  endProgramming();
  return ok;
}

bool Programmer::Base::doConnectDevice()
{
  if ( _state==NotConnected ) {
    if ( !connectHardware() ) return false;
    if ( !checkNormalMode() ) return false;
    if ( !verifyDeviceId() ) return false;
  } else {
    addProgress(OPERATION_DATA[Connect].nbSteps);
    setTargetPowerOn(!_targetSelfPowered);
  }
  _state = Stopped;
  return true;
}

bool Programmer::Base::run()
{
  emit actionMessage(i18n("Running..."));
  if ( !doConnectDevice() ) return false;
  log(Log::Information, i18n("Run..."));
  internalRun();
  return !hasError();
}

bool Programmer::Base::stop()
{
  emit actionMessage(i18n("Halting..."));
  return internalStop();
}

void Programmer::Base::endProgramming()
{
  Config config;
  if ( _state==Stopped && config.powerDownAfterProgramming() )
    setTargetPowerOn(false);
  if ( !(group().properties() & HasConnectedState) ) disconnectHardware();
}

bool Programmer::Base::uploadFirmware(const PURL::Url &url)
{
  log(Log::NormalDebug, QString("  Firmware file: %1").arg(url.pretty()));
  Log::StringView sview;
  PURL::File file(url, sview);
  if ( !file.openForRead() ) {
    log(Log::Error, i18n("Could not open firmware file \"%1\".").arg(url.pretty()));
    return false;
  }
  prepare(Connect);
  bool ok  = doUploadFirmware(file);
  _firmwareVersion.clear();
  if (ok) ok = readFirmwareVersion();
  endProgramming();
  return ok;
}

//-----------------------------------------------------------------------------
bool Programmer::Base::erase(const Device::MemoryRange &range)
{
  prepare(Erase);
  bool ok = doErase(range);
  endProgramming();
  return ok;
}

bool Programmer::Base::doErase(const Device::MemoryRange &range)
{
  if ( !checkErase() ) return false;
  if ( !doConnectDevice() ) return false;
  log(Log::Information, i18n("Erasing..."));
  if ( !internalErase(range) ) return false;
  log(Log::Information, i18n("Erasing done"));
  Config config;
  if ( config.blankCheckAfterErase() ) {
    log(Log::Information, i18n("Blank checking..."));
    if ( !doVerify(BlankCheckVerify, range, 0) ) return false;
    log(Log::Information, i18n("Blank checking done."));
  }
  return true;
}

//-----------------------------------------------------------------------------
bool Programmer::Base::checkCanRead()
{
  if ( !(group().properties() & CanReadMemory) ) {
    log(Log::Error, i18n("The selected programmer cannot read device memory."));
    return false;
  }
  return true;
}

bool Programmer::Base::read(Device::Memory &memory, const Device::MemoryRange &range)
{
  if ( !checkCanRead() ) return false;
  prepare(Read);
  bool ok = doRead(memory, range);
  endProgramming();
  return ok;
}

bool Programmer::Base::doRead(Device::Memory &memory, const Device::MemoryRange &range)
{
  if ( !checkRead() ) return false;
  if ( !doConnectDevice() ) return false;
  log(Log::Information, i18n("Reading device memory..."));
  memory.clear();
  if ( !internalRead(&memory, range, 0) ) return false;
  log(Log::Information, i18n("Reading done."));
  return true;
}

//-----------------------------------------------------------------------------
bool Programmer::Base::program(const Device::Memory &memory, const Device::MemoryRange &range)
{
  prepare(Write);
  bool ok = doProgram(memory, range);
  endProgramming();
  return ok;
}

bool Programmer::Base::doProgram(const Device::Memory &memory, const Device::MemoryRange &range)
{
  if ( !checkProgram(memory) ) return false;
  if ( !doConnectDevice() ) return false;
  log(Log::Information, i18n("Programming device memory..."));
  if ( !internalProgram(memory, range) ) return false;
  log(Log::Information, i18n("Programming successful."));
  if ( group().isDebugger() && !_debugger->init() ) return false;
  return true;
}

//-----------------------------------------------------------------------------
bool Programmer::Base::verify(const Device::Memory &memory, const Device::MemoryRange &range)
{
  if ( !checkCanRead() ) return false;
  prepare(Verify);
  bool ok = doVerify(memory, range);
  endProgramming();
  return ok;
}

bool Programmer::Base::doVerify(VerifyAction action, const Device::MemoryRange &range, const Device::Memory *memory)
{
  const Device::Memory *vmemory = memory;
  if ( memory==0 ) {
    Q_ASSERT( action & BlankCheckVerify );
    vmemory = _device->group().createMemory(*_device);
  }
  VerifyData vdata(action, *vmemory);
  bool ok = internalRead(0, range, &vdata);
  if ( memory==0 ) delete vmemory;
  return ok;
}

bool Programmer::Base::doVerify(const Device::Memory &memory, const Device::MemoryRange &range)
{
  if ( !checkRead() ) return false;
  if ( !doConnectDevice() ) return false;
  log(Log::Information, i18n("Verifying..."));
  if ( !doVerify(NormalVerify, range, &memory) ) return false;
  log(Log::Information, i18n("Verifying successful."));
  return true;
}

bool Programmer::Base::blankCheck(const Device::MemoryRange &range)
{
  if ( !checkCanRead() ) return false;
  prepare(BlankCheck);
  bool ok = doBlankCheck(range);
  endProgramming();
  return ok;
}

bool Programmer::Base::doBlankCheck(const Device::MemoryRange &range)
{
  if ( !checkRead() ) return false;
  if ( !doConnectDevice() ) return false;
  log(Log::Information, i18n("Blank checking..."));
  if ( !doVerify(BlankCheckVerify, range, 0) ) return false;
  log(Log::Information, i18n("Blank checking successful."));
  return true;
}
