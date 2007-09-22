/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "xml_to_data.h"

#include <qfile.h>
#include <qtextstream.h>

QDomElement XmlToData::findUniqueElement(QDomElement parent, const QString &tag,
                                         const QString &attribute, const QString &value) const
{
  QDomElement element;
  QDomNode child = parent.firstChild();
  while ( !child.isNull() ) {
    if ( child.nodeName()==tag && child.isElement()
         && child.toElement().attribute(attribute)==value ) {
      if ( !element.isNull() ) qFatal(QString("Duplicated element \"%1/%2\"").arg(tag).arg(value));
      element = child.toElement();
    }
    child = child.nextSibling();
  }
  return element;
}

void XmlToData::checkTagNames(QDomElement element, const QString &tag,
                              const QStringList &names) const
{
  QDomNodeList list = element.elementsByTagName(tag);
  for (uint i=0; i<uint(list.count()); i++) {
    if ( !list.item(i).isElement() ) continue;
    QString name = list.item(i).toElement().attribute("name");
    if ( names.find(name)==names.end() ) qFatal(QString("Illegal name %1 for %2 element").arg(name).arg(tag));
  }
}

QDomDocument XmlToData::parseFile(const QString &filename) const
{
  qDebug("Parsing XML file \"%s\"...", filename.latin1());
  QFile file(filename);
  if ( !file.open(IO_ReadOnly) ) qFatal("Cannot open file!");
  QDomDocument doc;
  QString error;
  int errorLine, errorColumn;
  if ( !doc.setContent(&file, false, &error, &errorLine, &errorColumn) )
    qFatal(QString("Error parsing XML file (%1 at line %2, column %3)").arg(error).arg(errorLine).arg(errorColumn));
  return doc;
}

void XmlToData::warning(const QString &message) const
{
  if ( currentDevice().isEmpty() ) ::qWarning("Warning: %s", message.latin1());
  else ::qWarning("Warning [%s]: %s", currentDevice().latin1(), message.latin1());
}
void XmlToData::qFatal(const QString &message) const
{
  if ( currentDevice().isEmpty() ) ::qFatal("Fatal: %s", message.latin1());
  else ::qFatal("Fatal [%s]: %s", currentDevice().latin1(), message.latin1());
}

void XmlToData::process()
{
  parse();
  qDebug("Parsing XML successful.");
  output();
  qDebug("Output written.");
}
