/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic.h"

#include "common/global/global.h"
#include "common/common/misc.h"
#include "pic_register.h"
#include "pic_config.h"

//------------------------------------------------------------------------------
const Pic::MemoryRangeTypeData Pic::MEMORY_RANGE_TYPE_DATA[Nb_MemoryRangeTypes] = {
  { "program",            I18N_NOOP("Code memory"),         Writable },
  { "calibration",        I18N_NOOP("Calibration"),         Writable },
  { "user_ids",           I18N_NOOP("User IDs"),            Writable },
  { "device_id",          I18N_NOOP("Device ID"),           ReadOnly },
  { "config",             I18N_NOOP("Configuration Bits"),  Writable },
  { "eeprom",             I18N_NOOP("Data EEPROM"),         Writable },
  { "debug_vector",       I18N_NOOP("Debug Vector"),        Writable },
  { "hardware_stack",     I18N_NOOP("Hardware Stack"),      ReadOnly },
  { "calibration_backup", I18N_NOOP("Calibration Backup"),  Writable },
  { "program_executive",  I18N_NOOP("Program Executive"),   Writable }
};

const Pic::ArchitectureData Pic::ARCHITECTURE_DATA[Nb_Architectures] = {
//  name family_label nbBytesPC nbBytesWord packed nbBitsRegister registerBankLength
//  {Code, Cal, UserID, DevId, Conf, EEPROM, DebugVec, HardStack, CalBackup, Program Executive} randomAccess
  { "10X", I18N_NOOP("Baseline Family"),  0, 2, false,  8, 0x020, { 12, 12, 12, 12, 12,  8, 12,  0, 12,  0 }, false }, // 9, 10, 11 or 12-bit program counter
  { "16X", I18N_NOOP("Midrange Family"), 13, 2, false,  8, 0x080, { 14, 14, 14, 14, 14,  8, 14,  0, 14,  0 }, false }, // max eeprom: 256 words
  { "17X", I18N_NOOP("17C Family"),      16, 2, false,  8, 0x100, { 16,  0,  0,  0, 16,  8,  0,  0,  0,  0 }, true  },
  { "18C", I18N_NOOP("18C Family"),      21, 2, true,   8, 0x100, { 16,  8,  8,  8,  8,  8, 16,  0,  8,  0 }, true  },
  { "18X", I18N_NOOP("18F Family"),      21, 2, true,   8, 0x100, { 16,  8,  8,  8,  8,  8, 16,  0,  8,  0 }, true  },
  { "18J", I18N_NOOP("18J Family"),      21, 2, true,   8, 0x100, { 16,  8,  8,  8,  8,  8, 16,  0,  8,  0 }, true  },
  { "24F", I18N_NOOP("24F Family"),      23, 4, true,  16, 0x800, { 24,  0, 16, 16, 16,  0, 24,  0,  0, 24 }, true  },
  { "24H", I18N_NOOP("24H Family"),      23, 4, true,  16, 0x800, { 24,  0, 16, 16, 16,  0, 24,  0,  0, 24 }, true  },
  { "30X", I18N_NOOP("30F Family"),      23, 4, false, 16, 0xA00, { 24,  0, 24, 16, 16, 16, 24,  0,  0, 24 }, true  } // dsPIC: eeprom max = 2 kwords = 4 kbytes
};

const Pic::ProgVoltageTypeData Pic::PROG_VOLTAGE_TYPE_DATA[Nb_ProgVoltageTypes] = {
  { "vpp"            },
  { "vdd_prog"       },
  { "vdd_prog_write" }
};

//-----------------------------------------------------------------------------
Pic::Data::Data()
{
  for (uint i=0; i<Nb_ProgVoltageTypes; i++) {
    _voltages[i].min = 0.0;
    _voltages[i].max = 0.0;
    _voltages[i].nominal = 0.0;
  }
  for (uint i=0; i<Nb_MemoryRangeTypes; i++) {
    _ranges[i].properties = NotPresent;
    _ranges[i].start = 0;
    _ranges[i].end = 0;
    _ranges[i].rmask = 0;
    _ranges[i].hexFileOffset = 0;
  }
  _config = new Config(*this);
  _calibration.opcode = 0;
  _calibration.opcodeMask = 0;
  _registersData = new RegistersData(*this);
}

