/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "disassembler.h"

#include "devices/pic/pic/pic_memory.h"
#include "coff.h"

//-----------------------------------------------------------------------------
QString SourceLine::comment(PURL::SourceFamily family, const QString &text)
{
  switch (family) {
  case PURL::Asm:   return "; " + text;
  case PURL::C:     return "/* " + text + " */";
  case PURL::JAL:   return "-- " + text;
  case PURL::Cpp:   return "// " + text;
  case PURL::Basic: return "' " + text;
  case PURL::Nb_SourceFamilies: break;
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

QStringList SourceLine::transformConfigName(const Pic::Data &data, uint wordIndex, const QString &name)
{
  if ( !data.is18Family() ) return QStringList::split(' ', name);
  Q_ASSERT( name.contains(' ')==0 );
  QString s = name + '_';
  if ( data.name()=="18C601" || data.name()=="18C801" || data.name().startsWith("18F" ) )
    s += QString::number(wordIndex/2+1) + (wordIndex%2==0 ? 'L' : 'H');
  else s += QString::number(wordIndex);
  return QStringList(s);
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
  QStringList names = value.extraCNames;
  for (uint i=0; i<uint(names.count()); i++) cnames += transformConfigName(data, wordIndex, names[i]);
  return cnames;
}

QStringList SourceLine::configNames(const Pic::Memory &memory, uint word, bool &ok)
{
  const Pic::Data &data = memory.device();
  const Pic::Config &config = data.config();
  uint v = memory.normalizedWord(Pic::MemoryConfig, word);
  const Pic::Config::Word &cword = config._words[word];
  QStringList cnames;
  for (uint k=0; k<uint(cword.masks.count()); k++) {
    const Pic::Config::Mask &cmask = cword.masks[k];
    if ( (cmask.value & cword.pmask)==cmask.value ) continue; // protected bits
    for (int l=cmask.values.count()-1; l>=0; l--) {
      if ( (v & cmask.values[l].value)==cmask.values[l].value ) {
        QString cname = cmask.values[l].cname;
        if ( cname=="_" ) break;
        if ( cname.isEmpty() ) {
          cnames += "?";
          ok = false;
        } else cnames += transformConfigName(data, word, cname);
        break;
      }
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

SourceLine::List GPUtils::generateConfigLines(const Pic::Memory &memory, bool &ok)
{
  SourceLine::List lines;
  const Pic::Data &data = memory.device();
  const Pic::Config &config = data.config();
  for (uint i=0; i<data.nbWords(Pic::MemoryConfig); i++) {
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
  lines.appendNotIndentedCode("#include <" + toDeviceName(data.name()) + ".h>");
  lines.appendNotIndentedCode("processor " + toDeviceName(data.name()));

  // memory config
  bool isDefault = true;
  for (uint k=0; k<data.nbWords(Pic::MemoryConfig); k++) {
    uint op = memory.normalizedWord(Pic::MemoryConfig, k);
    uint mask = data.config()._words[k].usedMask();
    if ( (op & mask)!=mask ) isDefault = false; // this is not completely correct but otherwise empty config is written...
  }
  if ( !isDefault ) {
    lines.appendEmpty();
    lines.appendSeparator();
    bool ok;
    lines += generateConfigLines(memory, ok);
  }

  // user ids
  QString tmp;
  for (uint k=0; k<data.nbWords(Pic::MemoryUserId); k++) {
    uint op = memory.normalizedWord(Pic::MemoryUserId, k);
    uint mask = data.range(Pic::MemoryUserId).rmask;
    if ( (op & mask)==mask ) continue;
    if ( data.is18Family() ) {
      uint ad = data.range(Pic::MemoryUserId).start + data.range(Pic::MemoryUserId).hexFileOffset + k*data.addressIncrement(Pic::MemoryUserId);
      lines.appendIndentedCode("__IDLOCS " + toHexLabel(ad, data.nbCharsAddress()) + ", " + toHexLabel(op, data.nbCharsWord(Pic::MemoryUserId)));
    } else tmp += toHex(op & 0xF, 1);
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
  uint nb = data.nbWords(Pic::MemoryCode);
  for (uint k=0; k<nb; k++) {
    uint op = memory.normalizedWord(Pic::MemoryCode, k);
    uint mask = data.range(Pic::MemoryCode).rmask;
    if ( (op & mask)==mask ) newOrg = true;
    else {
      if (newOrg) {
        if ( !first ) tmp += '\n';
        first = false;
        uint org = data.range(Pic::MemoryCode).start + data.range(Pic::MemoryCode).hexFileOffset + k*data.addressIncrement(Pic::MemoryCode);
        lines.appendNotIndentedCode("org " + toHexLabel(org, data.nbCharsAddress()));
        newOrg = false;
      }
      char buffer[512];
      buffer[0] = 0;
      uint op2 = ((k+1)<nb ? memory.word(Pic::MemoryCode, k+1) : 0);
      uint n = Coff::disassemble(op, op2, k, data.architecture(), buffer, 512);
      lines.appendIndentedCode(QString(buffer));
      if ( n==2 ) k++;
    }
  }

  // eeprom data
  lines.appendEmpty();
  lines.appendSeparator();
  lines.appendTitle("eeprom memory");
  newOrg = true;
  nb = data.nbWords(Pic::MemoryEeprom);
  for (uint k=0; k<nb; k++) {
    uint op = memory.normalizedWord(Pic::MemoryEeprom, k);
    uint mask = data.range(Pic::MemoryEeprom).rmask;
    if ( (op & mask)==mask ) newOrg = true;
    else {
      if (newOrg) {
        uint org = data.range(Pic::MemoryEeprom).start + data.range(Pic::MemoryEeprom).hexFileOffset + k*data.addressIncrement(Pic::MemoryEeprom);
        lines.appendNotIndentedCode("org " + toHexLabel(org, data.nbCharsAddress()));
        newOrg = false;
      }
      lines.appendIndentedCode("de " + toHexLabel(op, data.nbCharsWord(Pic::MemoryEeprom)));
    }
  }

  lines.appendIndentedCode("end");
  return lines;
}
