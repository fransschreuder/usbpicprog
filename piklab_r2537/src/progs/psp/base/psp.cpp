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
    BitValue v = config._words[i].usedMask();
    if ( data.nbBytesWord(Pic::MemoryRangeType::UserId)==1 ) array[i ^ 1] = v.byte(0);
    else {
      array[2*i] = v.byte(1);
      array[2*i + 1] = v.byte(0);
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
  BitValue v = data.nbWords(Pic::MemoryRangeType::Code);
  array[0] = v.byte(1);
  array[1] = v.byte(0);
  // address word width
  v = data.mask(Pic::MemoryRangeType::Code);
  array[2] = v.byte(1);
  array[3] = v.byte(0);
  // data word width
  array[4] = v.byte(1);
  array[5] = v.byte(0);

  if ( data.isReadable(Pic::MemoryRangeType::UserId) ) {
    // user id width
    v = data.range(Pic::MemoryRangeType::UserId).rmask;
    if ( data.nbBytesWord(Pic::MemoryRangeType::UserId)==1 ) v += v << 8;
    array[6] = v.byte(1);
    array[7] = v.byte(0);
    // user id mask
    array[8] = v.byte(1);
    array[9] = v.byte(0);
  }

  // memory config mask
  array[10] = config[0];
  array[11] = config[1];
  array[12] = config[16];
  array[13] = config[17];

  if ( data.isReadable(Pic::MemoryRangeType::Eeprom) ) {
    // memory eeprom width
    v = data.mask(Pic::MemoryRangeType::Eeprom);
    array[14] = v.byte(1);
    array[15] = v.byte(0);
    // memory eeprom mask
    array[16] = v.byte(1);
    array[17] = v.byte(0);
  }

  if ( data.isReadable(Pic::MemoryRangeType::Cal) ) {
    // memory calibration width
    v = data.mask(Pic::MemoryRangeType::Cal);
    array[18] = v.byte(1);
    array[19] = v.byte(0);
    // memory calibration mask
    array[20] = v.byte(1);
    array[21] = v.byte(0);
  }

  // memory code start
  array[22] = 0x00;
  array[23] = 0x00;
  // memory code length
  array[24] = 0x00; // array[0]; in lplab and for some devices in picp...
  array[25] = 0x01; // array[1]; in lplab and for some devices in picp...

  if ( data.isReadable(Pic::MemoryRangeType::UserId) ) {
    // user id start
    v = data.range(Pic::MemoryRangeType::UserId).start.toUInt();
    array[26] = v.byte(1);
    array[27] = v.byte(0);
    // user id length
    v = data.nbWords(Pic::MemoryRangeType::UserId);
    if ( data.nbBytesWord(Pic::MemoryRangeType::UserId)==1 ) v >>= 1;
    array[28] = v.byte(0);
  }

  if ( data.isReadable(Pic::MemoryRangeType::Config) ) {
    // config start
    v = data.range(Pic::MemoryRangeType::Config).start.toUInt();
    array[29] = v.byte(1);
    array[30] = v.byte(0);
    // config length
    v = data.nbWords(Pic::MemoryRangeType::Config);
    if ( data.nbBytesWord(Pic::MemoryRangeType::Config)==1 ) v >>= 1;
    array[31] = v.byte(0);
  }

  if ( data.isReadable(Pic::MemoryRangeType::Config) ) {
    // eeprom start
    array[32] = 0x00;
    array[33] = 0x00;
    // eeprom length
    v = data.nbWords(Pic::MemoryRangeType::Eeprom);
    array[34] = v.byte(1);
    array[35] = v.byte(0);
  }

  if ( data.isReadable(Pic::MemoryRangeType::Cal) ) {
    // calibration start
    v = data.range(Pic::MemoryRangeType::Cal).start.toUInt();
    array[36] = v.byte(1);
    array[37] = v.byte(0);
    // calibration length
    v = data.nbWords(Pic::MemoryRangeType::Cal);
    array[38] = v.byte(1);
    array[39] = v.byte(0);
  }

  // programming
  const Psp::Data &pdata = Psp::data(data.name());
  array[40] = pdata.overprogram;
  array[41] = pdata.tries;
  array[42] = pdata.algorithm;
  if ( data.memoryTechnology()==Device::MemoryTechnology::Flash ) array[43] |= 0x01;
  if ( data.isReadable(Pic::MemoryRangeType::UserId) ) array[43] |= 0x02;
  if ( data.isReadable(Pic::MemoryRangeType::Config) ) array[43] |= 0x04;
  if ( data.isReadable(Pic::MemoryRangeType::Eeprom) ) array[43] |= 0x08;
  if ( data.isReadable(Pic::MemoryRangeType::Cal) )    array[43] |= 0x10;
  if ( data.config().findMask("PARITY") )  array[43] |= 0x20;

  // checksum
  for (uint i=0; i<44; i++) array[44] += array[i];

  return array;
}

//-----------------------------------------------------------------------------
Psp::Hardware::Hardware(::Programmer::Base &base, const QString &portDevice)
  : Programmer::PicHardware(base, new SerialPort(portDevice, base), QString::null)
{}

