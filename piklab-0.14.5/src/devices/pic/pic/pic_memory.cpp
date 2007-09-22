/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_memory.h"

#include <qfile.h>

#include "common/common/misc.h"

Pic::Memory::Memory(const Data &data)
  : Device::Memory(data), _ranges(Pic::Nb_MemoryRangeTypes)
{
  for (uint i=0; i<Nb_MemoryRangeTypes; i++) {
    uint nb = device().nbWords(MemoryRangeType(i));
    _ranges[i].resize(nb);
  }
  fill(Device::INVALID);
}

void Pic::Memory::fill(MemoryRangeType type, uint value)
{
  for (uint i=0; i<_ranges[type].count(); i++) {
    if ( type==MemoryConfig && value==Device::INVALID ) _ranges[type][i] = device().config()._words[i].bvalue;
    else _ranges[type][i] = value;
  }
}

bool Pic::Memory::isClear(MemoryRangeType type) const
{
  for (uint i=0; i<_ranges[type].count(); i++) {
    if ( type==MemoryConfig ) {
      if ( _ranges[type][i]!=device().config()._words[i].bvalue ) return false;
    } else if (_ranges[type][i]!=Device::INVALID ) return false;
  }
  return true;
}

void Pic::Memory::fill(uint value)
{
  for (uint k=0; k<Nb_MemoryRangeTypes; k++) fill(MemoryRangeType(k), value);
}

void Pic::Memory::copyFrom(MemoryRangeType type, const Memory &memory)
{
  Q_ASSERT( memory.device().name()==device().name() );
  for (uint i=0; i<_ranges[type].count(); i++) _ranges[type][i] = memory._ranges[type][i];
}

void Pic::Memory::copyFrom(const Device::Memory &memory)
{
  Q_ASSERT( memory.device().name()==device().name() );
  for (uint i=0; i<Nb_MemoryRangeTypes; i++)
    copyFrom(MemoryRangeType(i), static_cast<const Memory &>(memory));
}

Device::Array Pic::Memory::arrayForWriting(MemoryRangeType type) const
{
  Device::Array data = _ranges[type];
  for (uint i=0; i<data.count(); i++)
    data[i] &= (type==MemoryConfig ? device().config()._words[i].wmask : device().mask(type));
  return data;
}

uint Pic::Memory::word(MemoryRangeType type, uint offset) const
{
  if ( offset>=_ranges[type].size() ) qCrash();
  return _ranges[type][offset];
}

uint Pic::Memory::normalizeWord(MemoryRangeType type, uint offset, uint value) const
{
  if ( type==MemoryConfig) {
    const Config::Word &cword = device().config()._words[offset];
    return value & cword.usedMask();
  }
  uint mask = device().range(type).rmask; // rmask is used in User Id
  if ( (value & mask)!=value ) return mask;
  return value;
}

uint Pic::Memory::normalizedWord(MemoryRangeType type, uint offset) const
{
  return normalizeWord(type, offset, word(type, offset));
}

void Pic::Memory::setWord(MemoryRangeType type, uint offset, uint value)
{
  Q_ASSERT( _ranges[type].size()>offset );
  //qDebug("Memory::setWord at %s %i: %s", MEMORY_RANGE_TYPE_DATA[type].label, offset, toHex(value, 4).data());
  _ranges[type][offset] = value;
}

void Pic::Memory::setArray(MemoryRangeType type, const Device::Array &data)
{
  Q_ASSERT( _ranges[type].size()==data.size() );
  _ranges[type] = data;
}

QString Pic::Memory::findValue(const QString &maskName) const
{
  if ( maskName.isEmpty() ) return QString::null;
  uint i;
  const Config::Mask *mask = device().config().findMask(maskName, &i);
  if ( mask==0 ) return QString::null;
  uint v = word(MemoryConfig, i) & mask->value;
  for (uint k=0; k<uint(mask->values.count()); k++)
    if ( (v & mask->values[k].value)==v ) return mask->values[k].name;
  Q_ASSERT(false);
  return QString::null;
}

