/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "bootloader_tbl.h"

#include "bootloader_data.h"

//-----------------------------------------------------------------------------
Port::Serial::Speed TinyBootloader::Config::readSpeed()
{
  uint speed = readUIntEntry("port_speed", 19200);
  for (uint i=0; i<Port::Serial::Nb_Speeds; i++)
    if ( speed==Port::Serial::SPEED_VALUES[i] ) return Port::Serial::Speed(i);
  return Port::Serial::S19200;
}
void TinyBootloader::Config::writeSpeed(Port::Serial::Speed speed)
{
  writeEntry("port_speed", Port::Serial::SPEED_VALUES[speed]);
}

uint TinyBootloader::Config::readTimeout()
{
  return readUIntEntry("timeout", 300);
}
void TinyBootloader::Config::writeTimeout(uint timeout)
{
  writeEntry("timeout", timeout);
}

uint TinyBootloader::Config::readRetries()
{
  return readUIntEntry("nb_retries", 5);
}
void TinyBootloader::Config::writeRetries(uint nb)
{
  writeEntry("nb_retries", nb);
}

//-----------------------------------------------------------------------------
TinyBootloader::Hardware::Hardware(::Programmer::Base &base, const QString &portDevice)
  : ::Bootloader::Hardware(base, new Port::Serial(portDevice, Port::Serial::NoProperty, base))
{
  Config config;
  _timeout = config.readTimeout();
  _retries = config.readRetries();
}

bool TinyBootloader::Hardware::openPort()
{
  if ( !port()->open() ) return false;
  Config config;
  if ( !port()->setMode(Port::Serial::IgnoreBreak | Port::Serial::IgnoreParity,
                        Port::Serial::ByteSize8 | Port::Serial::IgnoreControlLines,
                        config.readSpeed(), _timeout) )
    return false;
  return true;
}

bool TinyBootloader::Hardware::internalConnectHardware()
{
  if ( !openPort() ) return false;
  // #### possibly do hard (RTS) or soft reset (codes)
  uchar uc = 0xC1;
  if ( !port()->sendChar(uc, _timeout) ) return false;
  if ( !port()->receiveChar((char &)_id, _timeout) ) return false;
  if ( !waitReady(0) ) return false;
  return true;
}

bool TinyBootloader::Hardware::verifyDeviceId()
{
  uchar id = Bootloader::data(device().name()).ids[Bootloader::TinyBootloader];
  QValueVector<QString> list = _base.group().supportedDevices();
  QStringList devices;
  for (uint i=0; i<uint(list.count()); i++)
    if ( _id==Bootloader::data(list[i]).ids[Bootloader::TinyBootloader] ) devices.append(list[i]);
  if ( _id!=id ) {
    if ( devices.count()==0 ) log(Log::Error, i18n("Unknown id returned by bootloader (%1 read and %2 expected).").arg(toHexLabel(_id, 2)).arg(toHexLabel(id, 2)));
    else log(Log::Error, i18n("Id returned by bootloader corresponds to other devices: %1 (%2 read and %3 expected).").arg(devices.join(" ")).arg(toHexLabel(_id, 2)).arg(toHexLabel(id, 2)));
    // we can't ask for "continue anyway?" because bootloader can timeout...
    return false;
  }
  log(Log::Information, "  " + i18n("Bootloader identified device as: %1").arg(devices.join(" ")));
  return true;
}

bool TinyBootloader::Hardware::waitReady(bool *checkCRC)
{
  char c;
  if ( !port()->receiveChar(c, _timeout) ) return false;
  if ( c=='K' ) {
    if (checkCRC) *checkCRC = true;
    return true;
  }
  if (checkCRC) {
    *checkCRC = false;
    if ( c=='N' ) return true;
    log(Log::Error, i18n("Received unexpected character ('%1' received; 'K' or 'N' expected).").arg(toPrintable(c, PrintAlphaNum)));
    return true;
  }
  log(Log::Error, i18n("Received unexpected character ('%1' received; 'K' expected).").arg(toPrintable(c, PrintAlphaNum)));
  return false;
}

bool TinyBootloader::Hardware::sendChar(char c, uchar *crc)
{
  if (crc) *crc += c;
  return port()->sendChar(c, 10*_timeout);
}

