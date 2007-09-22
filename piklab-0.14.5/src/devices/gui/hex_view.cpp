/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "hex_view.h"

Device::HexView::HexView(const HexEditor &editor, QWidget *parent, const char *name)
  : MemoryEditorGroup(0, parent, name), _editor(editor)
{}

void Device::HexView::display(Memory *memory)
{
  _memory = memory;
  for (uint i=0; i<_editors.count(); i++) delete _editors[i];
  _editors.clear();
  if ( _memory==0 ) return;
  display();
}
