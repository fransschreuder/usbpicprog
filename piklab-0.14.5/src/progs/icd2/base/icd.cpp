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
  uint wordOffset = 0;
  uint nbWords = device().nbWords(type);
  if (vdata) {
    bool only = (vdata ? bool(vdata->actions & ::Programmer::OnlyProgrammedVerify) : false);
    const Device::Array tmp = static_cast<const Pic::Memory &>(vdata->memory).arrayForWriting(type);
    prepareRange(type, tmp, !only, wordOffset, nbWords);
  }
  return hardware().readMemory(type, wordOffset, nbWords, data, vdata);
}

void Icd::DeviceSpecific::prepareRange(Pic::MemoryRangeType type, const Device::Array &data,
                                       bool force, uint &wordOffset, uint &nbWords)
{
  wordOffset = 0;
  nbWords = device().nbWords(type);
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
}

bool Icd::DeviceSpecific::doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force)
{
  uint wordOffset, nbWords;
  prepareRange(type, data, force, wordOffset, nbWords);
  return hardware().writeMemory(type, data, wordOffset, nbWords);
}

bool Icd::DeviceSpecific::doErase(bool)
{
  Device::Array data(device().nbWords(Pic::MemoryConfig));
  for (uint i=0; i<data.size(); i++) data[i] = device().config()._words[i].wmask;
  if ( !doWrite(Pic::MemoryConfig, data, true) ) return false;
  return hardware().eraseAll();
}
