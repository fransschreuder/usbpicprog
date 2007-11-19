/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MEM24_H
#define MEM24_H

#include "common/common/misc.h"
#include "devices/base/generic_device.h"

namespace Mem24
{
class XmlToData;
class Group;

//-----------------------------------------------------------------------------
class Data : public Device::Data
{
public:
  Data() : Device::Data(0) {}
  virtual QString listViewGroup() const { return i18n("24 EEPROM"); }
  virtual QString informationHtml() const;
  uint nbBytes() const { return _nbBytes; }
  virtual bool matchId(BitValue, Device::IdData &) const { return false; }
  virtual uint nbBitsAddress() const { return nbBits(_nbBytes-1); }
  virtual bool canWriteCalibration() const { return false; }
  uint nbBlocks() const { return _nbBlocks; }
  uint nbBytesPage() const { return _nbBytesPage; }

private:
  uint _nbBytes, _nbBlocks, _nbBytesPage;

  friend class XmlToData;
  friend class Group;
  friend QDataStream &operator <<(QDataStream &s, const Data &data);
  friend QDataStream &operator >>(QDataStream &s, Data &data);
};

QDataStream &operator <<(QDataStream &s, const Data &data);
QDataStream &operator >>(QDataStream &s, Data &data);

} // namespace

#endif
