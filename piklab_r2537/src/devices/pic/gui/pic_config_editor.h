/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_CONFIG_EDITOR_H
#define PIC_CONFIG_EDITOR_H

#include "pic_memory_editor.h"

//----------------------------------------------------------------------------
namespace Pic
{
class HexView;

class MemoryConfigEditorWidget : public Device::MemoryEditorGroup, public MemoryCaster
{
Q_OBJECT
public:
  MemoryConfigEditorWidget(Memory &memory, bool withWordEditor, QWidget *parent);
};

//----------------------------------------------------------------------------
class MemoryConfigEditor : public MemoryTypeEditor
{
Q_OBJECT
public:
  MemoryConfigEditor(const HexView *hexview, Memory &memory, QWidget *parent);
  virtual void init(bool first);
};

} // namespace

#endif
