/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "gputils_compile.h"

#include <qregexp.h>
#include <klocale.h>

#include "gputils.h"
#include "gputils_config.h"
#include "devices/list/device_list.h"
#include "coff/base/disassembler.h"

//-----------------------------------------------------------------------------
QString GPUtils::Process::deviceName() const
{
  return toDeviceName(_data.device);
}

//-----------------------------------------------------------------------------
void GPUtils::AssembleFile::logStderrLine(const QString &line)
{
  QRegExp rexp("([^:]*):([0-9]+):(.+)\\[[0-9]+\\](.+)");
  if ( rexp.exactMatch(line) ) {
    doLog(rexp.cap(3), rexp.cap(4), rexp.cap(1), rexp.cap(2).toUInt()-1);
  } else _manager->log(Log::Data, line); // unrecognized
}

//-----------------------------------------------------------------------------
QStringList GPUtils::AssembleStandaloneFile::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  args += "-L"; // force list
  args += "-o%O";
  uint wl = static_cast<const Config &>(config).gpasmWarningLevel();
  if ( wl!=Config::Nb_WarningLevels ) args += "-w" + QString::number(wl);
  args += config.includeDirs(Tool::Category::Assembler, "-I");
  args += "$NO_AUTO_DEVICE(-p%DEVICE)";
  HexBuffer::Format format = config.hexFormat();
  if( format!=HexBuffer::Nb_Formats ) args += QString("-a") + HexBuffer::FORMATS[format];
  args += config.customOptions(Tool::Category::Assembler);
  args += "%I";
  return args;
}

QString GPUtils::AssembleStandaloneFile::outputFiles() const
{
  return "PURL::Lst PURL::Cod PURL::Hex";
}

//-----------------------------------------------------------------------------
QStringList GPUtils::AssembleProjectFile::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  args += "-c"; // relocatable code
  args += config.includeDirs(Tool::Category::Assembler, "-I");
  if ( !_data.items[0].generated ) args += "-p%DEVICE";
  uint wl = static_cast<const Config &>(config).gpasmWarningLevel() ;
  if( wl!=Config::Nb_WarningLevels ) args += "-w" + QString::number(wl);
  args += config.customOptions(Tool::Category::Assembler);
  args += "%I";
  return args;
}

QString GPUtils::AssembleProjectFile::outputFiles() const
{
  return "PURL::Object PURL::Lst";
}

//-----------------------------------------------------------------------------
QStringList GPUtils::LinkProject::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  args += "-o%O";
  args += "-c"; // create coff file
  HexBuffer::Format f = config.hexFormat();
  if ( f!=HexBuffer::Nb_Formats ) args += QString("-a") + HexBuffer::FORMATS[f];
  args += "-m";  // with map
  args += config.includeDirs(Tool::Category::Linker, "-I");
  args += "$LKR(-s%LKR)";
  args += config.customOptions(Tool::Category::Linker);
  args += "%OBJS";
  args += "%LIBS";
  return args;
}

QString GPUtils::LinkProject::outputFiles() const
{
  return "PURL::Lkr PURL::Map PURL::Lst PURL::Cod PURL::Coff PURL::Hex";
}
