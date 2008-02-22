/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef LOG_H
#define LOG_H

#include <qstringlist.h>

#include "common/common/key_enum.h"

namespace Log
{
  // immediat visibily by calling processEvent... BEWARE of side effects
  enum Action { Immediate, Delayed };
  enum ShowMode { DontShow, Show };
  struct LogData {
    const char *key, *label, *color;
    bool bold;
  };
  BEGIN_DECLARE_ENUM(DebugLevel)
    Quiet = 0, Normal, Extra, Max, LowLevel
  END_DECLARE_ENUM(DebugLevel, LogData)
  BEGIN_DECLARE_ENUM(LineType)
    Error = 0, SoftError, Warning, Normal, Information, Command
  END_DECLARE_ENUM(LineType, LogData)

//-----------------------------------------------------------------------------
class Generic
{
public:
  virtual ~Generic() {}
  virtual void log(LineType type, const QString &text, Action action = Immediate) = 0;
  virtual void log(DebugLevel level, const QString &text, Action action = Immediate) = 0;
  virtual void appendToLastLine(const QString &text) = 0;
  virtual void sorry(const QString &message, const QString &details) = 0;
  virtual bool askContinue(const QString &message) = 0;
  virtual void clear() = 0;
  virtual void logUserAbort() = 0;
};

class View : public Generic
{
public:
  View();
  ShowMode showMode(LineType type) const { return _modes[type.type()]; }
  void setShowMode(LineType type, ShowMode mode) { _modes[type.type()] = mode; }
  void setDebugLevel(DebugLevel level);
  virtual void log(LineType type, const QString &text, Action action = Immediate);
  virtual void log(DebugLevel level, const QString &text, Action action = Immediate);
  virtual void logUserAbort();

protected:
  ShowMode _modes[LineType::Nb_Types];
  DebugLevel _debugLevel;

  virtual void updateDebugLevel() {}
  virtual void doLog(LineType type, const QString &text, Action action) = 0;
  virtual void doLog(DebugLevel level, const QString &text, Action action) = 0;
};

//-----------------------------------------------------------------------------
class StringView : public View
{
public:
  StringView() {}
  QString string() const { return _s; }
  virtual void appendToLastLine(const QString &text) { _s += text; }
  virtual void clear() { _s = QString::null; }
  virtual void sorry(const QString &message, const QString &details);
  virtual bool askContinue(const QString &message);

private:
  QString _s;

  virtual void doLog(LineType, const QString &text, Action) { _s += text + "\n"; }
  virtual void doLog(DebugLevel, const QString &text, Action) { _s += text + "\n"; }
};

//-----------------------------------------------------------------------------
class Base : public Generic
{
public:
  Base(Base *parent = 0);
  virtual ~Base();
  void setParent(Base *parent);
  void setView(View *view);
  View *view() { return logData()->view; }

  virtual void log(LineType type, const QString &message, Action action = Immediate);
  virtual void log(DebugLevel level, const QString &message, Action action = Immediate);
  virtual void appendToLastLine(const QString &text);
  void setError(const QString &error) { logData()->error = error; }
  virtual bool hasError() const { return !logData()->error.isNull(); }
  virtual QString error() const { return logData()->error; }
  virtual void resetError() { logData()->error = QString::null; }
  virtual void sorry(const QString &message, const QString &details = QString::null);
  virtual bool askContinue(const QString &message);
  virtual void clear();
  void logUserAbort();

protected:
  Base *_parent;
  class LogData {
  public:
    LogData() : view(0) {}
    QString error;
    View *view;
  };
  LogData *_data;
  LogData *logData() { return _parent ? _parent->logData() : _data; }
  const LogData *logData() const { return _parent ? _parent->logData() : _data; }
};

class KeyList {
public:
  KeyList(const QString &title = QString::null) : _title(title) {}
  void setTitle(const QString &title) { _title = title; }
  void append(const QString &key, const QString &label) { _keys += key; _labels += label; }
  QString text() const;
  void display(Generic &log) const;

private:
  QString     _title;
  QStringList _keys, _labels;
};

} // namespace

#endif
