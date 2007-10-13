/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "icd1.h"

#include "common/global/global.h"
#include "common/common/misc.h"
#include "common/port/port_base.h"

//-----------------------------------------------------------------------------
Icd1::Hardware::Hardware(::Programmer::Base &base, const QString &portDevice)
  : Icd::Hardware(base, new SerialPort(portDevice, base))
{}

bool Icd1::Hardware::internalConnect(const QString &mode)
{
  if ( !port()->open() ) return false;
  if ( !port()->reset() ) return false;
  if ( hasError() ) return false;
  QByteArray a = toAscii(mode);
  if ( !port()->send(a.data(), a.count()) ) return false;
  QString s;
  if ( !port()->receive(1, s) ) return false;
  port()->setPinOn(Port::Serial::RTS, false, Port::PositiveLogic);
  Port::msleep(1);
  port()->setPinOn(Port::Serial::RTS, true, Port::PositiveLogic);
  if ( s.upper()!=mode ) {
    log(Log::Error, i18n("Failed to set port mode to '%1'.").arg(mode));
    return false;
  }
  return true;
}

bool Icd1::Hardware::sendCommand(uint cmd, BitValue *res, uint timeout)
{
  if ( !port()->sendCommand(cmd) ) return false;
  uchar byte;
  if ( !port()->receiveByte((char &)byte, false, timeout) ) return false;
  if (res) *res = byte << 8;
  if ( !port()->receiveByte((char &)byte, true, timeout) ) return false;
  if (res) *res += byte;
  return true;
}

bool Icd1::Hardware::readBlockCommand(uint nbBytes)
{
  Q_ASSERT( nbBytes<=0xFF );
  if ( !port()->sendCommand(0x7D00 + nbBytes) ) return false;
  QByteArray a(nbBytes);
  for (uint i=0; i<nbBytes; i++)
    if ( !port()->receiveByte(*(a.data()+i), i!=0) ) return false;
  _rx = QString(a);
  return true;
}

bool Icd1::Hardware::setTarget()
{
  return sendCommand(0x6300 + data(_base.device()->name()).part);
}

bool Icd1::Hardware::getFirmwareVersion(VersionData &version)
{
  BitValue v1, v2;
  if ( !sendCommand(0x7F00, &v1) || !sendCommand(0x7021, &v2) ) return false;
  version = VersionData(v1.byte(1), v1.byte(0), v2.toUInt());
  return true;
}

bool Icd1::Hardware::readVoltages(VoltagesData &voltages)
{
  if ( !sendCommand(0x7000) ) return false;
  BitValue res;
  if ( !sendCommand(0x701C, &res) ) return false;
  voltages[Pic::TargetVdd].value = (2.050 / 256) * res.toUInt();  // voltage at AN0 pin
  voltages[Pic::TargetVdd].value /= (4.7 / 14.7);        // voltage at Vcc
  log(Log::MaxDebug, QString("Vdd: %1 %2").arg(toHexLabel(res, 4)).arg(voltages[Pic::TargetVdd].value));
  voltages[Pic::TargetVdd].error = false;
  if ( !sendCommand(0x701D, &res) ) return false;
  voltages[Pic::TargetVpp].value = (2.050 / 256) * res.byte(0);  // voltage at AN1 pin
  voltages[Pic::TargetVpp].value /= (10.0 / 110.0);               // voltage at Vpp
  log(Log::MaxDebug, QString("Vpp: %1 %2").arg(toHexLabel(res, 4)).arg(voltages[Pic::TargetVpp].value));
  voltages[Pic::TargetVpp].error = false;
  return sendCommand(0x7001);
}

bool Icd1::Hardware::uploadFirmware(const Pic::Memory &memory)
{
  // #### TODO
  return false;
}

bool Icd1::Hardware::setTargetReset(Pic::ResetMode mode)
{
  // #### TODO
  return false;
}

bool Icd1::Hardware::selfTest()
{
  BitValue res;
  if ( !sendCommand(0x700B, &res, 5000) ) return false;
  if ( res!=0 ) {
    log(Log::Warning, i18n("Self-test failed (returned value is %1)").arg(toLabel(res)));
    return false;
  }
  return true;
}

bool Icd1::Hardware::gotoMemory(Pic::MemoryRangeType type, uint offset)
{
  if ( !sendCommand(0x7000) ) return false; // start sequence
  uint cmd = (type==Pic::MemoryRangeType::Eeprom ? 0x7006 : 0x7005);
  if ( !sendCommand(cmd) )  return false;
  if ( type==Pic::MemoryRangeType::Config || type==Pic::MemoryRangeType::UserId || type==Pic::MemoryRangeType::DeviceId ) {
    Q_ASSERT( offset==0 );
    offset = device().range(type).start - Address(0x2000);
    if ( !sendCommand(0xC000 + offset) )  return false;
  } else if ( !sendCommand(0x2000 + offset) )  return false;
  return true;
}

bool Icd1::Hardware::readMemory(Pic::MemoryRangeType type, uint wordOffset, Device::Array &data, const ::Programmer::VerifyData *vdata)
{
  if ( !gotoMemory(type, wordOffset) ) return false;
  for (uint i=0; i<data.count(); i++) if ( !sendCommand(0x7002, &data[i]) ) return false;
  if ( !sendCommand(0x7001) ) return false; // end sequence
  if (vdata) {
    for (uint i=0; i<data.count(); i++)
      if ( !verifyWord(wordOffset+i, data[i], type, *vdata) ) return false;
  }
  return true;
}

bool Icd1::Hardware::writeMemory(Pic::MemoryRangeType type, uint wordOffset, const Device::Array &data)
{
  if ( !gotoMemory(type, wordOffset) ) return false;
  for (uint i=0; i<data.count(); i++) if ( !sendCommand(0x8000 | data[i].toUInt()) ) return false;
  if ( !sendCommand(0x7001) ) return false; // end sequence
  return true;
}

bool Icd1::Hardware::eraseAll()
{
  BitValue res;
  if ( !sendCommand(0x7000) ) return false; // enable Vpp
  if ( !sendCommand(0x7007, &res) ) return false;
  if ( !sendCommand(0x7001) ) return false; // disable Vpp
  if ( res!=0x3FFF ) {
    log(Log::Error, i18n("Erase failed (returned value is %1)").arg(toHexLabel(res, 4)));
    return false;
  }
  return true;
}

bool Icd1::Hardware::haltRun()
{
  // #### TODO
  return false;
}

bool Icd1::Hardware::step()
{
  // #### TODO
  return false;
}

bool Icd1::Hardware::resumeRun()
{
  // #### TODO
  return false;
}

bool Icd1::Hardware::writeRegister(Address address, BitValue value, uint nbBytes)
{
  Q_ASSERT( nbBytes==1 || nbBytes==2 );
  // #### TODO
  return false;
}

bool Icd1::Hardware::readRegister(Address address, BitValue &value, uint nbBytes)
{
  Q_ASSERT( nbBytes==1 || nbBytes==2 );
  // #### TODO
  return false;
}

BitValue Icd1::Hardware::getProgramCounter()
{
  // #### TODO
  return 0;
}
