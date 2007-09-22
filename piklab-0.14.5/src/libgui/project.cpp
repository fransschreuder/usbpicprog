/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "project.h"

#include <qfile.h>
#include <qstringlist.h>
#include <ksimpleconfig.h>
#include <klocale.h>

#include "devices/list/device_list.h"
#include "progs/base/prog_group.h"
#include "main_global.h"
#include "tools/gputils/gputils_config.h"
#include "common/global/pfile.h"

//----------------------------------------------------------------------------
XmlDataFile::XmlDataFile(const PURL::Url &url, const QString &name)
  : _url(url), _name(name), _document(name)
{
  QDomElement root = _document.createElement(name);
  _document.appendChild(root);
}

bool XmlDataFile::load(QString &error)
{
  PURL::File file(_url, 0);
  if ( !file.openForRead() ) {
    error = file.errorString();
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
  PURL::File file(_url, 0);
  if ( !file.openForWrite() ) {
    error = file.errorString();
    return false;
  }
  QString s = _document.toString(2);
  file.appendText(s);
  if ( !file.close() ) {
    error = file.errorString();
    return false;
  }
  return true;
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

//----------------------------------------------------------------------------
bool Project::load(QString &error)
{
  if ( _url.fileType()==PURL::Project ) return XmlDataFile::load(error);

  if ( !_url.probablyExists() ) {
    error = i18n("Project file %1 does not exist.").arg(_url.pretty());
    return false;
  }
  PURL::Url tmp = _url;
  _url = _url.toFileType(PURL::Project);
  if ( _url.probablyExists() && XmlDataFile::load(error) ) return true;
  KConfig *config = new KSimpleConfig(tmp.filepath(false), false);

  config->setGroup("Files");
  QStringList list = config->readListEntry("inputFiles");
  QStringList::const_iterator it = list.begin();
  for (; it!=list.end(); ++it) addFile(PURL::Url(directory(), *it));

  config->setGroup("General");
  setVersion(config->readEntry("version", "0.1"));
  setDescription(config->readEntry("description", QString::null));

  config->setGroup("Assembler");
  QString device = config->readEntry("target-device");
  if ( device=="*" ) device = Main::programmerGroup().supportedDevices()[0]; // compatibility
  GPUtils::Config *gconfig = new GPUtils::Config(this);
  gconfig->setDevice(Device::lister().checkName(device));
  gconfig->setGPAsmWarningLevel(QMIN(config->readUnsignedNumEntry("warn-level", 0), uint(GPUtils::Config::Nb_WarningLevels)));
  gconfig->setRawIncludeDirs(Tool::Assembler, config->readEntry("include-dir", QString::null));
  gconfig->setRawCustomOptions(Tool::Assembler, config->readEntry("other-options", QString::null));

  config->setGroup("Linker") ;
  QString hexFormatString = config->readEntry("hex-format", HexBuffer::FORMATS[HexBuffer::IHX32]);
  for (uint i=0; i<HexBuffer::Nb_Formats; i++)
    if ( hexFormatString==HexBuffer::FORMATS[i] ) gconfig->setHexFormat(HexBuffer::Format(i));
  gconfig->setRawIncludeDirs(Tool::Linker, config->readEntry("objs-libs-dir", QString::null));
  gconfig->setRawCustomOptions(Tool::Linker, config->readEntry("other-options", QString::null));

  delete gconfig;
  delete config;
  return true;
}

PURL::UrlList Project::openedFiles() const
{
  PURL::UrlList files;
  QStringList list = listValues("general", "opened_files", QStringList());
  QStringList::const_iterator it = list.begin();
  for (; it!=list.end(); ++it) {
    if ( PURL::Url(*it).isRelative() ) files += PURL::Url(directory(), *it);
    else files += PURL::Url(*it);
  }
  return files;
}
void Project::setOpenedFiles(const PURL::UrlList &list)
{
  clearList("general", "opened_files");
  PURL::UrlList::const_iterator it = list.begin();
  for (; it!=list.end(); ++it)
    appendListValue("general", "opened_files", (*it).relativeTo(directory(), false));
}

PURL::UrlList Project::absoluteFiles() const
{
  PURL::UrlList abs;
  QStringList files = listValues("general", "files", QStringList());
  QStringList::const_iterator it = files.begin();
  for (; it!=files.end(); ++it) abs += PURL::Url(*it).toAbsolute(directory());
  return abs;
}
void Project::addFile(const PURL::Url &url)
{
  appendListValue("general", "files", url.relativeTo(directory(), false));
}
void Project::removeFile(const PURL::Url &url)
{
  removeListValue("general", "files", url.relativeTo(directory(), false));
}
void Project::clearFiles()
{
  clearList("general", "files");
}

QString Project::toSourceObject(const PURL::Url &url, const QString &extension, bool forWindows) const
{
  PURL::Url tmp;
  if ( extension.isEmpty() ) tmp = url.toFileType(PURL::Object);
  else tmp = url.toExtension(extension);
  return tmp.relativeTo(directory(), forWindows);
}

QStringList Project::objectsForLinker(const QString &extension, bool forWindows) const
{
  QStringList objs;
  // objects files corresponding to src files
  PURL::UrlList files = absoluteFiles();
  PURL::UrlList::const_iterator it;
  for (it=files.begin(); it!=files.end(); ++it)
    if ( (*it).data().group==PURL::Source ) objs += toSourceObject(*it, extension, forWindows);
  // objects
  for (it=files.begin(); it!=files.end(); ++it)
    if ( (*it).fileType()==PURL::Object ) objs += (*it).filepath(forWindows);
  return objs;
}

QStringList Project::librariesForLinker(const QString &prefix, bool forWindows) const
{
  QStringList libs;
  PURL::UrlList files = absoluteFiles();
  PURL::UrlList::const_iterator it;
  for (it=files.begin(); it!=files.end(); ++it)
    if ( (*it).fileType()==PURL::Library ) libs += prefix + (*it).filepath(forWindows);
  return libs;
}

QString Project::version() const
{
  return Compile::Config::globalValue(this, "version", "0.1");
}
void Project::setVersion(const QString &version)
{
  Compile::Config::setGlobalValue(this, "version", version);
}

QString Project::description() const
{
  return Compile::Config::globalValue(this, "description", QString::null);
}
void Project::setDescription(const QString &description)
{
  Compile::Config::setGlobalValue(this, "description", description);
}

PURL::Url Project::customLinkerScript() const
{
  return Compile::Config::globalValue(this, "custom_linker_script", QString::null);
}
void Project::setCustomLinkerScript(const PURL::Url &url)
{
  Compile::Config::setGlobalValue(this, "custom_linker_script", url.filepath(false));
}

QValueList<Register::TypeData> Project::watchedRegisters() const
{
  QValueList<Register::TypeData> watched;
  QStringList list = listValues("general", "watched_registers", QStringList());
  QStringList::const_iterator it;
  for (it=list.begin(); it!=list.end(); ++it) {
    Register::TypeData rtd = Register::TypeData::fromString(*it);
    if ( rtd.type()!=Register::Invalid ) watched.append(rtd);
  }
  return watched;
}
void Project::setWatchedRegisters(const QValueList<Register::TypeData> &watched)
{
  clearList("general", "watched_registers");
  QValueList<Register::TypeData>::const_iterator it;
  for (it=watched.begin(); it!=watched.end(); ++it)
    appendListValue("general", "watched_registers", (*it).toString());
}
