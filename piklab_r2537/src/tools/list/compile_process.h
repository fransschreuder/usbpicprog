/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef COMPILE_PROCESS_H
#define COMPILE_PROCESS_H

#include "common/common/qflags.h"
#include "common/global/purl.h"
#include "libgui/log_view.h"
#include "devices/base/hex_buffer.h"
#include "tools/base/tool_group.h"
#include "tools/base/generic_tool.h"
#include "libgui/main_global.h"
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

  class ParseErrorData {
  public:
    ParseErrorData(const QString &p, int iFile, int iLine, int iMessage, int iLogType,
                   Log::LineType dLineType = Log::LineType::Error, Log::LineType dLineTypeEmpty = Log::LineType::Error)
      : pattern(p), indexFile(iFile), indexLine(iLine), indexMessage(iMessage), indexLogType(iLogType),
        defaultLineType(dLineType), defaultLineTypeEmpty(dLineTypeEmpty) {}
    QString pattern;
    int indexFile, indexLine, indexMessage, indexLogType;
    Log::LineType defaultLineType, defaultLineTypeEmpty;
  };

//-----------------------------------------------------------------------------
class LogWidget : public Log::Widget
{
  Q_OBJECT
public:
  LogWidget(QWidget *parent);
  void appendLine(Log::LineType type, const QString &message, const QString &filepath, uint line);
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
class BaseProcess : public QObject
{
Q_OBJECT
public:
  BaseProcess();
  virtual void init(const Data &data, Manager *manager);
  virtual bool check() const = 0;
  virtual FileData::List files(bool *ok) const = 0;
  bool start();

signals:
  void success();
  void failure();

protected:
  Manager *_manager;
  Data    _data;
  ::Process::LineSignal *_process;
  QString _stdout, _stderr;

  const Tool::Group &group() const { return Main::toolGroup(); }
  PURL::Directory directory(uint i = 0) const;
  virtual void setupProcess() = 0;

protected slots:
  virtual void logStdoutLine(const QString &line) { logStderrLine(line); }
  virtual void logStderrLine(const QString &line) = 0;
  virtual void done(int code);
  void timeout();
};

//-----------------------------------------------------------------------------
class Process : public BaseProcess
{
Q_OBJECT
public:
  virtual ~Process();
  virtual void init(const Data &data, Manager *manager);
  virtual bool check() const;
  virtual FileData::List files(bool *ok) const;
  virtual QStringList genericArguments(const Compile::Config &config) const = 0;
  void checkArguments() const;

protected:
  Config  *_config;

  virtual PURL::Url url(PURL::FileType type = PURL::Nb_FileTypes, uint i = 0) const;
  QString filepath(PURL::FileType type, uint i=0) const;
  virtual QString outputFilepath() const { return filepath(PURL::Hex); }
  virtual QString outputFiles() const = 0;
  uint nbFiles() const { return _data.items.count(); }
  virtual QString inputFilepath(uint i) const { return filepath(PURL::Nb_FileTypes, i); }
  virtual QString deviceName() const = 0;
  virtual QString familyName() const { return QString::null; }
  virtual QString objectExtension() const { return QString::null; }
  virtual bool hasLinkerScript() const { return group().hasCustomLinkerScript(_data.project); }
  FileData fileData(PURL::FileType type, FileActions actions) const;
  bool parseErrorLine(const QString &s, const ParseErrorData &data);
  virtual Log::LineType filterType(const QString &type) const;
  void doLog(const QString &type, const QString &message, const QString &surl, uint line);
  void doLog(Log::LineType type, const QString &message, const QString &surl, uint line);
  virtual void setupProcess();
  QStringList arguments() const;
  const Tool::Base *tool() const { return group().base(_data.category); }

private:
  static bool checkIs(const QString &s, const QString &key);
  static QString replaceIf(const QString &s, const QString &key, bool condition);
};

//-----------------------------------------------------------------------------
class CustomProcess : public BaseProcess
{
Q_OBJECT
public:
  CustomProcess(const QString &command) : _command(command) {}
  virtual bool check() const { return true; }
  virtual FileData::List files(bool *ok) const { if (ok) *ok = true; return FileData::List(); }

protected:
  virtual void setupProcess();

protected slots:
  virtual void logStderrLine(const QString &line);

private:
  QString _command;
};

} // namespace

#endif
