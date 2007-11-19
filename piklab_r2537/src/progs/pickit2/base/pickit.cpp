/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit.h"

#include "devices/base/device_group.h"
#include "progs/base/prog_group.h"

//-----------------------------------------------------------------------------
Pickit::Array::Array(uint length, uchar fillChar, PrintMode mode)
  : _fillChar(fillChar), _mode(mode), _data(length)
{
  _data.fill(fillChar);
}

QString Pickit::Array::pretty() const
{
  int end = _data.count() - 1;
  for (; end>=0; end--)
    if ( _data[end]!=_fillChar ) break;
  QString s;
  for (int i=0; i<=end; i++) s += toPrintable(_data[i], _mode);
  return s;
}

//-----------------------------------------------------------------------------
Pickit::USBPort::USBPort(uint deviceId, Log::Base &log)
  : Port::USB(log, Microchip::VENDOR_ID, deviceId, CONFIG_VENDOR, 0)
{}

bool Pickit::USBPort::command(uchar c)
{
  Array a = array();
  a._data[0] = c;
  return command(a);
}

bool Pickit::USBPort::command(const char *s)
{
  Array a = array();
  if (s) {
    Q_ASSERT( strlen(s)<=a.length() );
    for (uint i=0; i<strlen(s); i++) a._data[i] = s[i];
  }
  return command(a);
}

bool Pickit::USBPort::command(const Array &cmd)
{
  log(Log::DebugLevel::Extra, QString("send command: \"%1\"").arg(cmd.pretty()));
  return write(writeEndPoint(), (const char *)cmd._data.data(), cmd.length());
}

bool Pickit::USBPort::receive(Pickit::Array &array)
{
  if ( !read(readEndPoint(), (char *)array._data.data(), array.length()) ) return false;
  log(Log::DebugLevel::Max, QString("received: \"%1\"").arg(array.pretty()));
  return true;
}

bool Pickit::USBPort::getMode(VersionData &version, ::Programmer::Mode &mode)
{
  if ( !command('v') ) return false;
  Array a = array();
  if ( !receive(a) ) return false;
  if ( a[5]=='B' ) {
    version = VersionData(a[6], a[7], 0);
    mode = ::Programmer::BootloadMode;
  } else {
    version = VersionData(a[0], a[1], a[2]);
    mode = ::Programmer::NormalMode;
  }
  return true;
}

bool Pickit::USBPort::receiveWords(uint nbBytesWord, uint nbRead, QValueVector<uint> &words, uint offset)
{
  log(Log::DebugLevel::Max, QString("receive words nbBytesWord=%1 nbRead=%2 offset=%3").arg(nbBytesWord).arg(nbRead).arg(offset));
  Array a = array();
  QMemArray<uchar> data(nbRead*a.length());
  uint l = 0;
  for (uint i=0; i<nbRead; i++) {
    if ( !receive(a) ) return false;
    for (uint k=offset; k<a.length(); k++) {
      data[l] = a[k];
      l++;
    }
  }
  words.resize(data.count()/nbBytesWord);
  for (uint i=0; i<uint(words.count()); i++) {
    words[i] = 0;
    for (uint k=0; k<nbBytesWord; k++) words[i] |= data[nbBytesWord*i + k] << (8*k);
  }
  return true;
}

//-----------------------------------------------------------------------------
Pickit::Hardware::Hardware(::Programmer::Base &base, USBPort *port)
  : ::Programmer::PicHardware(base, port, QString::null)
{}

bool Pickit::Hardware::internalConnectHardware()
{
  return port().open();
}

bool Pickit::Hardware::setTargetPower(uint v)
{
  Array cmd = port().array();
  cmd[0] = 'V';
  cmd[1] = v;
  return port().command(cmd);
}

bool Pickit::Hardware::writeWords(uint max, char c, uint nbBytesWord,
                                  uint &i, const Device::Array &data)
{
  Q_ASSERT( i<data.count() );
  Q_ASSERT( nbBytesWord==1 || nbBytesWord==2 );
  Array cmd = port().array();
  uint n = (nbBytesWord==1 ? 2 : 3);
  Q_ASSERT( n*max<=cmd.length() );
  for (uint k=0; k<max; k++) {
    cmd[n*k] = c;
    cmd[n*k+1] = data[i].byte(0);
    if ( nbBytesWord==2 ) cmd[n*k+2] = data[i].byte(1);
    i++;
    if ( i>=data.count() ) break;
  }
  return port().command(cmd);
}

