/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GENERIC_CONFIG_H
#define GENERIC_CONFIG_H

#include <qstring.h>
#include <qsize.h>
#include <qstringlist.h>
#include "global.h"

class GenericConfigPrivate;

class GenericConfig
{
public:
  GenericConfig(const QString &group);
  ~GenericConfig();
  QString group() const { return _group; }
  void rollback();

  QString readEntry(const QString &key, const QString &def = QString::null);
  void writeEntry(const QString &key, const QString &value);
  void writeEntry(const QString &key, const QCString &value) { writeEntry(key, QString(value)); }
  void writeEntry(const QString &key, const char *value) { writeEntry(key, QString(value)); }
  QStringList readListEntry(const QString &key, const QStringList &defaultValues);
  void writeEntry(const QString &key, const QStringList &value);
  QValueList<int> readIntListEntry(const QString &key);
  void writeEntry(const QString &key, const QValueList<int> &value);
  QSize readSizeEntry(const QString &key, const QSize *def = 0);
  void writeEntry(const QString &key, const QSize &value);
  bool readBoolEntry(const QString &key, bool def);
  void writeEntry(const QString &key, bool value);
  int readIntEntry(const QString &key, int def = 0);
  void writeEntry(const QString &key, int value);
  uint readUIntEntry(const QString &key, uint def = 0) { return qMax(0, readIntEntry(key, def)); }
  void writeEntry(const QString &key, uint value) { writeEntry(key, int(value)); }
  template <typename Enum>
  Enum readEnumEntry(const QString &key, Enum def = Enum::Nb_Types) { return Enum::fromKey(readEntry(key, def.key())); }
  template <typename Enum>
  void writeEnumEntry(const QString &key, Enum v) { writeEntry(key, v.key()); }

  static void deleteGroup(const QString &group);

private:
  QString _group;
  GenericConfigPrivate *_d;
};

#endif
