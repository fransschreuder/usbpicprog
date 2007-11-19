/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_GROUP_H
#define PIC_GROUP_H

#include "common/global/global.h"
#include "devices/base/device_group.h"
#include "devices/pic/base/pic.h"

namespace Pic
{
extern const uint DATA_SIZE;
extern const char *DATA_STREAM;

class Group : public Device::Group<Data>
{
public:
  virtual QString name() const { return "pic"; }
  virtual QString label() const { return i18n("PIC"); }
  virtual Device::Memory *createMemory(const Device::Data &data) const;

private:
  virtual uint dataSize() const { return DATA_SIZE; }
  virtual const char *dataStream() const { return DATA_STREAM; }
};

} // namespace

#endif
