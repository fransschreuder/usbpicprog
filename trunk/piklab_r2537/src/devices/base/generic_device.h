/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GENERIC_DEVICE_H
#define GENERIC_DEVICE_H

#include <qstringlist.h>

#include "common/common/misc.h"
#include "common/common/bitvalue.h"
#include "common/common/key_enum.h"
#include "common/global/global.h"

namespace Device
{
//----------------------------------------------------------------------------
BEGIN_DECLARE_ENUM(Status)
  InProduction = 0, Future, NotRecommended, EOL, Unknown, Mature
END_DECLARE_ENUM_STD(Status)

BEGIN_DECLARE_ENUM(MemoryTechnology)
  Flash = 0, Eprom, Rom, Romless
END_DECLARE_ENUM_STD(MemoryTechnology)

class RangeBox {
public:
  struct Value { double x, yMin, yMax; };
  Value start, end;
  QString osc, mode, special;
  double yMin() const { return qMin(start.yMin, end.yMin); }
  double yMax() const { return qMax(start.yMax, end.yMax); }
};

BEGIN_DECLARE_ENUM(OperatingCondition)
  Commercial = 0, Industrial, Extended
END_DECLARE_ENUM_STD(OperatingCondition)

BEGIN_DECLARE_ENUM(Special)
  Normal = 0, LowPower, LowVoltage, HighVoltage
END_DECLARE_ENUM_STD(Special)

class FrequencyRange {
public:
  OperatingCondition operatingCondition;
  Special special;
  QValueVector<RangeBox> vdds;
  double vddMin() const;
  double vddMax() const;
};

class IdData {
public:
  BitValue revision, minorRevision, process;
  Special special;
};

class Package
{
public:
  QValueVector<uint> types;
  QValueVector<QString> pins;

public:
  enum Shape { Bar, Square };
  enum { MAX_NB = 9 };
  struct TypeData {
    const char *name, *label;
    Shape shape;
    uint nbPins[MAX_NB];
  };
  static const TypeData TYPE_DATA[];
};

class Documents
{
public:
  QString webpage, datasheet, progsheet;
  QStringList erratas;
};

//----------------------------------------------------------------------------
class XmlToDataBase;
class GroupBase;
class RegistersData;

//----------------------------------------------------------------------------
// we don't want implicit sharing
class Array
{
public:
  Array(uint size = 0) : _data(size) {}
  Array(const Array &array);
  Array &operator =(const Array &);
  Array &operator +=(const Array &a);
  void append(uint v);
  Array mid(uint index, int size = -1) const;
  void resize(uint s) { _data.resize(s); }
  uint size() const { return _data.size(); }
  uint count() const { return _data.size(); }
  BitValue operator [](uint i) const { return _data[i]; }
  BitValue &operator [](uint i) { return _data[i]; }
  bool operator ==(const Array &array) const { return _data==array._data; }
  bool operator !=(const Array &array) const { return _data!=array._data; }

private:
  QMemArray<BitValue> _data;
};

//----------------------------------------------------------------------------
class Data
{
public:
  Data(RegistersData *rdata) : _group(0), _registersData(rdata) {}
  virtual ~Data();
  const GroupBase &group() const { return *_group; }
  virtual QString name() const { return _name; }
  virtual QString fname(Special) const { return _name; }
  virtual QString listViewGroup() const = 0;
  Status status() const { return _status; }
  const Documents &documents() const { return _documents; }
  const QStringList &alternatives() const { return _alternatives; }
  virtual QString informationHtml() const = 0;
  MemoryTechnology memoryTechnology() const { return _memoryTechnology; }
  virtual bool matchId(BitValue rawId, IdData &idata) const = 0;
  const QValueVector<FrequencyRange> &frequencyRanges() const { return _frequencyRanges; }
  double vddMin() const;
  double vddMax() const;
  virtual uint nbBitsAddress() const = 0;
  uint nbBytesAddress() const { return nbBitsAddress()/8 + (nbBitsAddress()%8 ? 1 : 0); }
  uint nbCharsAddress() const { return nbBitsAddress()/4 + (nbBitsAddress()%4 ? 1 : 0); }
  virtual bool canWriteCalibration() const = 0; // #### REMOVE ME
  const RegistersData *registersData() const { return _registersData; }
  const QValueVector<Package> &packages() const { return _packages; }

protected:
  const GroupBase *_group;
  QString _name;
  Documents _documents;
  QStringList _alternatives;
  Status _status;
  QValueVector<FrequencyRange> _frequencyRanges;
  MemoryTechnology _memoryTechnology;
  RegistersData *_registersData;
  QValueVector<Package> _packages;

  friend class XmlToDataBase;
  friend class GroupBase;
  friend QDataStream &operator <<(QDataStream &s, const Data &data);
  friend QDataStream &operator >>(QDataStream &s, Data &data);
};

QDataStream &operator <<(QDataStream &s, const RangeBox::Value &rbv);
QDataStream &operator >>(QDataStream &s, RangeBox::Value &rbv);
QDataStream &operator <<(QDataStream &s, const RangeBox &rb);
QDataStream &operator >>(QDataStream &s, RangeBox &rb);
QDataStream &operator <<(QDataStream &s, const FrequencyRange &frange);
QDataStream &operator >>(QDataStream &s, FrequencyRange &frange);
QDataStream &operator <<(QDataStream &s, const Package &package);
QDataStream &operator >>(QDataStream &s, Package &package);
QDataStream &operator <<(QDataStream &s, const Documents &documents);
QDataStream &operator >>(QDataStream &s, Documents &documents);
QDataStream &operator <<(QDataStream &s, const Data &data);
QDataStream &operator >>(QDataStream &s, Data &data);

} // namespace

#endif
