/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_CONFIG_WORD_EDITOR_H
#define PIC_CONFIG_WORD_EDITOR_H

#include <qcombobox.h>

#include "common/gui/dialog.h"
#include "pic_memory_editor.h"

namespace Pic
{
//----------------------------------------------------------------------------
class ComboBox : public QComboBox
{
Q_OBJECT
public:
  ComboBox(QWidget *parent = 0, const char *name = 0)
    : QComboBox(parent, name) {}

protected:
  void wheelEvent(QWheelEvent *e) { e->ignore(); }
};

//----------------------------------------------------------------------------
class ConfigWordDialog : public Dialog
{
Q_OBJECT
public:
  ConfigWordDialog(const Memory &memory, uint index, QWidget *parent);
};

//----------------------------------------------------------------------------
class ConfigWordComboBox : public QComboBox
{
Q_OBJECT
public:
  ConfigWordComboBox(QWidget *parent);
  void appendItem(const QString &text, uint index) { insertItem(text); _map.append(index); }
  uint index() const;
  void setItem(uint index);
  void setInvalidItem(uint index, const QString &label);

private:
  QValueVector<uint> _map; // item index -> value index
  uint _invalidIndex; // if invalid -> value index

  bool isValid() const { return uint(count())==_map.count(); }
};

//----------------------------------------------------------------------------
class ConfigWordEditor : public MemoryEditor
{
Q_OBJECT
public:
  ConfigWordEditor(Memory &memory, uint index, bool withWordEditor, QWidget *parent);
  virtual void setReadOnly(bool readOnly);

public slots:
  virtual void updateDisplay();

private slots:
  void slotModified();
  void showDialog();

private:
  uint _configIndex;
  MemoryRangeEditor *_mdb;
  QValueVector<ConfigWordComboBox *> _combos;
};

} // namespace

#endif
