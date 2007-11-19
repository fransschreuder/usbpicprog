/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "compile_process.h"

#include <qtimer.h>
#include <qregexp.h>

#include "devices/list/device_list.h"
#include "common/global/process.h"
#include "common/gui/misc_gui.h"
#include "libgui/text_editor.h"
#include "libgui/editor_manager.h"
#include "libgui/project.h"
#include "compile_config.h"
#include "compile_manager.h"

//-----------------------------------------------------------------------------
const Compile::ArgumentData Compile::ARGUMENT_DATA[] = {
  { "$(SRCPATH)",  I18N_NOOP("Replaced by the source directory.") },
  { "$LKR(xxx)",   I18N_NOOP("Replaced by \"xxx\" when there is a custom linker script.") },
  { "$WINE(xxx)",  I18N_NOOP("Replaced by \"xxx\" when \"wine\" is used.") },
  { "$NO_AUTO_DEVICE(xxx)", I18N_NOOP("Replaced by \"xxx\" when the device name is specified.") },
  { "%OBJS",       I18N_NOOP("Replaced by the list of additionnal objects.") },
  { "%LIBS",       I18N_NOOP("Replaced by the list of additionnal libraries.") },
  { "%O",          I18N_NOOP("Replaced by the output filename.") },
  { "%PROJECT",    I18N_NOOP("Replaced by the project name.") },
  { "%COFF",       I18N_NOOP("Replaced by the COFF filename.") },
  { "%MAP",        I18N_NOOP("Replaced by the map filename.") },
  { "%SYM",        I18N_NOOP("Replaced by the symbol filename.") },
  { "%LIST",       I18N_NOOP("Replaced by the list filename.") },
  { "%DEVICE",     I18N_NOOP("Replaced by the device name.") },
  { "%FAMILY",     I18N_NOOP("Replaced by the device family name (when needed).") },
  { "%I",          I18N_NOOP("Replaced by the relative input filepath(s).") },
  { "%OBJECT",     I18N_NOOP("Replaced by the object file name.") },
  { "%LKR_PATH",   I18N_NOOP("Replaced by the linker script path.") },
  { "%LKR_NAME",   I18N_NOOP("Replaced by the linker script basename.") },
  { "%LKR",        I18N_NOOP("Replaced by the linker script filename.") },
  { "%SEP",        I18N_NOOP("Replaced by a separation into two arguments.") },
  { 0, 0 }
};

//-----------------------------------------------------------------------------
Compile::FileData::List Compile::FileData::List::onlyExistingFiles() const
{
  List list;
  List::const_iterator it;
  for (it=begin(); it!=end(); ++it) {
    FileData data = *it;
    if ( PURL::findExistingUrl(data.url) ) list.append(data);
  }
  return list;
}

void Compile::FileData::List::cleanGenerated() const
{
  Log::StringView sview;
  List::const_iterator it;
  for (it=begin(); it!=end(); ++it)
    if ( (*it).actions & Generated ) (*it).url.del(sview);
}

//-----------------------------------------------------------------------------
Compile::LogWidget::LogWidget(QWidget *parent)
  : Log::Widget(parent, "compile_log")
{
  connect(this, SIGNAL(clicked(int, int)), SLOT(lineClicked(int)));
}

void Compile::LogWidget::clear()
{
  Log::Widget::clear();
  _map.clear();
}

void Compile::LogWidget::appendLine(Log::LineType type, const QString &message, const QString &filepath, uint line)
{
  log(type, message, Log::Delayed);
  if ( !filepath.isEmpty() ) _map[paragraphs()-1] = Data(filepath, line);
}

void Compile::LogWidget::appendLine(Log::DebugLevel level, const QString &message, const QString &filepath, uint line)
{
  log(level, message, Log::Delayed);
  if ( !filepath.isEmpty() ) _map[paragraphs()-1] = Data(filepath, line);
}

void Compile::LogWidget::lineClicked(int line)
{
  if ( !_map.contains(line) ) return;
  PURL::Url url = PURL::Url::fromPathOrUrl(_map[line].filepath);
  TextEditor *e = ::qt_cast<TextEditor *>(Main::editorManager().openEditor(url));
  if ( e==0 ) return;
  e->setCursor(_map[line].line, 0);
}

//-----------------------------------------------------------------------------
Compile::BaseProcess::BaseProcess()
  : QObject(0, "compile_process"), _manager(0), _process(0)
{}

void Compile::BaseProcess::init(const Data &data, Manager *manager)
{
  _data = data;
  _manager = manager;
}

PURL::Directory Compile::BaseProcess::directory(uint i) const
{
  if (_data.project) return _data.project->directory();
  Q_ASSERT( i<_data.items.count() );
  return _data.items[i].url.directory();
}

