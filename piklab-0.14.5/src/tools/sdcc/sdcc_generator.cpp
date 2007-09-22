/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "sdcc_generator.h"

#include "tools/list/tool_list.h"
#include "devices/pic/pic/pic_memory.h"

SourceLine::List SDCC::SourceGenerator::configLines(PURL::ToolType type, const Device::Memory &memory, bool &ok) const
{
  if ( type==PURL::Assembler ) {
    const Tool::SourceGenerator *generator = Tool::lister().group("gputils")->sourceGenerator();
    return generator->configLines(type, memory, ok);
  }
  const Pic::Memory &pmemory = static_cast<const Pic::Memory &>(memory);
  const Pic::Data &data = pmemory.device();
  const Pic::Config &config = data.config();
  SourceLine::List lines;
  if ( !data.is18Family() ) lines.appendNotIndentedCode("typedef unsigned int word;");
  uint address = data.range(Pic::MemoryConfig).start;
  for (uint i=0; i<data.nbWords(Pic::MemoryConfig); i++) {
    const Pic::Config::Word &cword = config._words[i];
    QStringList cnames = SourceLine::configNames(pmemory, i, ok);
    if ( cnames.isEmpty() ) continue;
    QString code;
    if ( data.is18Family() ) code += "code char at ";
    else code += "word at ";
    if ( cword.name.isEmpty() ) code += toHexLabel(address, data.nbCharsAddress()) + " CONFIG";
    else code += "__" + cword.name + " " + cword.name;
    code += " = " + cnames.join(" & ") + ";";
    lines.appendNotIndentedCode(code);
    address += data.addressIncrement(Pic::MemoryConfig);
  }
  return lines;
}

SourceLine::List SDCC::SourceGenerator::includeLines(PURL::ToolType type, const Device::Data &data) const
{
  if ( type==PURL::Assembler ) {
    const Tool::SourceGenerator *generator = Tool::lister().group("gputils")->sourceGenerator();
    return generator->includeLines(type, data);
  }
  SourceLine::List lines;
  lines.appendNotIndentedCode("#include <pic" + data.name().lower() + ".h>");
  return lines;
}

SourceLine::List SDCC::SourceGenerator::sourceFileContent(PURL::ToolType type, const Device::Data &data, bool &ok) const
{
  if ( type==PURL::Assembler ) {
    const Tool::SourceGenerator *generator = Tool::lister().group("gputils")->sourceGenerator();
    return generator->sourceFileContent(type, data, ok);
  }
  SourceLine::List lines;
  switch (static_cast<const Pic::Data &>(data).architecture()) {
    case Pic::Arch_16X:
      lines.appendNotIndentedCode("void isr() interrupt 0 {", i18n("interrupt service routine"));
      lines.appendIndentedCode(QString::null, "<< " + i18n("insert interrupt code") + " >>");
      lines.appendNotIndentedCode("}");
      lines.appendEmpty();
      break;
    case Pic::Arch_18C:
    case Pic::Arch_18F:
    case Pic::Arch_18J:
      // #### TODO: template interrupt code
      break;
    default: Q_ASSERT(false); break;
  }
  lines.appendNotIndentedCode("void main() {");
  lines.appendIndentedCode(QString::null, "<< " + i18n("insert code") + " >>");
  lines.appendNotIndentedCode("}");
  return lines;
}
