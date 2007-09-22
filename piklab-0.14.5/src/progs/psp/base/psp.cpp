/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "psp.h"

#include "common/global/global.h"
#include "common/common/misc.h"

//-----------------------------------------------------------------------------
QMemArray<uchar> Psp::createConfigInfo(const Pic::Data &data)
{
  QMemArray<uchar> array(33);
  array.fill(0x00);

  const Pic::Config &config = data.config();
  for (uint i=0; i<uint(config._words.count()); i++) {
    uint v = config._words[i].usedMask();
    if ( data.nbBytesWord(Pic::MemoryUserId)==1 ) array[i ^ 1] = v;
    else {
      array[2*i] = v >> 8;
      array[2*i + 1] = v & 0xFF;
    }
  }
  for (uint i=0; i<16; i++) array[i + 16] = array[i];

  // checksum
  for (uint i=0; i<32; i++) array[32] += array[i];

  return array;
}

QMemArray<uchar> Psp::createDeviceInfo(const Pic::Data &data)
{
  QMemArray<uchar> config = createConfigInfo(data);
  QMemArray<uchar> array(45);
  array.fill(0x00);

  // memory code length
  uint v = data.nbWords(Pic::MemoryCode);
  array[0] = v >> 8;
  array[1] = v & 0xFF;
  // address word width
  v = data.mask(Pic::MemoryCode);
  array[2] = v >> 8;
  array[3] = v & 0xFF;
  // data word width
  array[4] = v >> 8;
  array[5] = v & 0xFF;

  if ( data.isReadable(Pic::MemoryUserId) ) {
    // user id width
    v = data.range(Pic::MemoryUserId).rmask;
    if ( data.nbBytesWord(Pic::MemoryUserId)==1 ) v += v << 8;
    array[6] = v >> 8;
    array[7] = v & 0xFF;
    // user id mask
    array[8] = v >> 8;
    array[9] = v & 0xFF;
  }

  // memory config mask
  array[10] = config[0];
  array[11] = config[1];
  array[12] = config[16];
  array[13] = config[17];

  if ( data.isReadable(Pic::MemoryEeprom) ) {
    // memory eeprom width
    v = data.mask(Pic::MemoryEeprom);
    array[14] = v >> 8;
    array[15] = v & 0xFF;
    // memory eeprom mask
    array[16] = v >> 8;
    array[17] = v & 0xFF;
  }

  if ( data.isReadable(Pic::MemoryCal) ) {
    // memory calibration width
    v = data.mask(Pic::MemoryCal);
    array[18] = v >> 8;
    array[19] = v & 0xFF;
    // memory calibration mask
    array[20] = v >> 8;
    array[21] = v & 0xFF;
  }

  // memory code start
  array[22] = 0x00;
  array[23] = 0x00;
  // memory code length
  array[24] = 0x00; // array[0]; in lplab and for some devices in picp...
  array[25] = 0x01; // array[1]; in lplab and for some devices in picp...

  if ( data.isReadable(Pic::MemoryUserId) ) {
    // user id start
    v = data.range(Pic::MemoryUserId).start;
    array[26] = v >> 8;
    array[27] = v & 0xFF;
    // user id length
    v = data.nbWords(Pic::MemoryUserId);
    if ( data.nbBytesWord(Pic::MemoryUserId)==1 ) v /= 2;
    array[28] = v;
  }

  if ( data.isReadable(Pic::MemoryConfig) ) {
    // config start
    v = data.range(Pic::MemoryConfig).start;
    array[29] = v >> 8;
    array[30] = v & 0xFF;
    // config length
    v = data.nbWords(Pic::MemoryConfig);
    if ( data.nbBytesWord(Pic::MemoryConfig)==1 ) v /= 2;
    array[31] = v;
  }

  if ( data.isReadable(Pic::MemoryConfig) ) {
    // eeprom start
    array[32] = 0x00;
    array[33] = 0x00;
    // eeprom length
    v = data.nbWords(Pic::MemoryEeprom);
    array[34] = v >> 8;
    array[35] = v & 0xFF;
  }

  if ( data.isReadable(Pic::MemoryCal) ) {
    // calibration start
    v = data.range(Pic::MemoryCal).start;
    array[36] = v >> 8;
    array[37] = v & 0xFF;
    // calibration length
    v = data.nbWords(Pic::MemoryCal);
    array[38] = v >> 8;
    array[39] = v & 0xFF;
  }

  // programming
  const Psp::Data &pdata = Psp::data(data.name());
  array[40] = pdata.overprogram;
  array[41] = pdata.tries;
  array[42] = pdata.algorithm;
  if ( data.memoryTechnology()==Device::EFlash || data.memoryTechnology()==Device::Flash ) array[43] |= 0x01;
  if ( data.isReadable(Pic::MemoryUserId) ) array[43] |= 0x02;
  if ( data.isReadable(Pic::MemoryConfig) ) array[43] |= 0x04;
  if ( data.isReadable(Pic::MemoryEeprom) ) array[43] |= 0x08;
  if ( data.isReadable(Pic::MemoryCal) )    array[43] |= 0x10;
  if ( data.config().findMask("PARITY") )  array[43] |= 0x20;

  // checksum
  for (uint i=0; i<44; i++) array[44] += array[i];

  return array;
}

