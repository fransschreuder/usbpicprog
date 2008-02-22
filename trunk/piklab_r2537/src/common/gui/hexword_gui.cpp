/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "hexword_gui.h"

#include <qtimer.h>

#include "common/gui/number_gui.h"
#include "common/common/misc.h"

//-----------------------------------------------------------------------------
HexValueValidator::HexValueValidator(uint nbChars, QObject *parent)
  : QValidator(parent, "hex_value_validator"), _nbChars(nbChars) {}

QValidator::State HexValueValidator::validate(QString &input, int &) const
{
  if ( input.length()==0 ) return Acceptable;
  if ( input.length()>_nbChars ) return Invalid;
  for (uint i=0; i<input.length(); i++)
    if ( !isxdigit(input[i].latin1()) && input[i]!='-' ) return Invalid;
  return Acceptable;
}

//-----------------------------------------------------------------------------
GenericHexWordEditor::GenericHexWordEditor(uint nbChars, bool hasBlankValue, QWidget *parent)
  : KLineEdit(parent, "hex_word_editor"), _nbChars(nbChars), _hasBlankValue(hasBlankValue)
{
  setFocusPolicy(ClickFocus);
  setValidator(new HexValueValidator(nbChars, this));
  connect(this, SIGNAL(textChanged(const QString &)), SLOT(slotTextChanged()));
  setFrame(false);
}

void GenericHexWordEditor::slotTextChanged()
{
  if ( text().length()!=_nbChars ) return;
  if ( changeValue() ) emit moveNext();
}

bool GenericHexWordEditor::changeValue()
{
  if ( !isValid() ) return false;
  QString s = text();
  BitValue v = blankValue();
  if ( s!=QString(repeat("-", _nbChars)) ) {
    s = s.leftJustify(_nbChars, '0', true);
    for (uint i=0; i<_nbChars; i++)
      if ( !isxdigit(s[i].latin1()) ) s[i] = '0';
    v = normalizeWord(fromHex(s, 0));
    setText(toHex(v, _nbChars));
  }
  if ( v==word() ) return false;
  setWord(v);
  emit modified();
  return true;
}

void GenericHexWordEditor::set()
{
  blockSignals(true);
  setEnabled(isValid());
  if ( !isValid() ) clear();
  else {
    BitValue value = word();
    if ( _hasBlankValue && value==blankValue() ) setText(repeat("-", _nbChars));
    else setText(toHex(normalizeWord(value), _nbChars));
  }
  blockSignals(false);
}

bool GenericHexWordEditor::event(QEvent *e)
{
  switch (e->type()) {
  case QEvent::FocusOut:
    changeValue();
    break;
  case QEvent::FocusIn:
    QTimer::singleShot(0, this, SLOT(selectAll())); // ugly but it works
    break;
  case QEvent::KeyPress:
    switch ( static_cast<QKeyEvent *>(e)->key() ) {
      case Key_Next:
        emit moveNextPage();
        return true;
      case Key_Tab:
      case Key_Enter:
      case Key_Return:
        emit moveNext();
        return true;
      case Key_Prior:
        emit movePrevPage();
        return true;
      case Key_BackTab:
        emit movePrev();
        return true;
      case Key_Down:
        emit moveDown();
        return true;
      case Key_Up:
        emit moveUp();
        return true;
      case Key_Home:
        emit moveFirst();
        return true;
      case Key_End:
        emit moveLast();
        return true;
      case Key_Right:
        if ( cursorPosition()!=int(text().length()) ) break;
        emit moveNext();
        return true;
      case Key_Left:
        if ( cursorPosition()!=0 ) break;
        emit movePrev();
        return true;
    }
   default: break;
  }
  return QLineEdit::event(e);
}

QSize GenericHexWordEditor::sizeHint() const
{
  return QSize(maxCharWidth(NumberBase::Hex, font()) * (_nbChars+1), fontMetrics().height());
}

QSize GenericHexWordEditor::minimumSizeHint() const
{
  return QSize(maxCharWidth(NumberBase::Hex, font()) * (_nbChars+1), fontMetrics().height());
}