Range Pic::Memory::bootRange() const
{
  const Protection &protection = device().config().protection();
  // with boot size
  QString value = findValue(protection.bootSizeMaskName());
  if ( !value.isEmpty() ) {
    uint size = value.toUInt();
    if ( size==0 ) return Range();
    uint start = device().range(MemoryCode).start;
    if ( device().architecture()==Arch_30X ) start = 0x100;
    return Range(start, 2 * size - 1); // instruction words
  }
  // only CPB
  QString maskName = protection.bootMaskName(Protection::ProgramProtected);
  const Config::Mask *mask = device().config().findMask(maskName);
  for (uint k=0; k<uint(mask->values.count()); k++) {
    RangeVector rv = protection.extractRanges(mask->values[k].name, MemoryCode);
    if ( !rv.isEmpty() ) return rv[0];
  }
  Q_ASSERT(false);
  return Range();
}

Range Pic::Memory::blockRange(uint i) const
{
  const Protection &protection = device().config().protection();
  Q_ASSERT( i<protection.nbBlocks() );
  if ( protection.hasCodeGuard() && i==1 ) { // general segment
    Range previous = blockRange(0);
    if ( previous.isEmpty() ) previous = bootRange();
    uint start = (previous.isEmpty() ? device().range(MemoryCode).start : previous.end + 1);
    return Range(start, device().range(MemoryCode).end);
  }
  QString maskName = protection.blockSizeMaskName(i);
  if ( protection.hasCodeGuard() ) { // secure segment
    QString value = findValue(maskName);
    Q_ASSERT( !value.isEmpty() );
    uint size = value.toUInt();
    if ( size==0 ) return Range();
    Range previous = bootRange();
    uint start = (previous.isEmpty() ? device().range(MemoryCode).start : previous.end + 1);
    return Range(start, 2 * size - 1);
  }
  Range previous = (i==0 ? bootRange() : blockRange(i-1));
  const Config::Mask *mask = device().config().findMask(maskName);
  for (uint k=0; k<uint(mask->values.count()); k++) {
    RangeVector rv = protection.extractRanges(mask->values[k].name, MemoryCode);
    if ( !rv.isEmpty() ) return Range(previous.end + 1, rv[0].end);
  }
  Q_ASSERT(false);
  return Range();
}

Range Pic::Memory::bootProtectedRange(Protection::Type ptype) const
{
  const Protection &protection = device().config().protection();
  QString maskName = protection.bootMaskName(ptype);
  QString value = findValue(maskName);
  if ( value.isEmpty() ) return Range();
  if ( !protection.hasCodeGuard() ) {
    if ( protection.extractRanges(value, MemoryCode).isEmpty() ) return Range();
  } else {
    if ( value!=protection.securityValueName(ptype) ) return Range();
  }
  return bootRange();
}

Range Pic::Memory::blockProtectedRange(Protection::Type ptype, uint i) const
{
  const Protection &protection = device().config().protection();
  QString maskName = protection.blockMaskName(ptype, i);
  QString value = findValue(maskName);
  if ( value.isEmpty() ) return Range();
  if ( !protection.hasCodeGuard() ) {
    if ( protection.extractRanges(value, MemoryCode).isEmpty() ) return Range();
  } else {
    if ( value!=protection.securityValueName(ptype) ) return Range();
  }
  return blockRange(i);
}

RangeVector Pic::Memory::protectedRanges(Protection::Type ptype, MemoryRangeType type) const
{
  const Protection &protection = device().config().protection();
  RangeVector rv;
  if ( type==MemoryCode ) {
    if ( protection.hasBootBlock() ) rv.append(bootProtectedRange(ptype));
    if ( protection.nbBlocks()!=0 ) {
      for (uint i=0; i<protection.nbBlocks(); i++) rv.append(blockProtectedRange(ptype, i));
      return rv;
    }
  }
  if ( !protection.hasCodeGuard() ) {
    QString maskName = protection.maskName(ptype, type);
    QString value = findValue(maskName);
    if ( !value.isEmpty() ) {
      RangeVector tmp = protection.extractRanges(value, type);
      rv.append(tmp[0]);
    }
  }
  return rv;
}

