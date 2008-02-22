/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SYNCHRONOUS_H
#define SYNCHRONOUS_H

#include <qtimer.h>
#include <qeventloop.h>

class Synchronous : public QObject
{
Q_OBJECT
public:
  Synchronous(uint timeout = 0); // timeout is ms (0 == no timeout)
  bool enterLoop(); // return false on timeout

public slots:
  void done();

private:
  QTimer _timer;
#if QT_VERSION>=0x040000
  QEventLoop *_loop;
#endif
};

#endif
