/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "c18_compile.h"

#include "c18_config.h"
#include "c18.h"
#include "devices/list/device_list.h"

//-----------------------------------------------------------------------------
QStringList C18::CompileFile::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  args += config.includeDirs(Tool::Compiler, "-I");
  args += "-I" + config.directory(Compile::HeaderDirectory).path(false);
  args += "$NO_AUTO_DEVICE(-p)";
  args += "$NO_AUTO_DEVICE(%DEVICE)";
  args += config.customOptions(Tool::Compiler);
  args += "-fo=%OBJECT";
  args += "%I";
  return args;
}

QString C18::CompileFile::outputFiles() const
{
  return "PURL::Object";
}

void C18::CompileFile::logStderrLine(const QString &line)
{
  // #### TODO
  _manager->log(Log::Data, line);
}

//-----------------------------------------------------------------------------
QStringList C18::Link::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  args += "/k%LKR_PATH";
  args += "%LKR_NAME";
  args += "/l" + config.directory(Compile::LibraryDirectory).path(false);
  args += config.customOptions(Tool::Linker);
  args += "/o%COFF";
  args += "/m%MAP";
  args += "%OBJS";
  args += "%LIBS";
  args += config.customLibraries(Tool::Linker);
  return args;
}

QString C18::Link::outputFiles() const
{
  return "PURL::Lkr PURL::Map PURL::Hex PURL::Coff PURL::Lst PURL::Cod";
}

void C18::Link::logStderrLine(const QString &line)
{
  // #### TODO
  _manager->log(Log::Data, line);
}