bool Compile::BaseProcess::start()
{
  _stdout = QString::null;
  _stderr = QString::null;
  delete _process;
  _process = new ::Process::LineSignal;
  connect(_process, SIGNAL(done(int)), SLOT(done(int)));
  connect(_process, SIGNAL(timeout()), SLOT(timeout()));
  connect(_process, SIGNAL(logStdoutLine(const QString &)), SLOT(logStdoutLine(const QString &)));
  connect(_process, SIGNAL(logStderrLine(const QString &)), SLOT(logStderrLine(const QString &)));
  _process->setWorkingDirectory(directory().path());
  setupProcess();
  _manager->log(Log::LineType::Command, _process->arguments().join(" "));
  return _process->start(0); // no timeout
}

void Compile::BaseProcess::done(int code)
{
  if ( code!=0 ) {
    _manager->log(Log::LineType::Error, i18n("*** Exited with status: %1 ***").arg(code));
    _manager->processFailed();
  } else if ( _manager->hasError() ) {
    _manager->log(Log::LineType::Error, i18n("*** Error ***"));
    _manager->processFailed();
  } else _manager->processDone();
}

void Compile::BaseProcess::timeout()
{
  _manager->log(Log::LineType::Error, i18n("*** Timeout ***"));
  _manager->processFailed();
}

//-----------------------------------------------------------------------------
void Compile::Process::init(const Data &data, Manager *manager)
{
  BaseProcess::init(data, manager);
  _config = group().createConfig(const_cast<Project *>(data.project));
}

Compile::Process::~Process()
{
  delete _config;
}

bool Compile::Process::check() const
{
  return group().check(_data.device, _manager);
}

PURL::Url Compile::Process::url(PURL::FileType type, uint i) const
{
  PURL::Url url;
  if ( _data.project && (type==PURL::Hex || _data.category==Tool::Category::Linker || _data.category==Tool::Category::BinToHex) )
    url = _data.project->url().toFileType(PURL::Hex);
  else {
    Q_ASSERT( i<_data.items.count() );
    url = _data.items[i].url;
  }
  if ( type==PURL::Nb_FileTypes ) return url;
  return url.toFileType(type);
}

QString Compile::Process::filepath(PURL::FileType type, uint i) const
{
  return url(type, i).relativeTo(directory(), Compile::Config::withWine(group()) ? PURL::WindowsSeparator : PURL::UnixSeparator);
}

Compile::FileData Compile::Process::fileData(PURL::FileType type, FileActions actions) const
{
  return FileData(url(type, nbFiles()-1), actions);
}

Compile::FileData::List Compile::Process::files(bool *ok) const
{
  if (ok) *ok = true;
  FileData::List list;
  QRegExp rexp("PURL::(.*)");
  QStringList files = QStringList::split(" ", outputFiles());
  for (uint i=0; i<files.count(); i++) {
    if ( rexp.exactMatch(files[i]) ) {
      PURL::FileType type = PURL::FileType::fromKey(rexp.cap(1));
      if ( type==PURL::Nb_FileTypes ) {
        if (ok) *ok = false;
        qWarning("Unknown PURL::FileType in file list for %s", _manager->label().latin1());
        continue;
      }
      if ( type.data().group==PURL::LinkerScript ) {
        PURL::Url lkr = Main::toolGroup().linkerScript(_data.project, _data.linkType);
        list += FileData(lkr, Included | InProject);
      } else {
        FileActions actions = Generated;
        if ( type.data().group==PURL::Source || type==PURL::Hex
             || type==PURL::Map || type==PURL::Coff ) actions |= InProject;
        if ( type==PURL::Hex && _data.project==0 ) actions |= Show;
        list += fileData(type, actions);
      }
    } else list += FileData(url().toExtension(files[i]), Compile::Generated);
  }
  return list;
}

bool Compile::Process::checkIs(const QString &s, const QString &key)
{
  if ( !s.contains(key) ) return false;
  if ( s!=key ) qWarning("Argument should be only %s, the rest will be ignored...", key.latin1());
  return true;
}

QString Compile::Process::replaceIf(const QString &s, const QString &key, bool condition)
{
  QRegExp rexp("(.*)\\$" + key + "\\(([^)]*)\\)(.*)");
  if ( !rexp.exactMatch(s) ) return s;
  return rexp.cap(1) + (condition ? rexp.cap(2) : QString::null) + rexp.cap(3);
}

