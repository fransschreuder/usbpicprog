/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MEMORY_EDITOR_H
#define MEMORY_EDITOR_H

#include <qframe.h>
#include <qscrollbar.h>
#include <qlabel.h>
#include "common/common/qflags.h"
#include "common/common/bitvalue.h"
class QVBoxLayout;
class QHBoxLayout;
class QHBox;
class KAction;
class PopupButton;

namespace Device
{
class HexView;
class HexWordEditor;
class Memory;

enum Action { Clear = 0, Zero, ChecksumCheck, Reload,
              Program, Verify, Read, Erase, BlankCheck, Nb_Actions };
enum ActionProperty { NoProperty = 0, SeparatorAfter = 1, NeedProgrammer = 2,
                      NeedWrite = 4, NeedModified = 8 };
Q_DECLARE_FLAGS(ActionProperties, ActionProperty)
Q_DECLARE_OPERATORS_FOR_FLAGS(ActionProperties)
struct ActionData {
  const char *label, *icon;
  ActionProperties properties;
};
extern const ActionData ACTION_DATA[Nb_Actions];

//----------------------------------------------------------------------------
class MemoryEditor : public QFrame
{
Q_OBJECT
public:
  MemoryEditor(Device::Memory *memory, QWidget *parent, const char *name);
  virtual void setReadOnly(bool readOnly) = 0;

public slots:
  virtual void updateDisplay() = 0;

signals:
  void modified();

protected:
  Device::Memory *_memory;
  QVBoxLayout    *_top;
};

//----------------------------------------------------------------------------
class MemoryRangeEditor : public MemoryEditor
{
Q_OBJECT
public:
  MemoryRangeEditor(Device::Memory &memory, uint nbLines, uint nbCols,
                    uint offset, int nb, QWidget *parent, const char *name);
  virtual void init();
  virtual void setReadOnly(bool readOnly);
  void setComment(const QString &text);

public slots:
  virtual void updateDisplay();
  void moveNext();
  void movePrev();
  void moveFirst();
  void moveLast();
  void moveUp();
  void moveDown();
  void moveNextPage();
  void movePrevPage();

protected slots:
  void setStartWord(int index);
  void setEndWord(int index);
  void setIndex(int index);

protected:
  uint _nbLines, _nbCols, _offset;
  int  _nb;
  QValueList<QLabel *> _addresses;
  QValueList<QWidget *> _blocks;
  QValueList<HexWordEditor *> _editors;
  QScrollBar *_scrollbar;
  QLabel     *_comment;
  QWidget    *_spacer;

  uint wordOffset() const { return _offset + current() * _nbCols; }
  uint current() const { return _scrollbar->value(); }
  virtual uint nbWords() const = 0;
  virtual uint addressIncrement() const = 0;
  virtual Address startAddress() const = 0;
  virtual HexWordEditor *createHexWordEditor(QWidget *parent) = 0;
  virtual bool isRangeReadOnly() const = 0;
  virtual void updateAddressColor(uint i, Address address) { Q_UNUSED(i); Q_UNUSED(address); }
  virtual void addLegend(QVBoxLayout *vbox) { Q_UNUSED(vbox); }
};

//----------------------------------------------------------------------------
class MemoryEditorGroup : public MemoryEditor
{
Q_OBJECT
public:
  MemoryEditorGroup(Device::Memory *memory, QWidget *parent, const char *name);
  void addEditor(MemoryEditor *editor);
  virtual void setReadOnly(bool readOnly);

public slots:
  virtual void updateDisplay();

protected:
  bool _modified;
  QValueList<MemoryEditor *> _editors;
};

//----------------------------------------------------------------------------
class MemoryTypeEditor : public MemoryEditorGroup
{
Q_OBJECT
public:
  MemoryTypeEditor(const HexView *hexview, Device::Memory &memory, QWidget *parent, const char *name);
  virtual void init(bool first);
  virtual void setReadOnly(bool readOnly);

protected slots:
  void doAction();
  void stateChanged();

protected:
  PopupButton   *_title;
  QLabel        *_comment;
  const HexView *_hexview;
  virtual bool internalDoAction(Action action) = 0; // return true if memory modified
  virtual bool hasAction(Action) const { return true; }
  void addAction(KAction *action);
  const Device::Memory *originalMemory() const;

private:
  bool     _readOnly;
  KAction *_actions[Nb_Actions];

  void doAction(Action action);
};

} // namespace

#endif
