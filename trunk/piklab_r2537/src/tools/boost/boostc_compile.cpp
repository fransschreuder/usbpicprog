/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "boostc_compile.h"

#include "boost_config.h"

//----------------------------------------------------------------------------
void Boost::Process::logStderrLine(const QString &line)
{
  if ( parseErrorLine(line, Compile::ParseErrorData("(.*)\\((\\d+).*\\): (error|warning):(.*)", 1, 2, 4, 3)) ) return;
  doLog(Log::LineType::Normal, line, QString::null, 0);
}

//----------------------------------------------------------------------------
QStringList Boost::CompileFile::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  args += "-t %DEVICE";
  args += "-i";
  args += config.includeDirs(Tool::Category::Compiler, "-I", QString::null, ";");
  args += config.customOptions(Tool::Category::Compiler);
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
  args += config.includeDirs(Tool::Category::Linker, "-ld%SEP");
  PURL::Directory dir = Compile::Config::directory(group(), Compile::DirectoryType::Library);
  if ( !dir.isEmpty() ) args += "-ld%SEP" + dir.path();
  args += config.customOptions(Tool::Category::Linker);
  args += "%OBJS";
  return args;
}

QString Boost::Link::outputFiles() const
{
  return "PURL::Lst PURL::Hex PURL::Coff PURL::AsmGPAsm stat tree casm";
}

