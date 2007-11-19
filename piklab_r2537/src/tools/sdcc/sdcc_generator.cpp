/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "sdcc_generator.h"

#include "devices/pic/pic/pic_memory.h"
#include "devices/list/device_list.h"
#include "tools/gputils/gputils.h"

//----------------------------------------------------------------------------
const SDCC::FamilyData SDCC::FAMILY_DATA[Nb_Families] = {
  { "pic14" },
  { "pic16" }
};

SDCC::Family SDCC::family(const QString &device)
{
  const Device::Data *data = Device::lister().data(device);
  switch (static_cast<const Pic::Data *>(data)->architecture().type()) {
    case Pic::Architecture::P10X:
    case Pic::Architecture::P16X: return P14;
    case Pic::Architecture::P17C:
    case Pic::Architecture::P18C:
    case Pic::Architecture::P18F:
    case Pic::Architecture::P18J: return P16;
    case Pic::Architecture::P24F:
    case Pic::Architecture::P24H:
    case Pic::Architecture::P30F:
    case Pic::Architecture::P33F:
    case Pic::Architecture::Nb_Types: break;
  }
  Q_ASSERT(false);
  return Nb_Families;
}

QString SDCC::toDeviceName(const QString &device)
{
  return GPUtils::toDeviceName(device);
}

QValueList<const Device::Data *> SDCC::getSupportedDevices(const QString &s)
{
  return GPUtils::getSupportedDevices(s);
}

SourceLine::List SDCC::SourceGenerator::configLines(PURL::ToolType type, const Device::Memory &memory, bool &ok) const
{
  if ( type==PURL::ToolType::Assembler ) {
    GPUtils::SourceGenerator generator;
    return generator.configLines(type, memory, ok);
  }
  const Pic::Memory &pmemory = static_cast<const Pic::Memory &>(memory);
  const Pic::Data &data = pmemory.device();
  const Pic::Config &config = data.config();
  SourceLine::List lines;
  if ( !data.is18Family() ) lines.appendNotIndentedCode("typedef unsigned int word;");
  Address address = data.range(Pic::MemoryRangeType::Config).start;
  QString prefix = (data.nbWords(Pic::MemoryRangeType::Config)==2 || data.name().startsWith("16F9") ? "_" : "__");
  for (uint i=0; i<data.nbWords(Pic::MemoryRangeType::Config); i++) {
    const Pic::Config::Word &cword = config._words[i];
    QStringList cnames = SourceLine::configNames(Pic::ConfigNameType::SDCC, pmemory, i, ok);
    if ( cnames.isEmpty() ) continue;
    QString code;
    if ( data.is18Family() ) code += "code char at ";
    else code += "word at ";
    if ( cword.name.isEmpty() ) code += toHexLabel(address, data.nbCharsAddress()) + " CONFIG";
    else code += prefix + cword.name + " " + cword.name;
    code += " = " + cnames.join(" & ") + ";";
    lines.appendNotIndentedCode(code);
    address += data.addressIncrement(Pic::MemoryRangeType::Config);
  }
  return lines;
}

//----------------------------------------------------------------------------
SourceLine::List SDCC::SourceGenerator::includeLines(PURL::ToolType type, const Device::Data &data) const
{
  if ( type==PURL::ToolType::Assembler ) {
    GPUtils::SourceGenerator generator;
    return generator.includeLines(type, data);
  }
  SourceLine::List lines;
  QString name = data.name().lower().replace("CR", "C");
  lines.appendNotIndentedCode("#include <pic" + name + ".h>");
  return lines;
}

SourceLine::List SDCC::SourceGenerator::sourceFileContent(PURL::ToolType type, const Device::Data &data, bool &ok) const
{
  if ( type==PURL::ToolType::Assembler ) {
    GPUtils::SourceGenerator generator;
    return generator.sourceFileContent(type, data, ok);
  }
  SourceLine::List lines;
  switch (static_cast<const Pic::Data &>(data).architecture().type()) {
    case Pic::Architecture::P16X:
      lines.appendNotIndentedCode("void isr() interrupt 0 {", i18n("interrupt service routine"));
      lines.appendIndentedCode(QString::null, "<< " + i18n("insert interrupt code") + " >>");
      lines.appendNotIndentedCode("}");
      lines.appendEmpty();
      break;
    case Pic::Architecture::P18C:
    case Pic::Architecture::P18F:
    case Pic::Architecture::P18J:
      // #### TODO: template interrupt code
      break;
    default: Q_ASSERT(false); break;
  }
  lines.appendNotIndentedCode("void main() {");
  lines.appendIndentedCode(QString::null, "<< " + i18n("insert code") + " >>");
  lines.appendNotIndentedCode("}");
  return lines;
}
