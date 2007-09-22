/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_prog.h"

#include "common/global/global.h"
#include "devices/list/device_list.h"
#include "progs/base/prog_config.h"
#include "progs/base/prog_group.h"

//-----------------------------------------------------------------------------
bool Programmer::PicGroup::canReadVoltages() const
{
  for (uint i=0; i<Pic::Nb_VoltageTypes; i++)
    if ( canReadVoltage(Pic::VoltageType(i)) ) return true;
  return false;
}

//-----------------------------------------------------------------------------
Programmer::PicBase::PicBase(const Group &group, const Pic::Data *data, const char *name)
  : Base(group, data, name), _deviceMemory(0), _hasProtectedCode(false), _hasProtectedEeprom(false)
{
  if (data) _deviceMemory = new Pic::Memory(*data);
}

Programmer::PicBase::~PicBase()
{
  delete _deviceMemory;
}

void Programmer::PicBase::clear()
{
  ::Programmer::Base::clear();
  for (uint i=0; i<Pic::Nb_VoltageTypes; i++) {
    _voltages[i].error = false;
    _voltages[i].value = UNKNOWN_VOLTAGE;
  }
}

uint Programmer::PicBase::nbSteps(Operation operation) const
{
  uint length = Base::nbSteps(operation);
  if ( operation==Connect || operation==Erase ) return length;
  for (uint i=0; i<Pic::Nb_MemoryRangeTypes; i++) {
    Pic::MemoryRangeType type = Pic::MemoryRangeType(i);
    if ( type!=Pic::MemoryCode && type!=Pic::MemoryEeprom ) continue;
    if ( operation==Write && !device()->isWritable(type) ) continue;
    if ( operation==Write ) {
      length += device()->nbWords(type);
      if ( device()->memoryTechnology()==Device::Eprom ) length += nbSteps(Verify);
    }
    length += device()->nbWords(type);
  }
  return length;
}

bool Programmer::PicBase::readVoltages()
{
  if ( !hardware()->readVoltages(_voltages) ) return false;
  bool ok = true;
  for (uint i=0; i<Pic::Nb_VoltageTypes; i++) {
    if ( !group().canReadVoltage(Pic::VoltageType(i)) ) continue;
    if ( _voltages[i].error==true ) {
      ok = false;
      log(Log::Error, i18n("  %1 = %2 V: error in voltage level.").arg(i18n(Pic::VOLTAGE_TYPE_LABELS[i])).arg(_voltages[i].value));
    } else if ( _voltages[i].value!=UNKNOWN_VOLTAGE )
      log(Log::NormalDebug, QString("  %1 = %2 V").arg(i18n(Pic::VOLTAGE_TYPE_LABELS[i])).arg(_voltages[i].value));
  }
  return ok;
}

bool Programmer::PicBase::internalSetupHardware()
{
  if ( !Base::internalSetupHardware() ) return false;
  if ( group().properties() & ::Programmer::CanReleaseReset ) {
    log(Log::NormalDebug, "  Hold reset");
    if ( !hardware()->setTargetReset(Pic::ResetHeld) ) return false;
  }
  Pic::TargetMode mode;
  if ( !getTargetMode(mode) ) return false;
  if ( mode!=Pic::TargetInProgramming ) {
    log(Log::Error, i18n("Device not in programming"));
    return false;
  }
  return true;
}

