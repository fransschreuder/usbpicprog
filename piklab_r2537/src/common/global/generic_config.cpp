/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "generic_config.h"

#include "global.h"

#if defined(NO_KDE)
#  include <qsettings.h>
class GenericConfigPrivate
{
public:
  GenericConfigPrivate(const QString &group) { _settings.beginGroup("/piklab/" + group); }
  QSettings _settings;
};
#else
#  include <kapplication.h>
#  include <kconfig.h>
class GenericConfigPrivate
{
public:
  GenericConfigPrivate(const QString &group) : _group(group) {}
  ~GenericConfigPrivate() { kapp->config()->sync(); }
  KConfig &config() {
    KConfig *conf = kapp->config();
    conf->setGroup(_group);
    return *conf;
 }

private:
  QString _group;
};
#endif

GenericConfig::GenericConfig(const QString &group)
  : _group(group)
{
  _d = new GenericConfigPrivate(group);
}

GenericConfig::~GenericConfig()
{
  delete _d;
}

void GenericConfig::rollback()
{
#if defined(NO_KDE)
  qWarning("Config rollback not supported");
#else
  _d->config().rollback();
#endif
}

QString GenericConfig::readEntry(const QString &key, const QString &def) const
{
#if defined(NO_KDE)
#  if QT_VERSION<0x040000
  return _d->_settings.readEntry(key, def);
#  else
  return _d->_settings.value(key, def).toString();
#  endif
#else
  return _d->config().readEntry(key, def);
#endif
}
void GenericConfig::writeEntry(const QString &key, const QString &value)
{
#if defined(NO_KDE)
#  if QT_VERSION<0x040000
  _d->_settings.writeEntry(key, value);
#  else
  _d->_settings.setValue(key, value);
#  endif
#else
  _d->config().writeEntry(key, value);
#endif
}

QStringList GenericConfig::readListEntry(const QString &key, const QStringList &defaultValues) const
{
#if defined(NO_KDE)
#  if QT_VERSION<0x040000
  if ( _d->_settings.readEntry(key).isNull() ) return defaultValues;
  return _d->_settings.readListEntry(key);
#  else
  return _d->_settings.value(key, defaultValues).toStringList();
#  endif
#else
  if ( !_d->config().hasKey(key) ) return defaultValues;
  return _d->config().readListEntry(key);
#endif
}
void GenericConfig::writeEntry(const QString &key, const QStringList &value)
{
#if defined(NO_KDE)
#  if QT_VERSION<0x040000
  _d->_settings.writeEntry(key, value);
#  else
  _d->_settings.setValue(key, value);
#  endif
#else
  _d->config().writeEntry(key, value);
#endif
}

QValueList<int> GenericConfig::readIntListEntry(const QString &key) const
{
#if defined(NO_KDE)
  QValueList<int> ilist;
  QStringList list = readListEntry(key, QStringList());
  QStringList::const_iterator it;
  for (it=list.begin(); it!=list.end(); ++it) {
    bool ok;
    int v = (*it).toInt(&ok);
    if ( !ok ) return ilist;
    ilist.append(v);
  }
  return ilist;
#else
  return _d->config().readIntListEntry(key);
#endif
}
void GenericConfig::writeEntry(const QString &key, const QValueList<int> &value)
{
#if defined(NO_KDE)
  QStringList list;
  QValueList<int>::const_iterator it;
  for (it=value.begin(); it!=value.end(); ++it) list.append(QString::number(*it));
  writeEntry(key, list);
#else
  _d->config().writeEntry(key, value);
#endif
}

QSize GenericConfig::readSizeEntry(const QString &key, const QSize *def) const
{
#if defined(NO_KDE)
  QValueList<int> list = readIntListEntry(key);
  if ( list.count()!=2 ) return *def;
  return QSize(list[0], list[1]);
#else
  return _d->config().readSizeEntry(key, def);
#endif
}
void GenericConfig::writeEntry(const QString &key, const QSize &value)
{
#if defined(NO_KDE)
  QValueList<int> ilist;
  ilist.append(value.width());
  ilist.append(value.height());
  writeEntry(key, ilist);
#else
  _d->config().writeEntry(key, value);
#endif
}

bool GenericConfig::readBoolEntry(const QString &key, bool def) const
{
#if defined(NO_KDE)
#  if QT_VERSION<0x040000
  return _d->_settings.readBoolEntry(key, def);
#  else
  return _d->_settings.value(key, def).toBool();
#  endif
#else
  return _d->config().readBoolEntry(key, def);
#endif
}
void GenericConfig::writeEntry(const QString &key, bool value)
{
#if defined(NO_KDE)
#  if QT_VERSION<0x040000
  _d->_settings.writeEntry(key, value);
#  else
  _d->_settings.setValue(key, value);
#  endif
#else
  _d->config().writeEntry(key, value);
#endif
}

int GenericConfig::readIntEntry(const QString &key, int def) const
{
#if defined(NO_KDE)
#  if QT_VERSION<0x040000
  return _d->_settings.readNumEntry(key, def);
#  else
  return _d->_settings.value(key, def).toInt();
#  endif
#else
  return _d->config().readNumEntry(key, def);
#endif
}
void GenericConfig::writeEntry(const QString &key, int value)
{
#if defined(NO_KDE)
#  if QT_VERSION<0x040000
  _d->_settings.writeEntry(key, value);
#  else
  _d->_settings.setValue(key, value);
#  endif
#else
  _d->config().writeEntry(key, value);
#endif
}

void GenericConfig::deleteGroup(const QString &group)
{
#if defined(NO_KDE)
  Q_UNUSED(group);
  // #### cannot do that...
#else
  kapp->config()->deleteGroup(group);
#endif
}

QVariant GenericConfig::readVariantEntry(const QString &key, const QVariant &defValue) const
{
  switch (defValue.type()) {
    case QVariant::Bool: return QVariant(readBoolEntry(key, defValue.toBool()), 0);
    case QVariant::UInt: return readUIntEntry(key, defValue.toUInt());
    default: break;
  }
  Q_ASSERT(false);
  return QVariant();
}

void GenericConfig::writeEntry(const QString &key, const QVariant &v)
{
  switch (v.type()) {
    case QVariant::Bool: writeEntry(key, v.toBool()); break;
    case QVariant::UInt: writeEntry(key, v.toUInt()); break;
    default: Q_ASSERT(false); break;
  }
}
