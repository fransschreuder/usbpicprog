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
  : Device::Memory(data), _ranges(MemoryRangeType::Nb_Types)
{
  FOR_EACH(MemoryRangeType, i)
    _ranges[i].resize(device().nbWords(i));
  fill(BitValue());
}

void Pic::Memory::fill(MemoryRangeType type, BitValue value)
{
  for (uint i=0; i<_ranges[type].count(); i++) {
    if ( type==MemoryRangeType::Config && !value.isInitialized() ) _ranges[type][i] = device().config()._words[i].bvalue;
    else _ranges[type][i] = value;
  }
}

bool Pic::Memory::isClear(MemoryRangeType type) const
{
  for (uint i=0; i<_ranges[type].count(); i++) {
    if ( type==MemoryRangeType::Config ) {
      if ( _ranges[type][i]!=device().config()._words[i].bvalue ) return false;
    } else if ( _ranges[type][i].isInitialized() ) return false;
  }
  return true;
}

void Pic::Memory::fill(BitValue value)
{
  FOR_EACH(MemoryRangeType, k) fill(k, value);
}

void Pic::Memory::copyFrom(MemoryRangeType type, const Memory &memory)
{
  Q_ASSERT( memory.device().name()==device().name() );
  for (uint i=0; i<_ranges[type].count(); i++) _ranges[type][i] = memory._ranges[type][i];
}

void Pic::Memory::copyFrom(const Device::Memory &memory)
{
  Q_ASSERT( memory.device().name()==device().name() );
  FOR_EACH(MemoryRangeType, i) copyFrom(i, static_cast<const Memory &>(memory));
}

Device::Array Pic::Memory::arrayForWriting(MemoryRangeType type) const
{
  Device::Array data = _ranges[type];
  for (uint i=0; i<data.count(); i++)
    data[i] = data[i].maskWith(type==MemoryRangeType::Config ? device().config()._words[i].wmask : device().mask(type));
  return data;
}

BitValue Pic::Memory::word(MemoryRangeType type, uint offset) const
{
  CRASH_ASSERT( offset<_ranges[type].size() );
  return _ranges[type][offset];
}

BitValue Pic::Memory::normalizeWord(MemoryRangeType type, uint offset, BitValue value) const
{
  if ( type==MemoryRangeType::Config) {
    const Config::Word &cword = device().config()._words[offset];
    return value.maskWith(cword.usedMask());
  }
  BitValue mask = device().range(type).rmask; // rmask is used in User Id
  if ( !value.isInside(mask) ) return mask;
  return value;
}

BitValue Pic::Memory::normalizedWord(MemoryRangeType type, uint offset) const
{
  return normalizeWord(type, offset, word(type, offset));
}

void Pic::Memory::setWord(MemoryRangeType type, uint offset, BitValue value)
{
  Q_ASSERT( _ranges[type].size()>offset );
  if ( _ranges[type].size()<=offset ) qDebug("Memory::setWord at %s %s: %s", type.label(), toHexLabelAbs(offset).latin1(), toHex(value, 4).latin1());
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
  BitValue v = word(MemoryRangeType::Config, i).maskWith(mask->value);
  for (uint k=0; k<uint(mask->values.count()); k++)
    if ( v.isInside(mask->values[k].value) ) return mask->values[k].name;
  Q_ASSERT(false);
  return QString::null;
}

AddressRange Pic::Memory::bootRange() const
{
  const Protection &protection = device().config().protection();
  // with boot size
  QString value = findValue(protection.bootSizeMaskName());
  if ( !value.isEmpty() ) {
    uint size = value.toUInt();
    if ( size==0 ) return AddressRange();
    Address start = device().range(MemoryRangeType::Code).start;
    if ( device().architecture()==Architecture::P30X ) start = 0x100;
    return AddressRange(start, 2 * size - 1); // instruction words
  }
  // only CPB
  QString maskName = protection.bootMaskName(Protection::ProgramProtected);
  const Config::Mask *mask = device().config().findMask(maskName);
  for (uint k=0; k<uint(mask->values.count()); k++) {
    AddressRangeVector rv = protection.extractRanges(mask->values[k].name, MemoryRangeType::Code);
    if ( !rv.isEmpty() ) return rv[0];
  }
  Q_ASSERT(false);
  return AddressRange();
}

