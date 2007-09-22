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

namespace Device
{
//----------------------------------------------------------------------------
enum Status { Status_IP = 0, Status_Future, Status_NR, Status_EOL,
              Status_Unknown, Status_Mature, Nb_Status };
struct StatusData {
  const char *name, *label;
};
extern const StatusData STATUS_DATA[Nb_Status];

enum MemoryTechnology { EFlash = 0, Flash, Eprom, Rom, Romless, Nb_MemoryTechnologies };
struct MemoryTechnologyData {
  const char *name, *label;
};
extern const MemoryTechnologyData MEMORY_TECHNOLOGY_DATA[Nb_MemoryTechnologies];

extern const uint INVALID; // used to mark a word as "non initialized"

class RangeBox {
public:
  struct Value { double x, yMin, yMax; };
  Value start, end;
  QString osc, mode, special;
  double yMin() const { return qMin(start.yMin, end.yMin); }
  double yMax() const { return qMax(start.yMax, end.yMax); }
};
class FrequencyRange {
public:
  enum Type { Commercial = 0, Industrial, Extended, Nb_Types };
  enum Special { Normal = 0, LowPower, LowVoltage, Nb_Specials };
  Type type;
  Special special;
  QValueVector<RangeBox> vdds;
  double vddMin() const;
  double vddMax() const;
  struct Data {
    const char *name, *label;
  };
  static const Data TYPE_DATA[Nb_Types];
  static const Data SPECIAL_DATA[Nb_Specials];
};

struct IdData {
  uint revision, minorRevision, process;
};
typedef QMap<QString, IdData> IdList;

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

//----------------------------------------------------------------------------
class XmlToDataBase;
class GroupBase;
class GroupUI;
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
  uint operator [](uint i) const { return _data[i]; }
  uint &operator [](uint i) { return _data[i]; }
  bool operator ==(const Array &array) const { return _data==array._data; }
  bool operator !=(const Array &array) const { return _data!=array._data; }

private:
  QMemArray<uint> _data;
};

//----------------------------------------------------------------------------
class Data
{
public:
  Data() : _group(0), _registersData(0) {}
  virtual ~Data();
  const GroupBase &group() const { return *_group; }
  QString name() const { return _name; }
  virtual QString fname(Device::FrequencyRange::Special) const { return _name; }
  Status status() const { return _status; }
  QString document() const { return _document; }
  const QStringList &alternatives() const { return _alternatives; }
  virtual QString informationUrl() const = 0;
  virtual QString informationHtml() const = 0;
  MemoryTechnology memoryTechnology() const { return _memoryTechnology; }
  virtual bool matchId(uint rawId, IdData &idata) const = 0;
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
  QString _name, _document;
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
QDataStream &operator <<(QDataStream &s, const Data &data);
QDataStream &operator >>(QDataStream &s, Data &data);

} // namespace

#endif
