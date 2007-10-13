/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_H
#define PIC_H

#include <qstringlist.h>
#include <qmap.h>

#include "common/global/global.h"
#include "common/common/bitvalue.h"
#include "devices/base/generic_device.h"

namespace Pic
{
class XmlToData;
class Group;
class Config;
class RegistersData;

//-----------------------------------------------------------------------------
struct VoltageData {
  double min, max, nominal;
};
inline bool operator ==(const Pic::VoltageData &v1, const Pic::VoltageData &v2) { return ( v1.min==v2.min && v1.max==v2.max && v1.nominal==v2.nominal ); }

BEGIN_DECLARE_ENUM(ProgVoltageType)
  Vpp = 0, VddBulkErase, VddWrite
END_DECLARE_ENUM(ProgVoltageType, EnumData)

struct CalibrationData {
  BitValue opcode, opcodeMask;
};

enum MemoryRangeTypeProperty { ReadOnly = 0, Writable = 1 };
Q_DECLARE_FLAGS(MemoryRangeTypeProperties, MemoryRangeTypeProperty)
Q_DECLARE_OPERATORS_FOR_FLAGS(MemoryRangeTypeProperties)
struct MemoryRangeTypeData {
  const char *key, *label;
  MemoryRangeTypeProperties properties;
};
BEGIN_DECLARE_ENUM(MemoryRangeType)
  Code = 0, Cal, UserId, DeviceId, Config, Eeprom, DebugVector, HardwareStack, CalBackup, ProgramExecutive
END_DECLARE_ENUM(MemoryRangeType, MemoryRangeTypeData)

struct ArchitectureData {
  const char *key, *label;
  uint nbBitsPC;                     // nb bits program counter
  uint nbBytesWord;                  // nb bytes per word (hex file and icd2)
  bool packed;                       // addressIncrement = (packed ? nbBytesWord : nbBytesWord/2)
  uint nbBitsRegister;
  uint registerBankLength;
  uint nbBits[MemoryRangeType::Nb_Types];  // nb bits per word
  bool hasAddressAccess;             // memory can be accessed randomly
};
BEGIN_DECLARE_ENUM(Architecture)
  P10X = 0, P16X, P17C, P18C, P18F, P18J, P24J, P24H, P30X
END_DECLARE_ENUM(Architecture, ArchitectureData)

enum MemoryRangeProperty { NotPresent = 0, Present = 1, Programmable = 2 };
Q_DECLARE_FLAGS(MemoryRangeProperties, MemoryRangeProperty)
Q_DECLARE_OPERATORS_FOR_FLAGS(MemoryRangeProperties)
struct MemoryRangeData {
  MemoryRangeProperties properties;
  Address start, end;
  uint hexFileOffset;
  BitValue rmask;
};

namespace Checksum
{
  BEGIN_DECLARE_ENUM(Algorithm)
    Normal = 0, XOR4, XNOR7, XNOR8
  END_DECLARE_ENUM(Algorithm, EnumData)
  class Data {
  public:
    BitValue constant;
    Algorithm algorithm;
    QStringList protectedMaskNames;
    QString bbsize;
    BitValue blankChecksum, checkChecksum;
  };
} // namespace

BEGIN_DECLARE_ENUM(Feature)
  CCP, ADC, SSP, LVD, USB, USART, CAN, ECAN, Ethernet, LCD, MotorControl, MotionFeedback
END_DECLARE_ENUM(Feature, EnumData)

//-----------------------------------------------------------------------------
class Data : public Device::Data
{
public:
  Data();
  virtual ~Data();
  virtual QString fname(Device::Special special) const;
  virtual QString informationHtml() const;
  //BitValue id(Device::FrequencyRange::Special special) const;
  bool isPresent(MemoryRangeType type) const { return (range(type).properties & Present); }
  bool isReadable(MemoryRangeType type) const;
  bool isWritable(MemoryRangeType type) const;
  uint nbAddresses(MemoryRangeType type) const;
  uint nbWords(MemoryRangeType type) const;
  uint addressIncrement(MemoryRangeType type) const;
  uint nbWordsWriteAlignment(MemoryRangeType type) const;
  const MemoryRangeData &range(MemoryRangeType type) const { return _ranges[type]; }
  virtual uint nbBitsAddress() const { return _nbBitsPC; }
  uint nbBitsWord(MemoryRangeType type) const { return _architecture.data()->nbBits[type]; }
  uint nbBytesWord(MemoryRangeType type) const { return nbBitsToNbBytes(nbBitsWord(type)); }
  uint nbCharsWord(MemoryRangeType type) const { return nbBitsToNbChars(nbBitsWord(type)); }
  BitValue mask(MemoryRangeType type) const { return uint(1 << nbBitsWord(type))-1; }
  const Config &config() const { return *_config; }
  Architecture architecture() const { return _architecture; }
  bool is18Family() const { return ( _architecture==Architecture::P18C || _architecture==Architecture::P18F || _architecture==Architecture::P18J); }
  bool is16bitFamily() const { return ( _architecture==Architecture::P24J || _architecture==Architecture::P24H || _architecture==Architecture::P30X); }
  bool isJType() const { return ( _architecture==Architecture::P18J || _architecture==Architecture::P24J ); }
  const VoltageData &voltage(ProgVoltageType type) const { return _voltages[type]; }
  virtual bool canWriteCalibration() const { return isWritable(MemoryRangeType::Cal); }
  bool checkCalibration(const Device::Array &data, QString *message = 0) const;
  virtual bool matchId(BitValue rawId, Device::IdData &data) const;
  QStringList idNames(const QMap<QString, Device::IdData> &ids) const;
  const QMap<QString, Checksum::Data> checksums() const { return _checksums; }
  const RegistersData &registersData() const;
  const CalibrationData &calibrationData() const { return _calibration; }

