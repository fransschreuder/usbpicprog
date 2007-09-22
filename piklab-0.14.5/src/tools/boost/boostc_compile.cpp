/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "boostc_compile.h"

#include <qregexp.h>

#include "boostc_config.h"

//----------------------------------------------------------------------------
void Boost::Process::logStderrLine(const QString &line)
{
  QRegExp rexp("(.*)\\((\\d+).*\\): (error|warning):(.*)");
  if ( rexp.exactMatch(line) ) doLog(rexp.cap(3), rexp.cap(4), rexp.cap(1), rexp.cap(2).toUInt()-1);
  else _manager->log(Log::Data, line);
}

//----------------------------------------------------------------------------
QStringList Boost::CompileFile::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  args += "-t %DEVICE";
  args += "-i";
  args += config.includeDirs(Tool::Compiler, "-I", QString::null, ";");
  args += config.customOptions(Tool::Compiler);
  args += "%I";
  return args;
}

QString Boost::CompileFile::outputFiles() const
{
  return "obj";
}

//----------------------------------------------------------------------------
QStringList Boost::Link::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  args += "-t %DEVICE";
  args += "-p";
  args += "%PROJECT";
  args += config.includeDirs(Tool::Linker, "-ld%SEP");
  PURL::Directory dir = config.directory(Compile::LibraryDirectory);
  if ( !dir.isEmpty() ) args += "-ld%SEP" + dir.path(false);
  args += config.customOptions(Tool::Linker);
  args += "%OBJS";
  return args;
}

QString Boost::Link::outputFiles() const
{
  return "PURL::Lst PURL::Hex PURL::Coff PURL::AsmGPAsm stat tree casm";
}