Pic::Data::~Data()
{
  delete _config;
}

QString Pic::Data::informationUrl() const
{
  QString url = "http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=";
  if ( document().isEmpty() ) {
    url += "2044&AllWords=";
    if ( name().startsWith("PS")) url += name();
    if ( name().startsWith("30") ) url += "dsPIC" + name();
    else url += "PIC" + name();
  } else url += "1335&dDocName=en" + document();
  return url;
}

QString Pic::Data::informationHtml() const
{
  // memory type
  QString s = htmlTableRow(i18n("Memory Type"), i18n(Device::MEMORY_TECHNOLOGY_DATA[memoryTechnology()].label));
  if ( isPresent(Pic::MemoryCode) ) {
    uint nbw = nbWords(Pic::MemoryCode);
    QString tmp = i18n("%1 words").arg(formatNumber(nbw));
    tmp += i18n(" (%2 bits)").arg(nbBitsWord(Pic::MemoryCode));
    s += htmlTableRow(i18n(Pic::MEMORY_RANGE_TYPE_DATA[Pic::MemoryCode].label), tmp);
  }
  if ( isPresent(Pic::MemoryEeprom) ) {
    uint nbw = nbWords(Pic::MemoryEeprom);
    QString tmp = i18n("%1 bytes").arg(formatNumber(nbw));
    tmp += i18n(" (%2 bits)").arg(nbBitsWord(Pic::MemoryEeprom));
    if ( !(range(Pic::MemoryEeprom).properties & Programmable) ) tmp += i18n(" (not programmable)");
    s += htmlTableRow(i18n(Pic::MEMORY_RANGE_TYPE_DATA[Pic::MemoryEeprom].label), tmp);
  }
  // io ports
  QString tmp;
  uint nb = 0;
  for (uint i=0; i<Device::MAX_NB_PORTS; i++) {
    if ( !registersData().hasPort(i) ) continue;
    uint nbBits = 0;
    for (uint k=0; k<Device::MAX_NB_PORT_BITS; k++) if ( registersData().hasPortBit(i, k) ) nbBits++;
    tmp += registersData().portName(i) + "[" + QString::number(nbBits) + "] ";
    nb++;
  }
  if ( nb==0 ) tmp = i18n("<none>");
  s += htmlTableRow(i18n("IO Ports"), tmp);
  return s;
}

bool Pic::Data::isReadable(MemoryRangeType type) const
{
  return ( range(type).properties & Programmable );
}

bool Pic::Data::isWritable(MemoryRangeType type) const
{
  return ( (MEMORY_RANGE_TYPE_DATA[type].properties & Writable) && (range(type).properties & Programmable) );
}

uint Pic::Data::addressIncrement(MemoryRangeType type) const
{
  uint inc = ARCHITECTURE_DATA[_architecture].nbBytesWord;
  if ( ARCHITECTURE_DATA[_architecture].packed && ( type==MemoryCode || type==MemoryDebugVector ) ) return inc;
  return inc / 2;
}

uint Pic::Data::nbWords(MemoryRangeType type) const
{
  if ( !isPresent(type) ) return 0;
  return nbAddresses(type) / addressIncrement(type);
}

uint Pic::Data::nbAddresses(MemoryRangeType type) const
{
  if ( !isPresent(type) ) return 0;
  return (range(type).end - range(type).start + 1);
}

QString Pic::Data::fname(Device::FrequencyRange::Special special) const
{
  QString s = name();
  switch (special) {
    case Device::FrequencyRange::Normal: break;
    case Device::FrequencyRange::LowPower:
      // assume name is of form "NNX..."
      s.insert(2, 'L');
      break;
    case Device::FrequencyRange::LowVoltage:
      // assume name is of form "NNXN..."
      s.replace(2, 1, "LV");
      break;
    case Device::FrequencyRange::Nb_Specials: Q_ASSERT(false); break;
  }
  return s;
}

