/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "picc_compile.h"

#include <qregexp.h>
#include <klocale.h>

#include "picc_config.h"

//-----------------------------------------------------------------------------
QStringList PICC::Process::genericArguments(const Compile::Config &) const
{
  QStringList args;
  args += "--ERRFORMAT";
  args += "--WARNFORMAT";
  args += "--MSGFORMAT";
  args += "--CHIP=%DEVICE";
  args += "--IDE=mplab";
  args += "-Q"; // suppress copyright message
  return args;
}

void PICC::Process::logStderrLine(const QString &line)
{
  // #### TODO
  _manager->log(Log::Data, line);
}

//-----------------------------------------------------------------------------
QStringList PICC::CompileStandaloneFile::genericArguments(const Compile::Config &config) const
{
  QStringList args = Process::genericArguments(config);
  args += "-M%MAP";
  args += "-G%SYM";
  args += "--ASMLIST";
  args += config.includeDirs(Tool::Compiler, "-I");
  args += config.customOptions(Tool::Compiler);
  args += "%I";
  return args;
}

QString PICC::CompileStandaloneFile::outputFiles() const
{
  return "PURL::Lst PURL::Map obj PURL::Hex PURL::Coff sym sdb hxl rlf";
}

//-----------------------------------------------------------------------------
QStringList PICC::CompileProjectFile::genericArguments(const Compile::Config &config) const
{
  QStringList args = Process::genericArguments(config);
  args += "-S";
  args += config.includeDirs(Tool::Compiler, "-I");
  args += config.customOptions(Tool::Compiler);
  args += "%I";
  return args;
}

QString PICC::CompileProjectFile::outputFiles() const
{
  return "PURL::AsmPICC";
}

//-----------------------------------------------------------------------------
QStringList PICC::AssembleStandaloneFile::genericArguments(const Compile::Config &config) const
{
  QStringList args = Process::genericArguments(config);
  args += "-M%MAP";
  args += "-G%SYM";
  args += "--ASMLIST";
  args += config.includeDirs(Tool::Assembler, "-I");
  args += config.customOptions(Tool::Assembler);
  args += "%I";
  return args;
}

QString PICC::AssembleStandaloneFile::outputFiles() const
{
  return "PURL::Lst PURL::Map obj PURL::Hex sym sdb hxl rlf";
}

//-----------------------------------------------------------------------------
QStringList PICC::AssembleProjectFile::genericArguments(const Compile::Config &config) const
{
  QStringList args = Process::genericArguments(config);
  args += "-C";
  args += "--ASMLIST";
  args += config.includeDirs(Tool::Assembler, "-I");
  args += config.customOptions(Tool::Assembler);
  args += "%I";
  return args;
}

QString PICC::AssembleProjectFile::outputFiles() const
{
  return "obj PURL::Lst rlf";
}

//-----------------------------------------------------------------------------
QStringList PICC::LinkProject::genericArguments(const Compile::Config &config) const
{
  QStringList args = Process::genericArguments(config);
  args += "-O%O";
  args += "-M%MAP";
  args += "-G%SYM";
  if ( _data.linkType==Compile::Icd2Linking ) args += "--DEBUGGER=icd2";
  args += config.customOptions(Tool::Linker);
  args += "%OBJS";
  args += "%LIBS";
  args += config.customLibraries(Tool::Linker);
  return args;
}

QString PICC::LinkProject::outputFiles() const
{
  return "PURL::Map PURL::Hex PURL::Coff sym sdb hxl";
}
