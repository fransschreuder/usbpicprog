/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_HEX_VIEW_H
#define PIC_HEX_VIEW_H

class QVBoxLayout;

#include "devices/gui/hex_view.h"
#include "devices/pic/pic/pic_memory.h"

namespace Pic
{

class HexView : public Device::HexView
{
Q_OBJECT
public:
  HexView(const HexEditor &editor, QWidget *parent);
  Memory *memory() { return static_cast<Memory *>(_memory); }
  const Memory *memory() const { return static_cast<Memory *>(_memory); }
  virtual uint nbChecksumChars() const { return 4; }
  virtual uint checksum() const;

private:
  static const MemoryRangeType MEMORY_DATA[];

  virtual void display();
};

} // namespace

#endif
