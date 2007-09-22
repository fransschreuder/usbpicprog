/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_PROTECTION_H
#define PIC_PROTECTION_H

//#include "common/global/global.h"
//#include "common/common/misc.h"
#include "pic.h"

namespace Pic
{
class Data;

//----------------------------------------------------------------------------
class Protection
{
public:
  enum { MAX_NB_BLOCKS = 8 };
  enum Type { ProgramProtected = 0, WriteProtected, ReadProtected,
              StandardSecurity, HighSecurity, Nb_Types };

public:
  Protection(const Pic::Data &data, const Config &config) : _data(data), _config(config) {}
  bool hasCodeGuard() const;
  QString noneProtectedValueName() const;
  QString securityValueName(Type type) const;
  bool hasBootBlock() const;
  QString bootSizeMaskName() const;
  QString bootMaskName(Type ptype) const;
  QString bootLabel() const;
  uint nbBlocks() const;
  QString blockSizeMaskName(uint i) const;
  QString blockMaskName(Type ptype, uint i) const;
  QString blockLabel(uint i) const;
  RangeVector extractRanges(const QString &name, MemoryRangeType type) const;
  bool checkRange(const QString &mask, const QString &name) const;
  QString maskName(Type type, MemoryRangeType mtype) const;

private:
  const Pic::Data &_data;
  const Config    &_config;

  enum SegmentType { BootSegment = 0, SecureSegment, GeneralSegment, Nb_SegmentTypes };
  static bool extractRanges(const QString &name, QValueVector<uint> &starts, uint &end, bool &ok);
  class ProtectedRange {
  public:
    QValueVector<uint> starts, ends;
  };
  ProtectedRange extractRange(const QString &mask, const QString &name, bool &ok) const;
  bool isAllProtectedValueName(const QString &name) const;
  bool isNoneProtectedValueName(const QString &name) const;
};

} //namespace

#endif
