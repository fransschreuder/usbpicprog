/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "mpc_compile.h"

#include <qregexp.h>
#include <klocale.h>

#include "common/global/pfile.h"
#include "mpc_config.h"
#include "devices/list/device_list.h"
#include "devices/pic/base/pic.h"

QStringList MPC::CompileFile::genericArguments(const Compile::Config &) const
{
  QStringList args;
  args += "%I";
  return args;
}

void MPC::CompileFile::logStderrLine(const QString &)
{
  // ignore output
}

void MPC::CompileFile::parseLine(const QString &line)
{
  QRegExp rexp("(\\w+)\\s+(.*)\\s+(\\d+):\\d+:(.*)");
  if ( rexp.exactMatch(line) ) doLog(rexp.cap(1), rexp.cap(4), rexp.cap(2), rexp.cap(3).toUInt()-1);
  else _manager->log(Log::Data, line); // unrecognized
}

void MPC::CompileFile::done(int code)
{
  // rely on error file
  PURL::Url url = PURL::Url(directory(), inputFilepath(0)).toExtension("err");
  Log::StringView sview;
  PURL::File file(url, sview);
  if ( !file.openForRead() ) _manager->log(Log::Error, i18n("Could not find error file (%1).").arg(url.pretty()));
  else {
    QStringList lines = file.readLines();
    for (uint i=0; i<lines.count(); i++) parseLine(lines[i]);
  }
  Compile::Process::done(code);
}

QString MPC::CompileFile::outputFiles() const
{
  return "PURL::Lst PURL::Hex PURL::Cod err";
}