void Pic::Memory::setBootProtection(bool on, Protection::Type ptype)
{
  QString maskName = device().config().protection().bootMaskName(ptype);
  setProtection(on, maskName, ptype);
}

void Pic::Memory::setBlockProtection(bool on, Protection::Type ptype, uint block)
{
  QString maskName = device().config().protection().blockMaskName(ptype, block);
  setProtection(on, maskName, ptype);
}

void Pic::Memory::setProtection(bool on, Protection::Type ptype, MemoryRangeType type)
{
  const Protection &protection = device().config().protection();
  if ( type==MemoryCode ) {
    if ( protection.hasBootBlock() ) setBootProtection(on, ptype);
    if ( protection.nbBlocks()!=0 ) {
      for (uint i=0; i<protection.nbBlocks(); i++) setBlockProtection(on, ptype, i);
      return;
    }
  }
  setProtection(on, protection.maskName(ptype, type), ptype);
}

void Pic::Memory::setProtection(bool on, const QString &maskName, Protection::Type ptype)
{
  const Protection &protection = device().config().protection();
  QString valueName; 
  if ( ptype==Protection::StandardSecurity || ptype==Protection::HighSecurity )
    valueName = protection.securityValueName(ptype);
  else valueName = (on ? "All" : protection.noneProtectedValueName());
  uint i;
  const Config::Mask *mask = device().config().findMask(maskName, &i);
  if( mask==0 ) return;
  uint v = word(Pic::MemoryConfig, i);
  v &= ~mask->value; // clear mask bits
  for (uint k=0; k<uint(mask->values.count()); k++)
    if ( mask->values[k].name==valueName ) setWord(MemoryConfig, i, v | mask->values[k].value);
}

bool Pic::Memory::hasFlagOn(const QString &maskName) const
{
  return ( findValue(maskName)=="On" );
}

void Pic::Memory::setFlagOn(const QString &name, bool on)
{
  uint i;
  const Config::Mask *mask = device().config().findMask(name, &i);
  Q_ASSERT(mask);
  uint v = word(Pic::MemoryConfig, i);
  v &= ~mask->value; // clear mask bits
  for (uint k=0; k<uint(mask->values.count()); k++) {
    if ( XOR(on, mask->values[k].name=="On") ) continue;
    setWord(Pic::MemoryConfig, i, v | mask->values[k].value);
    return;
  }
  Q_ASSERT(false);
}

uint Pic::Memory::checksum() const
{
  uint cs = 0;
  // code
  uint mask = device().mask(MemoryCode);
  RangeVector rv = protectedRanges(Protection::ProgramProtected, MemoryCode);
  //qDebug("rv empty: %i  nb: %i (%s)", rv.isEmpty(), device().nbWords(MemoryCode), toHex(mask, 4).data());
  //for (uint i=0; i<rv.count(); i++)
  //  qDebug("protected: %s-%s", toHex(rv[i].start, 4).data(), toHex(rv[i].end, 4).data());
  for (uint i=0; i<device().nbWords(MemoryCode); i++) {
    if ( rv.contains(i) ) continue;
    uint v = word(MemoryCode, i) & mask;
    switch ( device().architecture() ) {
      case Arch_18F: // #### not true for all 18F ??
      case Arch_18J:
        cs += (v & 0xFF) + (v >> 8);
        break;
      case Arch_30X:
        cs += (v & 0xFF) + ((v >> 8) & 0xFF) + (v >> 16);
        break;
      default:
        cs += v;
        break;
    }
  }
  //qDebug("code: %s", toHex(cs, 9).data());
  // config
  const Config &config = device().config();
  for (uint i=0; i<uint(config._words.count()); i++) {
    const Config::Word &cword = config._words[i];
    uint v = word(MemoryConfig, i) & cword.cmask;
    //qDebug("%i: %s %s", i, toHex(word(MemoryConfig, i), 4).data(), toHex(cword.cmask, 4).data());
    if ( device().architecture()==Arch_30X ) cs += (v & 0xFF) + (v >>8);
    else cs += v;
  }
  const ChecksumData &cdata = device().checksumData();
  cs += (rv.isEmpty() ? cdata.constant : cdata.protectedConstant);
  //qDebug("%s", toHex(cs, 9).data());
  // user ids
  if ( !rv.isEmpty() && device().isPresent(MemoryUserId) ) {
    uint id = 0;
    uint nb = device().nbWords(MemoryUserId);
    uint nbb = (device().name()=="14000" ? 7 : 4);
    for (uint i=0; i<nb; i++) {
      uint v = word(MemoryUserId, nb-i-1) & 0xF;
      if ( device().is18Family() ) id += v;
      else {
        //qDebug("id %i (%i): %s %s", i, nbb, toHex(v, 4).data(), toHex(v << (nbb*i), 9).data());
        id += v << (nbb*i);
      }
    }
    //qDebug("id %s", toHex(id, 9).data());
    cs += id;
  }
  //qDebug("%s %s", toHex(cs, 9).data(), toHex(cs & 0xFFFF, 4).data());
  return cs & 0xFFFF;
}

