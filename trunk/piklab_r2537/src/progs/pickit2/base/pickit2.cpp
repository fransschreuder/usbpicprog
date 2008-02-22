/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit2.h"

#include "common/global/global.h"
#include "devices/list/device_list.h"
#include "common/port/port.h"

//-----------------------------------------------------------------------------
void Pickit2::USBPort::fillCommand(Pickit::Array &cmd, uchar command, uint nbBytes,
                                   uint address, uint i, bool longAddress) const
{
  cmd[i] = command;
  cmd[i+1] = nbBytes;
  cmd[i+2] = address & 0xFF;
  cmd[i+3] = (address >> 8) & 0xFF;
  if (longAddress) cmd[i+4] = (address >> 16) & 0xFF;
}

Pickit::Array Pickit2::USBPort::createCommand(uchar c, uint nbBytes, uint address, bool longAddress) const
{
  Array cmd;
  fillCommand(cmd, c, nbBytes, address, 0, longAddress);
  return cmd;
}

bool Pickit2::USBPort::readFirmwareCodeMemory(Device::Array &data, const Device::Array *vdata, ProgressMonitor &monitor)
{
  const Pic::Data *device = static_cast<const Pic::Data *>(Device::lister().data("18F2455"));
  log(Log::DebugLevel::Normal, "  Read pickit2 firmware");
  for (uint i=0; i<data.count(); i+=16) { // 2 bytes/word and 32 bytes/command max
    uint start = 0;
    for (; start<16; start++) if ( data[i + start].isInitialized() ) break;
    if ( start==16 ) continue;
    uint end = 15;
    for (; end>start; end--) if ( data[i + end].isInitialized() ) break;
    uint nb = end - start + 1;
    Pickit::Array cmd = createCommand(1, 2*nb, 2*i);
    if ( !command(cmd) ) return false;
    QValueVector<uint> read;
    if ( !receiveWords(1, 1, read) ) return false;
    for (uint k=0; k<nb; k++) {
      uint index = i + start + k;
      data[index]= read[5 + 2*k] & 0xFF | (read[6 + 2*k] << 8);
      if ( vdata && index>=0x1000 && index<0x3FF0 && data[index]!=(*vdata)[index] ) {
        log(Log::LineType::Error, i18n("Firmware memory does not match hex file (at address 0x%2: reading 0x%3 and expecting 0x%4).")
                    .arg(QString(toHex(index/2, device->nbCharsAddress())))
                    .arg(QString(toHex(data[index], device->nbCharsWord(Pic::MemoryRangeType::Code))))
                    .arg(QString(toHex((*vdata)[index], device->nbCharsWord(Pic::MemoryRangeType::Code)))));
         return false;
      }
    }
  }
  monitor.addTaskProgress(data.count());
  return true;
}

bool Pickit2::USBPort::eraseFirmwareCodeMemory()
{
  log(Log::DebugLevel::Normal, "  Erase pickit2 firmware");
  Pickit::Array cmd = createCommand(3, 0xC0, 0x2000);
  if ( !command(cmd) ) return false; // erase 0x2000-0x4FFF
  Port::usleep(1000000);
  cmd = createCommand(3, 0xC0, 0x5000);
  if ( !command(cmd) ) return false; // erase 0x5000-0x7FFF
  Port::usleep(1000000);
  return true;
}

bool Pickit2::USBPort::writeFirmwareCodeMemory(const Device::Array &data, ProgressMonitor &monitor)
{
  log(Log::DebugLevel::Normal, "  Write pickit2 firmware");
  for (uint i=0x1000; i<data.count(); i+=16) { // 2 bytes/word and 32 bytes/command max
    uint start = 0;
    for (; start<16; start++) if ( data[i + start].isInitialized() ) break;
    if ( start==16 ) continue;
    uint end = 15;
    for (; end>start; end--) if ( data[i + end].isInitialized() ) break;
    uint nb = end - start + 1;
    Pickit::Array cmd = createCommand(2, 2*nb, 2*i);
    for (uint k=0; k<nb; k++) {
      cmd[5 + 2*k] = data[i + start + k].byte(0);
      cmd[6 + 2*k] = data[i + start + k].byte(1);
    }
    if ( !command(cmd) ) return false;
    Port::usleep(100000);
  }
  monitor.addTaskProgress(data.count());
  Device::Array read;
  if ( !readFirmwareCodeMemory(read, &data, monitor) ) return false;
  log(Log::DebugLevel::Normal, "  Write pickit2 firmware-loaded key");
  Pickit::Array cmd = createCommand(2, 2, 0x7FFE);
  cmd[5] = 0x55;
  cmd[6] = 0x55;
  return command(cmd);
}