bool Programmer::PicBase::initProgramming(Operation)
{
/*
  if ( vpp()!=UNKNOWN_VOLTAGE ) {
    const Pic::VoltageData &tvpp = device()->voltage(Pic::Vpp);
    if ( vpp()<tvpp.min )
      log(Log::Warning, i18n("Vpp (%1 V) is lower than the minimum required voltage (%2 V).")
                        .arg(vpp()).arg(tvpp.min));
    if ( vpp()>tvpp.max ) {
      QString s = i18n("Vpp (%1 V) is higher than the maximum voltage (%2 V). You may damage the device.")
                  .arg(vpp()).arg(tvpp.max);
      log(Log::Warning, s);
      if ( !askContinue(s) ) {
        logUserAbort();
        return false;
      }
    }
  }
  if ( vdd()!=UNKNOWN_VOLTAGE ) {
    Q_ASSERT( type!=Pic::Vpp );
    const Pic::VoltageData &tvdd = device()->voltage(type);
    if ( vdd()<tvdd.min ) {
      if ( type==Pic::VddBulkErase && device()->voltage(Pic::VddWrite).min!=tvdd.min )
        log(Log::Warning, i18n("Vdd (%1 V) is too low for high-voltage programming\n(piklab only supports high-voltage programming at the moment).\nMinimum required is %2 V.")
                          .arg(vdd()).arg(tvdd.min));
      else if ( type==Pic::VddRead && device()->voltage(Pic::VddWrite).min!=tvdd.min )
        log(Log::Warning, i18n("Vdd (%1 V) is too low for reading\nMinimum required is %2 V.")
                          .arg(vdd()).arg(tvdd.min));
      else log(Log::Warning, i18n("Vdd (%1 V) is too low for programming\nMinimum required is %2 V.")
                            .arg(vdd()).arg(tvdd.min));
    } else if ( vdd()>tvdd.max ) {
      QString s = i18n("Vdd (%1 V) is higher than the maximum voltage (%2 V). You may damage the device.")
                  .arg(vdd()).arg(tvdd.max);
      log(Log::Warning, s);
      if ( !askContinue(s) ) {
        logUserAbort();
        return false;
      }
    }
  }
*/
  if ( specific()->canReadRange(Pic::MemoryConfig) ) {
    // read config
    Device::Array data;
    if ( !specific()->read(Pic::MemoryConfig, data, 0) ) return false;
    _deviceMemory->setArray(Pic::MemoryConfig, data);
    _hasProtectedCode = _deviceMemory->isProtected(Pic::Protection::ProgramProtected, Pic::MemoryCode);
    _hasProtectedEeprom = _deviceMemory->isProtected(Pic::Protection::ProgramProtected, Pic::MemoryEeprom);
    log(Log::NormalDebug, QString("  protected: code=%1 data=%2")
                          .arg(_hasProtectedCode ? "true" : "false").arg(_hasProtectedEeprom ? "true" : "false"));
    // read calibration
    if ( !readCalibration() ) return false;
  }

  return initProgramming();
}

bool Programmer::PicBase::preserveCode()
{
  if ( _hasProtectedCode && !askContinue(i18n("All or part of code memory is protected so cannot be preserved. Continue anyway?")) )
    return false;
  return readRange(Pic::MemoryCode, _deviceMemory, 0);
}

bool Programmer::PicBase::preserveEeprom()
{
  if ( _hasProtectedEeprom && !askContinue(i18n("All or part of data EEPROM is protected so cannot be preserved. Continue anyway?")) )
    return false;
  return readRange(Pic::MemoryEeprom, _deviceMemory, 0);
}

bool Programmer::PicBase::internalRun()
{
  _state = ::Programmer::Running;
  return hardware()->setTargetReset(Pic::ResetReleased);
}

bool Programmer::PicBase::internalStop()
{
  _state = ::Programmer::Stopped;
  return hardware()->setTargetReset(Pic::ResetHeld);
}

bool Programmer::PicBase::getTargetMode(Pic::TargetMode &mode)
{
  return hardware()->getTargetMode(mode);
}

bool Programmer::PicBase::initProgramming()
{
  _state = ::Programmer::Stopped;
  return hardware()->setTargetReset(Pic::ResetHeld);
}

//-----------------------------------------------------------------------------
uint Programmer::PicBase::readDeviceId()
{
  Device::Array data;
  if ( !specific()->read(Pic::MemoryDeviceId, data, 0) ) return 0;
  Q_ASSERT( data.count()!=0 );
  uint id = 0;
  switch ( device()->architecture()) {
    case Pic::Arch_10X:
    case Pic::Arch_16X:
    case Pic::Arch_17X: id = data[0]; break;
    case Pic::Arch_18C:
    case Pic::Arch_18F:
    case Pic::Arch_18J: id = data[0] | (data[1] << 8); break;
    case Pic::Arch_24F:
    case Pic::Arch_24H:
    case Pic::Arch_30X: id = data[1] | (data[0] << 16); break;
    case Pic::Nb_Architectures: Q_ASSERT(false); break;
  }
  return id;
}

