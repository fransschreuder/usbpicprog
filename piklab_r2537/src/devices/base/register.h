/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef REGISTER_H
#define REGISTER_H

#include "common/common/storage.h"
#include "devices/base/generic_device.h"
namespace Register { class TypeData; }

namespace Device
{
enum RegisterProperty { NotAccessible = 0x0, Readable = 0x1, Writable = 0x2 };
Q_DECLARE_FLAGS(RegisterProperties, RegisterProperty)
Q_DECLARE_OPERATORS_FOR_FLAGS(RegisterProperties)

enum { MAX_NB_PORTS = 8 };
enum { MAX_NB_PORT_BITS = 16 };
enum BitState { Low = 0, High, WeakPullUp, WeakPullDown, HighImpedance, Unknown };
enum IoState { IoLow = 0, IoHigh, IoUnknown };
class PortBitData {
public:
  PortBitData() : state(Unknown), driving(false), drivenState(IoUnknown), drivingState(IoUnknown) {}
  BitState state;
  bool driving;
  IoState drivenState, drivingState;
  bool operator !=(const PortBitData &pdb) const {
    return ( state!=pdb.state || driving!=pdb.driving || drivenState!=pdb.drivenState || drivingState!=pdb.drivingState );
  }
};

} // namespace

namespace Register
{
//----------------------------------------------------------------------------
enum Type { Regular, Special, Combined, Invalid };
class TypeData {
public:
  TypeData() : _nbChars(0) {}
  TypeData(Address address, uint nbChars);
  TypeData(const QString &name, uint nbChars);
  TypeData(const QString &name, Address address, uint nbChars);
  bool operator ==(const TypeData &data) const { return _name==data._name && _address==data._address && _nbChars==data._nbChars; }
  Type type() const;
  QString name() const { return _name; }
  Address address() const { return _address; }
  uint nbChars() const { return _nbChars; }
  QString toString() const;
  static TypeData fromString(const QString &s);

private:
  uint    _nbChars;
  Address _address;
  QString _name;
};

} // namespace

namespace Device
{
//----------------------------------------------------------------------------
class RegistersData
{
public:
  RegistersData() {}
  virtual ~RegistersData() {}
  virtual uint nbRegisters() const = 0;
  virtual uint nbBits() const = 0;
  uint nbBytes() const { return nbBitsToNbBytes(nbBits()); }
  uint nbChars() const { return nbBitsToNbChars(nbBits()); }
  virtual uint addressFromIndex(uint i) const = 0;
  virtual uint indexFromAddress(Address address) const = 0;
  virtual RegisterProperties properties(Address address) const = 0;
  virtual QValueList<Register::TypeData> relatedRegisters(const Register::TypeData &data) const = 0;
  virtual bool hasPort(uint index) const = 0;
  virtual int portIndex(Address address) const = 0;
  virtual QString portName(uint index) const = 0;
  virtual bool hasPortBit(uint index, uint bit) const = 0;
  virtual QString portBitName(uint index, uint bit) const = 0;
};

} // namespace

namespace Register
{
//----------------------------------------------------------------------------
class List;
extern List &list();

class List : public GenericStorage
{
Q_OBJECT
public:
  List() : GenericStorage(0, "register_list") {}
  void init();
  void setWatched(const TypeData &data, bool watched);
  void clearWatched();
  const QValueList<TypeData> &watched() const { return _watched; }
  bool isWatched(const TypeData &data) const { return _watched.contains(data); }
  void setValue(const TypeData &data, BitValue value);
  BitValue value(const TypeData &data) const;
  BitValue oldValue(const TypeData &data) const;
  void setPortData(uint index, const QMap<uint, Device::PortBitData> &data);
  QMap<uint, Device::PortBitData> portData(uint index) const { return _portDatas[index].current; }
  QMap<uint, Device::PortBitData> oldPortData(uint index) const { return _portDatas[index].old; }

private:
  class StateData {
  public:
    BitValue current, old;
  };
  QMap<Address, StateData> _regulars; // registers with address
  QMap<QString, StateData> _specials; // registers with no address
  class PortData {
  public:
    QMap<uint, Device::PortBitData> current, old;
  };
  QMap<uint, PortData> _portDatas; // port index
  QValueList<TypeData> _watched;
};

} // namespace

#endif
