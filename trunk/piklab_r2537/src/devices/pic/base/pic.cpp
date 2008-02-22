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
#include "common/global/purl.h"
#include "pic_register.h"
#include "pic_config.h"

//------------------------------------------------------------------------------
const Pic::ProgVoltageType::Data Pic::ProgVoltageType::DATA[Nb_Types] = {
  { "vpp",            0 },
  { "vdd_prog",       0 },
  { "vdd_prog_write", 0 }
};

const Pic::MemoryRangeType::Data Pic::MemoryRangeType::DATA[Nb_Types] = {
  { "code",               I18N_NOOP("Code memory"),         Writable },
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

const Pic::SelfWrite::Data Pic::SelfWrite::DATA[Nb_Types] = {
  { "yes", 0 },
  { "no",  0 }
};

const Pic::DeviceType::Data Pic::DeviceType::DATA[Nb_Types] = {
  { 0, I18N_NOOP("Normal") },
  { 0, I18N_NOOP("J")      },
  { 0, I18N_NOOP("K")      }
};

const Pic::Architecture::Data Pic::Architecture::DATA[Nb_Types] = {
//  name family_label nbBytesPC nbBytesWord packed nbBitsRegister registerBankLength
//  {Code, Cal, UserID, DevId, Conf, EEPROM, DebugVec, HardStack, CalBackup, Program Executive} randomAccess
  { "10X", I18N_NOOP("Baseline Family"),  0, 2, false,  8, 0x020, { 12, 12, 12, 12, 12,  8, 12,  0, 12,  0 }, false, SelfWrite::No,       DeviceType::Normal }, // 9, 10, 11 or 12-bit program counter
  { "16X", I18N_NOOP("Midrange Family"), 13, 2, false,  8, 0x080, { 14, 14, 14, 14, 14,  8, 14,  0, 14,  0 }, false, SelfWrite::Nb_Types, DeviceType::Normal }, // max eeprom: 256 words
  { "17C", I18N_NOOP("17C Family"),      16, 2, false,  8, 0x100, { 16,  0,  0,  0, 16,  8,  0,  0,  0,  0 }, true,  SelfWrite::No,       DeviceType::Normal },
  { "18C", I18N_NOOP("18C Family"),      21, 2, true,   8, 0x100, { 16,  8,  8,  8,  8,  8, 16,  0,  8,  0 }, true,  SelfWrite::No,       DeviceType::Normal },
  { "18F", I18N_NOOP("18F Family"),      21, 2, true,   8, 0x100, { 16,  8,  8,  8,  8,  8, 16,  0,  8,  0 }, true,  SelfWrite::Nb_Types, DeviceType::Normal },
  { "18J", I18N_NOOP("18J Family"),      21, 2, true,   8, 0x100, { 16,  8,  8,  8,  8,  8, 16,  0,  8,  0 }, true,  SelfWrite::Yes,      DeviceType::J      },
  { "24F", I18N_NOOP("24F Family"),      23, 4, false, 16, 0x800, { 24,  0,  0, 16, 24,  0, 24,  0,  0, 24 }, true,  SelfWrite::Yes,      DeviceType::J      },
  { "24H", I18N_NOOP("24H Family"),      23, 4, false, 16, 0x800, { 24,  0,  8, 16,  8,  0, 24,  0,  0, 24 }, true,  SelfWrite::Yes,      DeviceType::J      },
  { "30F", I18N_NOOP("30F Family"),      23, 4, false, 16, 0xA00, { 24,  0, 24, 16, 16, 16, 24,  0,  0, 24 }, true,  SelfWrite::Yes,      DeviceType::Normal }, // dsPIC: eeprom max = 2 kwords = 4 kbytes
  { "33F", I18N_NOOP("33F Family"),      23, 4, false, 16, 0x800, { 24,  0,  8, 16,  8,  0, 24,  0,  0, 24 }, true,  SelfWrite::Yes,      DeviceType::J      }
};

const Pic::Checksum::Algorithm::Data Pic::Checksum::Algorithm::DATA[Nb_Types] = {
  { "",      0 },
  { "XOR4",  0 },
  { "XNOR7", 0 },
  { "XNOR8", 0 }
};

const Pic::Feature::Data Pic::Feature::DATA[Nb_Types] = {
  { "ccp", I18N_NOOP("CCP")      },
  { "adc", I18N_NOOP("ADC")      },
  { "ssp", I18N_NOOP("SSP")      },
  { "lvd", I18N_NOOP("Low Voltage Detect") },
  { "usb", I18N_NOOP("USB")      },
  { "usart",           I18N_NOOP("USART")    },
  { "can",             I18N_NOOP("CAN")      },
  { "ecan",            I18N_NOOP("ECAN")     },
  { "ethernet",        I18N_NOOP("Ethernet") },
  { "lcd",             I18N_NOOP("LCD")      },
  { "motor_control",   I18N_NOOP("Motor Control") },
  { "motion_feedback", I18N_NOOP("Motion Feeback") },
  { "self_write",      I18N_NOOP("Self-Write") }
};

//-----------------------------------------------------------------------------
Pic::Data::Data()
  : Device::Data(new RegistersData(*this))
{
  FOR_EACH(ProgVoltageType, type) {
    _voltages[type].min = 0.0;
    _voltages[type].max = 0.0;
    _voltages[type].nominal = 0.0;
  }
  FOR_EACH(MemoryRangeType, type) {
    _ranges[type].properties = NotPresent;
    _ranges[type].start = 0;
    _ranges[type].end = 0;
    _ranges[type].rmask = 0;
    _ranges[type].hexFileOffset = 0;
  }
  _config = new Config(*this);
  _calibration.opcode = 0;
  _calibration.opcodeMask = 0;
}

Pic::Data::~Data()
{
  delete _config;
}

QString Pic::Data::informationHtml() const
{
  // memory type
  QString s = htmlTableRow(i18n("Memory Type"), i18n(memoryTechnology().label()));
  if ( isPresent(MemoryRangeType::Code) ) {
    uint nbw = nbWords(MemoryRangeType::Code);
    QString tmp = i18n("%1 words").arg(formatNumber(nbw));
    tmp += i18n(" (%2 bits)").arg(nbBitsWord(MemoryRangeType::Code));
    s += htmlTableRow(i18n(MemoryRangeType(MemoryRangeType::Code).label()), tmp);
  }
  if ( isPresent(MemoryRangeType::Eeprom) ) {
    uint nbw = nbWords(MemoryRangeType::Eeprom);
    QString tmp = i18n("%1 bytes").arg(formatNumber(nbw));
    tmp += i18n(" (%2 bits)").arg(nbBitsWord(MemoryRangeType::Eeprom));
    if ( !(range(MemoryRangeType::Eeprom).properties & Programmable) ) tmp += i18n(" (not programmable)");
    s += htmlTableRow(i18n(MemoryRangeType(MemoryRangeType::Eeprom).label()), tmp);
  }

  // io ports
  QString tmp;
  if ( registersData().nbBanks!=0 ) {
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
  }

  // features
  tmp = QString::null;
  FOR_EACH(Feature, feature) {
    if ( !hasFeature(feature) ) continue;
    if ( !tmp.isEmpty() ) tmp += ", ";
    tmp += i18n(feature.label());
  }
  if ( !tmp.isEmpty() ) s += htmlTableRow(i18n("Features"), tmp);

  return s;
}

bool Pic::Data::isReadable(MemoryRangeType type) const
{
  return ( range(type).properties & Programmable );
}

bool Pic::Data::isWritable(MemoryRangeType type) const
{
  return ( (type.data().properties & Writable) && (range(type).properties & Programmable) );
}

uint Pic::Data::addressIncrement(MemoryRangeType type) const
{
  uint inc = _architecture.data().nbBytesWord;
  if ( _architecture.data().packed
       && ( type==MemoryRangeType::Code || type==MemoryRangeType::DebugVector ) ) return inc;
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

QString Pic::Data::fname(Device::Special special) const
{
  QString s = name();
  switch (special.type()) {
    case Device::Special::Normal: break;
    case Device::Special::LowPower:
      // assume name is of form "NNX..."
      s.insert(2, 'L');
      break;
    case Device::Special::LowVoltage:
      // assume name is of form "NNXN..."
      s.replace(2, 1, "LV");
      break;
    case Device::Special::HighVoltage:
      // assume name is of form "NNXN..."
      s.replace(2, 1, "HV");
      break;
    case Device::Special::Nb_Types: Q_ASSERT(false); break;
  }
  return s;
}

bool Pic::Data::matchId(BitValue rawId, Device::IdData &idata) const
{
  if ( !isPresent(MemoryRangeType::DeviceId) ) return false;
  QMap<Device::Special, BitValue>::const_iterator it;
  for (it=_ids.begin(); it!=_ids.end(); ++it) {
    idata.special = it.key();
    BitValue nid = 0x0;
    switch (architecture().type()) {
      case Architecture::P10X:
      case Architecture::P16X:
      case Architecture::P17C:
      case Architecture::P18C:
      case Architecture::P18F:
      case Architecture::P18J:
        nid = rawId.clearMaskBits(0x1F);
        idata.revision = rawId.maskWith(0x1F);
        break;
      case Architecture::P24F:
        nid = (rawId >> 16).maskWith(0x3FFF);
        idata.revision = (rawId >> 6).maskWith(0x7);
        idata.minorRevision = rawId.maskWith(0x7);
        break;
      case Architecture::P30F:
        nid = (rawId >> 16).maskWith(0xFFFF);
        idata.revision = (rawId >> 6).maskWith(0x3F);
        idata.minorRevision = rawId.maskWith(0x3F);
        idata.process = (rawId >> 12).maskWith(0xF);
        break;
      case Architecture::P24H:
      case Architecture::P33F:
        nid = (rawId >> 16).maskWith(0xFFFF);
        idata.revision = rawId.maskWith(0xFFFF); // ??
        break;
      case Architecture::Nb_Types: Q_ASSERT(false); break;
    }
    if ( nid==it.data() ) return true;
  }
  return false;
}

QStringList Pic::Data::idNames(const QMap<QString, Device::IdData> &ids) const
{
  QStringList list;
  QMap<QString, Device::IdData>::const_iterator it;
  for (it=ids.begin(); it!=ids.end(); ++it) {
    switch (_architecture.type()) {
      case Architecture::P10X:
      case Architecture::P16X:
      case Architecture::P17C:
      case Architecture::P18C:
      case Architecture::P18F:
      case Architecture::P18J:
        list += i18n("%1 (rev. %2)").arg(it.key()).arg(toLabel(it.data().revision));
        break;
      case Architecture::P24F:
        list += i18n("%1 (rev. %2.%3)").arg(it.key()).arg(toLabel(it.data().revision)).arg(toLabel(it.data().minorRevision));
        break;
      case Architecture::P30F:
        list += i18n("%1 (proc. %2; rev. %3.%4)").arg(it.key()).arg(toLabel(it.data().process)).arg(toLabel(it.data().revision)).arg(toLabel(it.data().minorRevision));
        break;
      case Architecture::P24H:
      case Architecture::P33F:
        list += i18n("%1 (rev. %2)").arg(it.key()).arg(toLabel(it.data().revision));
        break;
      case Architecture::Nb_Types: Q_ASSERT(false); break;
    }
  }
  return list;
}

bool Pic::Data::checkCalibration(const Device::Array &data, QString *message) const
{
  Q_ASSERT( nbWords(MemoryRangeType::Cal)==data.count() );
  for (uint i=0; i<data.count(); i++) {
    QString address = toHexLabel(range(MemoryRangeType::Cal).start + i*addressIncrement(MemoryRangeType::Cal), nbCharsAddress());
    if ( data[i]==mask(MemoryRangeType::Cal) ) {
      if (message) *message = i18n("Calibration word at address %1 is blank.").arg(address);
      return false;
    }
  }
  if ( data.count()==1 ) {
    if ( data[0].maskWith(_calibration.opcodeMask)!=_calibration.opcode ) {
      if (message) *message = i18n("Calibration word is not a compatible opcode (%2).")
                              .arg(toHexLabel(_calibration.opcode, nbCharsWord(MemoryRangeType::Code)));
      return false;
    }
  }
  return true;
}

const Pic::RegistersData &Pic::Data::registersData() const
{
  return static_cast<const RegistersData &>(*_registersData);
}

bool Pic::Data::hasFeature(Feature feature, bool *unknown) const
{
  bool ok = ( registersData().nbBanks!=0 );
  if (unknown) *unknown = !ok;
  if (!ok) return false;
  switch (feature.type()) {
    case Feature::CCP: return registersData().sfrs.contains("CCP1CON");
    case Feature::ADC: return registersData().sfrs.contains("ADCON0");
    case Feature::SSP: return registersData().sfrs.contains("SSPCON");
    case Feature::LVD: return registersData().sfrs.contains("LVDCON");
    case Feature::USB: return registersData().sfrs.contains("UCON");
    case Feature::USART:
      return ( registersData().sfrs.contains("TXSTA")        // 16F
               || registersData().sfrs.contains("TXSTA1")    // 18F
               || registersData().sfrs.contains("U1MODE") ); // 30F
    case Feature::CAN: return registersData().sfrs.contains("CANCON") && !registersData().sfrs.contains("ECANCON");
    case Feature::ECAN: return registersData().sfrs.contains("ECANCON");
    case Feature::Ethernet: return registersData().sfrs.contains("ETHCON1");
    case Feature::LCD: return registersData().sfrs.contains("LCDCON");
    case Feature::MotorControl: return registersData().sfrs.contains("PWMCON0");
    case Feature::MotionFeedback: return registersData().sfrs.contains("CAP1CON");
    case Feature::SelfWrite: return _selfWrite==SelfWrite::Yes;
    case Feature::Nb_Types: Q_ASSERT(false); break;
  }
  return false;
}

Device::Array Pic::Data::gotoInstruction(Address address, bool withPageSelection) const
{
  Q_ASSERT( address<addressIncrement(MemoryRangeType::Code)*nbWords(MemoryRangeType::Code) );
  Device::Array a;
  switch (_architecture.type()) {
  case Architecture::P10X:
    if ( nbWords(MemoryRangeType::Code)>0x1FF && withPageSelection)
      a.append(0x4A3 | (address>0x1FF ? 0x100 : 0x000)); // bsf STATUS,PA0 or bcf STATUS,PA0
    a.append(0xA00 | (address.toUInt() & 0x1FF)); // goto
    break;
  case Architecture::P16X:
    if ( nbWords(MemoryRangeType::Code)>0x7FF && withPageSelection ) {
      if ( address<=0x7FF ) a.append(0x018A); // clrf PCLATH
      else {
        a.append(0x3000 | (address.toUInt() >> 8)); // movl high address
        a.append(0x008A); // movwf PCLATH
      }
    }
    a.append(0x2800 | (address.toUInt() & 0x7FF));
    break;
  case Architecture::P17C:
    a.append(0xC000 | (address.toUInt() & 0x1FFF));
    break;
  case Architecture::P18C:
  case Architecture::P18F:
  case Architecture::P18J:
    a.append(0xEF00 | ((address.toUInt()/2) & 0xFF));
    a.append(0xF000 | ((address.toUInt()/2) >> 8));
    break;
  case Architecture::P24F:
  case Architecture::P24H:
  case Architecture::P30F:
  case Architecture::P33F:
    a.append(0x040000 | (address.toUInt() & 0x00FFFE));
    a.append(0X000000 | (address.toUInt() >> 16));
    break;
  case Architecture::Nb_Types: Q_ASSERT(false); break;
  }
  return a;
}

bool Pic::Data::isGotoInstruction(BitValue instruction) const
{
  switch (_architecture.type()) {
  case Architecture::P10X: return ( instruction.maskWith(0xE00)==0xA00 );
  case Architecture::P16X: return ( instruction.maskWith(0xF800)==0x2800 );
  case Architecture::P17C: return ( instruction.maskWith(0xE000)==0xC000 );
  case Architecture::P18C:
  case Architecture::P18F:
  case Architecture::P18J: return ( instruction.maskWith(0xFF00)==0xEF00 );
  case Architecture::P24F:
  case Architecture::P24H:
  case Architecture::P30F:
  case Architecture::P33F: return ( instruction.maskWith(0xFF0000)==0x040000 );
  case Architecture::Nb_Types: Q_ASSERT(false); break;
  }
  return false;
}

uint Pic::Data::nbWordsWriteAlignment(MemoryRangeType type) const
{
  if ( type!=MemoryRangeType::Code ) return 1;
  return QMAX(_nbWordsCodeWrite, uint(16));
}

//----------------------------------------------------------------------------
QDataStream &operator <<(QDataStream &s, const Pic::VoltageData &vd)
{
  s << vd.min << vd.max << vd.nominal;
  return s;
}
QDataStream &operator >>(QDataStream &s, Pic::VoltageData &vd)
{
  s >> vd.min >> vd.max >> vd.nominal;
  return s;
}

QDataStream &operator <<(QDataStream &s, const Pic::MemoryRangeData &mrd)
{
  s << Q_UINT8(mrd.properties) << mrd.start << mrd.end << mrd.rmask << mrd.hexFileOffset;
  return s;
}
QDataStream &operator >>(QDataStream &s, Pic::MemoryRangeData &mrd)
{
  Q_UINT8 properties;
  s >> properties >> mrd.start >> mrd.end >> mrd.rmask >> mrd.hexFileOffset;
  mrd.properties = Pic::MemoryRangeProperties(properties);
  return s;
}

QDataStream &operator <<(QDataStream &s, const Pic::Checksum::Data &cd)
{
  s << cd.constant << cd.bbsize << cd.algorithm << cd.protectedMaskNames;
  s << cd.blankChecksum << cd.checkChecksum;
  return s;
}
QDataStream &operator >>(QDataStream &s, Pic::Checksum::Data &cd)
{
  s >> cd.constant >> cd.bbsize >> cd.algorithm >> cd.protectedMaskNames;
  s >> cd.blankChecksum >> cd.checkChecksum;
  return s;
}

QDataStream &operator <<(QDataStream &s, const Pic::CalibrationData &cd)
{
  s << cd.opcode << cd.opcodeMask;
  return s;
}
QDataStream &operator >>(QDataStream &s, Pic::CalibrationData &cd)
{
  s >> cd.opcode >> cd.opcodeMask;
  return s;
}

QDataStream &Pic::operator <<(QDataStream &s, const Pic::Data &data)
{
  s << static_cast<const Device::Data &>(data);
  s << data._architecture << data._ids << data._nbBitsPC;
  s << data._voltages << data._ranges;
  s << *data._config;
  s << data._checksums;
  s << data._calibration;
  s << static_cast<const Pic::RegistersData &>(*data._registersData);
  s << data._nbWordsCodeWrite << data._nbWordsCodeRowErase;
  s << data._selfWrite;
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, Pic::Data &data)
{
  s >> static_cast<Device::Data &>(data);
  s >> data._architecture >> data._ids >> data._nbBitsPC;
  s >> data._voltages >> data._ranges;
  s >> *data._config;
  s >> data._checksums;
  s >> data._calibration;
  s >> static_cast<Pic::RegistersData &>(*data._registersData);
  s >> data._nbWordsCodeWrite >> data._nbWordsCodeRowErase;
  s >> data._selfWrite;
  return s;
}
