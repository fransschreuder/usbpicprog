/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_PROG_SPECIFIC_H
#define PIC_PROG_SPECIFIC_H

#include "progs/base/prog_specific.h"
#include "progs/base/generic_prog.h"
#include "pic_memory.h"

//----------------------------------------------------------------------------
namespace Pic
{
  enum ResetMode { ResetHeld = 0, ResetReleased, Nb_ResetModes};
  extern const char * const RESET_MODE_LABELS[Nb_ResetModes];

  enum VoltageType { ProgrammerVpp = 0, TargetVdd, TargetVpp, Nb_VoltageTypes };
  extern const char * const VOLTAGE_TYPE_LABELS[Nb_VoltageTypes];

  enum TargetMode { TargetStopped = 0, TargetRunning, TargetInProgramming, Nb_TargetModes};
  extern const char * const TARGET_MODE_LABELS[Nb_TargetModes];

  enum WriteMode { WriteOnlyMode = 0, EraseWriteMode, Nb_WriteModes };
} // namespace

namespace Programmer
{
//-----------------------------------------------------------------------------
class PicDeviceSpecific : public DeviceSpecific
{
public:
  PicDeviceSpecific(::Programmer::Base &base) : DeviceSpecific(base) {}
  const Pic::Data &device() const { return static_cast<const Pic::Data &>(*_base.device()); }
  virtual bool canEraseAll() const = 0;
  virtual bool canEraseRange(Pic::MemoryRangeType type) const = 0;
  virtual bool emulatedErase() const { return false; }
  virtual bool canReadRange(Pic::MemoryRangeType type) const = 0;
  virtual bool canWriteRange(Pic::MemoryRangeType type) const = 0;
  bool eraseRange(Pic::MemoryRangeType type);
  bool erase(bool isProtected);
  bool read(Pic::MemoryRangeType type, Device::Array &data, const VerifyData *vdata);
  bool write(Pic::MemoryRangeType type, const Device::Array &data, bool force);
  uint findNonMaskStart(Pic::MemoryRangeType type, const Device::Array &data) const;
  uint findNonMaskEnd(Pic::MemoryRangeType type, const Device::Array &data) const;

protected:
  virtual bool doErase(bool isProtected) = 0;
  virtual bool doEraseRange(Pic::MemoryRangeType type) = 0;
  bool doEmulatedEraseRange(Pic::MemoryRangeType type);
  virtual bool doRead(Pic::MemoryRangeType type, Device::Array &data, const VerifyData *vdata) = 0;
  virtual bool doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force) = 0;
};

//-----------------------------------------------------------------------------
class PicHardware : public Hardware
{
public:
  class VoltageData {
  public:
    VoltageData() : value(UNKNOWN_VOLTAGE) {}
    double value;
    bool error;
  };
  class VoltagesData : public QValueVector<VoltageData> {
  public:
    VoltagesData() : QValueVector<VoltageData>(Pic::Nb_VoltageTypes) {}
  };

public:
  PicHardware(::Programmer::Base &base, Port::Base *port, const QString &name) : Hardware(base, port, name) {}
  const Pic::Data &device() const { return static_cast<const Pic::Data &>(*_base.device()); }
  virtual bool readVoltages(VoltagesData &) { return true; }
  virtual bool getTargetMode(Pic::TargetMode &mode) { mode = Pic::TargetInProgramming; return true; }
  virtual bool setTargetReset(Pic::ResetMode) { return true; }
  bool compareWords(Pic::MemoryRangeType type, uint index, BitValue v, BitValue d, VerifyActions actions);
  bool verifyWord(uint index, BitValue word, Pic::MemoryRangeType type, const VerifyData &vdata);
};

} // namespace

#endif
