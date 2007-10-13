/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ICD2_DATA_H
#define ICD2_DATA_H

#include "common/common/group.h"

namespace Icd2
{
  struct Version {
    uchar major, mid, minor;
  };
  struct FirmwareData {
    Version min, max;
  };
  enum { Nb_Firmwares = 15 };
  extern const FirmwareData FIRMWARE_DATA[Nb_Firmwares];
  struct FamilyData {
    const char *name;
    uchar efid;
    const char *debugExec;
    uint debugExecOffset;  // in the hex file
    uint wreg, fsr, status;
  };
  extern const FamilyData FAMILY_DATA[];
  struct Data {
    uint famid;
    ::Group::SupportType debugSupportType;
  };
  extern const Data &data(const QString &device);
  extern uint family(const QString &device);
} // namespace

#endif
