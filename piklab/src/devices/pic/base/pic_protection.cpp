/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_protection.h"

#include "pic_config.h"

#include <qregexp.h>

bool Pic::Protection::isNoneProtectedValueName(const QString &name) const
{
  if ( name=="Off" ) return true;
  if ( _data.architecture()==Architecture::P17C ) return !isAllProtectedValueName(name);
  return false;
}

bool Pic::Protection::isAllProtectedValueName(const QString &name) const
{
  if ( name=="All" ) return true;
  if ( _data.architecture()==Architecture::P17C ) return ( name=="Code-protected microcontroller" );
  return false;
}

Pic::Protection::Family Pic::Protection::family() const
{
  if ( _config.findMask("WRTBS") ) return CodeGuard;
  QString mask = maskName(ProgramProtected, MemoryRangeType::Code);
  if ( _config.findMask(QString("%1_%2").arg(mask).arg(0)) ) return BlockProtection;
  if ( _config.findMask(mask) ) return BasicProtection;
  return NoProtection;
}

QString Pic::Protection::securityValueName(Type type) const
{
  if ( type==StandardSecurity ) return "Standard Security";
  if ( type==HighSecurity ) return "High Security";
  Q_ASSERT( type==Nb_Types );
  return "Off";
}

QString Pic::Protection::bootSizeMaskName() const
{
  return (family()==CodeGuard ? "BSSIZ" : "BBSIZ");
}

QString Pic::Protection::bootMaskName(Type type) const
{
  Q_ASSERT( type!=Nb_Types );
  if ( family()==CodeGuard ) {
    if ( type==WriteProtected ) return "WRTBS";
    if ( type==StandardSecurity || type==HighSecurity ) return "BSSEC";
  } else {
    if ( type==ProgramProtected ) return "CPB";
    if ( type==WriteProtected ) return "WRTB";
    if ( type==ReadProtected ) return "EBTRB";
  }
  return QString::null;
}

QString Pic::Protection::blockSizeMaskName(uint block) const
{
  if ( family()==CodeGuard ) {
    Q_ASSERT( block==0 );
    return "SSSIZ";
  }
  return blockMaskName(ProgramProtected, block);
}

QString Pic::Protection::blockMaskName(Type type, uint block) const
{
  Q_ASSERT( type!=Nb_Types );
  if ( family()==CodeGuard ) {
    if ( type==WriteProtected ) return (block==0 ? "WRTSS" : "WRTGS");
    if ( type==StandardSecurity || type==HighSecurity ) return (block==0 ? "SSSEC" : "GSSEC");
    return QString::null;
  }
  return QString("%1_%2").arg(maskName(type, MemoryRangeType::Code)).arg(block);
}

QString Pic::Protection::maskName(Type type, MemoryRangeType mtype) const
{
  Q_ASSERT( type!=Nb_Types );
  switch (mtype) {
    case MemoryRangeType::Code:
      if ( type==ProgramProtected ) {
        if ( _data.architecture()==Architecture::P17C ) return "PM";
        if ( _data.architecture()==Architecture::P30X ) return "GCP";
        return "CP";
      }
      if ( type==WriteProtected ) {
        if ( _data.architecture()==Architecture::P30X ) return "GWRP";
        return "WRT";
      }
      if ( type==ReadProtected ) return "EBTR";
      break;
    case MemoryRangeType::Eeprom:
      if ( type==ProgramProtected ) return "CPD";
      if ( type==WriteProtected ) return "WRTD";
      break;
    case MemoryRangeType::Cal:
      if ( type==ProgramProtected ) return "CPC";
      break;
    case MemoryRangeType::Config:
      if ( type==WriteProtected ) return "WRTC";
      if ( type==ReadProtected ) return "EBTRC";
      break;
    case MemoryRangeType::Nb_Types: Q_ASSERT(false); break;
    default: break;
  }
  return QString::null;
}