bool Pickit::Hardware::regenerateOsccal(BitValue &newValue)
{
  if ( !setTargetPower(Osc2_5kHz) ) return false;
  if ( !setTargetPower(PowerOn + Osc2_5kHz) ) return false;
  Port::usleep(400000); // 400 ms
  if ( !setTargetPower(PowerOff) ) return false;
  Pickit::Array cmd = port().array();
  cmd[0] = 'P';
  cmd[1] = 'I';
  cmd[2] = 120;
  cmd[3] = 0;
  cmd[4] = 'r';
  cmd[5] = 'p';
  if ( !port().command(cmd) ) return false;
  QValueVector<uint> words;
  if ( !port().receiveWords(1, 1, words) ) return false;
  newValue = words[7] | 0x3400;
  return true;
}

//----------------------------------------------------------------------------
bool Pickit::DeviceSpecific::setPowerOn()
{
  return hardware().port().command(entryMode());
}

bool Pickit::DeviceSpecific::setPowerOff()
{
  return hardware().port().command('p');
}

bool Pickit::DeviceSpecific::setTargetPowerOn(bool on)
{
  return hardware().setTargetPower(on ? PowerOn : PowerOff);
}

//----------------------------------------------------------------------------
bool Pickit::BMDeviceSpecific::doRead(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata)
{
  data.resize(device().nbWords(type));
  gotoMemory(type);
  QValueVector<uint> words;
  switch (type.type()) {
    case Pic::MemoryRangeType::Config:
    case Pic::MemoryRangeType::Code:
    case Pic::MemoryRangeType::Cal:
    case Pic::MemoryRangeType::CalBackup:
    case Pic::MemoryRangeType::UserId:
    case Pic::MemoryRangeType::DeviceId:
      for (uint i=0; i<data.count();) {
        if ( !hardware().port().command('R') ) return false;
        if ( !hardware().port().receiveWords(2, 1, words) ) return false;
        for (uint k=0; k<uint(words.count()); k++) {
          data[i] = words[k];
          if ( vdata && !hardware().verifyWord(i, data[i], type, *vdata) ) return false;
          i++;
          if ( i>=data.count() ) break;
        }
      }
      break;
    case Pic::MemoryRangeType::Eeprom:
      for (uint i=0; i<data.count();) {
        if ( !hardware().port().command('r') ) return false; // #### not sure this is correct for Pickit1: "rrrrrrrr" used by usb_pickit
        if ( !hardware().port().receiveWords(1, 1, words) ) return false;
        for (uint k=0; k<uint(words.count()); k++) {
          data[i] = words[k];
          if ( vdata && !hardware().verifyWord(i, data[i], type, *vdata) ) return false;
          i++;
          if ( i>=data.count() ) break;
        }
      }
      break;
    case Pic::MemoryRangeType::DebugVector:
    case Pic::MemoryRangeType::HardwareStack:
    case Pic::MemoryRangeType::ProgramExecutive:
    case Pic::MemoryRangeType::Nb_Types: Q_ASSERT(false); return false;
  }
  if ( type==Pic::MemoryRangeType::Code || type==Pic::MemoryRangeType::Eeprom )
    _base.progressMonitor().addTaskProgress(data.count());
  return true;
}

bool Pickit::BMDeviceSpecific::incrementPC(uint nb)
{
  Pickit::Array cmd = hardware().port().array();
  cmd[0] = 'I';
  cmd[1] = nb & 0xFF;
  cmd[2] = (nb >> 8) & 0xFF;
  return hardware().port().command(cmd);
}

bool Pickit::BMDeviceSpecific::doEraseRange(Pic::MemoryRangeType type)
{
  Q_ASSERT( type==Pic::MemoryRangeType::Code );
  return hardware().port().command('E');
}

