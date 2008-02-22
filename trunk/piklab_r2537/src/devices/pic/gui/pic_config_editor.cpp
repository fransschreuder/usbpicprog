/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_config_editor.h"

#include <qlayout.h>
#include <qvgroupbox.h>
#include <qapplication.h>

#include "pic_config_word_editor.h"
#include "common/common/misc.h"
#include "common/gui/misc_gui.h"

//----------------------------------------------------------------------------
Pic::MemoryConfigEditorWidget::MemoryConfigEditorWidget(Memory &memory, bool withWordEditor, QWidget *parent)
  : Device::MemoryEditorGroup(&memory, parent, "pic_config_editor_widget"),
    MemoryCaster(MemoryRangeType::Config, memory)
{
  QHBoxLayout *hb = new QHBoxLayout(_top);

  TabWidget *tabw = 0;
  uint nbWords = device().nbWords(MemoryRangeType::Config);
  if ( nbWords>1 ) {
    tabw = new TabWidget(this);
    tabw->setIgnoreWheelEvent(true);
    hb->addWidget(tabw);
  }

  for(uint i=0; i<nbWords; ++i) {
    //qDebug("BinWordsEditor for config word #%i", i);
    //uint address = device().range(Device::MemoryConfig).start + device().addressIncrement(Device::MemoryConfig) * i;
    //qDebug("address: %s %s nb: %i", toHex(address, 8).data(), device().configWord(i).name.latin1(), device().configWord(i).masks.count());
    if ( device().config()._words[i].masks.count()==0 ) continue;
    QWidget *page = 0;
    if ( nbWords>1 ) {
      page = new QWidget(tabw);
      tabw->addTab(page, device().config()._words[i].name);
    } else {
      page = new QGroupBox(this);
      hb->addWidget(page);
    }
    QVBoxLayout *vbox = new QVBoxLayout(page, 10, 10);
    QHBoxLayout *hbox = new QHBoxLayout(vbox);
    ConfigWordEditor *we = new ConfigWordEditor(memory, i, withWordEditor, page);
    addEditor(we);
    hbox->addWidget(we);
    hbox->addStretch(1);
    vbox->addStretch(1);
  }
}

//----------------------------------------------------------------------------
Pic::MemoryConfigEditor::MemoryConfigEditor(const HexView *hexview, Memory &memory, QWidget *parent)
  : MemoryTypeEditor(hexview, MemoryRangeType::Config, memory, parent, "pic_config_editor")
{}

void Pic::MemoryConfigEditor::init(bool first)
{
  MemoryTypeEditor::init(first);
  MemoryConfigEditorWidget *w = new MemoryConfigEditorWidget(memory(), true, this);
  addEditor(w);
  _top->addWidget(w);
}