bool Pickit2::USBPort::uploadFirmware(const Pic::Memory &memory, ProgressMonitor &monitor)
{
  if ( !eraseFirmwareCodeMemory() ) return false;
  Device::Array data = memory.arrayForWriting(Pic::MemoryRangeType::Code);
  return writeFirmwareCodeMemory(data, monitor);
}

//-----------------------------------------------------------------------------
bool Pickit2::Hardware::readVoltages(VoltagesData &voltages)
{
  log(Log::DebugLevel::Extra, QString("readVoltages: Firmware is %1").arg(_base.firmwareVersion().pretty()));
  if ( _base.firmwareVersion()<VersionData(1, 20, 0) ) {
    log(Log::LineType::Warning, i18n("Cannot read voltages with this firmware version."));
    return true;
  }
  if ( !port().command('M') ) return false;
  Array a;
  if ( !port().receive(a) ) return false;
  voltages[Pic::TargetVdd].value = double(a[1] + a[2]*256) * 5.0 / 65535;
  voltages[Pic::TargetVpp].value = double(a[3] + a[4]*256) * (5.0 /65535) * (7.4 / 2.7);
  voltages[Pic::TargetVdd].error = ( a[0] & 0x08 );
  voltages[Pic::TargetVpp].error = ( a[0] & 0x10 );
  return true;
}

bool Pickit2::Hardware::setVddVpp(double vdd, double vpp)
{
  log(Log::DebugLevel::Extra, QString("setVddVpp: Firmware is %1").arg(_base.firmwareVersion().pretty()));
  if ( _base.firmwareVersion()<VersionData(1, 20, 0) ) return true;
  log(Log::DebugLevel::Normal, QString("  set Vdd = %1 V and Vpp = %2 V").arg(vdd).arg(vpp));
  Array cmd;
  cmd[0] = 's';
  uint cvdd = uint(32.0 * vdd + 12.5);
  cvdd <<= 6;
  cmd[1] = cvdd & 0xC0;
  cmd[2] = cvdd >> 8;
  uint cvpp = uint(18.61 * vpp);
  cmd[3] = 0x40;
  cmd[4] = cvpp;
  cmd[5] = uchar(4.5 * (cvdd >> 8)); // linit to 75% of vdd
  cmd[6] = uchar(0.75 * cvpp);       // linit to 75% of vpp
  if ( !port().command(cmd) ) return false;
  // wait until vpp is stabilized
  for (uint i=0; i<30; i++) {
    Port::usleep(50000);
    VoltagesData voltages;
    if ( !readVoltages(voltages) ) return false;
    if ( voltages[Pic::TargetVpp].value<(vpp+0.5) ) break;
  }
  return true;
}

//-----------------------------------------------------------------------------
bool Pickit2::Baseline::init()
{
  Array cmd;
  cmd[0] = 'c';
  cmd[1] = 0x00;   // Load Configuration
  cmd[2] = 0x02;   // Load Program latches
  cmd[3] = 0x03;   // Load Data latches
  cmd[4] = 0x04;   // Read Program memory
  cmd[5] = 0x05;   // Read Data latches
  cmd[6] = 0x06;   // Increment Address
  cmd[7] = 0x08;   // Begin programming internally timed
  cmd[8] = 0x08;   // Begin Programming externally timed
  cmd[9] = 0x0E;   // End externally time programming cycle
  cmd[10] = 0x09;  // Bulk Erase program memory
  cmd[11] = 0x0B;  // Bulk Erase Data memory
  cmd[12] = 0xFF;  // Read Program memory
  cmd[13] = 0xFF;  // Read Data latches
  cmd[14] = 0xFF;  // Increment Address
  cmd[15] = 0xFF;  // Begin programming internally timed
  return hardware().port().command(cmd);
}

