/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MEM24_HEX_VIEW_H
#define MEM24_HEX_VIEW_H

class QVBoxLayout;

#include "devices/gui/hex_view.h"

namespace Mem24
{

class HexView : public Device::HexView
{
Q_OBJECT
public:
  HexView(const HexEditor &editor, QWidget *parent);
  virtual uint nbChecksumChars() const { return 4; }
  virtual uint checksum() const;

private:
  virtual void display();
};

} // namespace

#endif
