/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DEVICE_LIST_H
#define DEVICE_LIST_H

#include "devices/base/generic_device.h"
#include "common/common/group.h"

namespace Device
{

struct AutoData {
  const char *name, *label;
};
extern const AutoData AUTO_DATA;

class Lister : public ::Group::Lister<GroupBase>
{
public:
  Lister();
  const Data *data(const QString &device) const;
  QString checkName(const QString &device) const;
};
extern const Lister &lister();

} // namespace

#endif