bool Pic::Data::matchId(uint rawId, Device::IdData &idata) const
{
  if ( !isPresent(MemoryDeviceId) ) return false;
  uint nid = 0;
  switch ( architecture() ) {
    case Arch_10X:
    case Arch_16X:
    case Arch_17X:
    case Arch_18C:
    case Arch_18F:
    case Arch_18J:
      nid = rawId & ~0x1F;
      idata.revision = rawId & 0x1F;
      break;
    case Arch_24F:
      nid = (rawId >> 16) & 0x3FFF;
      idata.revision = (rawId >> 6) & 0x7;
      idata.minorRevision = rawId & 0x7;
      break;
    case Arch_24H:
    case Arch_30X:
      nid = (rawId >> 16) & 0xFFFF;
      idata.revision = (rawId >> 6) & 0x3F;
      idata.minorRevision = rawId & 0x3F;
      idata.process = (rawId >> 12) & 0xF;
      break;
    case Nb_Architectures: Q_ASSERT(false); break;
  }
  return ( nid==id() );
}

QStringList Pic::Data::idNames(const Device::IdList &idList) const
{
  QStringList list;
  QMap<QString, Device::IdData>::const_iterator it = idList.begin();
  for (; it!=idList.end(); ++it) {
    switch (_architecture) {
      case Arch_10X:
      case Arch_16X:
      case Arch_17X:
      case Arch_18C:
      case Arch_18F:
      case Arch_18J:
        list += i18n("%1 (rev. %2)").arg(it.key()).arg(it.data().revision);
        break;
      case Arch_24F:
        list += i18n("%1 (rev. %2.%3)").arg(it.key())
                .arg(it.data().revision).arg(it.data().minorRevision);
        break;
      case Arch_24H:
      case Arch_30X:
        list += i18n("%1 (proc. %2; rev. %3.%4)").arg(it.key())
                .arg(it.data().process).arg(it.data().revision).arg(it.data().minorRevision);
        break;
      case Nb_Architectures: Q_ASSERT(false); break;
    }
  }
  return list;
}

bool Pic::Data::checkCalibration(const Device::Array &data, QString *message) const
{
  Q_ASSERT( nbWords(MemoryCal)==data.count() );
  for (uint i=0; i<data.count(); i++) {
    QString address = toHexLabel(range(MemoryCal).start + i*addressIncrement(MemoryCal), nbCharsAddress());
    if ( data[i]==mask(MemoryCal) ) {
      if (message) *message = i18n("Calibration word at address %1 is blank.").arg(address);
      return false;
    }
  }
  if ( data.count()==1 ) {
    if ( (data[0] & _calibration.opcodeMask)!=_calibration.opcode ) {
      if (message) *message = i18n("Calibration word is not a compatible opcode (%2).")
                              .arg(toHexLabel(_calibration.opcode, nbCharsWord(MemoryCode)));
      return false;
    }
  }
  return true;
}

const Pic::RegistersData &Pic::Data::registersData() const
{
  return static_cast<const RegistersData &>(*_registersData);
}

bool Pic::Data::hasUSART() const
{
  return ( registersData().sfrs.contains("TXSTA")        // 16F
           || registersData().sfrs.contains("TXSTA1")    // 18F
           || registersData().sfrs.contains("U1MODE") ); // 30F
}

bool Pic::Data::hasUSB() const
{
  return registersData().sfrs.contains("UCON");
}

Device::Array Pic::Data::gotoInstruction(uint address, bool withPageSelection) const
{
  Q_ASSERT( address<addressIncrement(MemoryCode)*nbWords(MemoryCode) );
  Device::Array a;
  switch (_architecture) {
  case Arch_10X:
    if ( nbWords(MemoryCode)>0x1FF && withPageSelection)
      a.append(0x4A3 | (address>0x1FF ? 0x100 : 0x000)); // bsf STATUS,PA0 or bcf STATUS,PA0
    a.append(0xA00 | (address & 0x1FF)); // goto
    break;
  case Arch_16X:
    if ( nbWords(MemoryCode)>0x7FF && withPageSelection ) {
      if ( address<=0x7FF ) a.append(0x018A); // clrf PCLATH
      else {
        a.append(0x3000 | (address >> 8)); // movl high address
        a.append(0x008A); // movwf PCLATH
      }
    }
    a.append(0x2800 | (address & 0x7FF));
    break;
  case Arch_17X:
    a.append(0xC000 | (address & 0x1FFF));
    break;
  case Arch_18C:
  case Arch_18F:
  case Arch_18J:
    a.append(0xEF00 | ((address/2) & 0xFF));
    a.append(0xF000 | ((address/2) >> 8));
    break;
  case Arch_24F:
  case Arch_24H:
  case Arch_30X:
    a.append(0x040000 | (address & 0x00FFFE));
    a.append(0X000000 | (address >> 16));
    break;
  case Nb_Architectures: Q_ASSERT(false); break;
  }
  return a;
}

