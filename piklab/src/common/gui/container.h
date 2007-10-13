/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CONTAINER_H
#define CONTAINER_H

#include <qframe.h>
#include <qwidgetstack.h>
#include <qtabwidget.h>
#include <qguardedptr.h>

#include "misc_gui.h"

//----------------------------------------------------------------------------
class Container : public QFrame
{
Q_OBJECT
public:
  enum Type { Flat, Sunken };
  Container(QWidget *parent = 0, Type type = Flat, const char *name = 0);
  Container(QWidgetStack *stack, uint index, Type type = Flat, const char *name = 0);
  Container(QTabWidget *tabw, const QString &title, Type type = Flat, const char *name = 0);
  void addWidget(QWidget *widget, uint row, uint col, int alignment = 0);
  void addMultiCellWidget(QWidget *widget, uint startRow, uint endRow,
                          uint startCol, uint endCol, int alignment = 0);
  uint numRows() const { return _gridLayout->numRows(); }
  uint numCols() const { return _gridLayout->numCols(); }
  void setFrame(Type type);
  void setMargin(uint margin) { _topLayout->setMargin(margin); }
  void setRowSpacing(uint row, uint spacing) { _gridLayout->setRowSpacing(row, spacing); }
  void setColSpacing(uint col, uint spacing) { _gridLayout->setColSpacing(col, spacing); }
  void setRowStretch(uint row, uint stretch) { _gridLayout->setRowStretch(row, stretch); }
  void setColStretch(uint col, uint stretch) { _gridLayout->setColStretch(col, stretch); }

private:
  Type         _type;
  QGridLayout *_topLayout, *_gridLayout;

  void initLayout();
};

//----------------------------------------------------------------------------
class ButtonContainer : public Container
{
Q_OBJECT
public:
  ButtonContainer(const QString &title, QWidget *parent);
  PopupButton &button() { return *_button; }

private:
  PopupButton *_button;
};

#endif
