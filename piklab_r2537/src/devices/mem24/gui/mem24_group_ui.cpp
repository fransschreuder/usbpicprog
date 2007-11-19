/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "mem24_group_ui.h"

#include "mem24_hex_view.h"

Device::HexView *Mem24::GroupUI::createHexView(const HexEditor &editor, QWidget *parent) const
{
  return new HexView(editor, parent);
}
