/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "compile_manager.h"

#include <qtimer.h>

#include "libgui/project.h"
#include "common/gui/misc_gui.h"
#include "compile_config.h"

Compile::Manager::Manager(QObject *parent)
  : QObject(parent, "compile_manager"), _base(0)
{}

void Compile::Manager::cleanFile(const PURL::Url &url)
{
  setupFile(Clean, TodoItem(url, false));
}

bool Compile::Manager::compileFile(const TodoItem &item)
{
  _label = i18n("Compiling file...");
  return setupFile(Clean | (Main::project() ? CompileOnly : Build), item);
}

bool Compile::Manager::setupFile(Operations op, const TodoItem &item)
{
  Log::Base::clear();
  clearAll();
  _operations = op;
  _type = NormalLinking;
  _todo.append(item);
  _action = Compiling;
  _wholeProject = false;
  QTimer::singleShot(0, this, SLOT(start()));
  return true;
}

void Compile::Manager::cleanProject(LinkType type)
{
  setupProject(Clean, type);
}

bool Compile::Manager::buildProject(LinkType type)
{
  _label = i18n("Building project...");
  return setupProject(Clean | Build, type);
}

bool Compile::Manager::setupProject(Operations op, LinkType type)
{
  Log::Base::clear();
  clearAll();
  _operations = op;
  _type = type;
  PURL::UrlList files = Main::project()->absoluteFiles();
  if ( files.count()==0 ) {
    MessageBox::sorry(i18n("Cannot build empty project."), Log::Show);
    return false;
  }
  PURL::UrlList::const_iterator it;
  for (it=files.begin(); it!=files.end(); ++it)
    if ( (*it).data().group==PURL::Source ) _todo.append(TodoItem(*it, false));
  if ( _todo.count()>1 && Main::toolGroup().compileType()==Tool::SingleFile ) {
    MessageBox::sorry(i18n("The selected toolchain only supports single-file project."), Log::Show);
    return false;
  }
  _action = Compiling;
  _wholeProject = true;
  QTimer::singleShot(0, this, SLOT(start()));
  return true;
}

void Compile::Manager::kill()
{
  if ( clearAll() ) Log::Base::log(Log::LineType::Error, i18n("*** Aborted ***"), Log::Delayed);
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
  PURL::FileType type = Main::toolGroup().implementationType(PURL::ToolType::Compiler);
  for (uint i=0; i<_items.count(); i++) {
    if ( _items[i].url.fileType()!=type ) {
      if ( _operations!=Clean ) {
        QString e = PURL::extensions(type);
        MessageBox::detailedSorry(i18n("The selected toolchain (%1) cannot compile file. It only supports files with extensions: %2")
                                  .arg(Main::toolGroup().label()).arg(e), i18n("File: %1").arg(_items[i].url.pretty()), Log::Show);
        Log::Base::log(Log::LineType::Error, i18n("*** Aborted ***"), Log::Delayed);
        processFailed();
      }
      return false;
    }
  }
  if ( !compileOnly() && needs(Tool::Category::Assembler) ) {
    PURL::FileType type = Main::toolGroup().implementationType(PURL::ToolType::Assembler);
    for (uint i=0; i<_items.count(); i++)
      _todo.append(TodoItem(_items[i].url.toFileType(type), true));
  }
  setup(Tool::Category::Compiler);
  return true;
}

bool Compile::Manager::setupAssemble()
{
  PURL::FileType type = Main::toolGroup().implementationType(PURL::ToolType::Assembler);
  for (uint i=0; i<_items.count(); i++) {
    if ( _items[i].url.fileType()!=type ) {
      if ( _operations!=Clean ) {
        if ( type==PURL::Nb_FileTypes ) type = Main::toolGroup().implementationType(PURL::ToolType::Compiler);
        QString e = PURL::extensions(type);
        MessageBox::detailedSorry(i18n("The selected toolchain (%1) cannot assemble file. It only supports files with extensions: %2")
                                  .arg(Main::toolGroup().label()).arg(e), i18n("File: %1").arg(_items[i].url.pretty()), Log::Show);
        Log::Base::log(Log::LineType::Error, i18n("*** Aborted ***"), Log::Delayed);
        processFailed();
      }
      return false;
    }
  }
  setup(Tool::Category::Assembler);
  return true;
}

