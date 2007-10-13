/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "disassembler.h"

#include "devices/base/device_group.h"
#include "devices/pic/pic/pic_memory.h"
#include "coff.h"

//-----------------------------------------------------------------------------
QString SourceLine::comment(PURL::SourceFamily family, const QString &text)
{
  switch (family) {
  case PURL::SourceFamily::Asm:   return "; " + text;
  case PURL::SourceFamily::C:     return "/* " + text + " */";
  case PURL::SourceFamily::JAL:   return "-- " + text;
  case PURL::SourceFamily::Cpp:   return "// " + text;
  case PURL::SourceFamily::Basic: return "' " + text;
  case PURL::SourceFamily::Nb_Types: break;
  }
  Q_ASSERT(false);
  return QString::null;
}

namespace SourceLine
{
class LineData {
public:
  LineData() : group(-1) {}
  QString text, comment;
  int group;
};
} // namespace

QStringList SourceLine::lines(PURL::SourceFamily family, const List &list, uint nbSpaces)
{
  QValueList<LineData> lines;
  QValueList<uint> groupCommentColumn;
  groupCommentColumn.append(0);
  List::const_iterator it;
  for (it=list.begin(); it!=list.end(); ++it) {
    LineData data;
    switch((*it).type) {
    case Indented:
      data.text = repeat(" ", nbSpaces);
    case NotIndented:
      if ( (*it).code.isEmpty() && !(*it).comment.isEmpty() ) data.text += comment(family, (*it).comment);
      else {
        data.text += (*it).code;
        data.comment = (*it).comment;
        data.group = groupCommentColumn.count() - 1;
        groupCommentColumn[data.group] = qMax(groupCommentColumn[data.group], uint(data.text.length()));
      }
      break;
    case Separator:
      data.text = comment(family, "-----------------------------------------------------------------------");
      groupCommentColumn.append(0);
      break;
    case Empty:
      break;
    case Title:
      data.text = comment(family, (*it).comment);
      break;
    }
    lines += data;
  }
  QStringList slines;
  QValueList<LineData>::const_iterator lit;
  for (lit=lines.begin(); lit!=lines.end(); ++lit) {
    if ( (*lit).group==-1 || (*lit).comment.isEmpty() ) slines += (*lit).text;
    else {
      uint col = groupCommentColumn[(*lit).group] + 1;
      slines += (*lit).text.leftJustify(col, ' ') + comment(family, (*lit).comment);
    }
  }
  return slines;
}

QString SourceLine::text(PURL::SourceFamily family, const List &list, uint nbSpaces)
{
  return lines(family, list, nbSpaces).join("\n") + "\n";
}

QString SourceLine::transformConfigName(const Pic::Data &data, uint wordIndex, const QString &name)
{
  if ( !data.is18Family() ) return name;
  bool ok;
  (void)fromHexLabel(name, &ok);
  if (ok) return name;
  QString s = name + '_';
  if ( data.name()=="18C601" || data.name()=="18C801" || data.name().startsWith("18F" ) )
    s += QString::number(wordIndex/2+1) + (wordIndex%2==0 ? 'L' : 'H');
  else s += QString::number(wordIndex);
  return s;
}

QStringList SourceLine::ignoredConfigNames(const Pic::Data &data, uint wordIndex)
{
  QStringList cnames;
  QStringList names = data.config()._words[wordIndex].ignoredCNames;
  for (uint i=0; i<uint(names.count()); i++) cnames += transformConfigName(data, wordIndex, names[i]);
  return cnames;
}

QStringList SourceLine::extraConfigNames(const Pic::Data &data, uint wordIndex, const Pic::Config::Value &value)
{
  QStringList cnames;
  QStringList names = value.extraCnames;
  for (uint i=0; i<uint(names.count()); i++) cnames += transformConfigName(data, wordIndex, names[i]);
  return cnames;
}

QStringList SourceLine::configNames(const Pic::Memory &memory, uint word, bool &ok)
{
  ok = true;
  const Pic::Data &data = memory.device();
  const Pic::Config &config = data.config();
  BitValue v = memory.normalizedWord(Pic::MemoryRangeType::Config, word);
  const Pic::Config::Word &cword = config._words[word];
  QStringList cnames;
  for (uint k=0; k<uint(cword.masks.count()); k++) {
    const Pic::Config::Mask &cmask = cword.masks[k];
    if ( cmask.value.isInside(cword.pmask) ) continue; // protected bits
    for (int l=cmask.values.count()-1; l>=0; l--) {
      if ( !cmask.values[l].value.isInside(v) ) continue;
      for (uint i=0; i<cmask.values[l].cnames.count(); i++) {
        QString cname = cmask.values[l].cnames[i];
        if ( cname.isEmpty() ) {
          cnames += "?";
          ok = false;
        } else cnames += transformConfigName(data, word, cname);
      }
      break;
    }
  }
  return cnames;
}

//-----------------------------------------------------------------------------
QString GPUtils::toDeviceName(const QString &device)
{
  if ( device.startsWith("PS") ) return device.lower();
  return "p" + device.lower();
}

SourceLine::List GPUtils::includeLines(const Device::Data &data)
{
  SourceLine::List lines;
  QString include = toDeviceName(data.name());
  if ( data.name()=="12CR509A" ) include = "p12c509a";
  else if ( data.name().startsWith("16C5") || data.name().startsWith("16CR5") ) include = "p16c5x";
  else if ( data.name().startsWith("16F5") ) include = "p16f5x";
  else if ( data.name()=="16CR620A" ) include = "p16c620a";
  lines.appendIndentedCode("#include <" + include + ".inc>");
  return lines;
}

