/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef LOG_VIEW_H
#define LOG_VIEW_H

#include <qtextedit.h>
#include "common/global/log.h"

namespace Log
{
enum OutputType { GuiOnly = 0, GuiConsole, Nb_OutputTypes };

class Widget : public QTextEdit, public View
{
  Q_OBJECT
public:
  Widget(QWidget *parent = 0, const char *name = 0);
  virtual void appendToLastLine(const QString &text);
  virtual void clear();
  virtual void sorry(const QString &message, const QString &details);
  virtual bool askContinue(const QString &message);

protected:
  QPopupMenu *createPopupMenu(const QPoint &pos);

private slots:
  void toggleVisible(int i);
  void toggleConsoleOutput();
  void saveAs();

private:
  int         _id[DebugLevel::Nb_Types];
  QPopupMenu *_popup;
  QString     _text;

  virtual void updateDebugLevel();
  virtual void doLog(LineType type, const QString &text, Action action = Immediate);
  virtual void doLog(DebugLevel level, const QString &text, Action action = Immediate);
  void doLog(const QString &text, const QString &color, bool bold, Action action = Immediate);
  void logExtra(const QString &text);
};

} // namespace

#endif
