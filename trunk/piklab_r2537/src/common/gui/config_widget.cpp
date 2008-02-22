/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2004 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "config_widget.h"

//----------------------------------------------------------------------------
GridConfigWidget::GridConfigWidget(QWidget *parent, const char *name)
  : ConfigWidget(parent, name)
{
  QGridLayout *top = new QGridLayout(this, 1, 1, 10, 10);
  top->setRowStretch(1, 1);
  _topLayout = new QGridLayout(1, 1, 10);
  top->addLayout(_topLayout, 0, 0);
}

//----------------------------------------------------------------------------
TabConfigWidget::TabConfigWidget(QWidget *parent, const char *name)
  : ConfigWidget(parent, name)
{
  QGridLayout *top = new QGridLayout(this, 1, 1, 0, 10);
  _tabWidget = new QTabWidget(this);
  top->addWidget(_tabWidget, 0, 0);
}
