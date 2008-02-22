/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef NUMBER_GUI_H
#define NUMBER_GUI_H

#include <qvalidator.h>
#include <klineedit.h>

#include "common/common/number.h"

//-----------------------------------------------------------------------------
extern uint maxCharWidth(const QString &s, const QFont &font);
extern uint maxCharWidth(NumberBase base, const QFont &font);
inline uint maxStringWidth(NumberBase base, uint nbChars, const QFont &font) { return nbChars * maxCharWidth(base, font); }
extern uint maxLabelWidth(NumberBase base, uint nbChars, const QFont &font);

extern QValidator::State validateNumber(const QString &s);

//-----------------------------------------------------------------------------
class NumberLineEdit : public KLineEdit
{
Q_OBJECT
public:
  NumberLineEdit(QWidget *parent = 0, const char *name = 0);
  NumberLineEdit(const QString &text, QWidget *parent = 0, const char *name = 0);
  uint value(bool *ok = 0) const { return fromAnyLabel(text(), ok); }
  void setColor(const QColor &color) { setPaletteForegroundColor(color); }
  void unsetColor() { unsetPalette(); }

private slots:
  void textChangedSlot();
};

#endif
