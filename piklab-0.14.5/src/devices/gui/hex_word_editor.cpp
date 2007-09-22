/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "hex_word_editor.h"

#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qapplication.h>
#include <qtimer.h>

//-----------------------------------------------------------------------------
Device::HexWordEditor::HexWordEditor(Memory &memory, uint nbChars, QWidget *parent)
  : GenericHexWordEditor(nbChars, true, parent), _memory(memory)
{
  setOffset(-1);
}

void Device::HexWordEditor::setOffset(int offset)
{
  _offset = offset;
  set();
}

//-----------------------------------------------------------------------------
Device::RegisterHexWordEditor::RegisterHexWordEditor(QWidget *parent, uint nbChars, uint mask)
  : GenericHexWordEditor(nbChars, true, parent), _mask(mask)
{
  clear();
}

void Device::RegisterHexWordEditor::setValue(uint word)
{
  _word = word;
  set();
}
