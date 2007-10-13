/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "ccsc_compile.h"

#include <qregexp.h>
#include <klocale.h>

#include "ccsc.h"
#include "common/global/pfile.h"
#include "ccsc_config.h"
#include "devices/list/device_list.h"
#include "devices/pic/base/pic.h"

QString CCSC::CompileFile::familyName() const
{
  const Pic::Data *pdata = static_cast<const Pic::Data *>(Device::lister().data(_data.device));
  switch (pdata->architecture()) {
    case Pic::Architecture::P10X: return "B";
    case Pic::Architecture::P16X: return "M";
    case Pic::Architecture::P17C: return "7";
    case Pic::Architecture::P18C:
    case Pic::Architecture::P18F:
    case Pic::Architecture::P18J: return "H";
    case Pic::Architecture::P24J:
    case Pic::Architecture::P24H:
    case Pic::Architecture::P30X: break;
    case Pic::Architecture::Nb_Types: break;
  }
  Q_ASSERT(false);
  return QString::null;
}

QStringList CCSC::CompileFile::genericArguments(const Compile::Config &config) const
{
  bool isVersion3 = ( static_cast<const Group &>(Main::toolGroup()).version().majorNum()==3 );
  QStringList args;
  args += "+STDOUT"; // output messages on stdout
  if ( !isVersion3 ) args += "+EA";     // show all messages and warnings
  args += "-P";      // close compile windows after compilation done
  if ( !isVersion3 ) args += "+DF";     // output COFF file
  args += "+LSlst";  // normal list file
  args += "+O8hex";  // produce 8bit Intel file
  if ( !isVersion3 ) args += "+M";      // generate symbol file
  args += "-J";      // do not create project file
  args += "-A";      // do not create stat file
  args += "+F%FAMILY";
  if ( isVersion3 ) args += config.includeDirs(Tool::Category::Compiler, "I=\"", "\"");
  else args += config.includeDirs(Tool::Category::Compiler, "I+=\"", "\"");
  args += config.customOptions(Tool::Category::Compiler);
  args += "%I";
  return args;
}

void CCSC::CompileFile::logStderrLine(const QString &line)
{
  // ignore output for wine
  if ( !Main::toolGroup().withWine() ) parseLine(line);
}

void CCSC::CompileFile::parseLine(const QString &line)
{
  Log::Type type;
  if ( line.startsWith(">>>") )      type = Log::Warning;
  else if ( line.startsWith("***") ) type = Log::Error;
  else if ( line.startsWith("---") ) type = Log::Information;
  else {
    _manager->log(Log::Data, line); // unrecognized
    return;
  }
  QRegExp rexp("[*>-]+\\s\\w+\\s\\d+\\s\"([^\"]*)\"\\sLine\\s(\\d+)\\([^)]*\\):(.*)");
  if ( rexp.exactMatch(line) ) {
    QString s = rexp.cap(1);
    if ( s.endsWith(".") ) s = s.mid(0, s.length()-1);
    doLog(type, rexp.cap(3), s, rexp.cap(2).toUInt()-1);
    return;
  }
  rexp.setPattern("[*>-]+\\s\"([^\"]*)\"\\sLine\\s(\\d+):\\s\\w+\\s#\\d+:(.*)");
  if ( rexp.exactMatch(line) ) {
    QString s = rexp.cap(1);
    if ( s.endsWith(".") ) s = s.mid(0, s.length()-1);
    doLog(type, rexp.cap(3), s, rexp.cap(2).toUInt()-1);
    return;
  }
  doLog(type, line, QString::null, 0);
}

void CCSC::CompileFile::done(int code)
{
  // with wine, rely on error file
  if ( Main::toolGroup().withWine() ) {
    PURL::Url url = PURL::Url(directory(), inputFilepath(0)).toExtension("err");
    Log::StringView sview;
    PURL::File file(url, sview);
    if ( !file.openForRead() ) _manager->log(Log::Error, i18n("Could not find error file (%1).").arg(url.pretty()));
    else {
      QStringList lines = file.readLines();
      for (uint i=0; i<lines.count(); i++) parseLine(lines[i]);
    }
  }
  Compile::Process::done(code);
}

PURL::Url CCSC::CompileFile::url(PURL::FileType type, uint i) const
{
  PURL::Url url;
  Q_ASSERT( i<_data.items.count() );
  url = _data.items[i].url;
  if ( type==PURL::Nb_FileTypes ) return url;
  return url.toFileType(type);
}

QString CCSC::CompileFile::outputFiles() const
{
  return "PURL::Lst PURL::Hex PURL::Coff PURL::Cod sym err esym occ";
}
