/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "compile.h"

#include <qtimer.h>
#include <qregexp.h>
#include <klocale.h>

#include "libgui/editor_manager.h"
#include "libgui/main_global.h"
#include "libgui/project.h"
#include "devices/list/device_list.h"
#include "compile_config.h"
#include "common/global/process.h"
#include "common/gui/misc_gui.h"
#include "libgui/text_editor.h"
#include "libgui/editor_manager.h"

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
  List::const_iterator it;
  for (it=begin(); it!=end(); ++it)
    if ( (*it).actions & Generated ) (*it).url.del(0);
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

void Compile::LogWidget::appendLine(Log::Type type, const QString &message, const PURL::Url &url, uint line)
{
  log(type, message, Log::Delayed);
  if ( !url.isEmpty() ) _map[paragraphs()-1] = Data(url, line);
}

void Compile::LogWidget::appendLine(Log::DebugLevel level, const QString &message, const PURL::Url &url, uint line)
{
  log(level, message, Log::Delayed);
  if ( !url.isEmpty() ) _map[paragraphs()-1] = Data(url, line);
}

void Compile::LogWidget::lineClicked(int line)
{
  if ( !_map.contains(line) ) return;
  TextEditor *e = ::qt_cast<TextEditor *>(Main::editorManager().openEditor(_map[line].url));
  if ( e==0 ) return;
  e->setCursor(_map[line].line, 0);
}

//-----------------------------------------------------------------------------
Compile::Process::Process()
  : QObject(0, "compile_process"), _manager(0), _process(0)
{}

void Compile::Process::init(const Data &data, Manager *manager)
{
  _data = data;
  _config = Main::toolGroup().createConfig(const_cast<Project *>(data.project));
  _manager = manager;
}

Compile::Process::~Process()
{
  delete _config;
}

PURL::Directory Compile::Process::directory(uint i) const
{
  if (_data.project) return _data.project->directory();
  Q_ASSERT( i<_data.items.count() );
  return _data.items[i].url.directory();
}