AddressRange Pic::Memory::blockRange(uint i) const
{
  const Protection &protection = device().config().protection();
  Q_ASSERT( i<protection.nbBlocks() );
  if ( protection.family()==Protection::CodeGuard && i==1 ) { // general segment
    AddressRange previous = blockRange(0);
    if ( previous.isEmpty() ) previous = bootRange();
    Address start = (previous.isEmpty() ? device().range(MemoryRangeType::Code).start : previous.end + 1);
    return AddressRange(start, device().range(MemoryRangeType::Code).end);
  }
  QString maskName = protection.blockSizeMaskName(i);
  if ( protection.family()==Protection::CodeGuard ) { // secure segment
    QString value = findValue(maskName);
    Q_ASSERT( !value.isEmpty() );
    uint size = value.toUInt();
    if ( size==0 ) return AddressRange();
    AddressRange previous = bootRange();
    Address start = (previous.isEmpty() ? device().range(MemoryRangeType::Code).start : previous.end + 1);
    return AddressRange(start, 2 * size - 1);
  }
  AddressRange previous = (i==0 ? bootRange() : blockRange(i-1));
  const Config::Mask *mask = device().config().findMask(maskName);
  for (uint k=0; k<uint(mask->values.count()); k++) {
    AddressRangeVector rv = protection.extractRanges(mask->values[k].name, MemoryRangeType::Code);
    if ( !rv.isEmpty() ) return AddressRange(previous.end + 1, rv[0].end);
  }
  Q_ASSERT(false);
  return AddressRange();
}

AddressRange Pic::Memory::bootProtectedRange(Protection::Type ptype) const
{
  const Protection &protection = device().config().protection();
  QString maskName = protection.bootMaskName(ptype);
  QString value = findValue(maskName);
  if ( value.isEmpty() ) return AddressRange();
  if ( protection.family()!=Protection::CodeGuard ) {
    if ( protection.extractRanges(value, MemoryRangeType::Code).isEmpty() ) return AddressRange();
  } else {
    if ( value!=protection.securityValueName(ptype) ) return AddressRange();
  }
  return bootRange();
}

AddressRange Pic::Memory::blockProtectedRange(Protection::Type ptype, uint i) const
{
  const Protection &protection = device().config().protection();
  QString maskName = protection.blockMaskName(ptype, i);
  QString value = findValue(maskName);
  if ( value.isEmpty() ) return AddressRange();
  if ( protection.family()!=Protection::CodeGuard ) {
    if ( protection.extractRanges(value, MemoryRangeType::Code).isEmpty() ) return AddressRange();
  } else {
    if ( value!=protection.securityValueName(ptype) ) return AddressRange();
  }
  return blockRange(i);
}

