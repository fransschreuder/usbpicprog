/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef HEX_VIEW_H
#define HEX_VIEW_H

#include "memory_editor.h"
#include "libgui/hex_editor.h"

namespace Device
{

class HexView : public MemoryEditorGroup
{
Q_OBJECT
public:
  HexView(const HexEditor &editor, QWidget *parent, const char *name);
  virtual void display(Memory *memory);
  virtual uint nbChecksumChars() const = 0;
  virtual BitValue checksum() const = 0;
  bool isModified() const { return _editor.isModified(); }
  const Memory *originalMemory() const { return _editor.originalMemory(); }

protected:
  virtual void display() = 0;

private:
  const HexEditor &_editor;
};

} // namespace

#endif
