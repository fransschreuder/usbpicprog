/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MAIN_GLOBAL_H
#define MAIN_GLOBAL_H

#include <qstring.h>
#include <qpopupmenu.h>
#include <kstdaction.h>

#include "progs/base/generic_prog.h"

class EditorManager;
namespace ProjectManager { class View; }
class Editor;
class Project;
namespace Compile { class LogWidget; class Manager; }
namespace Register { class List; class WatchView; }
namespace Breakpoint { class View; }
class MainWindow;
namespace Programmer { class Group; class Base; }
namespace Device { class Data; }
namespace Tool { class Group; }
class ConsoleView;

class Main
{
public:
  enum State { Idle, Compiling, Programming, Closing };
  static void setState(State state);
  static State state() { return _state; }

  static MainWindow &toplevel() { return *_toplevel; }
  static const Programmer::Group &programmerGroup();
  static Programmer::Base *programmer();
  static Programmer::State programmerState();
  static KAction *action(const char *name);
  static KAction *action(KStdAction::StdAction action);
  static QPopupMenu &popup(const char *name);
  static EditorManager &editorManager() { return *_editorManager; }
  static Editor *currentEditor();
  static QString device();
  static const Device::Data *deviceData();
  static Breakpoint::View &breakpointsView() { return *_breakpointsView; }
  static ProjectManager::View &projectManager() { return *_projectManager; }
  static Project *project();
  static Register::WatchView &watchView() { return *_watchView; }
  static Compile::LogWidget &compileLog() { return *_compileLog; }
  static const Tool::Group &toolGroup();

private:
  static State                 _state;
  static MainWindow           *_toplevel;
  static EditorManager        *_editorManager;
  static ProjectManager::View *_projectManager;
  static Breakpoint::View     *_breakpointsView;
  static Register::WatchView  *_watchView;
  static Compile::LogWidget   *_compileLog;
  static Compile::Manager     *_compileManager;
  static ConsoleView          *_consoleView;

  friend class MainWindow;
};

#endif
