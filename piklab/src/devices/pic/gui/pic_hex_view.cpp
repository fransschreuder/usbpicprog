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

const Pic::MemoryRangeType::Type Pic::HexView::MEMORY_DATA[] = {
  MemoryRangeType::Code, MemoryRangeType::Config, MemoryRangeType::Eeprom, MemoryRangeType::UserId,
  MemoryRangeType::Cal, MemoryRangeType::Nb_Types
};

void Pic::HexView::display()
{
  bool first = true;
  for (uint i=0; MEMORY_DATA[i]!=MemoryRangeType::Nb_Types; i++) {
    Pic::MemoryRangeType type = MEMORY_DATA[i];
    if ( !memory()->device().isReadable(type) ) continue;
    Device::MemoryTypeEditor *e = 0;
    switch (type) {
      case MemoryRangeType::Config: e = new MemoryConfigEditor(this, *memory(), this); break;
      case MemoryRangeType::Cal:    e = new MemoryCalibrationEditor(this, *memory(), this); break;
      case MemoryRangeType::Code:
      case MemoryRangeType::Eeprom: e = new MemoryTypeRangeEditor(this, type, *memory(), this); break;
      case MemoryRangeType::UserId: e = new MemoryUserIdEditor(this, *memory(), this); break;
      case MemoryRangeType::DeviceId:
      case MemoryRangeType::HardwareStack:
      case MemoryRangeType::ProgramExecutive:
      case MemoryRangeType::DebugVector:
      case MemoryRangeType::CalBackup:
      case MemoryRangeType::Nb_Types: Q_ASSERT(false); break;
    }
    e->init(first);
    e->show();
    _top->addWidget(e);
    addEditor(e);
    first = false;
  }
}

BitValue Pic::HexView::checksum() const
{
  return (_memory ? memory()->checksum() : 0x0000);
}
