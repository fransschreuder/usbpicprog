/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "log_view.h"

#include <qpopupmenu.h>
#include <qeventloop.h>
#include <qapplication.h>
#include <kiconloader.h>

#include "global_config.h"
#include "common/gui/purl_gui.h"

Log::Widget::Widget(QWidget *parent, const char *name)
  : QTextEdit(parent, name)
{
  setTextFormat(LogText);
  setMinimumWidth(300);
}

void Log::Widget::updateDebugLevel()
{
  GlobalConfig config;
  setDebugLevel(config.debugLevel());
}

void Log::Widget::logExtra(const QString &text)
{
  _text += text;
  GlobalConfig config;
  if ( config.logOutputType()==GuiConsole ) fprintf(stdout, "%s", text.latin1());
}

void Log::Widget::doLog(Type type, const QString &text, Action action)
{
  doLog(text, TYPE_DATA[type].color, TYPE_DATA[type].bold, action);
}

void Log::Widget::doLog(DebugLevel level, const QString &text, Action action)
{
  doLog(text, DEBUG_DATA[level].color, false, action);
}

void Log::Widget::doLog(const QString &text, const QString &color, bool bold, Action action)
{
  logExtra(text + "\n");
  QString s = QString("<font color=%1>").arg(color);
  if (bold) s += "<b>";
  s += escapeXml(text);
  if (bold) s += "</b>";
  s += "</font>";
  QTextEdit::append(s);
  updateContents(); // #### fix bug in Qt (display is messed up)
  ensureVisible(0, contentsHeight());
  if ( action==Immediate)
    QApplication::eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
}

void Log::Widget::appendToLastLine(const QString &text)
{
  logExtra(text);
  uint p = paragraphs() - 1;
  insertAt(escapeXml(text), p, paragraphLength(p));
  updateContents(); // #### fix bug in Qt (display is messed up)
  ensureVisible(0, contentsHeight());
  // immediately visible...
  QApplication::eventLoop()->processEvents(QEventLoop::ExcludeUserInput);
}

QPopupMenu *Log::Widget::createPopupMenu(const QPoint &pos)
{
  updateDebugLevel();
  _popup = QTextEdit::createPopupMenu(pos);
  KIconLoader loader;
  QIconSet iset = loader.loadIconSet("filesave", KIcon::Small, 0);
  _popup->insertItem(iset, "Save As...", this, SLOT(saveAs()));
  iset = loader.loadIconSet("fileclose", KIcon::Small, 0);
  _popup->insertItem(iset, "Clear", this, SLOT(clear()));
  _popup->insertSeparator();
  for (int i=0; i<=Nb_DebugLevels; i++) {
    int k = (i==0 ? Nb_DebugLevels : i-1);
    _id[k] = _popup->insertItem(i18n(DEBUG_DATA[k].label));
    _popup->setItemChecked(_id[k], _debugLevel==k);
  }
  _popup->insertSeparator();
  int id = _popup->insertItem(i18n("Output in console"), this, SLOT(toggleConsoleOutput()));
  GlobalConfig config;
  _popup->setItemChecked(id, config.logOutputType()==GuiConsole);
  connect(_popup, SIGNAL(activated(int)), SLOT(toggleVisible(int)));
  return _popup;
}

void Log::Widget::toggleVisible(int id)
{
  for (uint i=0; i<=Nb_DebugLevels; i++) {
    if ( _id[i]==id ) {
      _debugLevel = DebugLevel(i);
      GlobalConfig config;
      config.writeDebugLevel(DebugLevel(i));
      break;
    }
  }
}

void Log::Widget::toggleConsoleOutput()
{
  GlobalConfig config;
  config.writeLogOutputType(config.logOutputType()==GuiOnly ? GuiConsole : GuiOnly);
}

void Log::Widget::sorry(const QString &message, const QString &details)
{
  logExtra(message + " [" + details + "]\n");
  MessageBox::detailedSorry(message, details, Log::Show);
}

bool Log::Widget::askContinue(const QString &message)
{
  bool ok = MessageBox::askContinue(message);
  logExtra(message + " [" + (ok ? "continue" : "cancel") + "]\n");
  return ok;
}

void Log::Widget::clear()
{
  QTextEdit::clear();
  _text = QString::null;
}

void Log::Widget::saveAs()
{
  PURL::Url url = PURL::getSaveUrl(":save_log", "text/x-log", this, i18n("Save log to file"), PURL::AskOverwrite);
  if ( url.isEmpty() ) return;
  url.write(_text, this);
}
