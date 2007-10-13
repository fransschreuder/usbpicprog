/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "sdcc_compile.h"

#include <qregexp.h>
#include <qapplication.h>
#include <kmessagebox.h>
#include <klocale.h>

#include "sdcc.h"
#include "sdcc_config.h"
#include "tools/list/tool_list.h"
#include "coff/base/disassembler.h"

//-----------------------------------------------------------------------------
QString SDCC::Process::familyName() const
{
  return FAMILY_DATA[family(_data.device)].name;
}

QString SDCC::Process::deviceName() const
{
  return GPUtils::toDeviceName(_data.device);
}

QStringList SDCC::Process::genericArguments(const Compile::Config &) const
{
  QStringList args;
  args += "-m%FAMILY";
  args += "-%DEVICE";
  args += "-V"; // verbose
  //args += "--debug";
  //args += "-Wl-d";
  return args;
}

QString SDCC::Process::outputFiles() const
{
  return "PURL::Object PURL::AsmGPAsm adb p d";
}

void SDCC::Process::logStderrLine(const QString &line)
{
  QRegExp re("([^:]*):([0-9]+):([^:]+):(.+)");
  if ( !re.exactMatch(line) ) {
    re.setPattern("([^:]*):([0-9]+):(\\w+)\\s*\\[[0-9]+\\](.+)");
    if ( !re.exactMatch(line) ) {
      _manager->log(filterType(line), line);
      return;
    }
  }
  if ( re.cap(1)=="-" ) {
    doLog(Log::Error, re.cap(4), QString::null, 0);
    return;
  }
  doLog(re.cap(3), re.cap(4).stripWhiteSpace(), re.cap(1), re.cap(2).toUInt()-1);
}

//-----------------------------------------------------------------------------
QStringList SDCC::CompileStandaloneFile::genericArguments(const Compile::Config &config) const
{
  QStringList args = Process::genericArguments(config);
  args += config.includeDirs(Tool::Category::Compiler, "-I");
  args += config.customOptions(Tool::Category::Compiler);
  args += "-Wl-o%O";
  args += "-Wl-m"; // output map file
  HexBuffer::Format format = config.hexFormat();
  if( format!=HexBuffer::Nb_Formats )
    args += QString("-Wl-a") + HexBuffer::FORMATS[format];
  args += "%I";
  return args;
}

QString SDCC::CompileStandaloneFile::outputFiles() const
{
  return Process::outputFiles() + " PURL::Lst PURL::Map PURL::Hex PURL::Cod rst sym mem lnk";
}

//-----------------------------------------------------------------------------
QStringList SDCC::CompileProjectFile::genericArguments(const Compile::Config &config) const
{
  QStringList args = Process::genericArguments(config);
  args += config.includeDirs(Tool::Category::Compiler, "-I");
  args += config.customOptions(Tool::Category::Compiler);
  args += "-c"; // compile only
  args += "%I";
  return args;
}

//-----------------------------------------------------------------------------
QStringList SDCC::LinkProjectFile::genericArguments(const Compile::Config &config) const
{
  QStringList args = Process::genericArguments(config);
  args += "-Wl-c"; // create coff file
  args += "$LKR(-Wl-s%LKR)";
  args += config.includeDirs(Tool::Category::Linker, "-I");
  args += config.customOptions(Tool::Category::Linker);
  args += "-o%O";
  args += "%OBJS";
  args += "%LIBS";
  args += config.customLibraries(Tool::Category::Linker);
  return args;
}

QString SDCC::LinkProjectFile::outputFiles() const
{
  return Process::outputFiles() + "PURL::Lkr PURL::Hex PURL::Cod PURL::Coff";
}
