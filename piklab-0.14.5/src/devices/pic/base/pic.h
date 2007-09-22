/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_H
#define PIC_H

#include "common/global/global.h"
#include <qstringlist.h>
#include <qmap.h>

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
enum ProgVoltageType { Vpp = 0, VddBulkErase, VddWrite, Nb_ProgVoltageTypes };
struct ProgVoltageTypeData {
  const char *name;
};
extern const ProgVoltageTypeData PROG_VOLTAGE_TYPE_DATA[Nb_ProgVoltageTypes];

struct CalibrationData {
  uint opcode, opcodeMask;
};

enum MemoryRangeType { MemoryCode = 0, MemoryCal, MemoryUserId, MemoryDeviceId,
                       MemoryConfig, MemoryEeprom, MemoryDebugVector, MemoryHardwareStack,
                       MemoryCalBackup, MemoryProgramExecutive, Nb_MemoryRangeTypes };
enum MemoryRangeTypeProperty { ReadOnly = 0, Writable = 1 };
Q_DECLARE_FLAGS(MemoryRangeTypeProperties, MemoryRangeTypeProperty)
Q_DECLARE_OPERATORS_FOR_FLAGS(MemoryRangeTypeProperties)
struct MemoryRangeTypeData {
  const char *name, *label;
  MemoryRangeTypeProperties properties;
};
extern const MemoryRangeTypeData MEMORY_RANGE_TYPE_DATA[Nb_MemoryRangeTypes];

enum Architecture { Arch_10X = 0, Arch_16X, Arch_17X, Arch_18C, Arch_18F, Arch_18J,
                    Arch_24F, Arch_24H, Arch_30X, Nb_Architectures };
struct ArchitectureData {
  const char *name, *label;
  uint nbBitsPC;                     // nb bits program counter
  uint nbBytesWord;                  // nb bytes per word (hex file and icd2)
  bool packed;                       // addressIncrement = (packed ? nbBytesWord : nbBytesWord/2)
  uint nbBitsRegister;
  uint registerBankLength;
  uint nbBits[Nb_MemoryRangeTypes];  // nb bits per word
  bool hasAddressAccess;             // memory can be accessed randomly
};
extern const ArchitectureData ARCHITECTURE_DATA[Nb_Architectures];

enum MemoryRangeProperty { NotPresent = 0, Present = 1, Programmable = 2 };
Q_DECLARE_FLAGS(MemoryRangeProperties, MemoryRangeProperty)
Q_DECLARE_OPERATORS_FOR_FLAGS(MemoryRangeProperties)
struct MemoryRangeData {
  MemoryRangeProperties properties;
  uint start, end, rmask, hexFileOffset;
};

enum ChecksumType { NormalChecksum, XOR4Checksum, XNOR7Checksum, XNOR8Checksum };
struct ChecksumData
{
  uint constant, protectedConstant;
  ChecksumType protectedType;
};

//-----------------------------------------------------------------------------
class Data : public Device::Data
{
public:
  Data();
  virtual ~Data();
  virtual QString fname(Device::FrequencyRange::Special special) const;
  virtual QString informationUrl() const;
  virtual QString informationHtml() const;
  uint id() const { return _id; }
  bool isPresent(MemoryRangeType type) const { return (range(type).properties & Present); }
  bool isReadable(MemoryRangeType type) const;
  bool isWritable(MemoryRangeType type) const;
  uint nbAddresses(MemoryRangeType type) const;
  uint nbWords(MemoryRangeType type) const;
  uint addressIncrement(MemoryRangeType type) const;
  uint nbWordsAlignment(MemoryRangeType type) const;
  uint nbWordsCodeWrite() const { return _nbWordsCodeWrite; }
  const MemoryRangeData &range(MemoryRangeType type) const { return _ranges[type]; }
  virtual uint nbBitsAddress() const { return _nbBitsPC; }
  uint nbBitsWord(MemoryRangeType type) const { return ARCHITECTURE_DATA[_architecture].nbBits[type]; }
  uint nbBytesWord(MemoryRangeType type) const { return nbBitsToNbBytes(nbBitsWord(type)); }
  uint nbCharsWord(MemoryRangeType type) const { return nbBitsToNbChars(nbBitsWord(type)); }
  uint mask(MemoryRangeType type) const { return uint(1 << nbBitsWord(type))-1; }
  const Config &config() const { return *_config; }
  Architecture architecture() const { return _architecture; }
  bool is18Family() const { return ( _architecture==Arch_18C || _architecture==Arch_18F || _architecture==Arch_18J); }
  bool is16bitsFamily() const { return ( _architecture==Arch_24F || _architecture==Arch_24H || _architecture==Arch_30X); }
  const VoltageData &voltage(ProgVoltageType type) const { return _voltages[type]; }
  virtual bool canWriteCalibration() const { return isWritable(MemoryCal); }
  bool checkCalibration(const Device::Array &data, QString *message = 0) const;
  virtual bool matchId(uint rawId, Device::IdData &data) const;
  QStringList idNames(const Device::IdList &idList) const;
  const ChecksumData &checksumData() const { return _checksum; }
  const RegistersData &registersData() const;
  const CalibrationData &calibrationData() const { return _calibration; }

  bool hasUSART() const;
  bool hasUSB() const;
  uint nopInstruction() const { return 0x0; }
  Device::Array gotoInstruction(uint address, bool withPageSelection) const;
  bool isGotoInstruction(uint instruction) const;

private:
  Architecture     _architecture;
  uint             _id, _nbBitsPC;
  uint             _nbWordsCodeWrite;    // #### only for 18F/18J devices [0 for other devices]
  uint             _nbWordsCodeRowErase; // #### only for 18F/18J devices [0 for other devices or if not available]
  VoltageData      _voltages[Nb_ProgVoltageTypes];
  MemoryRangeData  _ranges[Nb_MemoryRangeTypes];
  Config          *_config;
  ChecksumData     _checksum;
  CalibrationData  _calibration;

  friend class XmlToData;
  friend class Group;
  friend QDataStream &operator <<(QDataStream &s, const Data &data);
  friend QDataStream &operator >>(QDataStream &s, Data &data);
};

QDataStream &operator <<(QDataStream &s, const VoltageData &vd);
QDataStream &operator >>(QDataStream &s, VoltageData &vd);
QDataStream &operator <<(QDataStream &s, const MemoryRangeData &mrd);
QDataStream &operator >>(QDataStream &s, MemoryRangeData &mrd);
QDataStream &operator <<(QDataStream &s, const ChecksumData &cd);
QDataStream &operator >>(QDataStream &s, ChecksumData &cd);
QDataStream &operator <<(QDataStream &s, const CalibrationData &cd);
QDataStream &operator >>(QDataStream &s, CalibrationData &cd);
QDataStream &operator <<(QDataStream &s, const Data &data);
QDataStream &operator >>(QDataStream &s, Data &data);

} // namespace

#endif