bool Pic::Protection::extractRanges(const QString &name, QValueVector<Address> &starts, Address &end, bool &ok)
{
  ok = false;
  QRegExp regexp("([A-F0-9]+)(/[A-F0-9]+)?(/[A-F0-9]+)?:([A-F0-9]+)");
  if ( !regexp.exactMatch(name) ) return false;
  bool ok1;
  end = fromHex(regexp.cap(regexp.numCaptures()), &ok1);
  if ( !ok1 ) {
    qDebug("Malformed end address");
    return true;
  }
  starts.clear();
  for (int i=1; i<regexp.numCaptures(); i++) {
    if ( regexp.cap(i).isEmpty() ) break;
    bool ok1;
    QString s = (i==1 ? regexp.cap(i) : regexp.cap(i).mid(1));
    Address start = fromHex(s, &ok1);
    if ( !ok1 ) {
      qDebug("Malformed start address %s", s.latin1());
      return true;
    }
    if ( start>=end && (starts.count()==0 || starts[starts.count()-1]<start) ) {
      qDebug("Start addresses should be ordered");
      return true;
    }
    starts.append(start);
  }
  ok = true;
  return true;
}

AddressRangeVector Pic::Protection::extractRanges(const QString &name, MemoryRangeType type) const
{
  if ( isNoneProtectedValueName(name) ) return AddressRange();
  if ( isAllProtectedValueName(name) ) {
    const MemoryRangeData &rdata = _data.range(type);
    return AddressRange(rdata.start, rdata.end);
  }
  bool ok1;
  QValueVector<Address> starts;
  Address end;
  bool ok2 = extractRanges(name, starts, end, ok1);
  Q_ASSERT(ok1);
  Q_ASSERT(ok2);
  Q_UNUSED(ok2);
  AddressRangeVector rv;
  for (uint i=0; i<uint(starts.count()); i++) rv.append(AddressRange(starts[i], end));
  return rv;
}

bool Pic::Protection::checkRange(const QString &mask, const QString &name) const
{
  if ( family()!=CodeGuard ) {
    bool ok;
    (void)extractRange(mask, name, ok);
    return ok;
  }

  bool isBootBlock = false;
  int block = 0;
  Type ptype = Nb_Types;
  for (uint i=0; i<3; i++) {
    isBootBlock = ( i==0 );
    block = i - 1;
    for (uint k=0; k<Nb_Types; k++) {
      QString mname = (isBootBlock ? bootMaskName(Type(k)) : blockMaskName(Type(k), block));
      if ( mask!=mname ) continue;
      ptype = Type(k);
      break;
    }
    if ( ptype!=Nb_Types ) break;
  }
  if ( ptype==Nb_Types ) {
    qDebug("Unknown protected memory range");
    return false;
  }
  // #### TODO
  return true;
}