bool Programmer::PicBase::verifyDeviceId()
{
  if ( !specific()->canReadRange(Pic::MemoryDeviceId ) ) return true;
  if ( !device()->isReadable(Pic::MemoryDeviceId) ) {
    log(Log::Information, i18n("Device not autodetectable: continuing with the specified device name \"%1\"...").arg(device()->name()));
    return true;
  }
  uint rawId = readDeviceId();
  if ( hasError() ) return false;
  uint nbChars = device()->nbWords(Pic::MemoryDeviceId) * device()->nbCharsWord(Pic::MemoryDeviceId);
  if ( rawId==0 || uint(rawId)==device()->mask(Pic::MemoryDeviceId) ) {
    log(Log::Error, i18n("Missing or incorrect device (Read id is %1).").arg(toHexLabel(rawId, nbChars)));
    return false;
  }
  Device::IdList ids;
  QValueVector<QString> names = group().supportedDevices();
  for (uint k=0; k<uint(names.count()); k++) {
    const Device::Data *data = group().deviceData(names[k]);
    if ( data->group().name()!="pic"
         || static_cast<const Pic::Data *>(data)->architecture()!=device()->architecture() ) continue;
    Device::IdData idata;
    if ( data->matchId(rawId, idata) ) ids[names[k]] = idata;
    }
  QString message;
  if ( ids.count()!=0 ) {
    log(Log::Information, i18n("Read id: %1").arg(device()->idNames(ids).join("; ")));
    if ( ids.contains(device()->name()) ) return true;
    message = i18n("Read id does not match the specified device name \"%1\".").arg(device()->name());
  } else {
    log(Log::Warning, i18n("  Unknown or incorrect device (Read id is %1).").arg(toHexLabel(rawId, nbChars)));
    message = i18n("Unknown device.");
  }
  if ( !askContinue(message) ) {
    logUserAbort();
    return false;
  }
  log(Log::Information, i18n("Continue with the specified device name: \"%1\"...").arg(device()->name()));
  return true;
}

//-----------------------------------------------------------------------------
QString Programmer::PicBase::prettyCalibration(const Device::Array &data) const
{
  QString s;
  for (uint i=0; i<data.count(); i++) {
    if ( i!=0 ) s += ", ";
    s += toHexLabel(data[i], device()->nbCharsWord(Pic::MemoryCal));
  }
  return s;
}

bool Programmer::PicBase::readCalibration()
{
  if ( device()->isReadable(Pic::MemoryCal) ) {
    if ( !specific()->canReadRange(Pic::MemoryCal) ) {
      log(Log::Warning, i18n("Osccal cannot be read by the selected programmer"));
      return true;
    }
    Device::Array data;
    if ( !specific()->read(Pic::MemoryCal, data, 0) ) return false;
    _deviceMemory->setArray(Pic::MemoryCal, data);
    log(Log::NormalDebug, QString("  Read osccal: %1").arg(prettyCalibration(data)));
    QString message;
    if ( !device()->checkCalibration(data, &message) ) log(Log::Warning, "  " + message);
    if ( device()->isReadable(Pic::MemoryCalBackup) ) {
      if ( !specific()->canReadRange(Pic::MemoryCalBackup) ) {
        log(Log::Warning, i18n("Osccal backup cannot be read by the selected programmer"));
        return true;
      }
      if ( !specific()->read(Pic::MemoryCalBackup, data, 0) ) return false;
      _deviceMemory->setArray(Pic::MemoryCalBackup, data);
      log(Log::NormalDebug, QString("  Read osccal backup: %1").arg(prettyCalibration(data)));
      if ( !device()->checkCalibration(data, &message) ) log(Log::Warning, "  " + message);
    }
  }
  return true;
}

