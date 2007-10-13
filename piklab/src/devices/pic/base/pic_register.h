/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_REGISTER_H
#define PIC_REGISTER_H

#include <qmap.h>

#include "devices/base/register.h"
#include "pic.h"

namespace Pic
{
class Data;
struct RangeData {
  Address start;
  uint length;
};

//-----------------------------------------------------------------------------
enum RegisterType { UnusedRegister, Sfr, Gpr, Mirrored};
enum RegisterBitProperty { RegisterBitUnused = 0x0,
  RegisterBitRead = 0x1, RegisterBitWrite = 0x2,
  RegisterBitOnlySoftwareClear = 0x4, RegisterBitOnlySoftwareSet = 0x8,
  MaxRegisterBitProperty = 0x15
};
Q_DECLARE_FLAGS(RegisterBitProperties, RegisterBitProperty)
Q_DECLARE_OPERATORS_FOR_FLAGS(RegisterBitProperties)
enum RegisterBitState { RegisterBitUnknown = 0, RegisterBitLow, RegisterBitHigh,
  RegisterBitUnchanged, RegisterBitDepends, RegisterBitDependsConfig, Nb_RegisterBitStates
};

//-----------------------------------------------------------------------------
class RegisterBitData
{
public:
  RegisterBitData() : properties(RegisterBitUnused) {}
  RegisterBitProperties properties;
  RegisterBitState por, mclr;
};
struct RegisterData
{
  Address address;
  RegisterBitData bits[Device::MAX_NB_PORT_BITS];
};
struct CombinedData
{
  Address address;
  uint    nbChars;
};

class RegistersData : public Device::RegistersData
{
public:
  RegistersData(const Data &data);
  virtual uint nbBits() const { return _data.architecture().data()->nbBitsRegister; }
  uint nbBytesPerBank() const { return _data.architecture().data()->registerBankLength; }
  uint nbRegistersPerBank() const { return nbBytesPerBank() / nbBytes(); }
  uint nbCharsAddress() const { return ::nbChars(nbRegisters() - 1); }
  virtual uint nbRegisters() const { return nbBanks * nbRegistersPerBank(); }
  virtual uint addressFromIndex(uint i) const { return nbBytes() * i; }
  virtual uint indexFromAddress(Address address) const { return address.toUInt() / nbBytes(); }
  bool isBankUsed(uint i) const;
  uint bankFromAddress(Address address) const { return indexFromAddress(address) / nbRegistersPerBank(); }
  bool bankHasSfrs(uint i) const; // slow
  bool hasSharedGprs(uint &firstIndex, bool &all) const; // i.e. mirrored in all banks (all is for first bank only)
  uint firstGprIndex() const; // in first bank

  uint nbBanks, accessBankSplit, unusedBankMask;
  QMap<QString, RegisterData> sfrs;
  QMap<Address, QString> sfrNames; // address -> name
  QValueVector<QValueVector<RangeData> > mirrored;
  QValueVector<RangeData> unused;
  QMap<QString, CombinedData> combined;

  virtual Device::RegisterProperties properties(Address address) const;
  RegisterType type(Address address) const;
  QString label(Address address) const;
  virtual QValueList<Register::TypeData> relatedRegisters(const Register::TypeData &data) const;

  virtual bool hasPort(uint index) const;
  virtual int portIndex(Address address) const;
  virtual QString portName(uint index) const;
  bool hasTris(uint index) const;
  QString trisName(uint index) const;
  bool hasLatch(uint index) const;
  QString latchName(uint index) const;
  virtual bool hasPortBit(uint index, uint bit) const;
  virtual QString portBitName(uint index, uint bit) const;

private:
  const Data &_data;
  Address mirroredAddress(Address address) const;
};

//-----------------------------------------------------------------------------
QDataStream &operator <<(QDataStream &s, const RangeData &rd);
QDataStream &operator >>(QDataStream &s, RangeData &rd);
QDataStream &operator <<(QDataStream &s, const RegisterBitData &rbd);
QDataStream &operator >>(QDataStream &s, RegisterBitData &rbd);
QDataStream &operator <<(QDataStream &s, const RegisterData &rd);
QDataStream &operator >>(QDataStream &s, RegisterData &rd);
QDataStream &operator <<(QDataStream &s, const CombinedData &rd);
QDataStream &operator >>(QDataStream &s, CombinedData &rd);
QDataStream &operator <<(QDataStream &s, const RegistersData &rd);
QDataStream &operator >>(QDataStream &s, RegistersData &rd);

} // namespace

#endif