uint Pic::Memory::unprotectedChecksum() const
{
  const Protection &protection = device().config().protection();
  Memory tmp(*this);
  if ( protection.hasBootBlock() ) tmp.setBootProtection(false, Protection::ProgramProtected);
  if ( protection.nbBlocks()!=0 ) {
    for (uint i=0; i<protection.nbBlocks(); i++)
      tmp.setBlockProtection(false, Protection::ProgramProtected, i);
  } else tmp.setProtection(false, Protection::ProgramProtected, MemoryCode);
  if ( device().isReadable(MemoryEeprom) ) tmp.setProtection(false, Protection::ProgramProtected, MemoryEeprom);
  tmp.setProtection(false, Protection::ProgramProtected, MemoryConfig);
  tmp.setProtection(false, Protection::ProgramProtected, MemoryCal);
  return tmp.checksum();
}

void Pic::Memory::setUserIdToUnprotectedChecksum()
{
  uint cs = unprotectedChecksum();
  uint nb = device().nbWords(MemoryUserId);
  for (uint i=0; i<nb; i++) setWord(MemoryUserId, nb-i-1, (cs >> (4*i)) & 0xF);
}

//-----------------------------------------------------------------------------
void Pic::Memory::savePartial(QTextStream &stream, HexBuffer::Format format) const
{
  // save memory ranges in the same order as MPLAB (for easy checking)
  const MemoryRangeType saveOrder[] = { MemoryCode, MemoryEeprom, MemoryConfig, MemoryUserId, MemoryCal, Nb_MemoryRangeTypes };
  HexBuffer hb;
  for (uint i=0; saveOrder[i]!=Nb_MemoryRangeTypes; i++) {
    hb.clear();
    toHexBuffer(saveOrder[i], hb);
    hb.savePartial(stream, format);
  }
}

//-----------------------------------------------------------------------------
void Pic::Memory::toHexBuffer(MemoryRangeType type, HexBuffer &hb) const
{
  if ( !device().isWritable(type) ) return;
  uint nbBytes = ARCHITECTURE_DATA[device().architecture()].nbBytesWord;
  bool packed = ARCHITECTURE_DATA[device().architecture()].packed;
  uint offset = device().range(type).hexFileOffset;
  if ( offset==0 ) offset = device().range(type).start;
  uint mask = device().mask(type);
  uint wNbBytes = nbBytes;
  if ( packed && type!=Pic::MemoryCode ) {
    offset /= 2;
    wNbBytes /= 2;
  }
  uint byte = 0;
  uint wOffset = 0;
  uint wByte = 0;
  //qDebug("%s wnb=%i snb=%i div=%i", MEMORY_RANGE_TYPE_DATA[type].label, wNbBytes, sNbBytes, div);
  for (uint k=0; k<wNbBytes*device().nbWords(type); k++) {
    // set byte
    uint s = _ranges[type][wOffset] & mask;
    //if ( k<4 ) qDebug("s=%s so=%s sb=%i wo=%i wb=%i", toHex(s, 8).data(), toHex(sOffset, 8).data(), sByte, wOffset, wByte);
    s = s >> (wByte*8);
    s = s & 0xFF;
    if ( (byte%2)==0 ) hb.set(offset, s);
    else hb.set(offset, hb[offset] | (s << ((byte%2)*8)));
    // increment offsets
    byte++;
    if ( (byte%nbBytes)==0 ) {
      byte = 0;
      offset++;
    } else if ( byte==2 ) offset++;
    wByte++;
    if ( (wByte%wNbBytes)==0 ) {
      wByte = 0;
      wOffset++;
    }
  }
}

