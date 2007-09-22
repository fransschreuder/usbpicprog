/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic30_compile.h"

#include <qregexp.h>
#include <klocale.h>

#include "config.h"
#include "pic30_config.h"
#include "devices/list/device_list.h"

//-----------------------------------------------------------------------------
QStringList PIC30::CompileFile::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  args += "$NO_AUTO_DEVICE(-mcpu=%DEVICE)";
  args += config.includeDirs(Tool::Compiler, "-I");
  args += config.customOptions(Tool::Compiler);
  return args;
}

QString PIC30::CompileFile::outputFiles() const
{
  return "PURL::AsmPIC30";
}

void PIC30::CompileFile::logStderrLine(const QString &line)
{
  // #### TODO
  _manager->log(Log::Data, line); // unrecognized
}

//-----------------------------------------------------------------------------
QStringList PIC30::CompileStandaloneFile::genericArguments(const Compile::Config &config) const
{
  QStringList args = CompileFile::genericArguments(config);
  args += "%I";
  return args;
}

QString PIC30::CompileStandaloneFile::outputFiles() const
{
  return CompileFile::outputFiles() + " PURL::Object PURL::Elf";
}

//-----------------------------------------------------------------------------
QStringList PIC30::CompileProjectFile::genericArguments(const Compile::Config &config) const
{
  QStringList args = CompileFile::genericArguments(config);
  if ( !Main::toolGroup().withWine() ) args += "-S"; // compile only
  args += "-g";
  args += "%I";
  return args;
}

//-----------------------------------------------------------------------------
QStringList PIC30::AssembleFile::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  args += "-g";
  args += "-a=%LIST"; // listing
  args += "-o%O";
  args += config.includeDirs(Tool::Assembler, "-I");
  args += config.customOptions(Tool::Assembler);
  return args;
}

QString PIC30::AssembleFile::outputFiles() const
{
  return "PURL::Object PURL::Lst";
}

void PIC30::AssembleFile::logStderrLine(const QString &line)
{
  // #### TODO
  _manager->log(Log::Data, line); // unrecognized
}

//-----------------------------------------------------------------------------
QStringList PIC30::AssembleStandaloneFile::genericArguments(const Compile::Config &config) const
{
  QStringList args = AssembleFile::genericArguments(config);
  args += "$NO_AUTO_DEVICE(-p%DEVICE)";
  args += "%I";
  return args;
}

//-----------------------------------------------------------------------------
QStringList PIC30::AssembleProjectFile::genericArguments(const Compile::Config &config) const
{
  QStringList args = AssembleFile::genericArguments(config);
  if ( !_data.items[0].generated ) args += "-p%DEVICE";
  args += "%I";
  return args;
}

//-----------------------------------------------------------------------------
QString PIC30::Link::outputFilepath() const
{
  PURL::FileType type = OUTPUT_TYPE_DATA[static_cast<const Config *>(_config)->outputType()].type;
  return filepath(type);
}

QStringList PIC30::Link::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  args += "-Map";
  args += "%MAP";
  args += "-o%O";
  args += config.includeDirs(Tool::Linker, "-L");
  args += "$LKR(-T%LKR)";
  args += config.customOptions(Tool::Linker);
  args += "%OBJS";
  return args;
}

QString PIC30::Link::outputFiles() const
{
  PURL::FileType type = OUTPUT_TYPE_DATA[static_cast<const Config *>(_config)->outputType()].type;
  return QString("PURL::Map PURL::") + PURL::DATA[type].key;
}

void PIC30::Link::logStderrLine(const QString &line)
{
  // #### TODO
  _manager->log(Log::Data, line); // unrecognized
}

//-----------------------------------------------------------------------------
QStringList PIC30::LinkStandalone::genericArguments(const Compile::Config &config) const
{
  QStringList args = Link::genericArguments(config);
  args += config.customLibraries(Tool::Linker);
  return args;
}

//-----------------------------------------------------------------------------
QStringList PIC30::LinkProject::genericArguments(const Compile::Config &config) const
{
  QStringList args = Link::genericArguments(config);
  args += "%LIBS";
  args += config.customLibraries(Tool::Linker);
  return args;
}

//-----------------------------------------------------------------------------
QString PIC30::BinToHex::inputFilepath(uint) const
{
  PURL::FileType type = OUTPUT_TYPE_DATA[static_cast<const Config *>(_config)->outputType()].type;
  return filepath(type);
}

QStringList PIC30::BinToHex::genericArguments(const Compile::Config &) const
{
  return "%I";
}

QString PIC30::BinToHex::outputFiles() const
{
  return "PURL::Hex";
}

void PIC30::BinToHex::logStderrLine(const QString &line)
{
  // #### TODO
  _manager->log(Log::Data, line); // unrecognized
}
