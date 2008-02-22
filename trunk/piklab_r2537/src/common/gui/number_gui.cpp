/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "number_gui.h"

#include <qfontmetrics.h>

//-----------------------------------------------------------------------------
uint maxCharWidth(const QString &s, const QFont &font)
{
  QFontMetrics fm(font);
  uint w = 0;
  for (uint i=0; i<uint(s.length()); i++)
    w = QMAX(w, uint(fm.width(s[i])));
  return w;
}

uint maxCharWidth(NumberBase base, const QFont &font)
{
  QString s;
  for (uint i=0; i<base.data().base; i++) s += toChar(base, i);
  return maxCharWidth(s, font);
}

uint maxLabelWidth(NumberBase base, uint nbChars, const QFont &font)
{
  uint w = maxStringWidth(base, nbChars, font);
  QFontMetrics fm(font);
  if ( base==NumberBase::String ) return w + 2 * fm.width("\"");
  return w + fm.width(base.data().prefix);
}

//-----------------------------------------------------------------------------
NumberLineEdit::NumberLineEdit(QWidget *parent, const char *name)
  : KLineEdit(parent, name)
{
  connect(this, SIGNAL(textChanged(const QString &)), SLOT(textChangedSlot()));
}

NumberLineEdit::NumberLineEdit(const QString &text, QWidget *parent, const char *name)
  : KLineEdit(text, parent, name)
{
  connect(this, SIGNAL(textChanged(const QString &)), SLOT(textChangedSlot()));
}

QValidator::State validateNumber(const QString &input)
{
  if ( input.isEmpty() ) return QValidator::Intermediate;
  bool ok;
  (void)fromAnyLabel(input, &ok);
  if (ok) return QValidator::Acceptable;
  FOR_EACH(NumberBase, base)
    if ( input==base.data().prefix ) return QValidator::Intermediate;
  if ( input[0]=='\"' ) return QValidator::Intermediate;
  if ( input[0]=='\'' ) return QValidator::Intermediate;
  return QValidator::Invalid;
}

void NumberLineEdit::textChangedSlot()
{
  QValidator::State state = validateNumber(text());
  switch (state) {
    case QValidator::Acceptable: unsetColor(); break;
    case QValidator::Intermediate: setColor(QColor("#FF9900")); break;
    case QValidator::Invalid: setColor(red); break;
  }
}
