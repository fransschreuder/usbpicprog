/***************************************************************************
 * Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit2v2.h"

#include "progs/base/prog_config.h"
#include "progs/icd2/base/microchip.h"
#include "pickit2v2_data.h"

//-----------------------------------------------------------------------------
const Pickit2V2::FamilyData *Pickit2V2::familyData(const Pic::Data &device)
{
  for (uint i=0; FAMILY_DATA[i].architecture!=Pic::Nb_Architectures; i++)
    if ( FAMILY_DATA[i].architecture==device.architecture() ) return &FAMILY_DATA[i];
  Q_ASSERT(false);
  return 0;
}

//-----------------------------------------------------------------------------
Pickit2V2::Hardware::Hardware(::Programmer::Base &base)
  : ::Programmer::PicHardware(base, new USBPort(base)),
    _scriptBufferChecksum(0), _deviceSet(false)
{}

bool Pickit2V2::Hardware::internalConnectHardware()
{
  return port().open();
}

bool Pickit2V2::Hardware::setTarget()
{
  setIcspSpeed(0); // #### ??
  _fastProgramming = true; // #### ??
  uint checksum;
  if ( !getScriptBufferChecksum(checksum) ) return false;
  if ( !_deviceSet || _scriptBufferChecksum!=checksum ) {
    if ( !downloadScripts() ) return false;
  }
  _deviceSet = true;
  return true;
}

bool Pickit2V2::Hardware::readStatus(ushort &status)
{
  if ( !port().command(ReadStatus) ) return false;
  Array a;
  if ( !port().receive(a) ) return false;
  status = (a[1] << 8) + a[0];
  return true;
}

bool Pickit2V2::Hardware::sendScript(const ushort *script, uint length)
{
  Array cmd;
  cmd[0] = ClearUploadBuffer;
  cmd[1] = ExecuteScript;
  cmd[2] = length;
  for (uint i=0; i<length; i++) cmd[3+i] = script[i];
  return port().command(cmd);
}

bool Pickit2V2::Hardware::executeScript(uint i)
{
  Q_ASSERT( i!=0 );
  const ScriptData &sdata = SCRIPT_DATA[i-1];
  return sendScript(sdata.data, sdata.length);
}

bool Pickit2V2::Hardware::getScriptBufferChecksum(uint &checksum)
{
  if ( !port().command(ScriptBufferChecksum) ) return false;
  Array array;
  if ( !port().receive(array) ) return false;
  checksum = (array[0] << 24) + (array[1] << 16) + (array[2] << 8) + array[3];
  return true;
}

bool Pickit2V2::Hardware::downloadScript(ScriptType type, uint i)
{
  if (i==0 ) return true; // empty script
  const ScriptData &sdata = SCRIPT_DATA[i-1];
  Array cmd;
  cmd[0] = DownloadScript;
  cmd[1] = type;
  cmd[2] = sdata.length;
  for (uint k=0; k<sdata.length; k++) {
    if ( !_fastProgramming && sdata.data[k]==0xAAE7 ) {
      ushort next = sdata.data[k+1];
      if ( (next & 0xFF)<170 && next!=0 ) {
        cmd[3+k] = sdata.data[k];
        cmd[3+k+1] = next + next/2;
      } else {
        cmd[3+k] = DelayLong;
        cmd[3+k+1] = 2;
      }
      k++;
    } else if ( !_fastProgramming && sdata.data[k]==0xAAE8 ) {
      ushort next = sdata.data[k+1];
      if ( (next & 0xFF)<171 && next!=0 ) {
        cmd[3+k] = sdata.data[k];
        cmd[3+k+1] = next + next/2;
      } else {
        cmd[3+k] = DelayLong;
        cmd[3+k+1] = 0;
      }
      k++;
    } else cmd[3+k] = sdata.data[k];
  }
  return port().command(cmd);
}

bool Pickit2V2::Hardware::downloadScripts()
{
  if ( !port().command(ClearScriptBuffer) ) return false;
  const Data &d = data(device().name());
  for (uint i=0; i<Nb_ScriptTypes; i++)
    if ( !downloadScript(ScriptType(i), d.scriptIndexes[i]) ) return false;
  return getScriptBufferChecksum(_scriptBufferChecksum);
}

bool Pickit2V2::Hardware::setTargetReset(Pic::ResetMode mode)
{
  ushort script[1];
  switch (mode) {
    case Pic::ResetHeld: script[0] = MclrGroundOn; break;
    case Pic::ResetReleased: script[0] = MclrGroundOff; break;
    case Pic::Nb_ResetModes: Q_ASSERT(false); return false;
  }
  return sendScript(script, 1);
}

bool Pickit2V2::Hardware::setVddVoltage(double value, double threshold)
{
  ushort v = ushort(qRound(32.0*value + 10.5)) << 6;
  uchar vfault = uchar(qRound(256.0 * (threshold * value) / 5.0));
  if ( vfault>210 ) vfault = 210;
  Array cmd;
  cmd[0] = SetVdd;
  cmd[1] = v & 0xFF;
  cmd[2] = v >> 8;
  cmd[3] = vfault;
  return port().command(cmd);
}

bool Pickit2V2::Hardware::setVppVoltage(double value, double threshold)
{
  Array cmd;
  cmd[0] = SetVpp;
  cmd[1] = 0x40;
  cmd[2] = uchar(qRound(18.61*value));
  cmd[3] = uchar(qRound(18.61*value*threshold));
  return port().command(cmd);
}

bool Pickit2V2::Hardware::setVddOn(bool on)
{
  ushort script[2];
  script[0] = (on ? VddGroundOff : VddOff);
  ::Programmer::Config config;
  if ( config.isTargetSelfPowered() ) script[1] = (on ? VddOff : VddGroundOff);
  else script[1] = (on ? VddOn : VddGroundOn);
  return sendScript(script, 2);
}

bool Pickit2V2::Hardware::setIcspSpeed(uchar speed)
{
  ushort script[2];
  script[0] = SetIcspSpeed;
  script[1] = speed;
  return sendScript(script, 2);
}

bool Pickit2V2::Hardware::getMode(VersionData &version, ::Programmer::Mode &mode)
{
  if ( !port().command(FirmwareVersion) ) return false;
  Array data;
  if ( !port().receive(data) ) return false;
  if ( data[0]=='B' ) {
    mode = ::Programmer::BootloadMode;
    version = VersionData();
  } else {
    mode = ::Programmer::NormalMode;
    version = VersionData(data[0], data[1], data[2]);
  }
  return true;
}

bool Pickit2V2::Hardware::readVoltages(VoltagesData &voltagesData)
{
  if ( !port().command(ReadVoltages) ) return false;
  Array array;
  if ( !port().receive(array) ) return false;
  double vadc = 256 * array[1] + array[0];
  voltagesData[Pic::TargetVdd].value = 5.0 * (vadc / 65536);
  voltagesData[Pic::TargetVdd].error = false;
  vadc = 256 * array[3] + array[2];
  voltagesData[Pic::TargetVpp].value = 13.7 * (vadc / 65536);
  voltagesData[Pic::TargetVpp].error = false;
  return true;
}

bool Pickit2V2::Hardware::downloadAddress(uint address)
{
  Array cmd;
  cmd[0] = ClearDownloadBuffer;
  cmd[1] = DownloadData;
  cmd[2] = 3;
  cmd[3] = address & 0xFF;
  cmd[4] = (address >> 8) & 0xFF;
  cmd[5] = (address >> 16) & 0xFF;
  return port().command(cmd);
}

bool Pickit2V2::Hardware::runScript(ScriptType stype, uint repetitions, uint nbNoLens)
{
  log(Log::MaxDebug, QString("run script %1: repetitions=%2 nbNoLen=%3").arg(stype).arg(repetitions).arg(nbNoLens));
#if 0 // ALTERNATE METHOD
  const Data &d = data(device().name());
  for (uint i=0; i<repetitions; i++)
    if ( !executeScript(d.scriptIndexes[stype]) ) return false;
  if (nbNoLens) {
    Array cmd;
    for (uint i=0; i<nbNoLens; i++) cmd[i] = UploadDataNoLen;
    if ( !port().command(cmd) ) return false;
  }
#else
  Array cmd;
  cmd[0] = ClearUploadBuffer;
  cmd[1] = RunScript;
  cmd[2] = stype;
  cmd[3] = repetitions;
  for (uint i=0; i<nbNoLens; i++) cmd[4+i] = UploadDataNoLen;
  if ( !port().command(cmd) ) return false;
#endif
  if ( stype==ProgExit && !setTargetReset(Pic::ResetReleased) ) return false;
  return true;
}

bool Pickit2V2::Hardware::prepareRead(Pic::MemoryRangeType type, uint wordIndex)
{
  ScriptType stype = prepareReadScript(type);
  if ( stype==Nb_ScriptTypes || data(device().name()).scriptIndexes[stype]==0 ) return true;
  switch (type) {
  case Pic::MemoryCode:
    if ( !Pic::ARCHITECTURE_DATA[device().architecture()].hasAddressAccess ) return true;
    if ( !downloadAddress(0x10000 * (wordIndex / 0x8000)) ) return false;
    break;
  case Pic::MemoryEeprom:
    if ( device().nbBytesWord(Pic::MemoryEeprom)==4 ) {
      if ( !downloadAddress(device().range(Pic::MemoryEeprom).start) ) return false; // #### correct ?
    } else if ( !downloadAddress(0) ) return false;
    break;
  case Pic::MemoryConfig: return true; // ConfigPrepareRead unused (?)
  case Pic::MemoryUserId: break;
  case Pic::MemoryCal: {
    uint offset = device().range(Pic::MemoryCal).start;
    Q_ASSERT( offset==device().range(Pic::MemoryCode).end+1 );
    return downloadAddress(offset);
  }
  default: return true;
  case Pic::Nb_MemoryRangeTypes: Q_ASSERT(false); return false;
  }
  return runScript(stype);
}

bool Pickit2V2::Hardware::readMemory(Pic::MemoryRangeType otype, Device::Array &data, const ::Programmer::VerifyData *vdata)
{
  uint nbWords = device().nbWords(otype);
  data.resize(nbWords);
  log(Log::MaxDebug, QString("read %1 nbWords=%2").arg(Pic::MEMORY_RANGE_TYPE_DATA[otype].label).arg(nbWords));
  uint nbBytesWord = device().nbBytesWord(otype);
  uint wordOffset = 0;
  Pic::MemoryRangeType type = otype;
  if ( otype==Pic::MemoryConfig && device().range(Pic::MemoryConfig).start==device().range(Pic::MemoryCode).end+1 ) {  // config in code memory
    wordOffset = device().range(Pic::MemoryConfig).start;
    type = Pic::MemoryCode;
  }
  bool setAddress = true;
  ScriptType stype = readScript(type);
  Q_ASSERT( stype!=Nb_ScriptTypes );
  const FamilyData *fdata = familyData(device());
  uint nbRunWords = QMIN(UploadBufferNbBytes / nbBytesWord, nbWords);
  uint nbRuns = 1;
  uint nbReceive = nbRunWords / UploadBufferNbBytes + 1;
  switch (type) {
    case Pic::MemoryCode:   nbRuns = nbRunWords / Pickit2V2::data(device().name()).codeMemoryNbReadWords; break;
    case Pic::MemoryEeprom: nbRuns = nbRunWords / Pickit2V2::data(device().name()).eepromMemoryNbReadWords; break;
    default: break;
  }

  if ( !runScript(ProgEntry) ) return false;
  for (uint i=0; i<nbWords; ) {
    if (setAddress) {
      setAddress = false;
      if ( !prepareRead(type, wordOffset + i) ) return false;
    }
    QValueVector<uint> words;
    if ( type==Pic::MemoryConfig || type==Pic::MemoryCal ) {
      if ( !runScript(stype, 1, 0) ) return false;
      if ( !port().command(UploadData) ) return false;
      if ( !port().receiveWords(nbBytesWord, nbReceive, words, 2) ) return false;
    } else {
      if ( !runScript(stype, nbRuns, nbReceive) ) return false;
      if ( !port().receiveWords(nbBytesWord, nbReceive, words) ) return false;
    }
    log(Log::MaxDebug, QString("nbRunWords=%1 readNbWords=%2 index=%3/%4").arg(nbRunWords).arg(words.count()).arg(i).arg(nbWords));
    for (uint k=0; k<words.count(); k++) {
      if ( i>=nbWords ) break;
      data[i] = words[k];
      if (fdata->progMemShift) data[i] >>= 1;
      if ( vdata && !verifyWord(i, data[i], type, *vdata) ) return false;
      if ( type==Pic::MemoryCode && i%0x8000==0 ) setAddress = true;
      i++;
    }
  }
  if ( !runScript(ProgExit) ) return false;
  return true;
}

bool Pickit2V2::Hardware::eraseAll()
{
  const Data &d = data(device().name());
  if ( d.scriptIndexes[ConfigErase]!=0 ) {
    if ( !runScript(ProgEntry) ) return false;
    if ( d.scriptIndexes[ConfigWritePrepare]!=0 ) {
      if ( !downloadAddress(0) ) return false;
      if ( !runScript(ConfigWritePrepare) ) return false;
    }
    if ( !runScript(ConfigErase) ) return false;
    if ( !runScript(ProgExit) ) return false;
  }
  if ( !runScript(ProgEntry) ) return false;
  if ( d.scriptIndexes[EraseChipPrepare]!=0 && !runScript(EraseChipPrepare) ) return false;
  if ( !runScript(EraseChip) ) return false;
  if ( !runScript(ProgExit) ) return false;
  return true;
}

bool Pickit2V2::Hardware::eraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryCode ) {
    if ( !runScript(ProgEntry) ) return false;
    if ( !runScript(ProgMemoryErase) ) return false;
    if ( !runScript(ProgExit) ) return false;
    return true;
  }
  if ( type==Pic::MemoryEeprom ) {
    if ( !runScript(ProgEntry) ) return false;
    if ( !runScript(EepromErase) ) return false;
    if ( !runScript(ProgExit) ) return false;
    return true;
  }
  Q_ASSERT(false);
  return false;
}

//-----------------------------------------------------------------------------
bool Pickit2V2::DeviceSpecific::canEraseRange(Pic::MemoryRangeType type) const
{
  const Data &d = data(device().name());
  if ( type==Pic::MemoryCode ) return d.scriptIndexes[ProgMemoryErase];
  if ( type==Pic::MemoryEeprom ) return d.scriptIndexes[EepromErase];
  return false;
}

bool Pickit2V2::DeviceSpecific::canReadRange(Pic::MemoryRangeType type) const
{
  const Data &d = data(device().name());
  if ( type==Pic::MemoryCal ) return d.scriptIndexes[OsccalRead];
  return true;
}

bool Pickit2V2::DeviceSpecific::canWriteRange(Pic::MemoryRangeType type) const
{
  const Data &d = data(device().name());
  if ( type==Pic::MemoryCal ) return d.scriptIndexes[OsccalWrite];
  return true;
}

bool Pickit2V2::DeviceSpecific::doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force)
{
  Q_ASSERT( data.size()==device().nbWords(type) );
  // #### TODO
  return false;
}
