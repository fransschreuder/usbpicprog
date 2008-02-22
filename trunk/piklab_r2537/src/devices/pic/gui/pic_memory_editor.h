/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_MEMORY_EDITOR_H
#define PIC_MEMORY_EDITOR_H

#include <qscrollbar.h>
#include <qgroupbox.h>
class KToggleAction;

#include "devices/gui/memory_editor.h"
#include "devices/gui/hex_word_editor.h"
#include "devices/pic/pic/pic_memory.h"
class PopupButton;

namespace Pic
{
class HexView;

//-----------------------------------------------------------------------------
class MemoryCaster
{
public:
  MemoryCaster(MemoryRangeType type, Device::Memory &memory) : _type(type), _memory(memory) {}
  MemoryRangeType type() const { return _type; }
  const Data &device() const { return static_cast<const Data &>(_memory.device()); }
  const Memory &memory() const { return static_cast<Memory &>(_memory); }
  Memory &memory() { return static_cast<Memory &>(_memory); }

private:
  MemoryRangeType _type;
  Device::Memory &_memory;
};

//-----------------------------------------------------------------------------
class MemoryEditor : public Device::MemoryEditor, public MemoryCaster
{
Q_OBJECT
public:
  MemoryEditor(MemoryRangeType type, Memory &memory, QWidget *parent, const char *name)
    : Device::MemoryEditor(&memory, parent, name), MemoryCaster(type, memory) {}
};

//-----------------------------------------------------------------------------
class MemoryEditorLegend : public MemoryEditor
{
Q_OBJECT
public:
  MemoryEditorLegend(MemoryRangeType type, Memory &memory, QWidget *parent);
  virtual void setReadOnly(bool) {}

  static QColor protectedColor() { return QColor("#FF8888"); }
  static QColor bootColor() { return QColor("#8888FF"); }
  static QColor blockColor(uint i) { return QColor(BLOCK_COLORS[i]); }

signals:
  void setStartWord(int i);
  void setEndWord(int i);

public slots:
  virtual void updateDisplay();

private slots:
  void gotoStart();
  void gotoEnd();

private:
  class Data {
  public:
    Data() : button(0), label(0) {}
    Data(const QString &text, QWidget *parent);
    void setProtected(bool on);
    bool hasAction(const KAction *action) const;
    PopupButton *button;
    QLabel      *label;
    QValueVector<KAction *> actions;
  };
  Data _boot;
  QValueVector<Data> _blocks;

  static const char * const BLOCK_COLORS[Protection::MAX_NB_BLOCKS];
};

//-----------------------------------------------------------------------------
class HexWordEditor : public Device::HexWordEditor, public MemoryCaster
{
Q_OBJECT
public:
  HexWordEditor(MemoryRangeType type, Memory &memory, QWidget *parent);

private:
  virtual BitValue mask() const { return memory().device().mask(type()); }
  virtual BitValue normalizeWord(BitValue value) const { return memory().normalizeWord(type(), _offset, value); }
  virtual BitValue word() const { return memory().word(type(), _offset); }
  virtual void setWord(BitValue value);
};

//-----------------------------------------------------------------------------
class MemoryRangeEditor : public Device::MemoryRangeEditor, public MemoryCaster
{
  Q_OBJECT
public:
  MemoryRangeEditor(MemoryRangeType type, Memory &memory,
                    uint nbLines, uint nbCols, uint wordOffset, int nbWords, QWidget *parent);

public slots:
  virtual void updateDisplay();

private:
  MemoryEditorLegend *_legend;
  AddressRange        _bootRange;
  AddressRangeVector  _blockRanges;
  AddressRangeVector  _codeProtected;

  virtual uint nbWords() const { return device().nbWords(type()); }
  virtual uint addressIncrement() const { return device().addressIncrement(type()); }
  virtual Address startAddress() const { return device().range(type()).start; }
  virtual Device::HexWordEditor *createHexWordEditor(QWidget *parent);
  virtual void updateAddressColor(uint i, Address address);
  virtual bool isRangeReadOnly() const;
  virtual void addLegend(QVBoxLayout *vbox);
};

//-----------------------------------------------------------------------------
class MemoryTypeEditor : public Device::MemoryTypeEditor, public MemoryCaster
{
Q_OBJECT
public:
  MemoryTypeEditor(const HexView *hexview, MemoryRangeType type, Memory &memory, QWidget *parent, const char *name);
  virtual void init(bool first);

private:
  virtual bool internalDoAction(Device::Action action);
};

//-----------------------------------------------------------------------------
class MemoryTypeRangeEditor : public MemoryTypeEditor
{
Q_OBJECT
public:
  MemoryTypeRangeEditor(const HexView *hexview, MemoryRangeType type, Memory &memory, QWidget *parent);
  virtual void init(bool first);

protected:
  MemoryRangeEditor *_mre;
};

//-----------------------------------------------------------------------------
class MemoryUserIdEditor : public MemoryTypeRangeEditor
{
Q_OBJECT
public:
  MemoryUserIdEditor(const HexView *hexview, Memory &memory, QWidget *parent);
  virtual void init(bool first);
  virtual void setReadOnly(bool readOnly);

public slots:
  virtual void updateDisplay();

private slots:
  void toggleSetToChecksum();

private:
  bool _saveReadOnly;
  KToggleAction *_setToChecksum;
};

//-----------------------------------------------------------------------------
class MemoryCalibrationEditor : public MemoryTypeEditor
{
Q_OBJECT
public:
  MemoryCalibrationEditor(const HexView *hexview, Memory &memory, QWidget *parent);
  virtual void init(bool first);

private:
  virtual bool hasAction(Device::Action action) const;
  virtual bool internalDoAction(Device::Action action);
};

} // namespace

#endif
