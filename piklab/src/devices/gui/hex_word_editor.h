/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef HEX_WORD_EDITOR_H
#define HEX_WORD_EDITOR_H

#include "common/gui/hexword_gui.h"
#include "devices/base/generic_memory.h"

namespace Device
{
//-----------------------------------------------------------------------------
class HexWordEditor : public GenericHexWordEditor
{
Q_OBJECT
public:
  HexWordEditor(Memory &memory, uint nbChars, QWidget *parent);
  void setOffset(int offset);
  int offset() const { return _offset; }

protected:
  Device::Memory &_memory;
  int _offset;

  virtual bool isValid() const { return _offset!=-1; }
  virtual BitValue mask() const = 0;
  virtual BitValue normalizeWord(BitValue value) const = 0;
  virtual BitValue word() const = 0;
  virtual void setWord(BitValue value) = 0;
  virtual BitValue blankValue() const { return BitValue(); }
};

//-----------------------------------------------------------------------------
class RegisterHexWordEditor : public GenericHexWordEditor
{
Q_OBJECT
public:
  RegisterHexWordEditor(QWidget *parent, uint nbChars, BitValue mask);
  void clear() { setValue(BitValue()); }
  void setValue(BitValue word);
  BitValue value() const { return _word; }
  void setColor(QColor color) { setPaletteForegroundColor(color); }
  void unsetColor() { unsetPalette(); }

private:
  BitValue _mask, _word;

  virtual bool isValid() const { return true; }
  virtual BitValue mask() const { return _mask; }
  virtual BitValue normalizeWord(BitValue value) const { return value.maskWith(_mask); }
  virtual BitValue word() const { return _word; }
  virtual void setWord(BitValue value) { _word = value; }
  virtual BitValue blankValue() const { return BitValue(); }
};

} // namespace

#endif
