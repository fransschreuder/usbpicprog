/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_prog_specific.h"

#include "common/global/global.h"

//-----------------------------------------------------------------------------
const char * const Pic::VOLTAGE_TYPE_LABELS[Nb_VoltageTypes] = {
  I18N_NOOP("Programmer Vpp"), I18N_NOOP("Target Vdd"), I18N_NOOP("Target Vpp")
};

const char * const Pic::TARGET_MODE_LABELS[Nb_TargetModes] = {
  I18N_NOOP("Stopped"), I18N_NOOP("Running"), I18N_NOOP("In Programming")
};

const char * const Pic::RESET_MODE_LABELS[Nb_ResetModes] = {
  I18N_NOOP("Reset Held"), I18N_NOOP("Reset Released")
};

//-----------------------------------------------------------------------------
uint Programmer::PicDeviceSpecific::findNonMaskStart(Pic::MemoryRangeType type, const Device::Array &data) const
{
  uint start = 0;
  for (; start<data.count(); start++)
    if ( data[start]!=device().mask(type) ) break;
  const_cast<PicDeviceSpecific *>(this)->log(Log::DebugLevel::Normal, QString("start before align: %1").arg(start));
  uint align = device().nbWordsWriteAlignment(type);
  start -= start % align;
  const_cast<PicDeviceSpecific *>(this)->log(Log::DebugLevel::Normal, QString("start after align: %1 (align=%2)").arg(start).arg(align));
  return start;
}

uint Programmer::PicDeviceSpecific::findNonMaskEnd(Pic::MemoryRangeType type, const Device::Array &data) const
{
  uint end = data.count()-1;
  for (; end>0; end--)
    if ( data[end]!=device().mask(type) ) break;
  const_cast<PicDeviceSpecific *>(this)->log(Log::DebugLevel::Normal, QString("end before align: %1").arg(end));
  uint align = device().nbWordsWriteAlignment(type);
  if ( (end+1) % align ) end += align - (end+1) % align;
  const_cast<PicDeviceSpecific *>(this)->log(Log::DebugLevel::Normal, QString("end after align: %1 (align=%2)").arg(end).arg(align));
  Q_ASSERT(end<data.count());
  return end;
}

bool Programmer::PicDeviceSpecific::read(Pic::MemoryRangeType type, Device::Array &data, const VerifyData *vdata)
{
  setPowerOn();
  bool ok = doRead(type, data, vdata);
  setPowerOff();
  return ok;
}

bool Programmer::PicDeviceSpecific::write(Pic::MemoryRangeType mtype, const Device::Array &data, bool force)
{
  setPowerOn();
  bool ok = doWrite(mtype, data, force);
  setPowerOff();
  return ok;
}

bool Programmer::PicDeviceSpecific::erase(bool isProtected)
{
  setPowerOn();
  bool ok = doErase(isProtected);
  setPowerOff();
  return ok;
}

bool Programmer::PicDeviceSpecific::eraseRange(Pic::MemoryRangeType type)
{
  setPowerOn();
  bool ok = doEraseRange(type);
  setPowerOff();
  return ok;
}

bool Programmer::PicDeviceSpecific::doEmulatedEraseRange(Pic::MemoryRangeType type)
{
  Pic::Memory memory(device());
  if ( !doWrite(type, memory.arrayForWriting(type), true) ) return false;
  if ( !canReadRange(type) ) return true;
  VerifyData vdata(BlankCheckVerify, memory);
  Device::Array data;
  return doRead(type, data, &vdata);
}

//-----------------------------------------------------------------------------
bool Programmer::PicHardware::compareWords(Pic::MemoryRangeType type, uint index, BitValue v, BitValue d, Programmer::VerifyActions actions)
{
  if ( v==d ) return true;
  uint inc = device().addressIncrement(type);
  Address address = device().range(type).start + inc * index;
  if ( actions & ::Programmer::BlankCheckVerify )
    log(Log::LineType::SoftError, i18n("Device memory is not blank (in %1 at address %2: reading %3 and expecting %4).")
      .arg(i18n(type.label())).arg(toHexLabel(address, device().nbCharsAddress()))
      .arg(toHexLabel(d, device().nbCharsWord(type))).arg(toHexLabel(v, device().nbCharsWord(type))));
  else log(Log::LineType::SoftError, i18n("Device memory does not match hex file (in %1 at address %2: reading %3 and expecting %4).")
      .arg(i18n(type.label())).arg(toHexLabel(address, device().nbCharsAddress()))
      .arg(toHexLabel(d, device().nbCharsWord(type))).arg(toHexLabel(v, device().nbCharsWord(type))));
  return false;
}

bool Programmer::PicHardware::verifyWord(uint i, BitValue word, Pic::MemoryRangeType type, const VerifyData &vdata)
{
  if ( !(vdata.actions & ::Programmer::IgnoreProtectedVerify) && vdata.protectedRanges.contains(i) ) return true; // protected
  BitValue v = static_cast<const Pic::Memory &>(vdata.memory).normalizedWord(type, i);
  BitValue d = static_cast<const Pic::Memory &>(vdata.memory).normalizeWord(type, i, word);
  if ( type==Pic::MemoryRangeType::Config ) {
    BitValue pmask = device().config()._words[i].pmask;
    v = v.clearMaskBits(pmask);
    d = d.clearMaskBits(pmask);
  }
  return compareWords(type, i, v, d, vdata.actions);
}
