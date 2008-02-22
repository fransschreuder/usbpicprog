/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_register.h"

#include "pic.h"

//-----------------------------------------------------------------------------
Pic::RegistersData::RegistersData(const Data &data)
  : nbBanks(0), accessBankSplit(0), _data(data)
{}

Address Pic::RegistersData::mirroredAddress(Address address) const
{
  Address mirror = address;
  for (uint i=0; i<uint(mirrored.count()); i++) {
    int delta = -1;
    for (uint k=0; k<uint(mirrored[i].count()); k++) {
      if ( address<mirrored[i][k].start
           || address>=mirrored[i][k].start+mirrored[i][k].length ) continue;
      delta = address - mirrored[i][k].start;
      break;
    }
    if ( delta==-1 ) continue;
    for (uint k=0; k<uint(mirrored[i].count()); k++)
      mirror = QMIN(mirrored[i][k].start + delta, mirror);
    break;
  }
  return mirror;
}

Device::RegisterProperties Pic::RegistersData::properties(Address address) const
{
  switch ( type(address) ) {
    case Mirrored:
    case UnusedRegister: return Device::NotAccessible;
    case Gpr: return (Device::Readable | Device::Writable);
    case Sfr: {
      Device::RegisterProperties properties = Device::NotAccessible;
      RegisterBitProperties rbp = RegisterBitUnused;
      QMap<QString, RegisterData>::const_iterator it;
      for (it=sfrs.begin(); it!=sfrs.end(); ++it) {
        if ( it.data().address!=address ) continue;
        for (uint i=0; i<Device::MAX_NB_PORT_BITS; i++) rbp |= it.data().bits[i].properties;
        if ( rbp & RegisterBitRead ) properties |= Device::Readable;
        if ( rbp & RegisterBitWrite ) properties |= Device::Writable;
        break;
      }
      return properties;
    }
  }
  Q_ASSERT(false);
  return Device::NotAccessible;
}

Pic::RegisterType Pic::RegistersData::type(Address address) const
{
  for (uint i=0; i<uint(unused.count()); i++)
    if ( address>=unused[i].start && address<unused[i].start+unused[i].length ) return UnusedRegister;
  if ( !sfrNames[address].isEmpty() ) return Sfr;
  Address mirror = mirroredAddress(address);
  if ( !sfrNames[mirror].isEmpty() ) return Mirrored;
  if ( address==mirror ) return Gpr;
  return Mirrored;
}

QString Pic::RegistersData::label(Address address) const
{
  switch ( type(address) ) {
    case UnusedRegister: return "---";
    case Mirrored: return i18n("Mirror of %1").arg(toHexLabel(mirroredAddress(address), nbCharsAddress()));
    case Gpr: return "<GPR>";
    case Sfr: return sfrNames[address];
  }
  Q_ASSERT(false);
  return QString::null;
}

bool Pic::RegistersData::hasPort(uint index) const
{
  Q_ASSERT( index<Device::MAX_NB_PORTS );
  if ( sfrs.contains("GPIO") ) return ( index==0 );
  if ( !sfrs.contains(portName(index)) ) return false;
  return true;
}

int Pic::RegistersData::portIndex(Address address) const
{
  QString name = sfrNames[address];
  if ( name.isEmpty() ) return -1;
  for (uint i=0; i<Device::MAX_NB_PORTS; i++) {
    if ( !hasPort(i) ) continue;
    if ( name==portName(i) || name==trisName(i) || name==latchName(i) ) return i;
  }
  return -1;
}

bool Pic::RegistersData::hasPortBit(uint index, uint bit) const
{
  if ( !hasPort(index) ) return false;
  const RegisterData &port = sfrs[portName(index)];
  return ( port.bits[bit].properties!=RegisterBitUnused );
}

QString Pic::RegistersData::portName(uint index) const
{
  if ( sfrs.contains("GPIO") ) {
    if ( index!=0 ) return QString::null;
    return "GPIO";
  }
  return QString("PORT") + char('A' + index);
}

QString Pic::RegistersData::trisName(uint index) const
{
  if ( sfrs.contains("GPIO") ) {
    if ( index!=0 ) return QString::null;
    return "TRISIO";
  }
  if ( _data.architecture()==Architecture::P17C ) {
    if ( index==0 ) return QString::null;
    return QString("DDR") + char('A' + index);
  }
  return QString("TRIS") + char('A' + index);
}

bool Pic::RegistersData::hasTris(uint index) const
{
  QString name = trisName(index);
  if ( name.isEmpty() ) return false;
  return sfrs.contains(name);
}

QString Pic::RegistersData::latchName(uint index) const
{
  if ( _data.architecture()==Architecture::P10X || _data.architecture()==Architecture::P16X || _data.architecture()==Architecture::P17C )
    return QString::null;
  return QString("LAT") + char('A' + index);
}

bool Pic::RegistersData::hasLatch(uint index) const
{
  QString name = latchName(index);
  if ( name.isEmpty() ) return false;
  return sfrs.contains(name);
}