bool Compile::Manager::setupLink()
{
  _action = Linking;
  if ( !needs(Tool::Category::Linker) ) {
    start();
    return false;
  }
  setup(Tool::Category::Linker);
  return true;
}

bool Compile::Manager::setupBinToHex()
{
  _action = BinToHex;
  if ( !needs(Tool::Category::BinToHex) ) {
    start();
    return false;
  }
  setup(Tool::Category::BinToHex);
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
        if ( family.data().toolType==PURL::ToolType::Compiler && Main::toolGroup().base(Tool::Category::Compiler) )
          return setupCompile();
        return setupAssemble();
      }
      if ( !compileOnly() ) return setupLink();
      break;
    case Linking: return setupBinToHex();
    case BinToHex: break;
  }
  if ( !(_operations & Clean) ) {
    Log::Base::log(Log::LineType::Information, i18n("*** Success ***"), Log::Delayed);
    emit success();
  } else if ( _operations!=Clean ) {
    _operations &= ~Clean;
    if (_wholeProject) setupProject(_operations, _type);
    else setupFile(_operations, _items[0]);
  }
  return false;
}

void Compile::Manager::start()
{
  if ( Main::toolGroup().isCustom() ) {
    executeCustomCommands();
    return;
  }
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
    Log::Base::log(Log::LineType::Error, i18n("Failed to execute command: check toolchain configuration."), Log::Delayed);
    processFailed();
  }
}

void Compile::Manager::log(Log::LineType type, const QString &message, const QString &filepath, uint line)
{
  if ( type==Log::LineType::Error ) setError(message);
  static_cast<LogWidget *>(view())->appendLine(type, message, filepath, line);
}

void Compile::Manager::log(Log::DebugLevel level, const QString &message, const QString &filepath, uint line)
{
  static_cast<LogWidget *>(view())->appendLine(level, message, filepath, line);
}

void Compile::Manager::processDone()
{
  if ( hasError() ) {
    processFailed();
    return;
  }
  if ( Main::toolGroup().isCustom() ) {
    _customCommandIndex++;
    startCustomCommand();
  } else {
    FileData::List list = _base->files(0).onlyExistingFiles();
    FileData::List::iterator it;
    for (it=list.begin(); it!=list.end(); ++it) emit updateFile(*it);
    QTimer::singleShot(0, this, SLOT(start()));
  }
}

void Compile::Manager::processFailed()
{
  clearAll();
  emit failure();
}

void Compile::Manager::startCustomCommand()
{
  delete _base;
  _base = 0;
  QStringList commands = Compile::Config::customCommands(Main::project());
  if ( _customCommandIndex==commands.count() ) {
    Log::Base::log(Log::LineType::Information, i18n("*** Success ***"), Log::Delayed);
    emit success();
    return;
  }
  QString command = commands[_customCommandIndex];
  _base = new CustomProcess(command);
  Compile::Data data(Tool::Category::Nb_Types, _todo, Main::device(), Main::project(), _type);
  _base->init(data, this);
  if ( !_base->start() ) {
    Log::Base::log(Log::LineType::Error, i18n("Failed to execute custom command #%1.").arg(_customCommandIndex+1), Log::Delayed);
    processFailed();
  }
}

void Compile::Manager::executeCustomCommands()
{
  _customCommandIndex = 0;
  if ( Compile::Config::customCommands(Main::project()).isEmpty() ) {
    MessageBox::sorry(i18n("No custom commands specified."), Log::Show);
    emit failure();
    return;
  }
  log(Log::LineType::Information, i18n("Executing custom commands..."));
  startCustomCommand();
}
