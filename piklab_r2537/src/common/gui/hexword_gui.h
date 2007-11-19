/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef HEXWORD_GUI_H
#define HEXWORD_GUI_H

#include <qvalidator.h>
#include <klineedit.h>

#include "common/common/bitvalue.h"

//-----------------------------------------------------------------------------
class HexValueValidator : public QValidator
{
Q_OBJECT
public:
  HexValueValidator(uint nbChars, QObject *parent);
  virtual State validate(QString &input, int &pos) const;

private:
  uint _nbChars;
};

//-----------------------------------------------------------------------------
class GenericHexWordEditor : public KLineEdit
{
Q_OBJECT
public:
  GenericHexWordEditor(uint nbChars, bool hasBlankValue, QWidget *parent);
  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;

signals:
  void modified();
  void moveNext();
  void movePrev();
  void moveUp();
  void moveDown();
  void moveFirst();
  void moveLast();
  void moveNextPage();
  void movePrevPage();

private slots:
  bool changeValue();
  void slotTextChanged();

protected:
  uint _nbChars;
  bool _hasBlankValue;

  virtual bool isValid() const = 0;
  virtual BitValue mask() const = 0;
  virtual BitValue normalizeWord(BitValue value) const = 0;
  virtual bool event(QEvent *e);
  virtual void set();
  virtual BitValue word() const = 0;
  virtual void setWord(BitValue value) = 0;
  virtual BitValue blankValue() const = 0;
};

//-----------------------------------------------------------------------------
class HexWordEditor : public GenericHexWordEditor
{
Q_OBJECT
public:
  HexWordEditor(uint nbChars, QWidget *parent) : GenericHexWordEditor(nbChars, false, parent) {}
  void setValue(BitValue word) { _word = word; set(); }
  BitValue value() const { return _word; }

protected:
  BitValue _word;

  virtual bool isValid() const { return true; }
  virtual BitValue mask() const { return maxValue(NumberBase::Hex, _nbChars); }
  virtual BitValue normalizeWord(BitValue value) const { return value.maskWith(mask()); }
  virtual BitValue word() const { return _word; }
  virtual void setWord(BitValue value) { _word = value; }
  virtual BitValue blankValue() const { return 0; }
};

#endif