bool Programmer::PicBase::restoreCalibration()
{
  if ( !specific()->canReadRange(Pic::MemoryCal) || !specific()->canWriteRange(Pic::MemoryCal) ) return true;
  if ( !device()->isWritable(Pic::MemoryCal) ) return true;
  Device::Array data = _deviceMemory->arrayForWriting(Pic::MemoryCal);
  Device::Array bdata = _deviceMemory->arrayForWriting(Pic::MemoryCalBackup);
  if ( device()->isReadable(Pic::MemoryCalBackup) && specific()->canReadRange(Pic::MemoryCalBackup) ) {
    if ( !device()->checkCalibration(data) && device()->checkCalibration(bdata) ) {
      log(Log::Information, i18n("  Replace invalid osccal with backup value."));
      data = bdata;
    }
  }
  Device::Array cdata;
  if ( !specific()->read(Pic::MemoryCal, cdata, 0) ) return false;
  if ( cdata==data ) {
    log(Log::Information, i18n("  Osccal is unchanged."));
    return true;
  }
  if ( !programRange(Pic::MemoryCal, data) ) return false;
  if ( !specific()->read(Pic::MemoryCal, cdata, 0) ) return false;
  if ( cdata==data ) log(Log::Information, i18n("  Osccal has been preserved."));

  if ( !device()->isWritable(Pic::MemoryCalBackup) || !device()->checkCalibration(bdata) ) return true;
  if ( !specific()->read(Pic::MemoryCalBackup, cdata, 0) ) return false;
  if ( cdata.count()==0 ) {
    log(Log::Warning, i18n("Osccal backup cannot be read by selected programmer"));
    return true;
  }
  if ( cdata==bdata ) {
    log(Log::Information, i18n("  Osccal backup is unchanged."));
    return true;
  }
  if ( !programRange(Pic::MemoryCalBackup, bdata) ) return false;
  if ( !specific()->read(Pic::MemoryCalBackup, cdata, 0) ) return false;
  if ( cdata==bdata ) log(Log::Information, i18n("  Osccal backup has been preserved."));
  return true;
}

bool Programmer::PicBase::restoreBandGapBits()
{
  if ( !specific()->canReadRange(Pic::MemoryConfig) ) return true;
  bool hasProtectedBits = false;
  for (uint i=0; i<device()->nbWords(Pic::MemoryConfig); i++)
    if ( device()->config()._words[i].pmask!=0 ) hasProtectedBits = true;
  if ( !hasProtectedBits ) return true;
  Device::Array cdata;
  if ( !specific()->read(Pic::MemoryConfig, cdata, 0) ) return false;
  Device::Array data = _deviceMemory->arrayForWriting(Pic::MemoryConfig);
  for (uint i=0; i<cdata.count(); i++) {
    uint pmask = device()->config()._words[i].pmask;
    if ( pmask==0 ) continue;
    cdata[i] &= ~pmask;
    cdata[i] |= (data[i] & pmask);
  }
  if ( !specific()->canWriteRange(Pic::MemoryConfig) ) {
    log(Log::Warning, i18n("Could not restore band gap bits because programmer does not support writing config bits."));
    return true;
  }
  log(Log::NormalDebug, QString("  Write config with band gap bits: %2").arg(toHexLabel(cdata[0], device()->nbCharsWord(Pic::MemoryConfig))));
  if ( !programRange(Pic::MemoryConfig, cdata) ) return false;
  if ( !specific()->read(Pic::MemoryConfig, data, 0) ) return false;
  if ( data==cdata ) log(Log::Information, i18n("  Band gap bits have been preserved."));
  return true;
}

bool Programmer::PicBase::eraseAll()
{
  if ( !specific()->canEraseAll() ) {
    log(Log::SoftError, i18n("The selected programmer does not support erasing the whole device."));
    return false;
  }
  if ( !specific()->erase(_hasProtectedCode || _hasProtectedEeprom) ) return false;
  addProgress(OPERATION_DATA[Erase].nbSteps);
  if ( !restoreCalibration() ) return false;
  return true;
}

bool Programmer::PicBase::checkErase()
{
  if ( device()->memoryTechnology()==Device::Rom || device()->memoryTechnology()==Device::Romless
       || device()->memoryTechnology()==Device::Eprom ) {
    log(Log::SoftError, i18n("Cannot erase ROM or EPROM device."));
    return false;
  }
  return true;
}