bool TinyBootloader::Hardware::sendCodeAddress(uint address, uchar &crc)
{
  switch (device().architecture()) {
    case Pic::Arch_16X:
      if ( !sendChar(address >> 8, &crc) ) return false;          // address high
      if ( !sendChar(address & 0xFF, &crc) ) return false;        // address low
      break;
    case Pic::Arch_18F:
      if ( !sendChar(address >> 16, &crc) ) return false;           // address upper
      if ( !sendChar((address >> 8) & 0xFF, &crc) ) return false;   // address high
      if ( !sendChar(address & 0xFF, &crc) ) return false;          // address low
      break;
    case Pic::Arch_30X:
      if ( !sendChar(address & 0xFF, &crc) ) return false;          // address low
      if ( !sendChar((address >> 8) & 0xFF, &crc) ) return false;   // address high
      if ( !sendChar(address >> 16, &crc) ) return false;           // address upper
      break;
    default: Q_ASSERT(false); return false;
  }
  return true;
}

bool TinyBootloader::Hardware::endWrite(uchar crc, uint &retries, bool &ok)
{
  if ( !sendChar(-crc & 0xFF, 0) ) return false;
  if ( !waitReady(&ok) ) return false;
  if ( !ok ) {
    if ( retries==0 ) {
      log(Log::Error, i18n("Too many failures: bailing out."));
      return false;
    }
    retries--;
    log(Log::Warning, i18n("CRC error from bootloader: retrying..."));
  }
  return true;
}

bool TinyBootloader::Hardware::writeCode(const Device::Array &data, bool erase)
{
  uint nb = data.count() - 100;
  Device::Array wdata(nb+4);

  // check that there is nothing on top of bootloader
  for (uint i=nb; i<data.size(); i++) {
    if ( data[i]==device().mask(Pic::MemoryCode) ) continue;
    uint address = device().addressIncrement(Pic::MemoryCode) * i;
    log(Log::Warning, "  " + i18n("Code is present in bootloader reserved area (at address %1). It will be ignored.").arg(toHexLabel(address, device().nbCharsAddress())));
    break;
  }

  // check first 4 instructions for "goto" and copy them
  if (erase) {
    for (uint i=0; i<4; i++) wdata[nb+i] = device().nopInstruction();
  } else {
    bool ok = false;
    for (uint i=0; i<4; i++) {
      wdata[nb+i] = data[i];
      if ( !ok && device().isGotoInstruction(data[i]) ) {
        ok = true;
        if ( i==3 && device().gotoInstruction(0x0, false).count()==2 )
          log(Log::Warning, "  " + i18n("Only the first word of the \"goto\" instruction is into the first four instructions."));
      }
    }
    if ( !ok ) log(Log::Warning, "  " + i18n("No \"goto\" instruction in the first four instructions."));
  }

  // place "goto size-100" at reset vector
  wdata[0] = device().nopInstruction(); // for icd2
  uint address = device().addressIncrement(Pic::MemoryCode) * (nb+4);
  Device::Array a = device().gotoInstruction(address, true);
  for (uint i=0; i<a.size(); i++) wdata[1+i] = a[i];
  // copy the rest
  for (uint i=4; i<nb; i++) wdata[i] = data[i];

  uint retries = _retries, nbWords = 0x20; // 16F: 64 bytes (80 bytes reserved) ; 18F: 64 bytes ; 30F: 96 bytes
  Q_ASSERT( (data.count()%nbWords)==0 );
  for (uint i=0; i<wdata.count(); i+=nbWords) {
    if ( !erase ) {
      bool write = false;
      for (uint k=0; k<nbWords; k++) {
        if ( wdata[i+k]==device().mask(Pic::MemoryCode) ) continue;
        write = true;
        break;
      }
      if ( !write ) continue; // skip
    }
    for (;;) {
      uchar crc = 0;
      uint address = device().addressIncrement(Pic::MemoryCode) * i;
      if ( !sendCodeAddress(address, crc) ) return false;
      uint nbw = device().nbBytesWord(Pic::MemoryCode);
      if ( !sendChar(nbw*nbWords, &crc) ) return false;
      log(Log::NormalDebug, QString("write code memory at %1: %2 bytes").arg(toHexLabel(address, 4)).arg(2*nbWords));
      for(uint k=0; k<nbWords; k++) {
        if ( !sendChar(wdata[i+k] & 0xFF, &crc) ) return false;          // data low
        if ( !sendChar((wdata[i+k] >> 8) & 0xFF, &crc) ) return false;   // data high
        if ( nbw==3 && !sendChar(wdata[i+k] >> 16, &crc) ) return false; // upper byte
      }
      bool ok;
      if ( !endWrite(crc, retries, ok) ) return false;
      if (ok) break;
    }
  }
  return true;
}

