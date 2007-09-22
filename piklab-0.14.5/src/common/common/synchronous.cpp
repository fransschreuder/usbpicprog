/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "synchronous.h"

#include "common/global/global.h"
#if QT_VERSION<0x040000
#  include <qwidget.h>
#endif

Synchronous::Synchronous(uint timeout)
{
  connect(&_timer, SIGNAL(timeout()), SLOT(done()));
  if (timeout) _timer.start(timeout, true);
#if QT_VERSION>=0x040000
  _loop = new QEventLoop(this);
#endif
}

#if QT_VERSION<0x040000
// uplifted from kdelibs...
void qt_enter_modal(QWidget *widget);
void qt_leave_modal(QWidget *widget);
#endif

bool Synchronous::enterLoop()
{
#if QT_VERSION<0x040000
  QWidget *dummy = 0;
  if ( qApp->type()!=QApplication::Tty ) {
    dummy = new QWidget(0, 0, WType_Dialog | WShowModal);
    dummy->setFocusPolicy(QWidget::NoFocus);
    qt_enter_modal(dummy);
  }
  QCoreApplication::eventLoop()->enterLoop();
  if ( qApp->type()!=QApplication::Tty ) {
    qt_leave_modal(dummy);
    delete dummy;
  }
#else
  _loop->exec();
#endif
  return _timer.isActive();
}

void Synchronous::done()
{
#if QT_VERSION<0x040000
  QApplication::eventLoop()->exitLoop();
#else
  _loop->exit();
#endif
}