  bool hasFeature(Feature feature, bool *unknown = 0) const;
  BitValue nopInstruction() const { return 0x0; }
  Device::Array gotoInstruction(Address address, bool withPageSelection) const;
  bool isGotoInstruction(BitValue instruction) const;

private:
  Architecture     _architecture;
  QMap<Device::Special, BitValue> _ids;
  uint             _nbBitsPC;
  uint             _nbWordsCodeWrite;    // #### only for 18F/18J devices [0 for other devices]
  uint             _nbWordsCodeRowErase; // #### only for 18F/18J devices [0 for other devices or if not available]
  VoltageData      _voltages[ProgVoltageType::Nb_Types];
  MemoryRangeData  _ranges[MemoryRangeType::Nb_Types];
  Config          *_config;
  QMap<QString, Checksum::Data> _checksums;
  CalibrationData  _calibration;

  friend class XmlToData;
  friend class Group;
  friend QDataStream &operator <<(QDataStream &s, const Data &data);
  friend QDataStream &operator >>(QDataStream &s, Data &data);
};

QDataStream &operator <<(QDataStream &s, const Data &data);
QDataStream &operator >>(QDataStream &s, Data &data);

} // namespace

QDataStream &operator <<(QDataStream &s, const Pic::VoltageData &vd);
QDataStream &operator >>(QDataStream &s, Pic::VoltageData &vd);
QDataStream &operator <<(QDataStream &s, const Pic::MemoryRangeData &mrd);
QDataStream &operator >>(QDataStream &s, Pic::MemoryRangeData &mrd);
QDataStream &operator <<(QDataStream &s, const Pic::Checksum::Data &cd);
QDataStream &operator >>(QDataStream &s, Pic::Checksum::Data &cd);
QDataStream &operator <<(QDataStream &s, const Pic::CalibrationData &cd);
QDataStream &operator >>(QDataStream &s, Pic::CalibrationData &cd);

#endif
