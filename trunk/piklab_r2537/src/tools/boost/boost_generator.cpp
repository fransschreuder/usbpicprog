/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "boost_generator.h"

#include "devices/pic/pic/pic_memory.h"

//---------------------------------------------------------------------------
SourceLine::List Boost::CSourceGenerator::configLines(PURL::ToolType, const Device::Memory &memory, bool &ok) const
{
  const Pic::Memory &pmemory = static_cast<const Pic::Memory &>(memory);
  const Pic::Data &data = pmemory.device();
  const Pic::Config &config = data.config();
  SourceLine::List lines;
  Address address = data.range(Pic::MemoryRangeType::Config).start;
  for (uint i=0; i<data.nbWords(Pic::MemoryRangeType::Config); i++) {
   const Pic::Config::Word &cword = config._words[i];
    QStringList cnames = SourceLine::configNames(Pic::ConfigNameType::Default, pmemory, i, ok);
    if ( cnames.isEmpty() ) continue;
      QString code = "#pragma DATA ";
    if ( cword.name.isEmpty() ) code += toHexLabel(address, data.nbCharsAddress());
    else code += "__" + cword.name;
     code += cnames.join(" & ") + ";";
    lines.appendNotIndentedCode(code);
    address += data.addressIncrement(Pic::MemoryRangeType::Config);
  }
  return lines;
}

SourceLine::List Boost::CSourceGenerator::includeLines(PURL::ToolType, const Device::Data &) const
{
  SourceLine::List lines;
  lines.appendNotIndentedCode("#include <system.h>");
  return lines;
}

SourceLine::List Boost::CSourceGenerator::sourceFileContent(PURL::ToolType, const Device::Data &, bool &) const
{
  SourceLine::List lines;
  lines.appendTitle(i18n("interrupt service routine"));
  lines.appendNotIndentedCode("void interrupt(void) {");
  lines.appendIndentedCode(QString::null, "<< " + i18n("insert interrupt code") + " >>");
  lines.appendNotIndentedCode("}");
  lines.appendEmpty();
  lines.appendTitle(i18n("main program"));
  lines.appendNotIndentedCode("void main() {");
  lines.appendIndentedCode(QString::null, "<< " + i18n("insert code") + " >>");
  lines.appendNotIndentedCode("}");
  return lines;
}

//---------------------------------------------------------------------------
SourceLine::List Boost::BasicSourceGenerator::configLines(PURL::ToolType, const Device::Memory &, bool &ok) const
{
  // config bits ?
  ok = false;
  return SourceLine::List();
}

SourceLine::List Boost::BasicSourceGenerator::includeLines(PURL::ToolType, const Device::Data &) const
{
  return SourceLine::List();
}

SourceLine::List Boost::BasicSourceGenerator::sourceFileContent(PURL::ToolType, const Device::Data &, bool &ok) const
{
  SourceLine::List lines;
  ok = true;
  lines.appendTitle(i18n("main program"));
  lines.appendNotIndentedCode("Sub main()");
  lines.appendIndentedCode(QString::null, "<< " + i18n("insert code") + " >>");
  lines.appendIndentedCode("Do while 1", i18n("loop forever"));
  lines.appendIndentedCode("Loop");
  lines.appendNotIndentedCode("End Sub");
  return lines;
}
