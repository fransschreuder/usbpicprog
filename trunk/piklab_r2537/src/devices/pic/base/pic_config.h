/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_CONFIG_H
#define PIC_CONFIG_H

#include <qmap.h>
#include <qstringlist.h>

#include "common/common/bitvalue.h"
#include "pic_protection.h"
#include "pic.h"

namespace Pic
{
class Data;

BEGIN_DECLARE_ENUM(ConfigNameType)
  Default = 0, Extra, SDCC
END_DECLARE_ENUM_STD(ConfigNameType)

class Config
{
public:
  class Value {
  public:
    QString name;
    QMap<ConfigNameType, QStringList> configNames;
    BitValue value;
    bool operator <(const Value &cv) const { return value<cv.value; }
    bool isValid() const { return !name.isEmpty(); }
  };

  class Mask {
  public:
    QString name;
    BitValue value;
    QValueVector<Value> values; // ordered from lower to higher
    bool operator <(const Mask &cm) const { return value<cm.value; }
  };

  class Word {
  public:
    QString name;
    QStringList ignoredCNames;
    BitValue wmask, pmask, cmask; // write, protected, and checksum bits masks
    BitValue bvalue;              // blank value
    QValueVector<Mask> masks; // ordered from lower to higher
    BitValue usedMask() const;
  };

public:
  Config(const Pic::Data &data) : _data(data), _protection(data, *this) {}
  QValueVector<Word> _words;
  const Protection &protection() const { return _protection; }

  const Value *findValue(const QString &mask, const QString &value) const;
  const Mask *findMask(const QString &mask, uint *wordIndex = 0) const;
  static bool hasMaskName(const QString &mask);
  static QString maskLabel(const QString &mask);
  bool checkValueName(const QString &mask, const QString &name) const;
  static QString valueLabel(const QString &mask, const QString &name);

private:
  class MapData {
  public:
    MapData() {}
    MapData(int i, int b) : index(i), block(b) {}
    int index, block;
  };
  static QMap<QString, MapData> &masks();
  static QMap<QString, MapData> *_masks; // mask name -> index in DATA

  struct NameData {
    const char *name, *label;
  };
  enum Type { Fixed, ValueDouble, ValueUInt, Ratio, MemoryRange, Toggle, Pin, Pins };
  class Data {
  public:
    const NameData mask;
    Type type;
    const NameData values[50];
  };
  static const Data DATA[];

private:
  const Pic::Data &_data;
  Protection _protection;
};

QDataStream &operator <<(QDataStream &s, const Config::Value &value);
QDataStream &operator >>(QDataStream &s, Config::Value &value);
QDataStream &operator <<(QDataStream &s, const Config::Mask &mask);
QDataStream &operator >>(QDataStream &s, Config::Mask &mask);
QDataStream &operator <<(QDataStream &s, const Config::Word &word);
QDataStream &operator >>(QDataStream &s, Config::Word &word);
QDataStream &operator <<(QDataStream &s, const Config &config);
QDataStream &operator >>(QDataStream &s, Config &config);

} //namespace

#endif
