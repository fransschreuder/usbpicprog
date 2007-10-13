/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "mem24_memory_editor.h"

#include <qlayout.h>
#include <klocale.h>
#include <kpushbutton.h>

#include "common/gui/misc_gui.h"
#include "mem24_hex_view.h"
#include "progs/base/generic_prog.h"
#include "libgui/main_global.h"
#include "devices/base/device_group.h"

//-----------------------------------------------------------------------------
Mem24::MemoryRangeEditor::MemoryRangeEditor(Memory &memory, QWidget *parent)
  : Device::MemoryRangeEditor(memory, 16, 16, 0, -1, parent, "mem24_memory_range_editor"),
    MemoryCaster(memory)
{}

Device::HexWordEditor *Mem24::MemoryRangeEditor::createHexWordEditor(QWidget *parent)
{
  return new HexWordEditor(memory(), parent);
}

//-----------------------------------------------------------------------------
Mem24::MemoryTypeEditor::MemoryTypeEditor(const HexView *hexview, Memory &memory, QWidget *parent)
  : Device::MemoryTypeEditor(hexview, memory, parent, "mem24_memory_type_editor"),
    MemoryCaster(memory)
{}

void Mem24::MemoryTypeEditor::init(bool first)
{
  Device::MemoryTypeEditor::init(first);
  _title->setText(i18n("EEPROM Memory"));
  MemoryRangeEditor *mre = new MemoryRangeEditor(memory(), this);
  mre->init();
  addEditor(mre);
  _top->addWidget(mre);
}

bool Mem24::MemoryTypeEditor::internalDoAction(Device::Action action)
{
  switch (action) {
    case Device::Clear:
    case Device::ChecksumCheck:
      memory().clear(); return true;
    case Device::Zero:  memory().fill(0);  return true;
    case Device::Reload:
      Q_ASSERT(originalMemory());
      memory().copyFrom(*originalMemory()); return true;
    case Device::Program:
      Main::programmer()->program(memory(), Device::MemoryRange());
      return false;
    case Device::Verify:
      Main::programmer()->verify(memory(), Device::MemoryRange());
      return false;
    case Device::Read: {
      Memory mem(device());
      if ( !Main::programmer()->read(mem, Device::MemoryRange()) ) return false;
      memory().copyFrom(mem);
      return true;
    }
    case Device::Erase:
      Main::programmer()->erase(Device::MemoryRange());
      return false;
    case Device::BlankCheck:
      Main::programmer()->blankCheck(Device::MemoryRange());
      return false;
    case Device::Nb_Actions: break;
  }
  Q_ASSERT(false);
  return false;
}