bool Programmer::PicBase::internalErase(const Device::MemoryRange &range)
{
  if ( !initProgramming(Erase) ) return false;
  bool ok = true;
  if ( range.all() ) ok = eraseAll();
  else ok = eraseRange(static_cast<const Pic::MemoryRange &>(range)._type);
  if ( !restoreBandGapBits() ) return false;
  return ok;
}

bool Programmer::PicBase::eraseSingle(Pic::MemoryRangeType type)
{
  return erase(Pic::MemoryRange(type));
}

bool Programmer::PicBase::eraseRange(Pic::MemoryRangeType type)
{
  bool ok = internalEraseRange(type);
  if ( !restoreCalibration() ) return false;
  Config config;
  if ( ok && config.blankCheckAfterErase() ) {
    Pic::Memory memory(*device());
    VerifyData vdata(BlankCheckVerify, memory);
    return readRange(type, 0, &vdata);
  }
  return ok;
}

bool Programmer::PicBase::internalEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryCode && _hasProtectedCode ) {
    log(Log::SoftError, i18n("Cannot erase protected code memory. Consider erasing the whole chip."));
    return false;
  }
  if ( type==Pic::MemoryEeprom && _hasProtectedEeprom ) {
    log(Log::SoftError, i18n("Cannot erase protected data EEPROM. Consider erasing the whole chip."));
    return false;
  }
  if ( specific()->canEraseRange(type) )  return specific()->eraseRange(type);
  bool softErase = true;
  if ( type!=Pic::MemoryCode && (!specific()->canReadRange(Pic::MemoryCode)
       || !specific()->canWriteRange(Pic::MemoryCode)) ) softErase = false;
  if ( type!=Pic::MemoryEeprom && (!specific()->canReadRange(Pic::MemoryEeprom)
       || !specific()->canWriteRange(Pic::MemoryEeprom)) ) softErase = false;
  if ( type!=Pic::MemoryConfig && (!specific()->canReadRange(Pic::MemoryConfig)
       || !specific()->canWriteRange(Pic::MemoryConfig)) ) softErase = false;
  if ( type!=Pic::MemoryUserId && (!specific()->canReadRange(Pic::MemoryUserId)
       || !specific()->canWriteRange(Pic::MemoryUserId)) ) softErase = false;
  if ( !softErase ) {
    log(Log::SoftError, i18n("Cannot erase specified range because of programmer limitations."));
    return false;
  }
  if ( !askContinue(i18n("%1: Erasing this range only is not supported with this programmer. This will erase the whole chip and restore the other memory ranges.").arg(i18n(Pic::MEMORY_RANGE_TYPE_DATA[type].label))) ) {
    logUserAbort();
    return false;
  }
  if ( type!=Pic::MemoryCode && !preserveCode() ) return false;
  if ( type!=Pic::MemoryEeprom && !preserveEeprom() ) return false;
  if ( type!=Pic::MemoryUserId && !readRange(Pic::MemoryUserId, _deviceMemory, 0) ) return false;
  specific()->erase(_hasProtectedCode || _hasProtectedEeprom);
  if ( type!=Pic::MemoryCode && !programAndVerifyRange(Pic::MemoryCode, *_deviceMemory) ) return false;
  if ( type!=Pic::MemoryEeprom && !programAndVerifyRange(Pic::MemoryEeprom, *_deviceMemory) ) return false;
  if ( type!=Pic::MemoryUserId && !programAndVerifyRange(Pic::MemoryUserId, *_deviceMemory) ) return false;
  if ( !programAndVerifyRange(Pic::MemoryConfig, *_deviceMemory) ) return false;
  return true;
}

//-----------------------------------------------------------------------------
bool Programmer::PicBase::readSingle(Pic::MemoryRangeType type, Pic::Memory &memory)
{
  if ( !specific()->canReadRange(type) ) {
    log(Log::SoftError, i18n("The selected programmer cannot read the specified memory range."));
    return false;
  }
  Pic::Memory tmp(*device());
  if ( !read(tmp, Pic::MemoryRange(type)) ) return false;
  memory.copyFrom(type, tmp);
  if ( type==Pic::MemoryCal ) memory.copyFrom(Pic::MemoryCalBackup, tmp);
  return true;
}

