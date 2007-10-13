/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MEM24_MEMORY_EDITOR_H
#define MEM24_MEMORY_EDITOR_H

#include "devices/gui/memory_editor.h"
#include "devices/gui/hex_word_editor.h"
#include "devices/mem24/mem24/mem24_memory.h"

namespace Mem24
{
class HexView;

//-----------------------------------------------------------------------------
class MemoryCaster
{
public:
  MemoryCaster(Memory &memory) : _memory(memory) {}
  const Data &device() const { return static_cast<const Data &>(_memory.device()); }
  const Memory &memory() const { return static_cast<Memory &>(_memory); }
  Memory &memory() { return static_cast<Memory &>(_memory); }

private:
  Memory &_memory;
};

//-----------------------------------------------------------------------------
class HexWordEditor : public Device::HexWordEditor, public MemoryCaster
{
Q_OBJECT
public:
  HexWordEditor(Memory &memory, QWidget *parent)
    : Device::HexWordEditor(memory, 2, parent), MemoryCaster(memory) {}

private:
  virtual BitValue mask() const { return 0xFF; }
  virtual BitValue normalizeWord(BitValue value) const { return value; }
  virtual BitValue word() const { return memory().byte(_offset); }
  virtual void setWord(BitValue value) { memory().setByte(_offset, value); }
};

//-----------------------------------------------------------------------------
class MemoryRangeEditor : public Device::MemoryRangeEditor, public MemoryCaster
{
Q_OBJECT
public:
  MemoryRangeEditor(Memory &memory, QWidget *parent);

private:
  virtual uint nbWords() const { return device().nbBytes(); }
  virtual uint addressIncrement() const { return 1; }
  virtual Address startAddress() const { return 0x0; }
  virtual Device::HexWordEditor *createHexWordEditor(QWidget *parent);
  virtual bool isRangeReadOnly() const { return false; }
};

//-----------------------------------------------------------------------------
class MemoryTypeEditor : public Device::MemoryTypeEditor, public MemoryCaster
{
Q_OBJECT
public:
  MemoryTypeEditor(const HexView *hexview, Memory &memory, QWidget *parent);
  virtual void init(bool first);

private:
  virtual bool internalDoAction(Device::Action action);
};

} // namespace

#endif
