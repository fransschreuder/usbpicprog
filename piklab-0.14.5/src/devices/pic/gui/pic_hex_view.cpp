/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_hex_view.h"

#include <qlayout.h>
#include <qlabel.h>

#include <klocale.h>

#include "pic_memory_editor.h"
#include "pic_config_editor.h"

Pic::HexView::HexView(const HexEditor &editor, QWidget *parent)
  : Device::HexView(editor, parent, "pic_hex_view")
{}

const Pic::MemoryRangeType Pic::HexView::MEMORY_DATA[] = {
  MemoryCode, MemoryConfig, MemoryEeprom, MemoryUserId, MemoryCal, Nb_MemoryRangeTypes
};

void Pic::HexView::display()
{
  bool first = true;
  for (uint i=0; MEMORY_DATA[i]!=Nb_MemoryRangeTypes; i++) {
    Pic::MemoryRangeType type = MEMORY_DATA[i];
    if ( !memory()->device().isReadable(type) ) continue;
    Device::MemoryTypeEditor *e = 0;
    switch (type) {
      case MemoryConfig: e = new MemoryConfigEditor(this, *memory(), this); break;
      case MemoryCal:    e = new MemoryCalibrationEditor(this, *memory(), this); break;
      case MemoryCode:
      case MemoryEeprom: e = new MemoryTypeRangeEditor(this, type, *memory(), this); break;
      case MemoryUserId: e = new MemoryUserIdEditor(this, *memory(), this); break;
      case MemoryDeviceId:
      case MemoryHardwareStack:
      case MemoryProgramExecutive:
      case MemoryDebugVector:
      case MemoryCalBackup:
      case Nb_MemoryRangeTypes: Q_ASSERT(false); break;
    }
    e->init(first);
    e->show();
    _top->addWidget(e);
    addEditor(e);
    first = false;
  }
}

uint Pic::HexView::checksum() const
{
  Q_ASSERT(_memory);
  return memory()->checksum();
}