bool Programmer::PicBase::readRange(Pic::MemoryRangeType type, Pic::Memory *memory, const VerifyData *vd)
{
  if ( !device()->isReadable(type) ) return true;
  VerifyData *vdata = (vd ? new VerifyData(vd->actions, vd->memory) : 0);
  if (vdata) {
    log(Log::Information, i18n("  Verify memory: %1").arg(i18n(Pic::MEMORY_RANGE_TYPE_DATA[type].label)));
    if ( !(vdata->actions & IgnoreProtectedVerify) ) {
      vdata->protectedRanges = static_cast<const Pic::Memory &>(vdata->memory).protectedRanges(Pic::Protection::ProgramProtected, type);
      if ( !vdata->protectedRanges.isEmpty() ) log(Log::Warning, i18n("  Part of device memory is protected (in %1) and cannot be verified.")
                                               .arg(i18n(Pic::MEMORY_RANGE_TYPE_DATA[type].label)));
    } else vdata->protectedRanges.clear();
  } else {
    log(Log::Information, i18n("  Read memory: %1").arg(i18n(Pic::MEMORY_RANGE_TYPE_DATA[type].label)));
    Q_ASSERT(memory);
  }
  bool ok = true;
  Device::Array data;
  if ( specific()->canReadRange(type) ) ok = specific()->read(type, data, vdata);
  else log(Log::Warning, i18n("The selected programmer cannot read memory range: operation skipped."));
  delete vdata;
  if (!ok) return false;
  if (memory) memory->setArray(type, data);
  return true;
}

bool Programmer::PicBase::checkRead()
{
  if ( device()->memoryTechnology()==Device::Romless ) {
    log(Log::SoftError, i18n("Cannot read ROMless device."));
    return false;
  }
  return true;
}

bool Programmer::PicBase::internalRead(Device::Memory *memory, const Device::MemoryRange &range, const VerifyData *vdata)
{
  if ( !initProgramming(Read) ) return false;
  Pic::Memory *pmemory = static_cast<Pic::Memory *>(memory);
  if ( !range.all() ) {
    Pic::MemoryRangeType type = static_cast<const Pic::MemoryRange &>(range)._type;
    if ( type==Pic::MemoryCal ) {
      if ( !readRange(Pic::MemoryCal, pmemory, vdata) ) return false;
      return readRange(Pic::MemoryCalBackup, pmemory, vdata);
    }
    return readRange(type, pmemory, vdata);
  }
  if ( !readRange(Pic::MemoryConfig,    pmemory, vdata) ) return false;
  if ( !readRange(Pic::MemoryUserId,    pmemory, vdata) ) return false;
  if ( vdata==0 ) if ( !readRange(Pic::MemoryCal,       pmemory, 0) ) return false;
  if ( vdata==0 ) if ( !readRange(Pic::MemoryCalBackup, pmemory, 0) ) return false;
  if ( !readRange(Pic::MemoryCode,      pmemory, vdata) ) return false;
  if ( !readRange(Pic::MemoryEeprom,    pmemory, vdata) ) return false;
  return true;
}

//-----------------------------------------------------------------------------
bool Programmer::PicBase::programSingle(Pic::MemoryRangeType type, const Pic::Memory &memory)
{
  if ( !specific()->canWriteRange(type) ) {
    log(Log::SoftError, i18n("The selected programmer cannot read the specified memory range."));
    return false;
  }
  return program(memory, Pic::MemoryRange(type));
}

bool Programmer::PicBase::programRange(Pic::MemoryRangeType mtype, const Device::Array &data)
{
  log(Log::Information, i18n("  Write memory: %1").arg(i18n(Pic::MEMORY_RANGE_TYPE_DATA[mtype].label)));
  Config config;
  bool only = ( config.onlyProgramNonMask() && (mtype==Pic::MemoryCode || mtype==Pic::MemoryEeprom) );
  return specific()->write(mtype, data, !only);
}

