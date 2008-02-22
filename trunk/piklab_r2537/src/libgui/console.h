/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CONSOLE_H
#define CONSOLE_H

#include <qwidget.h>

class ConsoleView : public QWidget
{
Q_OBJECT
public:
  ConsoleView(QWidget *parent);

protected:
  virtual void showEvent(QShowEvent *e);

private:
  bool _initialized;
};

#endif
