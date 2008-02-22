/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MEM24_GROUP_H
#define MEM24_GROUP_H

#include "common/global/global.h"
#include "devices/base/device_group.h"
#include "devices/mem24/base/mem24.h"

namespace Mem24
{
extern const uint DATA_SIZE;
extern const char *DATA_STREAM;

class Group : public Device::Group<Data>
{
public:
  virtual QString name() const { return "mem24"; }
  virtual QString label() const { return i18n("Serial Memory 24"); }
  virtual Device::Memory *createMemory(const Device::Data &data) const;

private:
  virtual uint dataSize() const { return DATA_SIZE; }
  virtual const char *dataStream() const { return DATA_STREAM; }
};

} // namespace

#endif
