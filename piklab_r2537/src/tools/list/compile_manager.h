/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef COMPILE_MANAGER_H
#define COMPILE_MANAGER_H

#include "compile_process.h"

namespace Compile
{

class Manager : public QObject, public Log::Base
{
Q_OBJECT
public:
  enum Operation { NoOperation = 0, Clean = 1, CompileOnly = 2, Build = 4 };
  Q_DECLARE_FLAGS(Operations, Operation)

public:
  Manager(QObject *parent);
  QString label() const { return _label; }
  bool compileFile(const TodoItem &item);
  void cleanFile(const PURL::Url &url);
  bool buildProject(LinkType type);
  void cleanProject(LinkType type);
  void kill();
  bool compileOnly() const { return (_operations & CompileOnly); }
  void processDone();
  void processFailed();
  void log(Log::LineType type, const QString &message, const QString &filepath = QString::null, uint line = 0);
  void log(Log::DebugLevel debug, const QString &message, const QString &filepath = QString::null, uint line = 0);

signals:
  void success();
  void failure();
  void updateFile(const Compile::FileData &fdata);

private slots:
  void start();

private:
  Operations _operations;
  enum Action { Compiling, Linking, BinToHex };
  Action     _action;
  QValueList<TodoItem> _todo, _items;
  BaseProcess *_base;
  QString    _label;
  LinkType   _type;
  bool       _wholeProject;
  uint       _customCommandIndex;

  bool startProject();
  bool clearAll();
  bool setupFile(Operations operations, const TodoItem &item);
  bool setupProject(Operations operations, LinkType type);
  void setup(Tool::Category category);
  bool setupCompile();
  bool setupAssemble();
  bool setupLink();
  bool setupBinToHex();
  bool prepareAction();
  bool needs(Tool::Category category) const;
  void startCustomCommand();
  void executeCustomCommands();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Manager::Operations)

} // namespace

#endif
