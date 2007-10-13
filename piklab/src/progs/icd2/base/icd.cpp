/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "icd.h"

#include "common/global/global.h"
#include "common/common/misc.h"
#include "common/port/port_base.h"

//-----------------------------------------------------------------------------
bool Icd::DeviceSpecific::doRead(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata)
{
  data.resize(device().nbWords(type));
  if ( vdata==0 ) return hardware().readMemory(type, 0, data, vdata);
  bool only = vdata->actions & ::Programmer::OnlyProgrammedVerify;
  const Device::Array tmp = static_cast<const Pic::Memory &>(vdata->memory).arrayForWriting(type);
  uint wordOffset;
  Device::Array pdata = prepareRange(type, tmp, !only, wordOffset);
  return hardware().readMemory(type, wordOffset, pdata, vdata);
}

Device::Array Icd::DeviceSpecific::prepareRange(Pic::MemoryRangeType type, const Device::Array &data,
                                                bool force, uint &wordOffset)
{
  wordOffset = 0;
  uint nbWords = data.count();
  if ( !force ) {
    wordOffset = findNonMaskStart(type, data);
    nbWords = 0;
    if ( wordOffset!=data.count() ) {
      uint end = findNonMaskEnd(type, data);
      nbWords = end - wordOffset + 1;
      log(Log::NormalDebug, QString("    start=%1 nbWords=%2 total=%3 force=%4")
        .arg(toHexLabel(wordOffset, device().nbCharsAddress())).arg(toHexLabel(nbWords, device().nbCharsAddress()))
        .arg(toHexLabel(data.count(), device().nbCharsAddress())).arg(force ? "true" : "false"));
    }
    addProgress(data.count()-nbWords);
  }
  return data.mid(wordOffset, nbWords);
}

bool Icd::DeviceSpecific::doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force)
{
  Q_ASSERT( data.count()==device().nbWords(type) );

  uint nb = device().nbWordsWriteAlignment(Pic::MemoryRangeType::Code);
  if ( device().architecture()==Pic::Architecture::P18J && type==Pic::MemoryRangeType::Config ) {
    Q_ASSERT( data.count()%2==0 );
    int delta = nb - data.count()/2; // config memory words contains 1 byte
    Q_ASSERT( delta>=0 );
    Device::Array rdata(delta);
    uint wordOffset = device().nbWords(Pic::MemoryRangeType::Code) - delta;
    if ( !hardware().readMemory(Pic::MemoryRangeType::Code, wordOffset, rdata, 0) ) return false;
    Device::Array pdata(nb);
    for (uint i=0; i<uint(delta); i++) pdata[i] = rdata[i];
    for (uint i=delta; i<nb; i++) {
      pdata[i] = data[2*(i-delta)];
      pdata[i] |= data[2*(i-delta)+1] << 8;
    }
    return hardware().writeMemory(Pic::MemoryRangeType::Code, wordOffset, pdata);
  }

  uint wordOffset;
  Device::Array pdata = prepareRange(type, data, force, wordOffset);
  if ( device().architecture()==Pic::Architecture::P18J && type==Pic::MemoryRangeType::Code ) {
    uint end = wordOffset + pdata.size();
    if ( end>=device().nbWords(Pic::MemoryRangeType::Code) ) {
      Device::Array rdata(device().nbWords(Pic::MemoryRangeType::Config));
      if ( !hardware().readMemory(Pic::MemoryRangeType::Code, device().nbWords(Pic::MemoryRangeType::Code), rdata, 0) ) return false;
      uint n = rdata.count() / 2;
      for (uint i=0; i<n; i++) {
        pdata[pdata.size() - n + i] = rdata[2*i];
        pdata[pdata.size() - n + i] |= rdata[2*i+1] << 8;
      }
    }
  }
  return hardware().writeMemory(type, wordOffset, pdata);
}

bool Icd::DeviceSpecific::doErase(bool)
{
  if ( device().architecture()==Pic::Architecture::P18J ) { // ### also true for others ?
    Device::Array data(device().nbWords(Pic::MemoryRangeType::Config));
    for (uint i=0; i<data.size(); i++) data[i] = device().config()._words[i].wmask;
    if ( !doWrite(Pic::MemoryRangeType::Config, data, true) ) return false;
  }
  return hardware().eraseAll();
}