bool Programmer::PicBase::programAndVerifyRange(Pic::MemoryRangeType mtype, const Pic::Memory &memory)
{
  if ( !device()->isWritable(mtype) ) return true;
  Device::Array data = memory.arrayForWriting(mtype);
  if ( !programRange(mtype, data) ) return false;
  VerifyActions actions = IgnoreProtectedVerify;
  Config config;
  if ( mtype==Pic::MemoryCode && config.onlyVerifyProgrammed() ) actions |= OnlyProgrammedVerify;
  VerifyData vdata(actions, memory);
  return readRange(mtype, 0, &vdata);
}

bool Programmer::PicBase::programAll(const Pic::Memory &memory)
{
  if ( !programAndVerifyRange(Pic::MemoryCode, memory) ) return false;
  Config config;
  const Pic::Memory &tmp = (config.preserveEeprom() ? *_deviceMemory : memory);
  if ( !programAndVerifyRange(Pic::MemoryEeprom, tmp) ) return false;
  if ( !programAndVerifyRange(Pic::MemoryUserId, memory) ) return false;
  if ( memory.isProtected(Pic::Protection::WriteProtected, Pic::MemoryConfig) ) {
    log(Log::NormalDebug, "  Config write protection is on: first program without it and then with it");
    Pic::Memory tmp(memory.device());
    tmp.copyFrom(Pic::MemoryConfig, memory);
    tmp.setProtection(false, Pic::Protection::WriteProtected, Pic::MemoryConfig);
    if ( !programAndVerifyRange(Pic::MemoryConfig, tmp) ) return false;
  }
  if ( !programAndVerifyRange(Pic::MemoryConfig, memory) ) return false;
  return true;
}

bool Programmer::PicBase::checkProgram(const Device::Memory &memory)
{
  if ( device()->memoryTechnology()==Device::Rom || device()->memoryTechnology()==Device::Romless ) {
    log(Log::SoftError, i18n("Cannot write ROM or ROMless device."));
    return false;
  }
  if ( !group().isDebugger() && static_cast<const Pic::Memory &>(memory).hasDebugOn() ) {
    if ( !askContinue(i18n("DEBUG configuration bit is on. Are you sure you want to continue programming the chip?")) ) {
      logUserAbort();
      return false;
    }
  }
  return true;
}

bool Programmer::PicBase::internalProgram(const Device::Memory &memory, const Device::MemoryRange &range)
{
  if ( !initProgramming(Erase) ) return false;
  const Pic::Memory &pmemory = static_cast<const Pic::Memory &>(memory);

  // blank check if OTP device
  bool eprom = ( device()->memoryTechnology()==Device::Eprom );
  if (eprom) {
    log(Log::Information, i18n("  EPROM device: blanck checking first..."));
    Pic::Memory memory(*device());
    VerifyData vdata(BlankCheckVerify, memory);
    if ( !internalRead(0, range, &vdata) ) return false;
    log(Log::Information, i18n("  Blank check successful"));
    // check if protecting device
    bool protectedCode = pmemory.isProtected(Pic::Protection::ProgramProtected, Pic::MemoryCode);
    bool protectedEeprom = pmemory.isProtected(Pic::Protection::ProgramProtected, Pic::MemoryEeprom);
    if ( protectedCode || protectedEeprom ) {
      log(Log::SoftError, i18n("Protecting code memory or data EEPROM on OTP devices is disabled as a security..."));
      return false;
    }
  }

  // programming
  bool ok = true;
  if ( !range.all() ) {
    Pic::MemoryRangeType type = static_cast<const Pic::MemoryRange &>(range)._type;
    if ( (type==Pic::MemoryCode && _hasProtectedCode) || (type==Pic::MemoryEeprom && _hasProtectedEeprom) ) {
      log(Log::SoftError, i18n("This memory range is programming protected."));
      return false;
    }
    if ( specific()->canEraseRange(type) ) {
      if ( !specific()->emulatedErase() && !eraseRange(type) ) return false;
    } else log(Log::Warning, i18n("The range cannot be erased first by the selected programmer so programming may fail..."));
    addProgress(OPERATION_DATA[Erase].nbSteps);
    ok = programRange(type, pmemory.arrayForWriting(type));
    VerifyData vdata(NormalVerify, pmemory);
    if (ok) ok = readRange(type, 0, &vdata);
  } else {
    if (eprom) addProgress(OPERATION_DATA[Erase].nbSteps);
    else {
      if ( specific()->canEraseAll() ) {
        if ( !specific()->emulatedErase() ) {
          log(Log::Information, i18n("  Erasing device"));
          Config config;
          ok = ( !config.preserveEeprom() || preserveEeprom() );
          if (ok) ok = eraseAll();
        }
      } else log(Log::Warning, i18n("The device cannot be erased first by the selected programmer so programming may fail..."));
    }
    if (ok) ok = programAll(pmemory);
  }
  if ( !restoreBandGapBits() ) return false;
  return ok;
}