//-----------------------------------------------------------------------------
bool Pickit2::P16F::init()
{
  Array cmd;
  cmd[0] = 'c';
  cmd[1] = 0x00;   // Load Configuration
  cmd[2] = 0x02;   // Load Program latches
  cmd[3] = 0x03;   // Load Data latches
  cmd[4] = 0x04;   // Read Program memory
  cmd[5] = 0x05;   // Read Data latches
  cmd[6] = 0x06;   // Increment Address
  cmd[7] = 0x08;   // Begin programming internally timed
  cmd[8] = 0x18;   // Begin Programming externally timed
  cmd[9] = 0x0A;   // End externally time programming cycle
  cmd[10] = 0x09;  // Bulk Erase program memory
  cmd[11] = 0x0B;  // Bulk Erase Data memory
  cmd[12] = 0xFF;  // Read Program memory
  cmd[13] = 0xFF;  // Read Data latches
  cmd[14] = 0xFF;  // Increment Address
  cmd[15] = 0xFF;  // Begin programming internally timed
  return hardware().port().command(cmd);
}

//-----------------------------------------------------------------------------
bool Pickit2::P16F87XA::init()
{
  Array cmd;
  cmd[0] = 'c';
  cmd[1] = 0x00;   // Load Configuration
  cmd[2] = 0x02;   // Load Program latches
  cmd[3] = 0x03;   // Load Data latches
  cmd[4] = 0x04;   // Read Program memory
  cmd[5] = 0x05;   // Read Data latches
  cmd[6] = 0x06;   // Increment Address
  cmd[7] = 0x08;   // Begin programming internally timed
  cmd[8] = 0x18;   // Begin Programming externally timed
  cmd[9] = 0x17;   // End externally time programming cycle
  cmd[10] = 0x1F;  // Bulk Erase program memory
  cmd[11] = 0x1F;  // Bulk Erase Data memory
  cmd[12] = 0xFF;  // Read Program memory
  cmd[13] = 0xFF;  // Read Data latches
  cmd[14] = 0xFF;  // Increment Address
  cmd[15] = 0xFF;  // Begin programming internally timed
  return hardware().port().command(cmd);
}

//-----------------------------------------------------------------------------
bool Pickit2::P16F7X::init()
{
  Array cmd;
  cmd[0] = 'c';
  cmd[1] = 0x00;   // Load Configuration
  cmd[2] = 0x02;   // Load Program latches
  cmd[3] = 0x03;   // Load Data latches
  cmd[4] = 0x04;   // Read Program memory
  cmd[5] = 0x05;   // Read Data latches
  cmd[6] = 0x06;   // Increment Address
  cmd[7] = 0x08;   // Begin programming internally timed
  cmd[8] = 0x08;   // Begin Programming externally timed
  cmd[9] = 0x0E;   // End externally time programming cycle
  cmd[10] = 0x09;  // Bulk Erase program memory
  cmd[11] = 0x0B;  // Bulk Erase Data memory
  cmd[12] = 0xFF;  // Read Program memory
  cmd[13] = 0xFF;  // Read Data latches
  cmd[14] = 0xFF;  // Increment Address
  cmd[15] = 0xFF;  // Begin programming internally timed
  return hardware().port().command(cmd);
}

