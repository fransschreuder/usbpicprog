/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DEVICE_INFO_H
#define DEVICE_INFO_H

#include <qstring.h>
namespace Device { class Data; }

namespace Device
{
  extern QString webpageHtml(const Device::Data &data);
  extern QString documentHtml(const Device::Data &data);
  extern QString supportedHtmlInfo(const Device::Data &data);
} // namespace

#endif
