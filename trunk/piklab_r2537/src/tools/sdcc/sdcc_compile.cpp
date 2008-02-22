/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "sdcc_compile.h"

#include "sdcc.h"
#include "sdcc_config.h"
#include "tools/list/tool_list.h"
#include "sdcc_generator.h"

//-----------------------------------------------------------------------------
QString SDCC::Process::familyName() const
{
  return FAMILY_DATA[family(_data.device)].name;
}

QString SDCC::Process::deviceName() const
{
  return toDeviceName(_data.device);
}

QStringList SDCC::Process::genericArguments(const Compile::Config &) const
{
  QStringList args;
  args += "-m%FAMILY";
  args += "-%DEVICE";
  args += "-V"; // verbose
  args += "--debug";
  return args;
}

QString SDCC::Process::outputFiles() const
{
  return "PURL::Object PURL::AsmGPAsm adb p d PURL::Lst";
}

void SDCC::Process::logStderrLine(const QString &line)
{
  if ( parseErrorLine(line, Compile::ParseErrorData("([^:]*):([0-9]+):(error|warning|message):(.+)", 1, 2, 4, 3)) ) return;
  if ( parseErrorLine(line, Compile::ParseErrorData("([^:]*):([0-9]+):(\\w+)\\s*\\[[0-9]+\\](.+)", 1, 2, 4, 3)) ) return;
  if ( parseErrorLine(line, Compile::ParseErrorData("([^:]*):([0-9]+):(.+)", 1, 2, 3, -1)) ) return;
  if ( parseErrorLine(line, Compile::ParseErrorData("([^:]*):([^:]+):([0-9]+):(.+)", 2, 3, 4, -1)) ) return;
  if ( parseErrorLine(line, Compile::ParseErrorData("([^:]+):(.+)", -1, -1, 2, 1)) ) return;
  doLog(filterType(line), line, QString::null, 0);
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
  return Process::outputFiles() + " PURL::Map PURL::Hex PURL::Cod rst sym mem lnk";
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
  args += "-Wl-m"; // output map file
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
  return Process::outputFiles() + " PURL::Lkr PURL::Hex PURL::Cod PURL::Coff PURL::Map";
}
