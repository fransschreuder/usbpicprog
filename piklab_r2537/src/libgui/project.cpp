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

#include <ksimpleconfig.h>

#include "devices/list/device_list.h"
#include "progs/base/prog_group.h"
#include "global_config.h"
#include "tools/gputils/gputils_config.h"

bool Project::load(QString &error)
{
  if ( _url.fileType()==PURL::Project ) return XmlDataFile::load(error);

  if ( !_url.exists() ) {
    error = i18n("Project file %1 does not exist.").arg(_url.pretty());
    return false;
  }
  PURL::Url tmp = _url;
  _url = _url.toFileType(PURL::Project);
  if ( _url.exists() && XmlDataFile::load(error) ) return true;
  KConfig *config = new KSimpleConfig(tmp.filepath(), false);

  config->setGroup("Files");
  QStringList list = config->readListEntry("inputFiles");
  QStringList::const_iterator it = list.begin();
  for (; it!=list.end(); ++it) addFile(PURL::Url(directory(), *it));

  config->setGroup("General");
  setVersion(config->readEntry("version", "0.1"));
  setDescription(config->readEntry("description", QString::null));

  config->setGroup("Assembler");
  QString device = config->readEntry("target-device");
  if ( device=="*" ) {
    GlobalConfig gc;
    device = gc.programmerGroup().supportedDevices()[0]; // compatibility
  }
  Compile::Config::setDevice(this, Device::lister().checkName(device));
  GPUtils::Config *gconfig = new GPUtils::Config(this);
  gconfig->setGPAsmWarningLevel(QMIN(config->readUnsignedNumEntry("warn-level", 0), uint(GPUtils::Config::Nb_WarningLevels)));
  gconfig->setRawIncludeDirs(Tool::Category::Assembler, config->readEntry("include-dir", QString::null));
  gconfig->setRawCustomOptions(Tool::Category::Assembler, config->readEntry("other-options", QString::null));

  config->setGroup("Linker") ;
  QString hexFormatString = config->readEntry("hex-format", HexBuffer::FORMATS[HexBuffer::IHX32]);
  for (uint i=0; i<HexBuffer::Nb_Formats; i++)
    if ( hexFormatString==HexBuffer::FORMATS[i] ) gconfig->setHexFormat(HexBuffer::Format(i));
  gconfig->setRawIncludeDirs(Tool::Category::Linker, config->readEntry("objs-libs-dir", QString::null));
  gconfig->setRawCustomOptions(Tool::Category::Linker, config->readEntry("other-options", QString::null));

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
    if ( PURL::Url::fromPathOrUrl(*it).isRelative() ) files += PURL::Url(directory(), *it);
    else files += PURL::Url::fromPathOrUrl(*it);
  }
  return files;
}
void Project::setOpenedFiles(const PURL::UrlList &list)
{
  clearList("general", "opened_files");
  PURL::UrlList::const_iterator it = list.begin();
  for (; it!=list.end(); ++it)
    appendListValue("general", "opened_files", (*it).relativeTo(directory()));
}

PURL::UrlList Project::absoluteFiles() const
{
  PURL::UrlList abs;
  QStringList files = listValues("general", "files", QStringList());
  QStringList::const_iterator it = files.begin();
  for (; it!=files.end(); ++it) abs += PURL::Url::fromPathOrUrl(*it).toAbsolute(directory());
  return abs;
}
void Project::addFile(const PURL::Url &url)
{
  appendListValue("general", "files", url.relativeTo(directory()));
}
void Project::removeFile(const PURL::Url &url)
{
  removeListValue("general", "files", url.relativeTo(directory()));
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
  return tmp.relativeTo(directory(), forWindows ? PURL::WindowsSeparator : PURL::UnixSeparator);
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
    if ( (*it).fileType()==PURL::Object ) objs += (*it).relativeTo(directory(), forWindows ? PURL::WindowsSeparator : PURL::UnixSeparator);
  return objs;
}

QStringList Project::librariesForLinker(const QString &prefix, bool forWindows) const
{
  QStringList libs;
  PURL::UrlList files = absoluteFiles();
  PURL::UrlList::const_iterator it;
  for (it=files.begin(); it!=files.end(); ++it)
    if ( (*it).fileType()==PURL::Library ) libs += prefix + (*it).relativeTo(directory(), forWindows ? PURL::WindowsSeparator : PURL::UnixSeparator);
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
  QString s = Compile::Config::globalValue(this, "custom_linker_script", QString::null);
  return PURL::Url::fromPathOrUrl(s);
}
void Project::setCustomLinkerScript(const PURL::Url &url)
{
  Compile::Config::setGlobalValue(this, "custom_linker_script", url.filepath());
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
