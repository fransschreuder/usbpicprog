/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "jalv2_compile.h"

#include <qregexp.h>

#include "jalv2.h"
#include "common/common/misc.h"
#include "tools/list/compile_config.h"

QStringList JALV2::CompileFile::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  QStringList includes = config.includeDirs(Tool::Compiler, QString::null, QString::null, ";");
  QString s = (includes.isEmpty() ? QString::null : includes[0]);
  PURL::Directory dir = config.directory(Compile::HeaderDirectory).path(false);
  if ( !dir.isEmpty() ) {
    if ( !s.isEmpty() ) s += ";";
    s += dir.path(false);
  }
  if ( !s.isEmpty() ) {
     args += "-s";
     args += s;
  }
  args += "%I";
  return args;
}

void JALV2::CompileFile::logStderrLine(const QString &line)
{
  QRegExp rexp("([^:]*):([0-9]+):\\s*(warning:|)(.+)");
  if ( rexp.exactMatch(line) ) {
    Log::Type type = (rexp.cap(3).isEmpty() ? Log::Error : Log::Warning);
    doLog(type, rexp.cap(4), rexp.cap(1), rexp.cap(2).toUInt()-1);
  } else _manager->log(Log::Data, line); // unrecognized
}

QString JALV2::CompileFile::outputFiles() const
{
  return "PURL::AsmGPAsm PURL::Hex";
}