bool Pic::Data::isGotoInstruction(uint instruction) const
{
  switch (_architecture) {
  case Arch_10X: return ( (instruction & 0xE00)==0xA00 );
  case Arch_16X: return ( (instruction & 0xF800)==0x2800 );
  case Arch_17X: return ( (instruction & 0xE000)==0xC000 );
  case Arch_18C:
  case Arch_18F:
  case Arch_18J: return ( (instruction & 0xFF00)==0xEF00 );
  case Arch_24F:
  case Arch_24H:
  case Arch_30X: return ( (instruction & 0xFF0000)==0x040000 );
  case Nb_Architectures: Q_ASSERT(false); break;
  }
  return false;
}

uint Pic::Data::nbWordsAlignment(MemoryRangeType type) const
{
  if ( type!=MemoryCode ) return 1;
  return QMAX(_nbWordsCodeWrite, uint(16));
}

//----------------------------------------------------------------------------
QDataStream &Pic::operator <<(QDataStream &s, const VoltageData &vd)
{
  s << vd.min << vd.max << vd.nominal;
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, VoltageData &vd)
{
  s >> vd.min >> vd.max >> vd.nominal;
  return s;
}

QDataStream &Pic::operator <<(QDataStream &s, const MemoryRangeData &mrd)
{
  s << Q_UINT8(mrd.properties) << mrd.start << mrd.end << mrd.rmask << mrd.hexFileOffset;
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, MemoryRangeData &mrd)
{
  Q_UINT8 properties;
  s >> properties >> mrd.start >> mrd.end >> mrd.rmask >> mrd.hexFileOffset;
  mrd.properties = MemoryRangeProperties(properties);
  return s;
}

QDataStream &Pic::operator <<(QDataStream &s, const ChecksumData &cd)
{
  s << cd.constant << cd.protectedConstant << Q_UINT8(cd.protectedType);
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, ChecksumData &cd)
{
  Q_UINT8 type;
  s >> cd.constant >> cd.protectedConstant >> type;
  cd.protectedType = ChecksumType(type);
  return s;
}

QDataStream &Pic::operator <<(QDataStream &s, const CalibrationData &cd)
{
  s << cd.opcode << cd.opcodeMask;
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, CalibrationData &cd)
{
  s >> cd.opcode >> cd.opcodeMask;
  return s;
}

QDataStream &Pic::operator <<(QDataStream &s, const Data &data)
{
  s << static_cast<const Device::Data &>(data);
  s << Q_UINT8(data._architecture) << data._id << data._nbBitsPC;
  for (uint i=0; i<Nb_ProgVoltageTypes; i++) s << data._voltages[i];
  for (uint i=0; i<Nb_MemoryRangeTypes; i++) s << data._ranges[i];
  s << *data._config << data._checksum << data._calibration << static_cast<const Pic::RegistersData &>(*data._registersData);
  s << data._nbWordsCodeWrite << data._nbWordsCodeRowErase;
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, Data &data)
{
  s >> static_cast<Device::Data &>(data);
  Q_UINT8 architecture;
  s >> architecture >> data._id >> data._nbBitsPC;
  data._architecture = Architecture(architecture);
  for (uint i=0; i<Nb_ProgVoltageTypes; i++) s >> data._voltages[i];
  for (uint i=0; i<Nb_MemoryRangeTypes; i++) s >> data._ranges[i];
  s >> *data._config >> data._checksum >> data._calibration >> static_cast<Pic::RegistersData &>(*data._registersData);
  s >> data._nbWordsCodeWrite >> data._nbWordsCodeRowErase;
  return s;
}
