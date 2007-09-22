/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef USBPICPROG_DATA_H
#define USBPICPROG_DATA_H

namespace Usbpicprog
{

struct Data {
};
extern const Data &data(const QString &device);

} // namespace

#endif