SourceLine::List GPUtils::generateConfigLines(const Pic::Memory &memory, bool &ok)
{
  SourceLine::List lines;
  const Pic::Data &data = memory.device();
  const Pic::Config &config = data.config();
  for (uint i=0; i<data.nbWords(Pic::MemoryRangeType::Config); i++) {
    const Pic::Config::Word &cword = config._words[i];
    QStringList cnames = SourceLine::configNames(memory, i, ok);
    if ( cnames.isEmpty() ) continue;
    QString code = "__CONFIG ";
    if ( !cword.name.isEmpty() ) code += "_" + cword.name + ", ";
    code += cnames.join(" & ");
    lines.appendIndentedCode(code);
  }
  return lines;
}

SourceLine::List GPUtils::disassemble(const Pic::Memory &memory)
{
  SourceLine::List lines;
  const Pic::Data &data = memory.device();

  // includes
  lines += includeLines(data);
  lines.appendNotIndentedCode("processor " + toDeviceName(data.name()));

  // memory config
  bool isDefault = true;
  for (uint k=0; k<data.nbWords(Pic::MemoryRangeType::Config); k++) {
    BitValue op = memory.normalizedWord(Pic::MemoryRangeType::Config, k);
    BitValue mask = data.config()._words[k].usedMask();
    if ( !mask.isInside(op) ) isDefault = false; // this is not completely correct but otherwise empty config is written...
  }
  if ( !isDefault ) {
    lines.appendEmpty();
    lines.appendSeparator();
    bool ok;
    lines += generateConfigLines(memory, ok);
  }

  // user ids
  QString tmp;
  for (uint k=0; k<data.nbWords(Pic::MemoryRangeType::UserId); k++) {
    BitValue op = memory.normalizedWord(Pic::MemoryRangeType::UserId, k);
    BitValue mask = data.range(Pic::MemoryRangeType::UserId).rmask;
    if ( mask.isInside(op) ) continue;
    if ( data.is18Family() ) {
      Address ad = data.range(Pic::MemoryRangeType::UserId).start + data.range(Pic::MemoryRangeType::UserId).hexFileOffset + k*data.addressIncrement(Pic::MemoryRangeType::UserId);
      lines.appendIndentedCode("__IDLOCS " + toHexLabel(ad, data.nbCharsAddress()) + ", " + toHexLabel(op, data.nbCharsWord(Pic::MemoryRangeType::UserId)));
    } else tmp += toHex(op.nybble(0), 1);
  }
  if ( !tmp.isEmpty() ) {
    lines.appendEmpty();
    lines.appendSeparator();
    lines.appendIndentedCode("__IDLOCS 0x" + tmp);
  }

  // memory code
  lines.appendEmpty();
  lines.appendSeparator();
  lines.appendTitle("code memory");
  bool first = true, newOrg = true;
  uint nb = data.nbWords(Pic::MemoryRangeType::Code);
  for (uint k=0; k<nb; k++) {
    BitValue op = memory.normalizedWord(Pic::MemoryRangeType::Code, k);
    BitValue mask = data.range(Pic::MemoryRangeType::Code).rmask;
    if ( mask.isInside(op) ) newOrg = true;
    else {
      if (newOrg) {
        if ( !first ) tmp += '\n';
        first = false;
        Address org = data.range(Pic::MemoryRangeType::Code).start + data.range(Pic::MemoryRangeType::Code).hexFileOffset + k*data.addressIncrement(Pic::MemoryRangeType::Code);
        lines.appendNotIndentedCode("org " + toHexLabel(org, data.nbCharsAddress()));
        newOrg = false;
      }
      char buffer[512];
      buffer[0] = 0;
      BitValue op2 = ((k+1)<nb ? memory.word(Pic::MemoryRangeType::Code, k+1) : 0);
      uint n = Coff::disassemble(op.toUInt(), op2.toUInt(), k, data.architecture(), buffer, 512);
      lines.appendIndentedCode(QString(buffer));
      if ( n==2 ) k++;
    }
  }

  // eeprom data
  lines.appendEmpty();
  lines.appendSeparator();
  lines.appendTitle("eeprom memory");
  newOrg = true;
  nb = data.nbWords(Pic::MemoryRangeType::Eeprom);
  for (uint k=0; k<nb; k++) {
    BitValue op = memory.normalizedWord(Pic::MemoryRangeType::Eeprom, k);
    BitValue mask = data.range(Pic::MemoryRangeType::Eeprom).rmask;
    if ( mask.isInside(op) ) newOrg = true;
    else {
      if (newOrg) {
        Address org = data.range(Pic::MemoryRangeType::Eeprom).start + data.range(Pic::MemoryRangeType::Eeprom).hexFileOffset + k*data.addressIncrement(Pic::MemoryRangeType::Eeprom);
        lines.appendNotIndentedCode("org " + toHexLabel(org, data.nbCharsAddress()));
        newOrg = false;
      }
      lines.appendIndentedCode("de " + toHexLabel(op, data.nbCharsWord(Pic::MemoryRangeType::Eeprom)));
    }
  }

  lines.appendNotIndentedCode("end");
  return lines;
}

//-----------------------------------------------------------------------------
SourceLine::List Tool::SourceGenerator::templateSourceFile(PURL::ToolType type, const Device::Data &data, bool &ok) const
{
  SourceLine::List lines;
  lines.appendSeparator();
  lines.appendTitle(i18n("Template source file generated by piklab"));
  lines += includeLines(type, data);
  lines.appendEmpty();
  lines.appendSeparator();
  lines.appendTitle(i18n("Configuration bits: adapt to your setup and needs"));
  Device::Memory *memory = data.group().createMemory(data);
  lines += configLines(type, *memory, ok);
  delete memory;
  lines.appendEmpty();
  lines += sourceFileContent(type, data, ok);
  return lines;
}
