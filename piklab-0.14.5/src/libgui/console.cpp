/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "console.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qdir.h>
#include <klibloader.h>
#include <klocale.h>
#include <kparts/part.h>
#include <kde_terminal_interface.h>

ConsoleView::ConsoleView(QWidget *parent)
  : QWidget(parent, "console_view"), _initialized(false)
{}

void ConsoleView::showEvent(QShowEvent *e)
{
  if ( !_initialized ) {
    _initialized = true;
    KLibFactory *factory = KLibLoader::self()->factory("libkonsolepart");
    QVBoxLayout *top = new QVBoxLayout(this, 0, 10);
    if ( factory==0 ) {
      QLabel *label = new QLabel(i18n("Could not find \"konsolepart\"; please install kdebase."), this);
      label->show();
      top->addWidget(label);
      return;
    } else {
      QWidget *pwidget = static_cast<KParts::Part *>(factory->create(this, "konsole"))->widget();
      pwidget->show();
      top->addWidget(pwidget);
      setFocusProxy(pwidget);
    }
  }
  QWidget::showEvent(e);
}
