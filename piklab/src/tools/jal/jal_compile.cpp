/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "jal_compile.h"

#include "jal.h"

void JAL::CompileFile::logStderrLine(const QString &line)
{
  if ( line.contains('\r') ) return; // what are those lines ?
  // #### TODO
  _manager->log(Log::Data, line); // unrecognized
}

QStringList JAL::CompileFile::genericArguments(const Compile::Config &) const
{
  return "%I";
}

QString JAL::CompileFile::outputFiles() const
{
  return "PURL::AsmGPAsm PURL::Hex";
}
