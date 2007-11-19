/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef XML_DATA_FILE_H
#define XML_DATA_FILE_H

#include <qdom.h>

#include "common/global/purl.h"

class XmlDataFile
{
public:
  XmlDataFile(const PURL::Url &url, const QString &name);
  virtual ~XmlDataFile() {}
  PURL::Url url() const { return _url; }
  virtual bool load(QString &error);
  bool save(QString &error) const;

  QString value(const QString &group, const QString &key, const QString &defaultValue) const;
  void setValue(const QString &group, const QString &key, const QString &value);
  QStringList listValues(const QString &group, const QString &key, const QStringList &defaultValues) const;
  void setListValues(const QString &group, const QString &key, const QStringList &values);
  void appendListValue(const QString &group, const QString &key, const QString &value);
  void removeListValue(const QString &group, const QString &key, const QString &value);
  void clearList(const QString &group, const QString &key);

protected:
  PURL::Url _url;

private:
  QString      _name;
  QDomDocument _document;

  QDomElement findChildElement(QDomElement element, const QString &tag) const;
  QDomElement createChildElement(QDomElement element, const QString &tag);
  void removeChilds(QDomNode node) const;
};

#endif