//-----------------------------------------------------------------------------
Psp::Hardware::Hardware(::Programmer::Base &base, const QString &portDevice)
  : Programmer::PicHardware(base, new SerialPort(portDevice, base))
{}

bool Psp::Hardware::internalConnectHardware()
{
  if ( !port()->open() ) return false;
  if ( !port()->reset() ) return false;
  // #### TODO: detect Warp13 or JuPic
  QMemArray<uchar> a;
  if ( !port()->command(0x88, 1, a) ) return false;
  if ( a[0]!=0xAB ) {
    log(Log::Error, i18n("Wrong programmer connected"));
    return false;
  }
  return true;
}

bool Psp::Hardware::getFirmwareVersion(VersionData &version)
{
  QMemArray<uchar> a1;
  if ( !port()->commandAck(0x8D, 2, &a1) ) return false;
  if ( a1[1]==0xFF ) {
    log(Log::Warning, i18n("Invalid firmware version"));
    version = VersionData(0xFF, 0, 0);
  } else {
    QMemArray<uchar> a2;
    if ( !port()->receive(2, a2) ) return false;
    version = VersionData(a1[1], a2[0], a2[1]);
  }
  return true;
}

bool Psp::Hardware::setTarget()
{
  log(Log::MaxDebug, "set target");
  // load device info
  if ( !port()->commandAck(0x81) ) return false;
  QMemArray<uchar> a = createDeviceInfo(device());
  if ( !port()->send(a) ) return false;
  if ( !port()->receiveEnd() ) return false;

  // load config info
  if ( !port()->commandAck(0x82) ) return false;
  a = createConfigInfo(device());
  if ( !port()->send(a) ) return false;
  if ( !port()->receiveEnd() ) return false;

  return true;
}

bool Psp::Hardware::setRange(uint start, uint size)
{
  QMemArray<uchar> a(6);
  a[0] = 0x8E;
  a[1] = start >> 16;
  a[2] = (start >> 8) & 0xFF;
  a[3] = start & 0xFF;
  a[4] = size >> 8;
  a[5] = size & 0xFF;
  if ( !port()->send(a) ) return false;
  QMemArray<uchar> r;
  if ( !port()->receive(6, r) ) return false;
  for (uint i=0; i<6; i++) {
    if ( r[i]!=a[i] ) {
      log(Log::Error, i18n("Failed to set range"));
      return false;
    }
  }
  return true;
}

char Psp::Hardware::readCommand(Pic::MemoryRangeType type)
{
  switch (type) {
    case Pic::MemoryCode:    return 'T';
    case Pic::MemoryEeprom:  return 'd';
    case Pic::MemoryConfig:  return 'f';
    case Pic::MemoryUserId:  return 'e';
    case Pic::MemoryCal:     return 'c';
    case Pic::MemoryDeviceId:
    case Pic::MemoryCalBackup:
    case Pic::MemoryDebugVector:
    case Pic::MemoryProgramExecutive:
    case Pic::MemoryHardwareStack:
    case Pic::Nb_MemoryRangeTypes: break;
  }
  return 0;
}

