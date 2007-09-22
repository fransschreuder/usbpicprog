/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "cc5x_compile.h"

#include <qregexp.h>

#include "cc5x_config.h"

QStringList CC5X::CompileFile::genericArguments(const Compile::Config &config) const
{
  QStringList args;
  args += "-a";   // produce asm file
  args += "-CC";  // produce cod c file
  args += "-L";   // produce list file
  args += "-eL";  // error details
  //args += "-FM";  // error format for MPLAB
  args += "-o%O"; // set output file
  args += config.includeDirs(Tool::Compiler, "-I");
  args += config.customOptions(Tool::Compiler);
  args += "%I";
  return args;
}

void CC5X::CompileFile::logStderrLine(const QString &line)
{
  QRegExp rexp("(.*):([0-9]+):(.+)\\[([0-9]+)\\](.+)");
  if ( rexp.exactMatch(line) ) doLog(rexp.cap(3), rexp.cap(5), rexp.cap(1), rexp.cap(2).toUInt()-1);
  else _manager->log(Log::Data, line); // unrecognized
}

QString CC5X::CompileFile::outputFiles() const
{
  return "PURL::Lst PURL::AsmGPAsm PURL::Hex PURL::Cod occ";
}
