/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "mem24_hex_view.h"

#include <qlayout.h>
#include <qlabel.h>
#include <klocale.h>

#include "mem24_memory_editor.h"

Mem24::HexView::HexView(const HexEditor &editor, QWidget *parent)
  : Device::HexView(editor, parent, "mem24_hex_view")
{}

void Mem24::HexView::display()
{
  Memory *pmemory = static_cast<Memory *>(_memory);
  MemoryTypeEditor *e = new MemoryTypeEditor(this, *pmemory, this);
  e->init(true);
  e->show();
  _top->addWidget(e);
  addEditor(e);
}

BitValue Mem24::HexView::checksum() const
{
  BitValue cs = 0x0000;
  for (uint i=0; i<static_cast<const Data &>(_memory->device()).nbBytes(); i++)
    cs += static_cast<const Memory *>(_memory)->byte(i);
  return cs.maskWith(0xFFFF);
}