char Psp::Hardware::writeCommand(Pic::MemoryRangeType type)
{
  switch (type) {
    case Pic::MemoryCode:    return 'Q';
    case Pic::MemoryEeprom:  return 'i';
    case Pic::MemoryConfig:  return 'g';
    case Pic::MemoryUserId:  return 'h';
    case Pic::MemoryCal:     return 'q';
    case Pic::MemoryDeviceId:
    case Pic::MemoryCalBackup:
    case Pic::MemoryDebugVector:
    case Pic::MemoryProgramExecutive:
    case Pic::MemoryHardwareStack:
    case Pic::Nb_MemoryRangeTypes: break;
  }
  return 0;
}

bool Psp::Hardware::readMemory(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata)
{
  QMemArray<uchar> a;
  uint nbWords = device().nbWords(type);
  data.resize(nbWords);
  uint nbBytes = nbWords * device().nbBytesWord(type);
  char c = readCommand(type);
  switch (type) {
    case Pic::MemoryCode:
      if ( !setRange(0, nbWords) ) return false;
      if ( !port()->commandAck(c) ) return false;
      for (uint i=0; i<data.count(); i++) {
        if ( !port()->receive(2, a) ) return false;
        data[i] = (a[0] << 8) + a[1];
        log(Log::MaxDebug, QString("code data %1: %2 (%3, %4)").arg(i).arg(toHexLabel(data[i], 4))
                           .arg(toHexLabel(a[0], 2)).arg(toHexLabel(a[1], 2))); // #### REMOVE ME
      }
      if ( !port()->receiveEnd() ) return false;
      break;
    case Pic::MemoryEeprom:
      if ( !port()->commandAck(c) ) return false;
      for (uint i=0; i<data.count(); i++) {
        if ( !port()->receive(1, a) ) return false;
        data[i] = a[0];
      }
      if ( !port()->receiveEnd() ) return false;
      break;
    case Pic::MemoryUserId:
      if ( !port()->commandAckEnd(c, nbBytes+2, a) ) return false;
      for (uint i=0; i<data.count(); i++) data[i] = (a[1+2*i] << 8) + a[1+2*i+1];
      break;
    case Pic::MemoryConfig:
      if ( !port()->commandAckEnd(c, nbBytes+2, a) ) return false;
      for (uint i=0; i<data.count(); i++) data[i] = (a[1+2*i] << 8) + a[1+2*i+1];
      break;
    case Pic::MemoryCal:
      if ( !setRange(0, nbWords) ) return false;
      if ( !port()->commandAckEnd(c, nbBytes+2, a) ) return false;
      for (uint i=0; i<data.count(); i++) data[i] = (a[1+2*i] << 8) + a[1+2*i+1];
      break;
    case Pic::MemoryDeviceId:
    case Pic::MemoryCalBackup:
      data.resize(0);
      return true;
    case Pic::MemoryDebugVector:
    case Pic::MemoryHardwareStack:
    case Pic::MemoryProgramExecutive:
    case Pic::Nb_MemoryRangeTypes: Q_ASSERT(false); return false;
  }
  if (vdata) {
    for (uint i=0; i<data.count(); i++)
      if ( !verifyWord(i, data[i], type, *vdata) ) return false;
  }
  return true;
}

bool Psp::Hardware::writeMemory(Pic::MemoryRangeType type, const Device::Array &data, bool force)
{
  Q_UNUSED(force); // #### TODO: optimize for program memory
  if ( type==Pic::MemoryCode && !setRange(0, data.count()) ) return false;
  uint nbBytes = device().nbBytesWord(type);
  if ( !port()->commandAck(writeCommand(type)) ) return false;
  for (uint i=0; i<data.count(); i++)
    if ( !port()->sendData(data[i], nbBytes) ) return false;
  return port()->receiveEnd();
}

bool Psp::Hardware::eraseAll()
{
  QMemArray<uchar> a;
  if ( !port()->commandAck(0x8F, 2, &a) ) return false;
  if ( a[1]!=0x00 ) {
    log(Log::Error, i18n("Erase failed"));
    return false;
  }
  return true;
}
