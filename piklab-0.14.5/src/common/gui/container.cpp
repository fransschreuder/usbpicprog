/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "container.h"

#include <qobjectlist.h>

//----------------------------------------------------------------------------
Container::Container(QWidget *parent, Type type, const char *name)
  : QFrame(parent, name), _type(type)
{
  initLayout();
}

Container::Container(QWidgetStack *stack, uint index, Type type,
                     const char *name)
  : QFrame(stack, name), _type(type)
{
  initLayout();
  stack->addWidget(this, index);
}

Container::Container(QTabWidget *tabw, const QString &title, Type type,
                     const char *name)
  : QFrame(tabw, name), _type(type)
{
  initLayout();
  tabw->addTab(this, title);
}

void Container::setFrame(Type type)
{
  _type = type;
  switch (type) {
  case Flat:
    setMargin(parent() && parent()->inherits("QTabWidget") ? 10 : 0);
    setFrameStyle(QFrame::NoFrame);
    break;
  case Sunken:
    setMargin(10);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    break;
  }
}

void Container::initLayout()
{
  _topLayout = new QGridLayout(this, 1, 1, 0, 10);
  _gridLayout = new QGridLayout(1, 1, 10);
  _topLayout->addLayout(_gridLayout, 0, 0);
  _topLayout->setRowStretch(1, 1);
  setFrame(_type);
}

void Container::addWidget(QWidget *w, uint row, uint col, int alignment)
{
  w->show();
  _gridLayout->addWidget(w, row, col, alignment);
}

void Container::addMultiCellWidget(QWidget *w, uint startRow, uint endRow,
                                   uint startCol, uint endCol, int alignment)
{
  Q_ASSERT( startRow<=endRow );
  Q_ASSERT( startCol<=endCol );
  w->show();
  _gridLayout->addMultiCellWidget(w, startRow, endRow, startCol, endCol, alignment);
}

//----------------------------------------------------------------------------
ButtonContainer::ButtonContainer(const QString &title, QWidget *parent)
  : Container(parent, Sunken)
{
  _button = new PopupButton(title, this);
  addMultiCellWidget(_button, 0,0, 0,1);
  setColStretch(2, 1);
}