AddressRangeVector Pic::Memory::protectedRanges(Protection::Type ptype, MemoryRangeType type) const
{
  const Protection &protection = device().config().protection();
  AddressRangeVector rv;
  if ( type==MemoryRangeType::Code ) {
    if ( protection.hasBootBlock() ) rv.append(bootProtectedRange(ptype));
    if ( protection.nbBlocks()!=0 ) {
      for (uint i=0; i<protection.nbBlocks(); i++) rv.append(blockProtectedRange(ptype, i));
      return rv;
    }
  }
  if ( protection.family()!=Protection::CodeGuard ) {
    QString maskName = protection.maskName(ptype, type);
    QString value = findValue(maskName);
    //qDebug("%s %s", maskName.latin1(), value.latin1());
    if ( !value.isEmpty() ) {
      AddressRangeVector tmp = protection.extractRanges(value, type);
      Q_ASSERT( tmp.count()==1 );
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
  if ( type==MemoryRangeType::Code ) {
    if ( protection.hasBootBlock() ) setBootProtection(on, ptype);
    if ( protection.nbBlocks()!=0 ) {
      for (uint i=0; i<protection.nbBlocks(); i++) setBlockProtection(on, ptype, i);
      return;
    }
  }
  setProtection(on, protection.maskName(ptype, type), ptype);
}

void Pic::Memory::setConfigValue(const QString &maskName, const QString &valueName)
{
  uint i;
  const Config::Mask *mask = device().config().findMask(maskName, &i);
  Q_ASSERT(mask);
  BitValue v = word(MemoryRangeType::Config, i);
  v = v.clearMaskBits(mask->value);
  for (int k=mask->values.count()-1; k>=0; k--) { // important to get the highest value in case of identical values
    if ( mask->values[k].name!=valueName ) continue;
    setWord(MemoryRangeType::Config, i, v | mask->values[k].value);
    return;
  }
  Q_ASSERT(false);
}

void Pic::Memory::setProtection(bool on, const QString &maskName, Protection::Type ptype)
{
  const Config::Mask *mask = device().config().findMask(maskName, 0);
  if( mask==0 ) return;
  const Protection &protection = device().config().protection();
  QString valueName;
  if ( ptype==Protection::StandardSecurity || ptype==Protection::HighSecurity )
    valueName = protection.securityValueName(ptype);
  else {
    for (int k=mask->values.count()-1; k>=0; k--) {
      if ( (on && protection.isAllProtectedValueName(mask->values[k].name))
           || (!on && protection.isNoneProtectedValueName(mask->values[k].name)) ) valueName = mask->values[k].name;
    }
  }
  setConfigValue(maskName, valueName);
}

bool Pic::Memory::hasFlagOn(const QString &maskName, bool valueIfNotPresent) const
{
  const Config::Mask *mask = device().config().findMask(maskName, 0);
  if ( mask==0 ) return valueIfNotPresent;
  Q_ASSERT(mask);
  Q_ASSERT( mask->values.count()==2 );
  return ( findValue(maskName)=="On" );
}

void Pic::Memory::setFlagOn(const QString &maskName, bool on)
{
  const Config::Mask *mask = device().config().findMask(maskName, 0);
  Q_ASSERT(mask);
  Q_ASSERT( mask->values.count()==2 );
  setConfigValue(maskName, on ? "On" : "Off");
}

void Pic::Memory::checksumCheckFill()
{
  clear();
  switch ( device().architecture() ) {
    case Architecture::P10X:
      setWord(MemoryRangeType::Code, device().range(MemoryRangeType::Code).start.toUInt(), 0x723);
      setWord(MemoryRangeType::Code, device().range(MemoryRangeType::Code).end.toUInt(), 0x723);
      break;
    case Architecture::P16X:
      if ( device().name()=="16F72" || device().name()=="16F73" || device().name()=="16F74" || device().name()=="16F76" || device().name()=="16F77"
           || device().name()=="16CR73" || device().name()=="16CR74" || device().name()=="16CR76" || device().name()=="16CR77" ) {
        setWord(MemoryRangeType::Code, device().range(MemoryRangeType::Code).start.toUInt(), 0x05E6);
        setWord(MemoryRangeType::Code, device().range(MemoryRangeType::Code).end.toUInt(), 0x05E6);
      } else {
        setWord(MemoryRangeType::Code, device().range(MemoryRangeType::Code).start.toUInt(), 0x25E6);
        setWord(MemoryRangeType::Code, device().range(MemoryRangeType::Code).end.toUInt(), 0x25E6);
      }
      break;
    case Architecture::P17C:
      setWord(MemoryRangeType::Code, device().range(MemoryRangeType::Code).start.toUInt(), 0xC0DE);
      setWord(MemoryRangeType::Code, device().range(MemoryRangeType::Code).end.toUInt(), 0xC0DE);
      break;
    case Architecture::P18C:
    case Architecture::P18F:
    case Architecture::P18J:
      setWord(MemoryRangeType::Code, device().range(MemoryRangeType::Code).start.toUInt()/2, 0xAAFF);
      setWord(MemoryRangeType::Code, device().range(MemoryRangeType::Code).end.toUInt()/2, 0xFFAA);
      break;
    case Architecture::P24J:
    case Architecture::P24H:
    case Architecture::P30X:
      setWord(MemoryRangeType::Code, device().range(MemoryRangeType::Code).start.toUInt(), 0xAAAAAA);
      setWord(MemoryRangeType::Code, device().range(MemoryRangeType::Code).end.toUInt(), 0xAAAAAA);
      break;
    case Architecture::Nb_Types: Q_ASSERT(false); break;
  }
}

BitValue Pic::Memory::checksum() const
{
  // code
  BitValue mask = device().mask(MemoryRangeType::Code);
  AddressRangeVector rv = protectedRanges(Protection::ProgramProtected, MemoryRangeType::Code);
  bool isProtected = !rv.isEmpty();
  uint inc = device().addressIncrement(MemoryRangeType::Code);
  //uint nbChars = device().nbCharsWord(MemoryRangeType::Code);
  //qDebug("protected: %i  nb: %s (%s)", isProtected, toHexLabelAbs(inc*device().nbWords(MemoryRangeType::Code)).latin1(), toHexLabel(mask, nbChars).latin1());
  //for (uint i=0; i<rv.count(); i++)
  //  qDebug("protected: %s:%s", toHex(rv[i].start, nbChars).latin1(), toHex(rv[i].end, nbChars).latin1());
  if ( isProtected && device().isJType() ) return 0x0000;
  Checksum::Algorithm algorithm = Checksum::Algorithm::Normal;
  BitValue cs = 0x0000;
  const Protection &protection = device().config().protection();
  if ( protection.family()==Protection::BasicProtection ) {
    QString maskName = protection.maskName(Protection::ProgramProtected, MemoryRangeType::Code);
    QString valueName = findValue(maskName);
    const QMap<QString, Checksum::Data> &checksums = device().checksums();
    if ( checksums.contains(valueName) ) { // #### REMOVE ME !!
      algorithm = checksums[valueName].algorithm;
      cs = checksums[valueName].constant;
    }
  }
  //qDebug("constant: %s", toHexLabelAbs(cs).data());
  //qDebug("algo: %s", Checksum::ALGORITHM_DATA[algorithm].name);
  for (uint i=0; i<device().nbWords(MemoryRangeType::Code); i++) {
    if ( algorithm==Checksum::Algorithm::Normal && rv.contains(inc*i) ) continue;
    BitValue v = word(MemoryRangeType::Code, i).maskWith(mask);
    //if ( i==0 || i==device().nbWords(MemoryRangeType::Code)-1 ) qDebug("%s %s", toHexLabel(i, 4).latin1(), toHexLabel(v, 4).latin1());
    switch ( device().architecture() ) {
      case Architecture::P10X:
      case Architecture::P16X:
      case Architecture::P17C:
        if ( rv.contains(i) ) {
          switch (algorithm) {
            case Checksum::Algorithm::Normal: cs += v;           break;
            case Checksum::Algorithm::XOR4:   cs += v.XORn(4);  break;
            case Checksum::Algorithm::XNOR7:  cs += v.XNORn(7); break;
            case Checksum::Algorithm::XNOR8:  cs += v.XNORn(8) + (v.XNORn(8) << 8); break;
            case Checksum::Algorithm::Nb_Types: Q_ASSERT(false); break;
          }
        } else cs += v;
        break;
      case Architecture::P18C:
      case Architecture::P18F: // #### not true for all 18F ??
      case Architecture::P18J:
        cs += v.byte(0) + v.byte(1);
        break;
      case Architecture::P24J:
      case Architecture::P24H:
      case Architecture::P30X:
        cs += v.byte(0) + v.byte(1) + v.byte(2);
        break;
      case Architecture::Nb_Types: Q_ASSERT(false); break;
    }
  }
  //qDebug("after code: %s", toHexLabelAbs(cs).latin1());
  // config
  const Config &config = device().config();
  for (uint i=0; i<uint(config._words.count()); i++) {
    const Config::Word &cword = config._words[i];
    BitValue v = word(MemoryRangeType::Config, i).maskWith(cword.cmask);
    //uint nbChars = device().nbCharsWord(MemoryRangeType::Config);
    // qDebug("%i: %s %s", i, toHex(word(MemoryRangeType::Config, i), nbChars).latin1(), toHex(cword.cmask, nbChars).latin1());
    if ( ( device().name()=="16C61" || device().name()=="16C71" ) && isProtected ) cs += v | 0x0060;
    else if ( device().is16bitFamily() ) cs += v.byte(0) + v.byte(1);
    else cs += v;
  }
  //qDebug("after config: %s", toHexLabelAbs(cs).latin1());
  // user ids
  if ( isProtected && device().isPresent(MemoryRangeType::UserId) && !device().is16bitFamily() && algorithm==Checksum::Algorithm::Normal ) {
    BitValue id = 0x0;
    uint nb = device().nbWords(MemoryRangeType::UserId);
    for (uint i=0; i<nb; i++) {
      BitValue v = word(MemoryRangeType::UserId, nb-i-1).maskWith(0xF);
      if ( device().is18Family() ) id += v;
      else {
        // qDebug("id %i (%i): %s %s", i, nbb, toHex(v, 4).latin1(), toHex(v << (nbb*i), 9).latin1());
        id += v << (4*i);
      }
    }
    //qDebug("id %s", toHexLabelAbs(id).latin1());
    cs += id;
  }
  //qDebug("checksum: %s %s", toHexLabelAbs(cs).latin1(), toHex(cs & 0xFFFF, 4).latin1());
  return cs.maskWith(0xFFFF);
}

BitValue Pic::Memory::unprotectedChecksum() const
{
  const Protection &protection = device().config().protection();
  Memory tmp(*this);
  if ( protection.hasBootBlock() ) tmp.setBootProtection(false, Protection::ProgramProtected);
  if ( protection.nbBlocks()!=0 ) {
    for (uint i=0; i<protection.nbBlocks(); i++)
      tmp.setBlockProtection(false, Protection::ProgramProtected, i);
  } else tmp.setProtection(false, Protection::ProgramProtected, MemoryRangeType::Code);
  tmp.setProtection(false, Protection::ProgramProtected, MemoryRangeType::Eeprom);
  tmp.setProtection(false, Protection::ProgramProtected, MemoryRangeType::Config);
  tmp.setProtection(false, Protection::ProgramProtected, MemoryRangeType::Cal);
  return tmp.checksum();
}

void Pic::Memory::setUserIdToUnprotectedChecksum()
{
  BitValue cs = unprotectedChecksum();
  uint nb = device().nbWords(MemoryRangeType::UserId);
  for (uint i=0; i<nb; i++) setWord(MemoryRangeType::UserId, nb-i-1, cs.nybble(i));
}

//-----------------------------------------------------------------------------
void Pic::Memory::savePartial(QTextStream &stream, HexBuffer::Format format) const
{
  // save memory ranges in the same order as MPLAB (for easy checking)
  const MemoryRangeType saveOrder[] = { MemoryRangeType::Code, MemoryRangeType::Eeprom, MemoryRangeType::Config, MemoryRangeType::UserId, MemoryRangeType::Cal, MemoryRangeType::Nb_Types };
  HexBuffer hb;
  for (uint i=0; saveOrder[i]!=MemoryRangeType::Nb_Types; i++) {
    hb.clear();
    toHexBuffer(saveOrder[i], hb);
    hb.savePartial(stream, format);
  }
}

//-----------------------------------------------------------------------------
void Pic::Memory::toHexBuffer(MemoryRangeType type, HexBuffer &hb) const
{
  if ( !device().isWritable(type) ) return;
  uint nbBytes = device().architecture().data()->nbBytesWord;
  bool packed = device().architecture().data()->packed;
  uint offset = device().range(type).hexFileOffset;
  if ( offset==0 ) offset = device().range(type).start.toUInt();
  BitValue mask = device().mask(type);
  uint wNbBytes = nbBytes;
  if ( packed && type!=Pic::MemoryRangeType::Code ) {
    offset /= 2;
    wNbBytes /= 2;
  }
  uint byte = 0;
  uint wOffset = 0;
  uint wByte = 0;
  //qDebug("%s wnb=%i snb=%i div=%i", MEMORY_RANGE_TYPE_DATA[type].label, wNbBytes, sNbBytes, div);
  for (uint k=0; k<wNbBytes*device().nbWords(type); k++) {
    // set byte
    BitValue s = _ranges[type][wOffset].maskWith(mask);
    //if ( k<4 ) qDebug("s=%s so=%s sb=%i wo=%i wb=%i", toHex(s, 8).data(), toHex(sOffset, 8).data(), sByte, wOffset, wByte);
    s = s.byte(wByte);
    if ( (byte%2)==0 ) hb.insert(offset, s);
    else hb.insert(offset, hb[offset] | (s << ((byte%2)*8)));
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
  FOR_EACH(MemoryRangeType, i) toHexBuffer(i, hb);
  return hb;
}

void Pic::Memory::fromHexBuffer(MemoryRangeType type, const HexBuffer &hb, WarningTypes &result,
                                QStringList &warnings, QMap<uint, bool> &inRange)
{
  if ( !device().isWritable(type) ) return;
  uint nbBytes = device().architecture().data()->nbBytesWord;
  bool packed = device().architecture().data()->packed;
  uint offset = device().range(type).hexFileOffset;
  if ( offset==0 ) offset = device().range(type).start.toUInt();
  BitValue mask = device().mask(type);
  uint wNbBytes = nbBytes;
  if ( packed && type!=Pic::MemoryRangeType::Code ) {
    offset /= 2;
    wNbBytes /= 2;
  }
  uint byte = 0;
  uint wOffset = 0;
  uint wByte = 0;
  //qDebug("%s wnb=%i snb=%i", MEMORY_RANGE_TYPE_DATA[type].label, wNbBytes, nbBytes);
  for (uint k=0; k<wNbBytes*device().nbWords(type); k++) {
    // set byte
    BitValue s = hb[offset];
    //if ( k<4 ) qDebug("s=%s so=%s sb=%i wo=%i wb=%i", toHex(s, 8).data(), toHex(offset, 8).data(), byte, wOffset, wByte);
    if ( !s.isInitialized() ) {
      if ( type==MemoryRangeType::Config ) _ranges[type][wOffset] = mask;
      else _ranges[type][wOffset] = BitValue();
    } else {
      inRange[offset] = true;
      s = s.byte(byte%2);
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
      if ( _ranges[type][wOffset].isInitialized() ) {
        if ( !(result & ValueTooLarge) && _ranges[type][wOffset].maskWith(mask)!=_ranges[type][wOffset] ) {
          result |= ValueTooLarge;
          warnings += i18n("At least one word (at offset %1) is larger (%2) than the corresponding mask (%3).")
                      .arg(toHexLabel(offset, 8)).arg(toHexLabel(_ranges[type][wOffset], 8)).arg(toHexLabel(mask, 8));
        }
        _ranges[type][wOffset] = _ranges[type][wOffset].maskWith(mask);
      }
      wByte = 0;
      wOffset++;
    }
  }
}

void Pic::Memory::fromHexBuffer(const HexBuffer &hb, WarningTypes &result,
                                QStringList &warnings, QMap<uint, bool> &inRange)
{
  FOR_EACH(MemoryRangeType, i) fromHexBuffer(i, hb, result, warnings, inRange);
}