QStringList Compile::Process::arguments() const
{
  bool custom = _config->hasCustomArguments(_data.category);
  bool withWine = Compile::Config::withWine(group());
  QStringList args = (custom ? _config->customArguments(_data.category) : genericArguments(*_config));
  PURL::Url lkr = Main::toolGroup().linkerScript(_data.project, _data.linkType);
  QStringList nargs;
  for (uint i=0; i<args.count(); i++) {
    if ( args[i].contains("$(SRCPATH)") ) {
      args[i].replace("$(SRCPATH)", directory().path());
      args[i].replace("//", "/");
    }
    args[i] = replaceIf(args[i], "WINE", withWine);
    args[i] = replaceIf(args[i], "LKR", hasLinkerScript());
    args[i] = replaceIf(args[i], "NO_AUTO_DEVICE", _data.device!=Device::AUTO_DATA.name);
    if ( checkIs(args[i], "%OBJS") ) { // before %O
      if (_data.project) nargs += _data.project->objectsForLinker(objectExtension(), withWine);
      else {
        PURL::Url tmp = url(PURL::Object);
        if ( !objectExtension().isEmpty() ) tmp = tmp.toExtension(objectExtension());
        nargs += tmp.relativeTo(directory(), withWine ? PURL::WindowsSeparator : PURL::UnixSeparator);
      }
      continue;
    }
    if ( checkIs(args[i], "%LIBS") ) {
      if (_data.project) nargs += _data.project->librariesForLinker(QString::null, withWine);
      continue;
    }
    args[i].replace("%OBJECT", filepath(PURL::Object)); // before %O
    args[i].replace("%O", outputFilepath());
    args[i].replace("%COFF", filepath(PURL::Coff));
    if (_data.project) args[i].replace("%PROJECT", _data.project->name());
    else  args[i].replace("%PROJECT", url().basename());
    args[i].replace("%MAP", filepath(PURL::Map));
    args[i].replace("%SYM", url().toExtension("sym").relativeTo(directory(), withWine ? PURL::WindowsSeparator : PURL::UnixSeparator));
    args[i].replace("%LIST", filepath(PURL::Lst));
    args[i].replace("%DEVICE", deviceName());
    args[i].replace("%FAMILY", familyName());
    args[i].replace("%LKR_PATH", lkr.path()); // before %LKR
    args[i].replace("%LKR_NAME", lkr.filename()); // before LKR
    args[i].replace("%LKR", lkr.filepath());
    if ( checkIs(args[i], "%I") ) {
      for (uint k=0; k<nbFiles(); k++) nargs += inputFilepath(k);
      continue;
    }
    if ( !args[i].isEmpty() ) nargs += args[i];
  }
  args.clear();
  for (uint i=0; i<nargs.count(); i++) args += QStringList::split("%SEP", nargs[i]);
  return args;
}

void Compile::Process::setupProcess()
{
  bool withWine = Compile::Config::withWine(group());
  QString exec = tool()->baseExecutable(withWine, Compile::Config::outputType(group()));
  QString path = tool()->executableDirectory().path();
  _process->setup(path + exec, arguments(), withWine);
}

Log::LineType Compile::Process::filterType(const QString &type) const
{
  QString s = type.lower();
  if ( s.startsWith("warning") ) return Log::LineType::Warning;
  if ( s.startsWith("error") )   return Log::LineType::Error;
  if ( s.startsWith("message") ) return Log::LineType::Information;
  return Log::LineType::Normal;
}

bool Compile::Process::parseErrorLine(const QString &s, const ParseErrorData &data)
{
  QRegExp re(data.pattern);
  if ( !re.exactMatch(s) ) return false;
  QString file;
  if ( data.indexFile>=0 ) {
    file = re.cap(data.indexFile).stripWhiteSpace();
    if ( file.endsWith(".") ) file = file.mid(0, file.length()-1);
    if ( file=="-" ) file = QString::null;
  }
  bool ok;
  int line = -1;
  if ( data.indexLine>=0 ) line = re.cap(data.indexLine).stripWhiteSpace().toUInt(&ok) - 1;
  if ( !ok ) line = -1;
  QString message;
  if ( data.indexMessage>=0 ) message= re.cap(data.indexMessage).stripWhiteSpace();
  Log::LineType type = data.defaultLineType;
  if ( data.indexLogType>=0 ) {
    QString s = re.cap(data.indexLogType).stripWhiteSpace();
    if ( s.isEmpty() ) type = data.defaultLineTypeEmpty;
    else type = filterType(s);
  }
  doLog(type, message, file, line);
  return true;
}

void Compile::Process::doLog(const QString &type, const QString &message, const QString &surl, uint line)
{
  doLog(filterType(type), message, surl, line);
}

void Compile::Process::doLog(Log::LineType type, const QString &message, const QString &surl, uint line)
{
  if ( surl.isEmpty() ) {
    _manager->log(type, message);
    return;
  }
  PURL::Url url = PURL::Url::fromPathOrUrl(surl);
  QString s;
  if ( !url.isEmpty() ) {
    if ( !url.exists() && !url.isInto(directory()) ) url = PURL::Url(directory(), surl);
    s += url.filename() + ":" + QString::number(line+1) + ": ";
  }
  switch (type.type()) {
    case Log::LineType::Warning:     s += i18n("warning: "); break;
    case Log::LineType::Error:       s += i18n("error: ");   break;
    case Log::LineType::Information: s += i18n("message: "); break;
    default: break;
  }
  _manager->log(type, s + message.stripWhiteSpace(), url.filepath(), line);
}

//-----------------------------------------------------------------------------
void Compile::CustomProcess::setupProcess()
{
  _process->setUseShell(true);
  _process->setup(_command, QStringList(), false);
}

void Compile::CustomProcess::logStderrLine(const QString &line)
{
  _manager->log(Log::LineType::Normal, line);
}