bool TinyBootloader::Hardware::writeConfig(const Device::Array &data)
{
  if ( device().architecture()==Pic::Arch_16X ) return false;
  uint retries = _retries;
  for (uint i=0; i<data.count(); i++) {
    for (;;) {
      uchar crc = 0;
      uint address = device().range(Pic::MemoryConfig).start + i;
      switch (device().architecture() ) {
        case Pic::Arch_18F:
          if ( !sendChar(0x80 | (address >> 16), &crc) ) return false; // address upper | flag
          if ( !sendChar((address >> 8) & 0xFF, &crc) ) return false;  // address high
          if ( !sendChar(address & 0xFF, &crc) ) return false;         // address low
          if ( !sendChar(1, &crc) ) return false;                      // nb bytes
          if ( !sendChar(data[i], &crc) ) return false;                // data
          break;
        case Pic::Arch_30X:
          if ( !sendChar(i & 0xFF, &crc) ) return false;        // address low
          if ( !sendChar((i >> 8) & 0xFF, &crc) ) return false; // address high
          if ( !sendChar(char(0xF8), &crc) ) return false;      // address upper
          if ( !sendChar(2, &crc) ) return false;                     // nb bytes
          if ( !sendChar(data[i] & 0xFF, &crc) ) return false;        // data low
          if ( !sendChar((data[i] >> 8) & 0xFF, &crc) ) return false; // data high
          break;
        default: Q_ASSERT(false); return false;
      }
      bool ok;
      if ( !endWrite(crc, retries, ok) ) return false;
      if (ok) break;
    }
  }
  return true;
}

bool TinyBootloader::Hardware::writeEeprom(const Device::Array &data)
{
  uint retries = _retries;
  for (uint i=0; i<data.count(); i++) {
    for (;;) {
      uchar crc = 0;
      switch (device().architecture() ) {
        case Pic::Arch_16X:
          if ( !sendChar(0x40, &crc) ) return false;    // flag
          if ( !sendChar(i, &crc) ) return false;       // address
          if ( !sendChar(1, &crc) ) return false;       // nb bytes
          if ( !sendChar(data[i], &crc) ) return false; // data
          break;
        case Pic::Arch_18F:
          if ( !sendChar(0x40, &crc) ) return false;    // flag
          if ( !sendChar(i, &crc) ) return false;       // address
          if ( !sendChar(data[i], &crc) ) return false; // data
          if ( !sendChar(0, &crc) ) return false;       // dummy
          break;
        case Pic::Arch_30X:
          if ( !sendChar(i & 0xFF, &crc) ) return false;        // address low
          if ( !sendChar((i >> 8) & 0xFF, &crc) ) return false; // address high
          if ( !sendChar(0x7F, &crc) ) return false;            // address upper
          if ( !sendChar(2, &crc) ) return false;               // nb bytes
          if ( !sendChar(data[i] & 0xFF, &crc) ) return false;          // data low
          if ( !sendChar((data[i] >> 8) & 0xFF, &crc) ) return false;   // data high
          break;
        default: Q_ASSERT(false); return false;
      }
      bool ok;
      if ( !endWrite(crc, retries, ok) ) return false;
      if (ok) break;
    }
  }
  return true;
}

bool TinyBootloader::Hardware::write(Pic::MemoryRangeType type, const Device::Array &data)
{
  Q_ASSERT( data.count()==device().nbWords(type) );
  if ( type==Pic::MemoryCode ) return writeCode(data, false);
  if ( type==Pic::MemoryConfig ) return writeConfig(data);
  if ( type==Pic::MemoryEeprom ) return writeEeprom(data);
  return false;
}

//-----------------------------------------------------------------------------
bool TinyBootloader::DeviceSpecific::canWriteRange(Pic::MemoryRangeType type) const
{
  if ( type==Pic::MemoryEeprom || type==Pic::MemoryCode ) return true;
  if ( device().architecture()==Pic::Arch_18F && type==Pic::MemoryConfig ) return true;
  return false;
}

bool TinyBootloader::DeviceSpecific::doErase(bool)
{
  Bootloader::Config config(static_cast<const Bootloader::Group &>(_base.group()));
  bool eeprom = config.readProgramEeprom();
  if (eeprom) log(Log::Warning, "  " + i18n("Only code and EEPROM will be erased."));
  else log(Log::Warning, "  " + i18n("Only code will be erased."));
  if ( doEraseRange(Pic::MemoryCode)==false ) return false;
  if ( eeprom && doEraseRange(Pic::MemoryEeprom)==false ) return false;
  return true;
}

bool TinyBootloader::DeviceSpecific::doEraseRange(Pic::MemoryRangeType type)
{
  Pic::Memory memory(device());
  if ( type==Pic::MemoryCode ) return static_cast<Hardware &>(hardware()).writeCode(memory.arrayForWriting(type), true);
  return hardware().write(type, memory.arrayForWriting(type));
}

//-----------------------------------------------------------------------------
TinyBootloader::Base::Base(const Programmer::Group &group, const Pic::Data *data)
  : Bootloader::Base(group, data, "tiny_bootloader_programmer_base")
{}
