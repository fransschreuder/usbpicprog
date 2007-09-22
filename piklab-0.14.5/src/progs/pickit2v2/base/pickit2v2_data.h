/***************************************************************************
 * Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICKIT2V2_DATA_H
#define PICKIT2V2_DATA_H

#include <qstring.h>

namespace Pickit2V2
{

enum ScriptType { ProgEntry = 0x00, ProgExit = 0x01, DeviceIdRead = 0x02, ProgMemoryRead = 0x03,
                  EraseChipPrepare = 0x04, ProgMemoryAddressSet = 0x05, ProgMemoryWritePrepare = 0x06,
                  ProgMemoryWrite = 0x07, EepromReadPrepare = 0x08, EepromRead = 0x09, EepromWritePrepare = 0x0A,
                  EepromWrite = 0x0B, ConfigReadPrepare = 0x0C, ConfigRead = 0x0D, ConfigWritePrepare = 0x0E,
                  ConfigWrite = 0x0F, UserIdReadPrepare = 0x10, UserIdRead = 0x11, UserIdWritePrepare = 0x12,
                  UserIdWrite = 0x13, OsccalRead = 0x14, OsccalWrite = 0x15, EraseChip = 0x16,
                  ProgMemoryErase = 0x17, EepromErase = 0x18, ConfigErase = 0x19, TestMemoryRead = 0x1A,
                  TestMemoryWrite = 0x1B, Nb_ScriptTypes = 0x1C
};
inline ScriptType prepareReadScript(Pic::MemoryRangeType type)
{
  switch (type) {
    case Pic::MemoryCode: return ProgMemoryAddressSet;
    case Pic::MemoryEeprom: return EepromReadPrepare;
    case Pic::MemoryConfig: return ConfigReadPrepare;
    case Pic::MemoryUserId: return UserIdReadPrepare;
    default: break;
  }
  return Nb_ScriptTypes;
}
inline ScriptType readScript(Pic::MemoryRangeType type)
{
  switch (type) {
    case Pic::MemoryCode: return ProgMemoryRead;
    case Pic::MemoryEeprom: return EepromRead;
    case Pic::MemoryConfig: return ConfigRead;
    case Pic::MemoryUserId: return UserIdRead;
    case Pic::MemoryCal: return OsccalRead;
    default: break;
  }
  return Nb_ScriptTypes;
}
inline ScriptType prepareWriteScript(Pic::MemoryRangeType type)
{
  switch (type) {
    case Pic::MemoryCode: return ProgMemoryWritePrepare;
    case Pic::MemoryEeprom: return EepromWritePrepare;
    case Pic::MemoryConfig: return ConfigWritePrepare;
    case Pic::MemoryUserId: return UserIdWritePrepare;
    default: break;
  }
  return Nb_ScriptTypes;
}
inline ScriptType writeScript(Pic::MemoryRangeType type)
{
  switch (type) {
    case Pic::MemoryCode: return ProgMemoryWrite;
    case Pic::MemoryEeprom: return EepromWrite;
    case Pic::MemoryConfig: return ConfigWrite;
    case Pic::MemoryUserId: return UserIdWrite;
    case Pic::MemoryCal: return OsccalWrite;
    default: break;
  }
  return Nb_ScriptTypes;
}

struct Data {
  uchar codeMemoryNbReadWords, eepromMemoryNbReadWords;
  uchar codeMemoryNbWriteWords, eepromMemoryNbWriteWords;
  uchar scriptIndexes[Nb_ScriptTypes];
};
extern const Data &data(const QString &device);

struct FamilyData {
  Pic::Architecture architecture;
  double vpp;
  ushort progEntryScript, progExitScript, readDevIdScript;
  bool progMemShift;
};
extern const FamilyData FAMILY_DATA[];

struct ScriptData {
  const char *name;
  ushort version, length;
  const ushort data[64];
};
extern const ScriptData SCRIPT_DATA[];

} // namespace

#endif
