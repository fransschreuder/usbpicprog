/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "xml_data_file.h"

#include <qfile.h>
#include <qstringlist.h>
#include <ksimpleconfig.h>
#include <klocale.h>
#include "common/global/pfile.h"

XmlDataFile::XmlDataFile(const PURL::Url &url, const QString &name)
  : _url(url), _name(name), _document(name)
{
  QDomElement root = _document.createElement(name);
  _document.appendChild(root);
}

bool XmlDataFile::load(QString &error)
{
  Log::StringView sview;
  PURL::File file(_url, sview);
  if ( !file.openForRead() ) {
    error = i18n("Error opening file: %1").arg(sview.string());
    return false;
  }
  if ( !_document.setContent(file.qfile(), false, &error) ) return false;
  if ( _document.doctype().name()!=_name
       || _document.documentElement().nodeName()!=_name ) {
    error = i18n("File is not of correct type.");
    return false;
  }
  return true;
}

bool XmlDataFile::save(QString &error) const
{
  Log::StringView sview;
  PURL::File file(_url, sview);
  bool ok = file.openForWrite();
  if (ok) {
    QString s = _document.toString(2);
    file.appendText(s);
    ok = file.close();
  }
  if ( !ok ) error = i18n("Error saving file: %1").arg(sview.string());
  return ok;
}

QDomElement XmlDataFile::findChildElement(QDomElement parent, const QString &name) const
{
  QDomNodeList list = parent.elementsByTagName(name);
  return list.item(0).toElement();
}

QDomElement XmlDataFile::createChildElement(QDomElement parent, const QString &name)
{
  QDomNodeList list = parent.elementsByTagName(name);
  if ( list.count()==0 ) {
    QDomElement element = _document.createElement(name);
    parent.appendChild(element);
    return element;
  }
  return list.item(0).toElement();
}

void XmlDataFile::removeChilds(QDomNode parent) const
{
  QDomNodeList list = parent.childNodes();
  for (uint i=0; i<list.count(); i++)
    parent.removeChild(list.item(i));
}

QString XmlDataFile::value(const QString &group, const QString &key, const QString &defValue) const
{
  QDomElement root = _document.documentElement();
  QDomElement groupElement = findChildElement(root, group);
  if ( groupElement.isNull() ) return defValue;
  QDomElement element = findChildElement(groupElement, key);
  if ( element.isNull() ) return defValue;
  QDomText text = element.firstChild().toText();
  if ( text.isNull() ) return defValue;
  return text.data();
}

void XmlDataFile::setValue(const QString &group, const QString &key, const QString &value)
{
  QDomElement root = _document.documentElement();
  QDomElement groupElement = createChildElement(root, group);
  QDomElement element = createChildElement(groupElement, key);
  removeChilds(element);
  QDomText text = _document.createTextNode(value);
  element.appendChild(text);
}

QStringList XmlDataFile::listValues(const QString &group, const QString &key, const QStringList &defaultValues) const
{
  QStringList list;
  QDomElement root = _document.documentElement();
  QDomElement groupElement = findChildElement(root, group);
  if ( groupElement.isNull() ) return defaultValues;
  QDomElement element = findChildElement(groupElement, key);
  if ( element.isNull() ) return defaultValues;
  QDomNodeList childs = element.childNodes();
  if ( childs.count()==1 ) { // legacy compatibility
    QDomText text = element.firstChild().toText();
    if ( !text.isNull() ) return text.data();
  }
  for (uint i=0; i<childs.count(); i++) {
    QDomText text = childs.item(i).toElement().firstChild().toText();
    if ( text.isNull() ) continue;
    list.append(text.data());
  }
  return list;
}

void XmlDataFile::appendListValue(const QString &group, const QString &key, const QString &value)
{
  QDomElement root = _document.documentElement();
  QDomElement groupElement = createChildElement(root, group);
  QDomElement element = createChildElement(groupElement, key);
  QDomElement item = _document.createElement("item");
  element.appendChild(item);
  QDomText text = _document.createTextNode(value);
  item.appendChild(text);
}

void XmlDataFile::removeListValue(const QString &group, const QString &key, const QString &value)
{
  QDomElement root = _document.documentElement();
  QDomElement groupElement = createChildElement(root, group);
  QDomElement element = createChildElement(groupElement, key);
  QDomNodeList list = element.childNodes();
  for (uint i=0; i<list.count(); i++) {
    QDomElement item = list.item(i).toElement();
    QDomText text = item.firstChild().toText();
    if ( text.isNull() || text.data()!=value ) continue;
    element.removeChild(item);
    break;
  }
}

void XmlDataFile::clearList(const QString &group, const QString &key)
{
  QDomElement root = _document.documentElement();
  QDomElement groupElement = createChildElement(root, group);
  QDomElement element = createChildElement(groupElement, key);
  groupElement.removeChild(element);
}

void XmlDataFile::setListValues(const QString &group, const QString &key, const QStringList &values)
{
  clearList(group, key);
  for (uint i=0; i<values.count(); i++) appendListValue(group, key, values[i]);
}