bool Psp::Hardware::internalConnectHardware()
{
  if ( !port()->open() ) return false;
  if ( !port()->reset() ) return false;
  // #### TODO: detect Warp13 or JuPic
  QMemArray<uchar> a;
  if ( !port()->command(0x88, 1, a) ) return false;
  if ( a[0]!=0xAB ) {
    log(Log::LineType::Error, i18n("Wrong programmer connected"));
    return false;
  }
  return true;
}

bool Psp::Hardware::getFirmwareVersion(VersionData &version)
{
  QMemArray<uchar> a1;
  if ( !port()->commandAck(0x8D, 2, &a1) ) return false;
  if ( a1[1]==0xFF ) {
    log(Log::LineType::Warning, i18n("Invalid firmware version"));
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
  log(Log::DebugLevel::Max, "set target");
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
      log(Log::LineType::Error, i18n("Failed to set range"));
      return false;
    }
  }
  return true;
}

char Psp::Hardware::readCommand(Pic::MemoryRangeType type)
{
  switch (type.type()) {
    case Pic::MemoryRangeType::Code:    return 'T';
    case Pic::MemoryRangeType::Eeprom:  return 'd';
    case Pic::MemoryRangeType::Config:  return 'f';
    case Pic::MemoryRangeType::UserId:  return 'e';
    case Pic::MemoryRangeType::Cal:     return 'c';
    case Pic::MemoryRangeType::DeviceId:
    case Pic::MemoryRangeType::CalBackup:
    case Pic::MemoryRangeType::DebugVector:
    case Pic::MemoryRangeType::ProgramExecutive:
    case Pic::MemoryRangeType::HardwareStack:
    case Pic::MemoryRangeType::Nb_Types: break;
  }
  return 0;
}

char Psp::Hardware::writeCommand(Pic::MemoryRangeType type)
{
  switch (type.type()) {
    case Pic::MemoryRangeType::Code:    return 'Q';
    case Pic::MemoryRangeType::Eeprom:  return 'i';
    case Pic::MemoryRangeType::Config:  return 'g';
    case Pic::MemoryRangeType::UserId:  return 'h';
    case Pic::MemoryRangeType::Cal:     return 'q';
    case Pic::MemoryRangeType::DeviceId:
    case Pic::MemoryRangeType::CalBackup:
    case Pic::MemoryRangeType::DebugVector:
    case Pic::MemoryRangeType::ProgramExecutive:
    case Pic::MemoryRangeType::HardwareStack:
    case Pic::MemoryRangeType::Nb_Types: break;
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
  switch (type.type()) {
    case Pic::MemoryRangeType::Code:
      if ( !setRange(0, nbWords) ) return false;
      if ( !port()->commandAck(c) ) return false;
      for (uint i=0; i<data.count(); i++) {
        if ( !port()->receive(2, a) ) return false;
        data[i] = (a[0] << 8) + a[1];
        log(Log::DebugLevel::Max, QString("code data %1: %2 (%3, %4)").arg(i).arg(toHexLabel(data[i], 4))
                           .arg(toHexLabel(a[0], 2)).arg(toHexLabel(a[1], 2))); // #### REMOVE ME
      }
      if ( !port()->receiveEnd() ) return false;
      break;
    case Pic::MemoryRangeType::Eeprom:
      if ( !port()->commandAck(c) ) return false;
      for (uint i=0; i<data.count(); i++) {
        if ( !port()->receive(1, a) ) return false;
        data[i] = a[0];
      }
      if ( !port()->receiveEnd() ) return false;
      break;
    case Pic::MemoryRangeType::UserId:
      if ( !port()->commandAckEnd(c, nbBytes+2, a) ) return false;
      for (uint i=0; i<data.count(); i++) data[i] = (a[1+2*i] << 8) + a[1+2*i+1];
      break;
    case Pic::MemoryRangeType::Config:
      if ( !port()->commandAckEnd(c, nbBytes+2, a) ) return false;
      for (uint i=0; i<data.count(); i++) data[i] = (a[1+2*i] << 8) + a[1+2*i+1];
      break;
    case Pic::MemoryRangeType::Cal:
      if ( !setRange(0, nbWords) ) return false;
      if ( !port()->commandAckEnd(c, nbBytes+2, a) ) return false;
      for (uint i=0; i<data.count(); i++) data[i] = (a[1+2*i] << 8) + a[1+2*i+1];
      break;
    case Pic::MemoryRangeType::DeviceId:
    case Pic::MemoryRangeType::CalBackup:
      data.resize(0);
      return true;
    case Pic::MemoryRangeType::DebugVector:
    case Pic::MemoryRangeType::HardwareStack:
    case Pic::MemoryRangeType::ProgramExecutive:
    case Pic::MemoryRangeType::Nb_Types: Q_ASSERT(false); return false;
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
  if ( type==Pic::MemoryRangeType::Code && !setRange(0, data.count()) ) return false;
  uint nbBytes = device().nbBytesWord(type);
  if ( !port()->commandAck(writeCommand(type)) ) return false;
  for (uint i=0; i<data.count(); i++)
    if ( !port()->sendData(data[i].toUInt(), nbBytes) ) return false;
  return port()->receiveEnd();
}

bool Psp::Hardware::eraseAll()
{
  QMemArray<uchar> a;
  if ( !port()->commandAck(0x8F, 2, &a) ) return false;
  if ( a[1]!=0x00 ) {
    log(Log::LineType::Error, i18n("Erase failed"));
    return false;
  }
  return true;
}