QString Pic::RegistersData::portBitName(uint index, uint bit) const
{
  if ( sfrs.contains("GPIO") ) return QString("GP") + QString::number(bit);
  return QString("R") + char('A' + index) + QString::number(bit);
}

QValueList<Register::TypeData> Pic::RegistersData::relatedRegisters(const Register::TypeData &data) const
{
  QValueList<Register::TypeData> list;
  if ( data.type()==Register::Regular ) {
    int i = portIndex(data.address());
    if ( i==-1 ) list.append(data);
    else {
      list.append(Register::TypeData(sfrs[portName(i)].address, nbChars()));
      if ( hasTris(i) ) list.append(Register::TypeData(sfrs[trisName(i)].address, nbChars()));
      if ( hasLatch(i) ) list.append(Register::TypeData(sfrs[latchName(i)].address, nbChars()));
    }
  } else if ( data.type()==Register::Combined ) {
    uint nb = nbBitsToNbBytes(4*data.nbChars()) / nbBytes();
    for (uint i=0; i<nb; i++) list.append(Register::TypeData(data.address() + i*nbBytes(), nbChars()));
  } else list.append(data);
  return list;
}

bool Pic::RegistersData::isBankUsed(uint i) const
{
  Q_ASSERT( i<nbBanks );
  return !(unusedBankMask & (1 << i));
}

bool Pic::RegistersData::bankHasSfrs(uint i) const
{
  if ( i==0 ) return true;
  if ( (_data.architecture()==Pic::Architecture::P18F || _data.architecture()==Pic::Architecture::P18J) && i==15 ) return true;
  if ( !isBankUsed(i) ) return false;
  QMap<Address, QString>::const_iterator it;
  for (it=sfrNames.begin(); it!=sfrNames.end(); ++it)
    if ( bankFromAddress(it.key())==i ) return true;
  return false;
}

bool Pic::RegistersData::hasSharedGprs(uint &firstIndex, bool &all) const
{
  bool ok = false;
  all = true;
  for (uint i=0; i<nbRegistersPerBank(); i++) {
    Address address = addressFromIndex(i);
    if ( type(address)!=Gpr ) continue;
    uint k = 1;
    for (; k<nbBanks; k++) {
      RegisterType t = type(address + k*nbBytesPerBank());
      if ( t!=Mirrored ) break;
    }
    if ( k==nbBanks ) {
      if ( !ok ) firstIndex = i;
      ok = true;
    } else all = false;
  }
  return ok;
}

uint Pic::RegistersData::firstGprIndex() const
{
  for (uint i=0; i<nbRegistersPerBank(); i++)
    if ( type(addressFromIndex(i))==Gpr ) return i;
  Q_ASSERT(false);
  return 0;
}

//----------------------------------------------------------------------------
QDataStream &Pic::operator <<(QDataStream &s, const RangeData &rd)
{
  s << rd.start << rd.length;
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, RangeData &rd)
{
  s >> rd.start >> rd.length;
  return s;
}

QDataStream &Pic::operator <<(QDataStream &s, const RegisterBitData &rbd)
{
  s << Q_UINT8(rbd.properties) << Q_UINT8(rbd.por) << Q_UINT8(rbd.mclr);
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, RegisterBitData &rbd)
{
  Q_UINT8 properties, por, mclr;
  s >> properties >> por >> mclr;
  rbd.properties = RegisterBitProperties(properties);
  rbd.por = RegisterBitState(por);
  rbd.mclr = RegisterBitState(mclr);
  return s;
}

QDataStream &Pic::operator <<(QDataStream &s, const RegisterData &rd)
{
  s << rd.address;
  for (int i=0; i<Device::MAX_NB_PORT_BITS; i++) s << rd.bits[i];
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, RegisterData &rd)
{
  s >> rd.address;
  for (int i=0; i<Device::MAX_NB_PORT_BITS; i++) s >> rd.bits[i];
  return s;
}

QDataStream &Pic::operator <<(QDataStream &s, const CombinedData &rd)
{
  s << rd.address << rd.nbChars;
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, CombinedData &rd)
{
  s >> rd.address >> rd.nbChars;
  return s;
}

QDataStream &Pic::operator <<(QDataStream &s, const RegistersData &rd)
{
  s << rd.nbBanks << rd.accessBankSplit << rd.unusedBankMask;
  s << rd.sfrs << rd.mirrored << rd.unused << rd.combined;
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, RegistersData &rd)
{
  s >> rd.nbBanks >> rd.accessBankSplit >> rd.unusedBankMask;
  s >> rd.sfrs >> rd.mirrored >> rd.unused >> rd.combined;
  rd.sfrNames.clear();
  QMap<QString, RegisterData>::const_iterator it;
  for(it=rd.sfrs.begin(); it!=rd.sfrs.end(); ++it) rd.sfrNames[it.data().address] = it.key();
  return s;
}
