/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "log.h"

#include <qeventloop.h>
#include "global.h"

//-----------------------------------------------------------------------------
const Log::LogData Log::TYPE_DATA[Nb_Types] = {
  { 0, "red",    false }, // error
  { 0, "red",    false }, // soft error
  { 0, "orange", false }, // warning
  { 0, "black",  false }, // data
  { 0, "blue",   false }, // info
  { 0, "black",  true  }, // command
};

const Log::LogData Log::DEBUG_DATA[Nb_DebugLevels+1] = {
  { I18N_NOOP("Normal debug messages"), "darkGreen", false },
  { I18N_NOOP("Extra debug messages"),  "darkGreen", false },
  { I18N_NOOP("Max debug messages"),    "darkGreen", false },
  { I18N_NOOP("All debug messages"),    "darkGreen", false },
  { I18N_NOOP("No debug message"),      0,           false }
};

//-----------------------------------------------------------------------------
Log::View::View()
  : _debugLevel(Nb_DebugLevels)
{
 for (uint i=0; i<Nb_Types; i++) _modes[i] = Show;
}

void Log::View::setQuiet()
{
  _debugLevel = Nb_DebugLevels;
  for (uint i=0; i<Nb_Types; i++) _modes[i] = DontShow;
}

void Log::View::log(Type type, const QString &text, Action action)
{
  if ( _modes[type]==DontShow || text.isEmpty() ) return;
  doLog(type, text, action);
}

void Log::View::log(DebugLevel level, const QString &text, Action action)
{
  Q_ASSERT( level!=Nb_DebugLevels );
  updateDebugLevel();
  if ( _debugLevel==Nb_DebugLevels || level>_debugLevel || text.isEmpty() ) return;
  doLog(level, text, action);
}

void Log::View::logUserAbort()
{
  doLog(Log::SoftError, i18n("Operation aborted by user."), Immediate);
}

//-----------------------------------------------------------------------------
void Log::StringView::sorry(const QString &message, const QString &details)
{
  if ( details.isEmpty() ) _s += message;
  else _s += message + ": " + details;
}

bool Log::StringView::askContinue(const QString &message)
{
  log(Warning, message, Immediate);
  return false; // always fail
}

//-----------------------------------------------------------------------------
Log::Base::Base(Base *parent)
  : _parent(0), _data(0)
{
  setParent(parent);
}

void Log::Base::setParent(Base *parent)
{
  delete _data;
  _parent = parent;
  _data = (parent ? 0 : new LogData);
}

Log::Base::~Base()
{
  delete _data;
}

void Log::Base::setView(View *view)
{
  Q_ASSERT(_data);
  _data->view = view;
}

void Log::Base::logUserAbort()
{
  if ( view() ) view()->logUserAbort();
}

void Log::Base::log(Type type, const QString &message, Action action)
{
  if ( type==Error ) setError(message);
  if ( view() ) view()->log(type, message, action);
}

void Log::Base::log(DebugLevel level, const QString &message, Action action)
{
  if ( view() ) view()->log(level, message, action);
}

void Log::Base::appendToLastLine(const QString &text)
{
  if ( view() ) view()->appendToLastLine(text);
}

void Log::Base::sorry(const QString &message, const QString &details)
{
  if ( view() ) view()->sorry(message, details);
}

bool Log::Base::askContinue(const QString &message)
{
  if ( view()==0 ) return false;
  return view()->askContinue(message);
}

void Log::Base::clear()
{
  resetError();
  if ( view() ) view()->clear();
}