//-----------------------------------------------------------------------------
bool Programmer::PicBase::checkProgramCalibration(const Device::Array &data)
{
  QString message, s = prettyCalibration(data);
  if ( !device()->checkCalibration(data, &message) ) {
    sorry(i18n("The calibration word %1 is not valid.").arg(s), message);
    return false;
  }
  return askContinue(i18n("Do you want to overwrite the device calibration with %1?").arg(s));
}

bool Programmer::PicBase::tryProgramCalibration(const Device::Array &data, bool &success)
{
  log(Log::Information, i18n("  Write memory: %1").arg(i18n(Pic::MEMORY_RANGE_TYPE_DATA[Pic::MemoryCal].label)));
  success = true;
  if ( !specific()->write(Pic::MemoryCal, data, true) ) return false;
  Device::Array read;
  if ( !specific()->read(Pic::MemoryCal, read, 0) ) return false;
  for (uint i=0; i<data.count(); i++)
    if ( data[i]!=read[i] ) success = false;
  if ( !success ) return true;
  if ( device()->isWritable(Pic::MemoryCalBackup) ) {
    if ( !specific()->read(Pic::MemoryCalBackup, read, 0) ) return false;
    if ( device()->checkCalibration(read) ) return true; // do not overwrite correct backup value
    log(Log::Information, i18n("  Write memory: %1").arg(i18n(Pic::MEMORY_RANGE_TYPE_DATA[Pic::MemoryCalBackup].label)));
    if ( !specific()->write(Pic::MemoryCalBackup, data, true) ) return false;
    if ( !specific()->read(Pic::MemoryCalBackup, read, 0) ) return false;
    for (uint i=0; i<data.count(); i++)
      if ( data[i]!=read[i] ) success = false;
  }
  return true;
}

bool Programmer::PicBase::internalProgramCalibration(const Device::Array &data)
{
  if ( !initProgramming(Write) ) return false;
  // try without erase
  bool success;
  if ( !tryProgramCalibration(data, success) ) return false;
  if (success) return true;
  if ( !askContinue(i18n("Programming calibration data needs a chip erase. Continue anyway?")) ) {
    logUserAbort();
    return false;
  }
  log(Log::Information, i18n("  Erasing device"));
  bool ok = specific()->erase(_hasProtectedCode || _hasProtectedEeprom);
  if ( !restoreBandGapBits() ) return false;
  if ( !ok ) return false;
  // retry
  if ( !tryProgramCalibration(data, success) ) return false;
  return success;
}

bool Programmer::PicBase::programCalibration(const Device::Array &data)
{
  prepare(Connect);
  emit actionMessage(i18n("Programming calibration..."));
  bool ok = doProgramCalibration(data);
  endProgramming();
  return ok;
}

bool Programmer::PicBase::doProgramCalibration(const Device::Array &data)
{
  if ( !checkProgramCalibration(data) ) return false;
  if ( !doConnectDevice() ) return false;
  log(Log::Information, i18n("Programming calibration..."));
  if ( !internalProgramCalibration(data) ) return false;
  log(Log::Information, i18n("Programming calibration successful"));
  return true;
}

//-----------------------------------------------------------------------------
bool Programmer::PicBase::verifySingle(Pic::MemoryRangeType type, const Pic::Memory &memory)
{
  return verify(memory, Pic::MemoryRange(type));
}

bool Programmer::PicBase::blankCheckSingle(Pic::MemoryRangeType type)
{
  return blankCheck(Pic::MemoryRange(type));
}