Pic::Protection::ProtectedRange Pic::Protection::extractRange(const QString &mask, const QString &name, bool &ok) const
{
  Q_ASSERT( family()!=CodeGuard );
  //qDebug("extract range %s %s", mask.latin1(), name.latin1());
  ProtectedRange pr;
  ok = false;

  QRegExp rexp("([A-Z]+)(?:_([0-9])|)");
  if ( !rexp.exactMatch(mask) ) {
    qDebug("Malformed block range");
    return pr;
  }

  bool isBootBlock = false;
  MemoryRangeType rtype = MemoryRangeType::Nb_Types;
  Type ptype = Nb_Types;

  for (MemoryRangeType i; i<=MemoryRangeType::Nb_Types; ++i) {
    isBootBlock = ( i==MemoryRangeType::Nb_Types );
    for (uint k=0; k<Nb_Types; k++) {
      QString mname = (isBootBlock ? bootMaskName(Type(k)) : maskName(Type(k), i));
      if ( rexp.cap(1)!=mname ) continue;
      rtype = (isBootBlock ? MemoryRangeType(MemoryRangeType::Code) : i);
      ptype = Type(k);
      if ( !rexp.cap(2).isEmpty() ) {
        if ( isBootBlock || (rtype!=MemoryRangeType::Code && rtype!=MemoryRangeType::Eeprom) ) {
          qDebug("Multiple blocks only for code and eeprom");
          return pr;
        }
      }
      break;
    }
    if ( rtype!=MemoryRangeType::Nb_Types ) break;
  }
  if ( rtype==MemoryRangeType::Nb_Types ) {
    qDebug("Unknown protected memory range");
    return pr;
  }

  if ( isNoneProtectedValueName(name) ) {
    ok = true;
    return pr;
  }

  const Config::Mask *bmask = _config.findMask(bootMaskName(ptype));
  const Config::Mask *bsmask = _config.findMask(bootSizeMaskName());
  const MemoryRangeData &rdata = _data.range(rtype);
  if ( isAllProtectedValueName(name) ) {
    if ( rtype==MemoryRangeType::Code && !isBootBlock && bmask ) {
      qDebug("Protected range should be explicit with boot block");
      return pr;
    }
    if (isBootBlock) {
      if ( bsmask==0 ) {
        qDebug("Protected range should be explicit when boot size not present");
        return pr;
      }
      Address start = _data.range(MemoryRangeType::Code).start;
      pr.starts.append(start);
      for (uint k=0; k<uint(bsmask->values.count()); k++) {
        bool ok1;
        uint size = bsmask->values[k].name.toUInt(&ok1);
        if ( !ok1 ) {
          qDebug("Could not recognize boot size value");
          return pr;
        }
        if ( size==0 ) {
          qDebug("Boot size cannot be zero");
          return pr;
        }
        Address end = 2 * size - 1; // instruction words
        if ( pr.ends.count()!=0 && end==pr.ends[pr.ends.count()-1] ) continue;
        pr.ends.append(end);
        qHeapSort(pr.ends);
      }
    } else {
      pr.starts.append(rdata.start);
      pr.ends.append(rdata.end);
    }
    ok = true;
    return pr;
  }
  if ( isBootBlock && bsmask ) {
    qDebug("Protected range should not be explicit when boot size is present");
    return pr;
  }

  // extract start and end
  Address end;
  bool ok1;
  if ( !extractRanges(name, pr.starts, end, ok1) ) {
    qDebug("Could not recognized explicit range");
    return pr;
  }
  if ( !ok1 ) return pr;
  if ( end>rdata.end ) {
    qDebug("End is beyond memory range");
    return pr;
  }
  if ( (rtype!=MemoryRangeType::Code || isBootBlock) && (pr.starts.count()>1 || !rexp.cap(2).isEmpty() || bmask==0)  ) {
    qDebug("Only code with blocks and boot can have multiple protected ranges");
    return pr;
  }
  if ( isBootBlock && pr.starts[0]!=0 ) {
    qDebug("Boot block start should be zero");
    return pr;
  }
  pr.ends.append(end);

  // check with boot block
  if ( pr.starts.count()>1 ) {
    if ( bmask==0 ) {
      qDebug("No boot mask");
      return pr;
    }
    for (uint i=0; i<uint(bmask->values.count()); i++) {
      if ( bmask->values[i].name=="Off" ) continue;
      bool ok1;
      ProtectedRange bpr = extractRange(bmask->name, bmask->values[i].name, ok1);
      if ( !ok1 ) return pr;
      if ( bpr.ends.count()!=pr.starts.count() ) {
        qDebug("Boot number of ends (%i) should be the same as code number of starts (%i)", int(bpr.ends.count()), int(pr.starts.count()));
        return pr;
      }
      for (uint k=0; k<uint(bpr.ends.count()); k++) {
        if ( bpr.ends[k]+1!=pr.starts[k] ) {
          qDebug("%i: End of boot block (%s) doesn't match start of code block (%s)", k, toHexLabelAbs(bpr.ends[k]).latin1(), toHexLabelAbs(pr.starts[k]).latin1());
          return pr;
        }
      }
    }
  }

  ok = true;
  return pr;
}

bool Pic::Protection::hasBootBlock() const
{
  return ( _config.findMask("CPB") || _config.findMask(bootSizeMaskName()) );
}

uint Pic::Protection::nbBlocks() const
{
  if ( family()==CodeGuard ) return 2; // codeguard : secure segment + general segment
  for (uint i=0; i<MAX_NB_BLOCKS; i++)
    if ( _config.findMask(QString("CP_%1").arg(i))==0 ) return i;
  return MAX_NB_BLOCKS;
}

QString Pic::Protection::bootLabel() const
{
  if ( family()==CodeGuard ) return i18n("Boot Segment");
  return i18n("Boot Block");
}

QString Pic::Protection::blockLabel(uint i) const
{
  if ( family()==CodeGuard ) {
    if ( i==0 ) return i18n("Secure Segment");
    return i18n("General Segment");
  }
  return i18n("Block #%1").arg(i);
}
