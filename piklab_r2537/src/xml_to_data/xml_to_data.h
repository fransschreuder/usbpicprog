/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef XML_TO_DATA_H
#define XML_TO_DATA_H

#include "common/global/global.h"
#if QT_VERSION<0x040000
#  include <qdom.h>
#else
#  include <QtXml/QDomDocument>
#endif
#include <qstringlist.h>

class XmlToData
{
public:
  XmlToData() {}
  virtual ~XmlToData() {}
  void process();

protected:
  virtual void parse() = 0;
  virtual void output() = 0;
  virtual QString currentDevice() const = 0;
  virtual void warning(const QString &message) const;
  virtual void qFatal(const QString &message) const;
  QDomElement findUniqueElement(QDomElement parent, const QString &nodeName,
                                const QString &attribute, const QString &value) const;
  void checkTagNames(QDomElement element, const QString &tag, const QStringList &names) const;
  QDomDocument parseFile(const QString &filename) const;
};

#define XML_MAIN(_type) \
  int main(int, char **) \
  { \
    _type dx; \
    dx.process(); \
    return 0; \
  }

#endif