bool Pickit2::P16F716::init()
{
  Array cmd;
  cmd[0] = 'c';
  cmd[1] = 0x00;   // Load Configuration
  cmd[2] = 0x02;   // Load Program latches
  cmd[3] = 0x03;   // Load Data latches
  cmd[4] = 0x04;   // Read Program memory
  cmd[5] = 0x05;   // Read Data latches
  cmd[6] = 0x06;   // Increment Address
  cmd[7] = 0x08;   // Begin programming internally timed
  cmd[8] = 0x18;   // Begin Programming externally timed
  cmd[9] = 0x0E;   // End externally time programming cycle
  cmd[10] = 0x09;  // Bulk Erase program memory
  cmd[11] = 0x0B;  // Bulk Erase Data memory
  cmd[12] = 0xFF;  // Read Program memory
  cmd[13] = 0xFF;  // Read Data latches
  cmd[14] = 0xFF;  // Increment Address
  cmd[15] = 0xFF;  // Begin programming internally timed
  return hardware().port().command(cmd);
}

//-----------------------------------------------------------------------------
bool Pickit2::P18F::init()
{
  Array cmd;
  cmd[0] = 'c';
  cmd[1] = 0x00;   // Load Configuration
  cmd[2] = 0x02;   // Load Program latches
  cmd[3] = 0x03;   // Load Data latches
  cmd[4] = 0x04;   // Read Program memory
  cmd[5] = 0x05;   // Read Data latches
  cmd[6] = 0x06;   // Increment Address
  cmd[7] = 0x08;   // Begin programming internally timed
  cmd[8] = 0x18;   // Begin Programming externally timed
  cmd[9] = 0x0A;   // End externally time programming cycle
  cmd[10] = 0x09;  // Bulk Erase program memory
  cmd[11] = 0x0B;  // Bulk Erase Data memory
  cmd[12] = 0xFF;  // Read Program memory
  cmd[13] = 0xFF;  // Read Data latches
  cmd[14] = 0xFF;  // Increment Address
  cmd[15] = 0xFF;  // Begin programming internally timed
  return hardware().port().command(cmd);
}

bool Pickit2::P18F::doEraseCommand(uint cmd1, uint cmd2)
{
  Array cmd;
  cmd[0] = 0x85;
  cmd[1] = cmd1;
  cmd[2] = cmd2;
  return hardware().port().command(cmd);
}

bool Pickit2::P18F::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryRangeType::Eeprom ) return doEraseCommand(0x84, 0x00);
  Q_ASSERT( type==Pic::MemoryRangeType::Code );
  if ( !doEraseCommand(0x81, 0x00) ) return false; // boot
  for (uint i=0; i<device().config().protection().nbBlocks(); i++)
    if ( !doEraseCommand(1 << i, 0x80) ) return false;
  return true;
}

bool Pickit2::P18F::doRead(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata)
{
  USBPort &port = static_cast<USBPort &>(hardware().port());
  data.resize(device().nbWords(type));
  QValueVector<uint> words;
  switch (type.type()) {
    case Pic::MemoryRangeType::DeviceId:
    case Pic::MemoryRangeType::UserId:
    case Pic::MemoryRangeType::Config: {
      Pickit::Array cmd = port.createCommand(0x80, data.count(), device().range(type).start.toUInt());
      if ( !port.command(cmd) ) return false;
      if ( !hardware().port().receiveWords(1, 1, words) ) return false;
      for (uint k=0; k<data.count(); k++) {
        data[k] = words[k];
        if ( vdata && !hardware().verifyWord(k, data[k], type, *vdata) ) return false;
      }
      return true;
    }
    case Pic::MemoryRangeType::Code:
      for (uint i=0; i<data.count();) {
        Array cmd;
        for (uint k=0; k<8; k++) port.fillCommand(cmd, 0x80, 64, 2*(i + 32 * k), 5*k);
        if ( !port.command(cmd) ) return false;
        for (uint k=0; k<8; k++) {
          if ( !hardware().port().receiveWords(2, 1, words) ) return false;
          for (uint k=0; k<32; k++) {
            data[i] = words[k];
            if ( vdata && !hardware().verifyWord(i, data[i], type, *vdata) ) return false;
            i++;
          }
        }
      }
      _base.progressMonitor().addTaskProgress(data.count());
      return true;
    case Pic::MemoryRangeType::Eeprom:
      for (uint i=0; i<data.count();) {
       Pickit::Array cmd = port.createCommand(0x81, 64, i, false);
        if ( !port.command(cmd) ) return false;
        if ( !hardware().port().receiveWords(1, 1, words) ) return false;
        for (uint k=0; k<64; k++) {
          data[i] = words[k];
          if ( vdata && !hardware().verifyWord(i, data[i], type, *vdata) ) return false;
          i++;
        }
      }
      _base.progressMonitor().addTaskProgress(data.count());
      return true;
    case Pic::MemoryRangeType::Cal:
    case Pic::MemoryRangeType::CalBackup:
    case Pic::MemoryRangeType::DebugVector:
    case Pic::MemoryRangeType::HardwareStack:
    case Pic::MemoryRangeType::ProgramExecutive:
    case Pic::MemoryRangeType::Nb_Types: break;
  }
  Q_ASSERT(false);
  return false;
}