PURL::Url Compile::Process::url(PURL::FileType type, uint i) const
{
  PURL::Url url;
  if ( _data.project && (type==PURL::Hex || _data.category==Tool::Linker || _data.category==Tool::BinToHex) )
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
  return url(type, i).relativeTo(directory(), Main::toolGroup().withWine());
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
      uint k = 0;
      for (; k<PURL::Nb_FileTypes; k++)
        if ( rexp.cap(1)==PURL::DATA[k].key ) break;
      if ( k==PURL::Nb_FileTypes ) {
        if (ok) *ok = false;
        qWarning("Unknown PURL::FileType in file list for %s", _manager->label().latin1());
        continue;
      }
      if ( PURL::DATA[k].group==PURL::LinkerScript ) {
        PURL::Url lkr = Main::toolGroup().linkerScript(_data.project, _data.linkType);
        list += FileData(lkr, Included | InProject);
      } else {
        FileActions actions = Generated;
        PURL::FileType type = PURL::FileType(k);
        if ( PURL::DATA[k].group==PURL::Source || type==PURL::Hex
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
  QStringList args = (custom ? _config->customArguments(_data.category) : genericArguments(*_config));
  PURL::Url lkr = Main::toolGroup().linkerScript(_data.project, _data.linkType);
  QStringList nargs;
  for (uint i=0; i<args.count(); i++) {
    if ( args[i].contains("$(SRCPATH)") ) {
      args[i].replace("$(SRCPATH)", directory().path(false));
      args[i].replace("//", "/");
    }
    args[i] = replaceIf(args[i], "WINE", Main::toolGroup().withWine());
    args[i] = replaceIf(args[i], "LKR", hasLinkerScript());
    args[i] = replaceIf(args[i], "NO_AUTO_DEVICE", _data.device!=Device::AUTO_DATA.name);
    if ( checkIs(args[i], "%OBJS") ) { // before %O
      if (_data.project) nargs += _data.project->objectsForLinker(objectExtension(), Main::toolGroup().withWine());
      else {
        PURL::Url tmp = url(PURL::Object);
        if ( !objectExtension().isEmpty() ) tmp = tmp.toExtension(objectExtension());
        nargs += tmp.relativeTo(directory(), Main::toolGroup().withWine());
      }
      continue;
    }
    if ( checkIs(args[i], "%LIBS") ) {
      if (_data.project) nargs += _data.project->librariesForLinker(libraryPrefix(), Main::toolGroup().withWine());
      continue;
    }
    args[i].replace("%OBJECT", filepath(PURL::Object)); // before %O
    args[i].replace("%O", outputFilepath());
    args[i].replace("%COFF", filepath(PURL::Coff));
    if (_data.project) args[i].replace("%PROJECT", _data.project->name());
    else  args[i].replace("%PROJECT", url().basename());
    args[i].replace("%MAP", filepath(PURL::Map));
    args[i].replace("%SYM", url().toExtension("sym").relativeTo(directory(), Main::toolGroup().withWine()));
    args[i].replace("%LIST", filepath(PURL::Lst));
    args[i].replace("%DEVICE", deviceName());
    args[i].replace("%FAMILY", familyName());
    args[i].replace("%LKR_PATH", lkr.path(false)); // before %LKR
    args[i].replace("%LKR_NAME", lkr.filename()); // before LKR
    args[i].replace("%LKR", lkr.filepath(false));
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

bool Compile::Process::start()
{
  _stdout = QString::null;
  _stderr = QString::null;
  delete _process;
  _process = new ::Process::LineSignal;
  connect(_process, SIGNAL(done(int)), SLOT(done(int)));
  connect(_process, SIGNAL(timeout()), SLOT(timeout()));
  connect(_process, SIGNAL(logStdoutLine(const QString &)), SLOT(logStdoutLine(const QString &)));
  connect(_process, SIGNAL(logStderrLine(const QString &)), SLOT(logStderrLine(const QString &)));
  _process->setWorkingDirectory(directory().path(false));
  QString exec = tool()->executableDirectory().path(false) + tool()->baseExecutable(Main::toolGroup().withWine());
  _process->setup(exec, arguments(), Main::toolGroup().withWine());
  _manager->log(Log::Command, _process->arguments().join(" "));
  return _process->start(100000);
}

Log::Type Compile::Process::filterType(const QString &type) const
{
  QString s = type.lower();
  if ( s.startsWith("warning") ) return Log::Warning;
  if ( s.startsWith("error") )   return Log::Error;
  if ( s.startsWith("message") ) return Log::Information;
  return Log::Data;
}

void Compile::Process::doLog(const QString &type, const QString &message, const QString &surl, uint line)
{
  doLog(filterType(type), message, surl, line);
}

void Compile::Process::doLog(Log::Type type, const QString &message, const QString &surl, uint line)
{
  PURL::Url url(surl);
  QString s;
  if ( !url.isEmpty() ) {
    if ( !url.probablyExists() && !url.isInto(directory()) ) url = PURL::Url(directory(), surl);
    s += url.filename() + ":" + QString::number(line+1) + ": ";
  }
  switch (type) {
    case Log::Warning:     s += i18n("warning: "); break;
    case Log::Error:       s += i18n("error: ");   break;
    case Log::Information: s += i18n("message: "); break;
    default: break;
  }
  _manager->log(type, s + message.stripWhiteSpace(), url, line);
}

void Compile::Process::done(int code)
{
  if ( code!=0 ) {
    _manager->log(Log::Error, i18n("*** Exited with status: %1 ***").arg(code));
    _manager->processFailed();
  } else if ( _manager->hasError() ) {
    _manager->log(Log::Error, i18n("*** Error ***"));
    _manager->processFailed();
  } else _manager->processDone();
}

void Compile::Process::timeout()
{
  _manager->log(Log::Error, i18n("*** Timeout ***"));
  _manager->processFailed();
}

//-----------------------------------------------------------------------------
Compile::Manager::Manager(QObject *parent)
  : QObject(parent, "compile_manager"), _base(0)
{}

void Compile::Manager::cleanFile(const PURL::Url &url)
{
  setupFile(Clean, TodoItem(url, false));
}

void Compile::Manager::compileFile(const TodoItem &item)
{
  _label = i18n("Compiling file...");
  setupFile(Clean | (Main::project() ? CompileOnly : Build), item);
}

void Compile::Manager::setupFile(Operations op, const TodoItem &item)
{
  Log::Base::clear();
  clearAll();
  _operations = op;
  _type = NormalLinking;
  _todo.append(item);
  _action = Compiling;
  QTimer::singleShot(0, this, SLOT(start()));
}

void Compile::Manager::cleanProject(LinkType type)
{
  setupProject(Clean, type);
}

void Compile::Manager::buildProject(LinkType type)
{
  _label = i18n("Building project...");
  setupProject(Clean | Build, type);
}

void Compile::Manager::setupProject(Operations op, LinkType type)
{
  Log::Base::clear();
  clearAll();
  _operations = op;
  _type = type;
  PURL::UrlList files = Main::project()->absoluteFiles();
  PURL::UrlList::const_iterator it = files.begin();
  for (; it!=files.end(); ++it)
    if ( (*it).data().group==PURL::Source ) _todo.append(TodoItem(*it, false));
  if ( _todo.count()>1 && Main::toolGroup().compileType()==Tool::SingleFile ) {
    log(Log::Error, i18n("The selected toolchain only supports single-file project."));
    processFailed();
    return;
  }
  _action = Compiling;
  QTimer::singleShot(0, this, SLOT(start()));
}

void Compile::Manager::kill()
{
  if ( clearAll() ) Log::Base::log(Log::Error, i18n("*** Aborted ***"), Log::Delayed);
  emit failure();
}

bool Compile::Manager::clearAll()
{
  bool running = ( _base!=0 );
  delete _base;
  _base = 0;
  _todo.clear();
  return running;
}

bool Compile::Manager::needs(Tool::Category category) const
{
  Tool::ActionType atype = (Main::project() ? Tool::Project : Tool::StandaloneFile);
  return Main::toolGroup().needs(atype, category);
}

void Compile::Manager::setup(Tool::Category category)
{
  delete _base;
  Compile::Data data(category, _items, Main::device(), Main::project(), _type);
  _base = Main::toolGroup().createCompileProcess(data, this);
}

bool Compile::Manager::setupCompile()
{
  PURL::FileType type = Main::toolGroup().implementationType(PURL::Compiler);
  for (uint i=0; i<_items.count(); i++) {
    if ( _items[i].url.fileType()!=type ) {
      if ( _operations!=Clean ) {
        QString e = PURL::extensions(type);
        MessageBox::detailedSorry(i18n("The selected toolchain (%1) cannot compile file. It only supports files with extensions: %2")
                                  .arg(Main::toolGroup().label()).arg(e), i18n("File: %1").arg(_items[i].url.pretty()), Log::Show);
        Log::Base::log(Log::Error, i18n("*** Aborted ***"), Log::Delayed);
        processFailed();
      }
      return false;
    }
  }
  if ( !compileOnly() && needs(Tool::Assembler) ) {
    PURL::FileType type = Main::toolGroup().implementationType(PURL::Assembler);
    for (uint i=0; i<_items.count(); i++)
      _todo.append(TodoItem(_items[i].url.toFileType(type), true));
  }
  setup(Tool::Compiler);
  return true;
}

bool Compile::Manager::setupAssemble()
{
  PURL::FileType type = Main::toolGroup().implementationType(PURL::Assembler);
  for (uint i=0; i<_items.count(); i++) {
    if ( _items[i].url.fileType()!=type ) {
      if ( _operations!=Clean ) {
        if ( type==PURL::Nb_FileTypes ) type = Main::toolGroup().implementationType(PURL::Compiler);
        QString e = PURL::extensions(type);
        MessageBox::detailedSorry(i18n("The selected toolchain (%1) cannot assemble file. It only supports files with extensions: %2")
                                  .arg(Main::toolGroup().label()).arg(e), i18n("File: %1").arg(_items[i].url.pretty()), Log::Show);
        Log::Base::log(Log::Error, i18n("*** Aborted ***"), Log::Delayed);
        processFailed();
      }
      return false;
    }
  }
  setup(Tool::Assembler);
  return true;
}

bool Compile::Manager::setupLink()
{
  _action = Linking;
  if ( !needs(Tool::Linker) ) {
    start();
    return false;
  }
  setup(Tool::Linker);
  return true;
}

bool Compile::Manager::setupBinToHex()
{
  _action = BinToHex;
  if ( !needs(Tool::BinToHex) ) {
    start();
    return false;
  }
  setup(Tool::BinToHex);
  return true;
}

bool Compile::Manager::prepareAction()
{
  switch (_action) {
    case Compiling:
      if ( _todo.count()!=0 ) {
        _items.clear();
        if ( Main::toolGroup().compileType()==Tool::AllFiles ) {
          _items = _todo;
          _todo.clear();
        } else {
          _items.append(_todo[0]);
          _todo.remove(_todo.begin());
        }
        PURL::SourceFamily family = _items[0].url.data().sourceFamily;
        if ( PURL::SOURCE_FAMILY_DATA[family].toolType==PURL::Compiler && Main::toolGroup().base(Tool::Compiler) )
          return setupCompile();
        return setupAssemble();
      }
      if ( !compileOnly() ) return setupLink();
      break;
    case Linking: return setupBinToHex();
    case BinToHex: break;
  }
  if ( !(_operations & Clean) ) {
    Log::Base::log(Log::Information, i18n("*** Success ***"), Log::Delayed);
    emit success();
  } else if ( _operations!=Clean ) {
    _operations &= ~Clean;
    if ( Main::project() ) setupProject(_operations, _type);
    else setupFile(_operations, _items[0]);
  }
  return false;
}

void Compile::Manager::start()
{
  delete _base;
  _base = 0;
  if ( !prepareAction() ) return;
  if ( !_base->check() ) {
    processFailed();
    return;
  }
  if ( _operations & Clean ) {
    _base->files(0).onlyExistingFiles().cleanGenerated();
    QTimer::singleShot(0, this, SLOT(start()));
    return;
  }
  if ( !_base->start() ) {
    Log::Base::log(Log::Error, i18n("Failed to execute command: check toolchain configuration"), Log::Delayed);
    processFailed();
  }
}

void Compile::Manager::log(Log::Type type, const QString &message, const PURL::Url &url, uint line)
{
  if ( type==Log::Error ) setError(message);
  static_cast<LogWidget *>(view())->appendLine(type, message, url, line);
}

void Compile::Manager::log(Log::DebugLevel level, const QString &message, const PURL::Url &url, uint line)
{
  static_cast<LogWidget *>(view())->appendLine(level, message, url, line);
}

void Compile::Manager::processDone()
{
  if ( hasError() ) {
    processFailed();
    return;
  }
  FileData::List list = _base->files(0).onlyExistingFiles();
  FileData::List::iterator it;
  for (it=list.begin(); it!=list.end(); ++it) emit updateFile(*it);
  QTimer::singleShot(0, this, SLOT(start()));
}

void Compile::Manager::processFailed()
{
  clearAll();
  emit failure();
}
