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

#include <qobject.h>

namespace Log
{
  // immediat visibily by calling processEvent... BEWARE of side effects
  enum Action { Immediate, Delayed };
  enum ShowMode { DontShow, Show };
  enum DebugLevel { NormalDebug = 0, ExtraDebug, MaxDebug, LowLevelDebug, Nb_DebugLevels };
  enum Type { Error = 0, SoftError, Warning, Data, Information, Command, Nb_Types };
  struct LogData {
    const char *label, *color;
    bool bold;
  };
  extern const LogData TYPE_DATA[Nb_Types];
  extern const LogData DEBUG_DATA[Nb_DebugLevels+1];

//-----------------------------------------------------------------------------
class Generic
{
public:
  Generic() {}
  virtual ~Generic() {}
  virtual void log(Type type, const QString &text, Action action = Immediate) = 0;
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
  ShowMode showMode(Type type) const { return _modes[type]; }
  void setShowMode(Type type, ShowMode mode) { _modes[type] = mode; }
  void setDebugLevel(DebugLevel level) { _debugLevel = level; }
  void setQuiet();
  virtual void log(Type type, const QString &text, Action action = Immediate);
  virtual void log(DebugLevel level, const QString &text, Action action = Immediate);
  virtual void logUserAbort();

protected:
  ShowMode _modes[Nb_Types];
  DebugLevel _debugLevel;

  virtual void updateDebugLevel() {}
  virtual void doLog(Type type, const QString &text, Action action) = 0;
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

  virtual void doLog(Type, const QString &text, Action) { _s += text + "\n"; }
  virtual void doLog(DebugLevel, const QString &text, Action) { _s += text + "\n"; }
};

//-----------------------------------------------------------------------------
class Proxy : public QObject
{
Q_OBJECT
public:
#if QT_VERSION<0x040000
  Proxy() : QObject(0, "log_proxy") {}
#else
  Proxy() : QObject(0) { setObjectName("log_proxy"); }
#endif

signals:
  void addProgress(uint step);
  void setTotalSteps(uint total);
  void addToTotalSteps(uint add);

private:
  friend class Base;
};

class Base : public Generic
{
public:
  Base(Base *parent = 0);
  virtual ~Base();
  void setParent(Base *parent);
  void setView(View *view);
  View *view() { return logData()->view; }
  Proxy *proxy() { return logData()->proxy; }

  virtual void log(Type type, const QString &message, Action action = Immediate);
  virtual void log(DebugLevel level, const QString &message, Action action = Immediate);
  virtual void appendToLastLine(const QString &text);
  void setError(const QString &error) { logData()->error = error; }
  virtual bool hasError() const { return !logData()->error.isNull(); }
  virtual QString error() const { return logData()->error; }
  virtual void resetError() { logData()->error = QString::null; }
  virtual void addProgress(uint steps) { emit proxy()->addProgress(steps); }
  void setTotalSteps(uint total) { emit proxy()->setTotalSteps(total); }
  virtual void sorry(const QString &message, const QString &details = QString::null);
  virtual bool askContinue(const QString &message);
  virtual void clear();
  void logUserAbort();

protected:
  Base *_parent;
  class LogData {
  public:
    LogData() : proxy(new Proxy), view(0) {}
    ~LogData() { delete proxy; }
    QString error;
    Proxy *proxy;
    View *view;
  };
  LogData *_data;
  LogData *logData() { return _parent ? _parent->logData() : _data; }
  const LogData *logData() const { return _parent ? _parent->logData() : _data; }
};

} // namespace

#endif
