/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "mem24_prog.h"

#include "common/global/global.h"
#include "devices/list/device_list.h"
#include "progs/base/prog_config.h"

//-----------------------------------------------------------------------------
bool Programmer::Mem24DeviceSpecific::read(Device::Array &data, const VerifyData *vdata)
{
  setPowerOn();
  doRead(data, vdata);
  setPowerOff();
  return !hasError();
}

bool Programmer::Mem24DeviceSpecific::write(const Device::Array &data)
{
  setPowerOn();
  doWrite(data);
  setPowerOff();
  return !hasError();
}

bool Programmer::Mem24DeviceSpecific::verifyByte(uint index, uint d, const VerifyData &vdata)
{
  uint v = static_cast<const Mem24::Memory &>(vdata.memory).byte(index) & 0xFF;
  if ( v==d ) return true;
  uint address = index;
  if ( vdata.actions & BlankCheckVerify )
    log(Log::Error, i18n("Device memory is not blank (at address %1: reading %2 and expecting %3).")
                    .arg(toHexLabel(address, device().nbCharsAddress())).arg(toHexLabel(d, 2)).arg(toHexLabel(v, 2)));
  else log(Log::Error, i18n("Device memory doesn't match hex file (at address %1: reading %2 and expecting %3).")
                       .arg(toHexLabel(address, device().nbCharsAddress())).arg(toHexLabel(d, 2)).arg(toHexLabel(v, 2)));
  return false;
}

//----------------------------------------------------------------------------
uint Programmer::Mem24Base::nbSteps(Operation operation) const
{
  uint length = Base::nbSteps(operation);
  if ( operation==Connect ) return length;
  if ( operation==Write ) length += device()->nbBytes();
  length += device()->nbBytes();
  return length;
}

//----------------------------------------------------------------------------
bool Programmer::Mem24Base::internalErase(const Device::MemoryRange &)
{
  initProgramming();
  addProgress(OPERATION_DATA[Erase].nbSteps);
  Mem24::Memory memory(*device());
  return specific()->write(memory.arrayForWriting());
}

bool Programmer::Mem24Base::internalRead(Device::Memory *memory, const Device::MemoryRange &, const VerifyData *vdata)
{
  initProgramming();
  Device::Array data;
  if ( !specific()->read(data, vdata) ) return false;
  if (memory) for (uint i=0; i<data.count(); i++) static_cast<Mem24::Memory *>(memory)->setByte(i, data[i]);
  return true;
}

bool Programmer::Mem24Base::internalProgram(const Device::Memory &memory, const Device::MemoryRange &)
{
  initProgramming();
  addProgress(OPERATION_DATA[Erase].nbSteps);
  const Mem24::Memory &pmemory = static_cast<const Mem24::Memory &>(memory);
  const Device::Array &data = pmemory.arrayForWriting();
  if ( !specific()->write(data) ) return false;
  VerifyActions actions = IgnoreProtectedVerify;
  Config config;
  if ( config.onlyVerifyProgrammed() ) actions |= OnlyProgrammedVerify;
  VerifyData vdata(actions, pmemory);
  Device::Array adata;
  return specific()->read(adata, &vdata);
}

bool Programmer::Mem24Base::verifyDeviceId()
{
  return specific()->verifyPresence();
}
