/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "jalv2_compile.h"

#include "jalv2.h"
#include "common/common/misc.h"
#include "tools/list/compile_config.h"

QStringList JALV2::CompileFile::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  QStringList includes = config.includeDirs(Tool::Category::Compiler, QString::null, QString::null, ";");
  QString s = (includes.isEmpty() ? QString::null : includes[0]);
  PURL::Directory dir = Compile::Config::directory(group(), Compile::DirectoryType::Header).path();
  if ( !dir.isEmpty() ) {
    if ( !s.isEmpty() ) s += ";";
    s += dir.path();
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
  if ( parseErrorLine(line, Compile::ParseErrorData("([^:]*):([0-9]+):\\s*(warning:|)(.+)", 1, 2, 4, 3, Log::LineType::Warning, Log::LineType::Error)) ) return;
  doLog(Log::LineType::Normal, line, QString::null, 0); // unrecognized
}

QString JALV2::CompileFile::outputFiles() const
{
  return "PURL::AsmGPAsm PURL::Hex";
}