bool Pickit::BMDeviceSpecific::doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force)
{
  // #### TODO: speed optimize...
  Q_UNUSED(force);
  gotoMemory(type);
  uint nb = nbWrites(type);
  switch (type.type()) {
    case Pic::MemoryRangeType::Config:
    case Pic::MemoryRangeType::Code:
    case Pic::MemoryRangeType::Cal:
    case Pic::MemoryRangeType::UserId:
      for (uint i=0; i<data.count(); )
        hardware().writeWords(nb, writeCode(), 2, i, data);
      break;
    case Pic::MemoryRangeType::Eeprom:
      for (uint i=0; i<data.count(); )
        hardware().writeWords(nb, writeData(), 1, i, data);
      break;
    case Pic::MemoryRangeType::CalBackup:
    case Pic::MemoryRangeType::DeviceId:
    case Pic::MemoryRangeType::DebugVector:
    case Pic::MemoryRangeType::HardwareStack:
    case Pic::MemoryRangeType::ProgramExecutive:
    case Pic::MemoryRangeType::Nb_Types: Q_ASSERT(false); return false;
  }
  if ( type==Pic::MemoryRangeType::Code || type==Pic::MemoryRangeType::Eeprom )
    _base.progressMonitor().addTaskProgress(data.count());
  return true;
}

//----------------------------------------------------------------------------
bool Pickit::Baseline::gotoMemory(Pic::MemoryRangeType type)
{
  switch (type.type()) {
    case Pic::MemoryRangeType::Config:
    case Pic::MemoryRangeType::Eeprom: return true;
    case Pic::MemoryRangeType::Code:
    case Pic::MemoryRangeType::Cal:
    case Pic::MemoryRangeType::UserId:
    case Pic::MemoryRangeType::CalBackup: return incrementPC(1+device().range(type).start.toUInt());
    case Pic::MemoryRangeType::DeviceId:
    case Pic::MemoryRangeType::DebugVector:
    case Pic::MemoryRangeType::HardwareStack:
    case Pic::MemoryRangeType::ProgramExecutive:
    case Pic::MemoryRangeType::Nb_Types: break;
  }
  Q_ASSERT(false);
  return false;
}

//----------------------------------------------------------------------------
bool Pickit::P16F::gotoMemory(Pic::MemoryRangeType type)
{
  Pickit::Array cmd = hardware().port().array();
  cmd[0] = 'C';
  switch (type.type()) {
    case Pic::MemoryRangeType::Code: return true;
    case Pic::MemoryRangeType::Eeprom: return true;
    case Pic::MemoryRangeType::UserId: return hardware().port().command(cmd);
    case Pic::MemoryRangeType::DeviceId:
      cmd[1] = 'I';
      cmd[2] = 0x06;
      cmd[3] = 0x00;
      return hardware().port().command(cmd);
    case Pic::MemoryRangeType::Config:
      cmd[1] = 'I';
      cmd[2] = 0x07;
      cmd[3] = 0x00;
      return hardware().port().command(cmd);
    case Pic::MemoryRangeType::Cal:
      if ( device().range(type).start==device().range(Pic::MemoryRangeType::Code).end+1 )
        return incrementPC(device().range(type).start.toUInt());
      cmd[1] = 'I';
      cmd[2] = 0x08;
      cmd[3] = 0x00;
      return hardware().port().command(cmd);
    case Pic::MemoryRangeType::CalBackup:
    case Pic::MemoryRangeType::DebugVector:
    case Pic::MemoryRangeType::HardwareStack:
    case Pic::MemoryRangeType::ProgramExecutive:
    case Pic::MemoryRangeType::Nb_Types: break;
  }
  Q_ASSERT(false);
  return false;
}

bool Pickit::P16F::doErase(bool)
{
  Pickit::Array cmd = hardware().port().array();
  cmd[0] = 'C';
  cmd[1] = writeCode();
  cmd[2] = 0xFF;
  cmd[3] = 0x3F;
  cmd[4] = 'E';
  cmd[5] = 'e';
  return hardware().port().command(cmd);
}

bool Pickit::P16F::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryRangeType::Eeprom ) return hardware().port().command('e');
  return BMDeviceSpecific::doEraseRange(type);
}
