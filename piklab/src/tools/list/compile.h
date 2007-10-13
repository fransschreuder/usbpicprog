/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef COMPILE_H
#define COMPILE_H

#include "common/common/qflags.h"

#include "common/global/purl.h"
#include "libgui/log_view.h"
#include "devices/base/hex_buffer.h"
#include "tools/base/tool_group.h"
#include "tools/base/generic_tool.h"
#include "libgui/main_global.h"
class MainWindow;
namespace Process { class LineSignal; }

namespace Compile
{
  class Manager;
  enum FileAction { NoAction = 0, Show = 1, InProject = 2, Generated = 8, Included = 16 };
  Q_DECLARE_FLAGS(FileActions, FileAction)
  Q_DECLARE_OPERATORS_FOR_FLAGS(FileActions)

  class FileData {
  public:
    FileData() {}
    FileData(const PURL::Url &u, FileActions a) : url(u), actions(a) {}
    PURL::Url url;
    FileActions actions;
    class List : public QValueList<FileData> {
    public:
      List() {}
      List(const FileData &data) { append(data); }
      List onlyExistingFiles() const;
      void cleanGenerated() const;
    };
  };

  struct ArgumentData {
    const char *key, *description;
  };
  extern const ArgumentData ARGUMENT_DATA[];

//-----------------------------------------------------------------------------
class LogWidget : public Log::Widget
{
  Q_OBJECT
public:
  LogWidget(QWidget *parent);
  void appendLine(Log::Type type, const QString &message, const QString &filepath, uint line);
  void appendLine(Log::DebugLevel debug, const QString &message, const QString &filepath, uint line);
  virtual void clear();

private slots:
  void lineClicked(int line);

private:
  class Data {
  public:
    Data() {}
    Data(const QString &fp, uint l) : filepath(fp), line(l) {}
    QString filepath;
    uint line;
  };
  QMap<uint, Data> _map;
};

//-----------------------------------------------------------------------------
class Process : public QObject
{
Q_OBJECT
public:
  Process();
  void init(const Data &data, Manager *manager);
  virtual ~Process();
  virtual bool check() const;
  virtual bool start();
  virtual FileData::List files(bool *ok) const;
  virtual QStringList genericArguments(const Compile::Config &config) const = 0;
  void checkArguments() const;

signals:
  void success();
  void failure();

protected:
  Config  *_config;
  Data     _data;
  Manager *_manager;

  virtual PURL::Url url(PURL::FileType type = PURL::Nb_FileTypes, uint i = 0) const;
  PURL::Directory directory(uint i = 0) const;
  QString filepath(PURL::FileType type, uint i=0) const;
  virtual QString outputFilepath() const { return filepath(PURL::Hex); }
  virtual QString outputFiles() const = 0;
  uint nbFiles() const { return _data.items.count(); }
  virtual QString inputFilepath(uint i) const { return filepath(PURL::Nb_FileTypes, i); }
  virtual QString deviceName() const = 0;
  virtual QString familyName() const { return QString::null; }
  virtual QString objectExtension() const { return QString::null; }
  virtual bool hasLinkerScript() const { return Main::toolGroup().hasCustomLinkerScript(_data.project); }
  FileData fileData(PURL::FileType type, FileActions actions) const;
  virtual Log::Type filterType(const QString &type) const;
  void doLog(const QString &type, const QString &message, const QString &surl, uint line);
  void doLog(Log::Type type, const QString &message, const QString &surl, uint line);

  QStringList arguments() const;
  const Tool::Base *tool() const { return Main::toolGroup().base(_data.category); }
  QString _stdout, _stderr;

protected slots:
  virtual void logStdoutLine(const QString &line) { logStderrLine(line); }
  virtual void logStderrLine(const QString &line) = 0;
  virtual void done(int code);
  void timeout();

private:
  ::Process::LineSignal *_process;

  static bool checkIs(const QString &s, const QString &key);
  static QString replaceIf(const QString &s, const QString &key, bool condition);
};

//-----------------------------------------------------------------------------
class Manager : public QObject, public Log::Base
{
Q_OBJECT
public:
  enum Operation { NoOperation = 0, Clean = 1, CompileOnly = 2, Build = 4 };
  Q_DECLARE_FLAGS(Operations, Operation)

public:
  Manager(QObject *parent);
  QString label() const { return _label; }
  void compileFile(const TodoItem &item);
  void cleanFile(const PURL::Url &url);
  void buildProject(LinkType type);
  void cleanProject(LinkType type);
  void kill();
  bool compileOnly() const { return (_operations & CompileOnly); }
  void processDone();
  void processFailed();
  void log(Log::Type type, const QString &message, const QString &filepath = QString::null, uint line = 0);
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
  Process   *_base;
  QString    _label;
  LinkType   _type;

  bool startProject();
  bool clearAll();
  void setupFile(Operations operations, const TodoItem &item);
  void setupProject(Operations operations, LinkType type);
  void setup(Tool::Category category);
  bool setupCompile();
  bool setupAssemble();
  bool setupLink();
  bool setupBinToHex();
  bool prepareAction();
  bool needs(Tool::Category category) const;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Manager::Operations)

} // namespace

#endif
