/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "jal_generator.h"

#include "devices/pic/base/pic.h"

SourceLine::List JAL::SourceGenerator::configLines(PURL::ToolType, const Device::Memory &, bool &) const
{
  // no config lines (?)
  return SourceLine::List();
}

SourceLine::List JAL::SourceGenerator::includeLines(PURL::ToolType, const Device::Data &data) const
{
  SourceLine::List lines;
  lines.appendNotIndentedCode("include " + data.name().lower() + "_10", i18n("10MHz crystal"));
  lines.appendNotIndentedCode("include jlib", i18n("jal standard library"));
  return lines;
}

SourceLine::List JAL::SourceGenerator::sourceFileContent(PURL::ToolType, const Device::Data &, bool &) const
{
  SourceLine::List lines;
  lines.appendTitle(i18n("main code"));
  lines.appendNotIndentedCode(QString::null, "<< " + i18n("insert code") + " >>");
  lines.appendNotIndentedCode("forever loop", i18n("loop forever"));
  lines.appendNotIndentedCode("end loop");
  return lines;
}