bool Pickit2::P18F::doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force)
{
  Q_UNUSED(force);
  USBPort &port = static_cast<USBPort &>(hardware().port());
  switch (type.type()) {
    case Pic::MemoryRangeType::UserId: {
      Pickit::Array cmd = port.createCommand(0x82, data.count() / 2, device().range(type).start.toUInt());
      for (uint i=0; i<data.count(); i++) cmd[i + 5] = data[i].byte(0);
      return port.command(cmd);
    }
    case Pic::MemoryRangeType::Config: {
      Array cmd;
      for (uint i=0; i<data.count()/2; i++) {
        cmd[4*i]     = 0x84;
        cmd[1 + 4*i] = 2*i;
        cmd[2 + 4*i] = data[2*i].byte(0);
        cmd[3 + 4*i] = data[2*i+1].byte(0);
      }
      return port.command(cmd);
    }
    case Pic::MemoryRangeType::Code: {
      uint nb = Pickit2::data(device().name()).progWidth;
      for (uint i=0; i<data.count();) {
        bool allBlank = true;
        Pickit::Array cmd = port.createCommand(0x82, nb, 2*i);
        for (uint k=0; k<nb; k++) {
          if ( data[i].byte(0)!=0xFF || data[i].byte(1)!=0xFF ) allBlank = false;
          cmd[5 + 2*k] = data[i].byte(0);
          cmd[6 + 2*k] = data[i].byte(1);
          i++;
        }
        if ( !allBlank && !port.command(cmd) ) return false;
      }
      _base.progressMonitor().addTaskProgress(data.count());
      return true;
    }
    case Pic::MemoryRangeType::Eeprom:
      for (uint i=0; i<data.count();) {
        bool allBlank = true;
        Pickit::Array cmd = port.createCommand(0x87, 32, i, false);
        for (uint k=0; k<32; k++) {
          if ( data[i].byte(0)!=0xFF ) allBlank = false;
          cmd[4+k] = data[i].byte(0);
          i++;
        }
        if ( !allBlank && !port.command(cmd) ) return false;
      }
      _base.progressMonitor().addTaskProgress(data.count());
      return true;
    case Pic::MemoryRangeType::DeviceId:
    case Pic::MemoryRangeType::Cal:
    case Pic::MemoryRangeType::CalBackup:
    case Pic::MemoryRangeType::DebugVector:
    case Pic::MemoryRangeType::HardwareStack:
    case Pic::MemoryRangeType::ProgramExecutive:
    case Pic::MemoryRangeType::Nb_Types: break;
  }
  Q_ASSERT(false);
  return false;
}

bool Pickit2::P18F2X20::doEraseCommand(uint cmd1)
{
  Array cmd;
  cmd[0] = 0x86;
  cmd[1] = cmd1;
  return hardware().port().command(cmd);
}

bool Pickit2::P18F2X20::doEraseRange(Pic::MemoryRangeType type)
{
  if ( type==Pic::MemoryRangeType::Eeprom ) return doEraseCommand(0x81);
  Q_ASSERT( type==Pic::MemoryRangeType::Code );
  if ( !doEraseCommand(0x83) ) return false; // boot
  for (uint i=0; i<device().config().protection().nbBlocks(); i++)
    if ( !doEraseCommand(0x88 + i) ) return false;
  return true;
}