HexBuffer Pic::Memory::toHexBuffer() const
{
  HexBuffer hb;
  for (uint i=0; i<Nb_MemoryRangeTypes; i++) toHexBuffer(MemoryRangeType(i), hb);
  return hb;
}

void Pic::Memory::fromHexBuffer(MemoryRangeType type, const HexBuffer &hb, WarningTypes &result,
                                QStringList &warnings, QMap<uint, bool> &inRange)
{
  if ( !device().isWritable(type) ) return;
  uint nbBytes = ARCHITECTURE_DATA[device().architecture()].nbBytesWord;
  bool packed = ARCHITECTURE_DATA[device().architecture()].packed;
  uint offset = device().range(type).hexFileOffset;
  if ( offset==0 ) offset = device().range(type).start;
  uint mask = device().mask(type);
  uint wNbBytes = nbBytes;
  if ( packed && type!=Pic::MemoryCode ) {
    offset /= 2;
    wNbBytes /= 2;
  }
  uint byte = 0;
  uint wOffset = 0;
  uint wByte = 0;
  //qDebug("%s wnb=%i snb=%i", MEMORY_RANGE_TYPE_DATA[type].label, wNbBytes, nbBytes);
  for (uint k=0; k<wNbBytes*device().nbWords(type); k++) {
    // set byte
    uint s = hb[offset];
    //if ( k<4 ) qDebug("s=%s so=%s sb=%i wo=%i wb=%i", toHex(s, 8).data(), toHex(offset, 8).data(), byte, wOffset, wByte);
    if ( s==Device::INVALID ) {
      if ( type==MemoryConfig ) _ranges[type][wOffset] = mask;
      else _ranges[type][wOffset] = Device::INVALID;
    } else {
      inRange[offset] = true;
      s = s >> ((byte%2)*8);
      s = s & 0xFF;
      if ( wByte==0 ) _ranges[type][wOffset] = s;
      else _ranges[type][wOffset] |= (s << (wByte*8));
    }
    // increment offsets
    byte++;
    if ( (byte%nbBytes)==0 ) {
      byte = 0;
      offset++;
    } else if ( byte==2 ) offset++;
    wByte++;
    if ( (wByte%wNbBytes)==0 ) {
      if ( _ranges[type][wOffset]!=Device::INVALID ) {
        if ( !(result & ValueTooLarge) && (_ranges[type][wOffset] & mask)!=_ranges[type][wOffset] ) {
          result |= ValueTooLarge;
          warnings += i18n("At least one word (at offset %1) is larger (%2) than the corresponding mask (%3).")
                      .arg(toHexLabel(offset, 8)).arg(toHexLabel(_ranges[type][wOffset], 8)).arg(toHexLabel(mask, 8));
        }
        _ranges[type][wOffset] = (_ranges[type][wOffset] & mask);
      }
      wByte = 0;
      wOffset++;
    }
  }
}

void Pic::Memory::fromHexBuffer(const HexBuffer &hb, WarningTypes &result,
                                QStringList &warnings, QMap<uint, bool> &inRange)
{
  for (uint i=0; i<Nb_MemoryRangeTypes; i++)
    fromHexBuffer(MemoryRangeType(i), hb, result, warnings, inRange);
}
