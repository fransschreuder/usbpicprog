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

#include "common/gui/misc_gui.h"
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
  virtual uint mask() const = 0;
  virtual uint normalizeWord(uint value) const = 0;
  virtual uint word() const = 0;
  virtual void setWord(uint value) = 0;
  virtual uint blankValue() const { return Device::INVALID; }
};

//-----------------------------------------------------------------------------
class RegisterHexWordEditor : public GenericHexWordEditor
{
Q_OBJECT
public:
  RegisterHexWordEditor(QWidget *parent, uint nbChars, uint mask);
  void clear() { setValue(Device::INVALID); }
  void setValue(uint word);
  uint value() const { return _word; }
  void setColor(QColor color) { setPaletteForegroundColor(color); }
  void unsetColor() { unsetPalette(); }

private:
  uint _mask, _word;

  virtual bool isValid() const { return true; }
  virtual uint mask() const { return _mask; }
  virtual uint normalizeWord(uint value) const { return value & _mask; }
  virtual uint word() const { return _word; }
  virtual void setWord(uint value) { _word = value; }
  virtual uint blankValue() const { return Device::INVALID; }
};

} // namespace

#endif
