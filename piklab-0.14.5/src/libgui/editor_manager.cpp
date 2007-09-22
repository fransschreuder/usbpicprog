/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "editor_manager.h"

#include <qiconset.h>
#include <qdragobject.h>
#include <qpainter.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kaction.h>

#include "text_editor.h"
#include "hex_editor.h"
#include "object_view.h"
#include "new_dialogs.h"
#include "main_global.h"
#include "gui_debug_manager.h"
#include "common/gui/purl_gui.h"
#include "device_gui.h"
#include "register_view.h"
#include "device_editor.h"
#include "project_manager.h"
#include "global_config.h"
#include "project.h"

//-----------------------------------------------------------------------------
SwitchToDialog::SwitchToDialog(const QStringList &names, QWidget *parent)
  : Dialog(parent, "switch_to_dialog", true, i18n("Switch to editor"), Ok | Cancel, Ok, false)
{
  QVBoxLayout *top = new QVBoxLayout(mainWidget(), 10, 10);
  _edit = new KLineEdit(mainWidget());
  _edit->setCompletedItems(names);
  top->addWidget(_edit);
}

//-----------------------------------------------------------------------------
void EditorTabBar::paintLabel(QPainter *p, const QRect &br, QTab *t, bool has_focus) const
{
  QFont f = p->font();
  f.setItalic(_readOnly[t]);
  p->setFont(f);
  QTabBar::paintLabel(p, br, t, has_focus);
}

//-----------------------------------------------------------------------------
QString EditorHistory::goBack()
{
  if ( !hasBack() ) return QString::null;
  _current--;
  return _names[_current];
}

QString EditorHistory::goForward()
{
  if ( !hasForward() ) return QString::null;
  _current++;
  return _names[_current];
}

void EditorHistory::add(const QString &name)
{
  if ( _names.count()!=0 ) {
    _current = QMIN(_current, _names.count()-1);
    if ( _names[_current]==name ) return;
    if ( _current!=0 && _names[_current-1]==name ) {
      _current--;
      return;
    }
    _current++;
    if ( _current<_names.count() && _names[_current]==name ) return;
  }
  _names.resize(_current+1);
  _names[_current] = name;
}

void EditorHistory::closedLast()
{
  if ( _names.count()==0 ) return;
  _names.resize(_current);
  if ( _current!=0 ) _current--;
}

//-----------------------------------------------------------------------------
const char * const EditorManager::EDITOR_TAGS[Nb_EditorTypes] = { "device", "registers" };

EditorManager::EditorManager(QWidget *parent)
  : TabWidget(parent, "editor_manager"), _current(0)
{
  setTabBar(new EditorTabBar(this));
  connect(this, SIGNAL(currentChanged(QWidget *)), SLOT(showEditor(QWidget *)));
  setHoverCloseButton(true);
  setHoverCloseButtonDelayed(false);
}

bool EditorManager::openFile(const PURL::Url &url)
{
  if ( url.isEmpty() ) return false;
  Editor *e = findEditor(url);
  if (e) { // document already loaded
    if ( !MessageBox::askContinue(i18n("File \"%1\" already loaded. Reload?").arg(url.kurl().prettyURL()),
                                 i18n("Warning"), i18n("Reload")) ) return true;
    if ( !e->slotLoad() ) {
      closeEditor(e, false);
      return false;
    }
    return true;
  }
  if ( !openEditor(url) ) {
    static_cast< KRecentFilesAction *>(Main::action("file_open_recent"))->removeURL(url.kurl());
    return false;
  }
  static_cast<KRecentFilesAction *>(Main::action("file_open_recent"))->addURL(url.kurl());
  return true;
}

void EditorManager::connectEditor(Editor *editor)
{
    disconnectEditor(currentEditor());
    if ( editor==0 ) return;
    editor->addGui();
    connect(editor, SIGNAL(modified()), SLOT(modifiedSlot()));
    connect(editor, SIGNAL(guiChanged()), SIGNAL(guiChanged()));
    connect(editor, SIGNAL(dropEventPass(QDropEvent *)), SLOT(slotDropEvent(QDropEvent *)));
    connect(editor, SIGNAL(statusTextChanged(const QString &)), SIGNAL(statusChanged(const QString &)));
}

void EditorManager::modifiedSlot()
{
  emit modified(currentEditor()->url());
}

void EditorManager::disconnectEditor(Editor *editor)
{
  if ( editor==0 ) return;
  editor->disconnect(this);
  editor->removeGui();
}

QString EditorManager::title(const Editor &e) const
{
  return (e.url().isEmpty() ? "<" + e.name() + ">" : e.url().filename());
}

void EditorManager::updateTitles()
{
  KIconLoader loader;
  QPixmap def = loader.loadIcon("piklab", KIcon::Small);
  QPixmap modified = loader.loadIcon("filesave", KIcon::Small);
  QPixmap chip = loader.loadIcon("piklab_chip", KIcon::Small);
  QValueList<Editor *>::iterator it = _editors.begin();
  for (; it!=_editors.end(); ++it) {
    static_cast<EditorTabBar *>(tabBar())->setReadOnly(indexOf(*it), (*it)->isReadOnly());
    QPixmap pixmap;
    if ( (*it)->isModified() ) pixmap = modified;
    else if ( ::qt_cast< ::DeviceEditor *>(*it)==0 ) pixmap = PURL::icon((*it)->fileType());
    else pixmap = chip;
    changeTab(*it, pixmap.isNull() ? def : pixmap, title(**it));
  }
}

bool EditorManager::closeCurrentEditor()
{
  if ( !closeEditor(_current, true) ) return false;
  emit guiChanged();
  return true;
}

bool EditorManager::closeAllEditors()
{
  if ( currentEditor()==0 ) return true;
  while ( currentEditor() )
    if ( !closeEditor(currentEditor(), true) ) break;
  emit guiChanged();
  return ( currentEditor()==0 );
}

bool EditorManager::closeAllOtherEditors()
{
  if ( nbEditors()==1 ) return true;
  QValueList<Editor *> list = _editors;
  list.remove(currentEditor());
  QValueList<Editor *>::iterator it = list.begin();
  bool ok = true;
  for (; it!=list.end(); ++it) {
    if ( !closeEditor(*it, true) ) {
      ok = false;
      break;
    }
  }
  emit guiChanged();
  return ok;
}

bool EditorManager::closeEditor(const PURL::Url &url)
{
  Editor *e = findEditor(url);
  if ( e==0 ) return true;
  if ( !closeEditor(e, true) ) return false;
  emit guiChanged();
  return true;
}

void EditorManager::closeRequest(int i)
{
  closeEditor(static_cast<Editor *>(page(i)), true);
  emit guiChanged();
}

bool EditorManager::closeEditor(Editor *e, bool ask)
{
  if ( e==0 ) return true;
  if ( ask && !e->checkSaved() ) return false;
  removePage(e);
  _editors.remove(e);
  Editor *g = static_cast<Editor *>(currentPage());
  changeToEditor(g);
  delete e;
  return true;
}

void EditorManager::showEditor(Editor *e)
{
  changeToEditor(e);
  emit guiChanged();
}

void EditorManager::changeToEditor(Editor *e)
{
  if ( e==_current ) return;
  connectEditor(e);
  _current = e;
  if (e) {
    showPage(e);
    e->clearFocus();   // force a got focus signal
    e->setFocus();
    e->statusChanged();
    _history.add(name(*e));
  } else {
    emit statusChanged(QString::null);
    _history.closedLast();
  }
}

Editor *EditorManager::findEditor(const QString &tag)
{
  QValueList<Editor *>::iterator it = _editors.begin();
  for (; it!=_editors.end(); ++it) if ( (*it)->tag()==tag ) return *it;
  return 0;
}

Editor *EditorManager::findEditor(const PURL::Url &url)
{
  QValueList<Editor *>::iterator it = _editors.begin();
  for (; it!=_editors.end(); ++it) if ( (*it)->url()==url ) return *it;
  return 0;
}

Editor *EditorManager::createEditor(PURL::FileType type, const PURL::Url &url)
{
  Editor *e = findEditor(url);
  if (e) closeEditor(e, false);
  switch (type) {
    case PURL::Hex:
      e = new HexEditor(this);
      if ( Main::deviceData() ) static_cast<HexEditor *>(e)->setDevice(Main::deviceData()->name());
      break;
    case PURL::CSource:
    case PURL::CppSource:
    case PURL::CHeader:
    case PURL::AsmGPAsm:
    case PURL::AsmPIC30:
    case PURL::AsmPICC:
    case PURL::Inc:
    case PURL::JalSource:
    case PURL::BasicSource:
      e = new TextEditor(true, this);
      break;
    case PURL::Lkr:
    case PURL::Gld:
    case PURL::Map:
    case PURL::Lst:
    case PURL::Cod:
    case PURL::Unknown:
      e = new TextEditor(false, this);
      break;
    case PURL::Coff:
      if ( Main::project()==0 ) {
        if ( Main::deviceData()==0 ) return 0;
        Log::Base log;
        log.setView(&Main::compileLog());
        e = new Coff::Editor(url, *Main::deviceData(), log, this);
      } else {
        if ( Debugger::manager->coff()==0 ) return 0;
        e = new Coff::Editor(*Debugger::manager->coff(), this);
      }
      break;
    case PURL::Object:
    case PURL::Elf:
    case PURL::Library:
    case PURL::Project:
    case PURL::PikdevProject:
    case PURL::Nb_FileTypes: break;
  }
  return e;
}

void EditorManager::addEditor(Editor *e)
{
  QValueList<Editor *>::iterator it = _editors.begin();
  for (; it!=_editors.end(); ++it) if ( *it==e ) return;
  _editors.append(e);
  addTab(e, QString::null);
  setTabEnabled(e, true);
  showEditor(e);
}

void EditorManager::slotDropEvent(QDropEvent *event)
{
  QStringList urls;
  if ( !QUriDrag::decodeLocalFiles(event, urls)) return;
  QStringList::const_iterator it = urls.begin();
  for(; it!=urls.end(); ++it) openEditor(PURL::Url(*it));
}

Editor *EditorManager::openEditor(const PURL::Url &url)
{
  Editor *e = findEditor(url);
  if ( e==0 ) {
    e = createEditor(url.fileType(), url);
    if ( e==0 ) return 0;
    if ( !e->open(url) ) {
      closeEditor(e, false);
      return 0;
    }
    addEditor(e);
  } else showEditor(e);
  return e;
}

void EditorManager::saveAllFiles()
{
  QValueList<Editor *>::iterator it = _editors.begin();
  for (; it!=_editors.end(); ++it) {
    if ( !(*it)->isModified() ) continue;
    (*it)->save();
  }
  emit guiChanged();
}

PURL::UrlList EditorManager::files() const
{
  PURL::UrlList names;
  QValueList<Editor *>::const_iterator it = _editors.begin();
  for(; it!=_editors.end(); ++it) {
    if ( (*it)->url().isEmpty() ) continue;
    names.push_back((*it)->url());
  }
  return names;
}

void EditorManager::contextMenu(int i, const QPoint &p)
{
  Editor *editor = static_cast<Editor *>(page(i));
  if ( editor==0 ) return;

  KIconLoader loader;
  QPixmap closeIcon = loader.loadIcon("fileclose", KIcon::Small);
  QPixmap saveIcon = loader.loadIcon("filesave", KIcon::Small);
  QPixmap saveAsIcon = loader.loadIcon("filesaveas", KIcon::Small);
  QPixmap reloadIcon = loader.loadIcon("reload", KIcon::Small);
  KPopupMenu *popup = new KPopupMenu;
  popup->insertTitle(title(*editor));
  popup->insertItem(closeIcon, i18n("Close"), 1);
  if ( nbEditors()>1 ) popup->insertItem(i18n("Close All Others"), 2);
  if ( editor->isModified() ) popup->insertItem(saveIcon, i18n("Save"), 3);
  popup->insertItem(saveAsIcon, i18n("Save As..."), 4);
  if ( !editor->url().isEmpty() ) popup->insertItem(reloadIcon, i18n("Reload"), 5);
  switch (popup->exec(p)) {
    case 1: closeEditor(editor, true); break;
    case 2: closeAllOtherEditors(); break;
    case 3: editor->save(); break;
    case 4: editor->saveAs(); break;
    case 5: editor->reload(); break;
  }
  emit guiChanged();
  delete popup;
}

void EditorManager::switchHeaderImplementation()
{
  if ( currentEditor()==0 ) return;
  PURL::Url url = currentEditor()->url();
  PURL::FileType type = url.fileType();
  const PURL::Data &data = PURL::DATA[type];
  Q_ASSERT( data.sourceFamily!=PURL::Nb_SourceFamilies );
  const PURL::SourceFamilyData &sdata = PURL::SOURCE_FAMILY_DATA[data.sourceFamily];
  if ( data.group==PURL::Source ) type = sdata.headerType;
  else {
    Q_ASSERT( data.group==PURL::Source );
    type = Main::toolGroup().implementationType(sdata.toolType);
  }
  if ( type==PURL::Nb_FileTypes ) return;
  url = url.toFileType(type);
  if ( !url.probablyExists() ) return;
  openEditor(url);
}

void EditorManager::switchToEditor()
{
  QStringList names;
  for (uint i=0; i<_editors.count(); i++) names.append(title(*_editors[i]));
  SwitchToDialog dialog(names, this);
  if ( dialog.exec()!=QDialog::Accepted ) return;
  for (uint i=0; i<names.count(); i++) {
    if ( dialog.name()!=names[i] && dialog.name()!=QString("%1").arg(i+1) ) continue;
    showEditor(_editors[i]);
    return;
  }
}

QString EditorManager::name(const Editor &e) const
{
  return (!e.name().isEmpty() ? e.name() : e.url().filepath(false));
}

void EditorManager::goBack()
{
  Q_ASSERT( _history.hasBack() );
  QString s = _history.goBack();
  for (uint i=0; i<_editors.count(); i++)
    if ( s==name(*_editors[i]) ) showEditor(_editors[i]);
}

void EditorManager::goForward()
{
  Q_ASSERT( _history.hasForward() );
  QString s = _history.goForward();
  for (uint i=0; i<_editors.count(); i++)
    if ( s==name(*_editors[i]) ) showEditor(_editors[i]);
}

Editor *EditorManager::openEditor(EditorType type)
{
  bool created = false;
  Editor *e = 0;
  QString tag = EDITOR_TAGS[type];
  switch (type) {
  case DeviceEditor: {
    e = findEditor(tag);
    if ( e==0 ) {
      e = new DeviceChooser::Editor(i18n("Device"), tag, this);
      static_cast<DeviceChooser::Editor *>(e)->setDevice(false);
      static_cast<DeviceChooser::Editor *>(e)->setDevice(true); // #### needed to fix GUI glitch ???
      created = true;
    }
    break;
  }
  case RegisterEditor: {
    e = findEditor(tag);
    if ( e==0 ) {
      ::BusyCursor bc;
      e = new Register::MainView(i18n("Registers"), tag);
      static_cast<Register::MainView *>(e)->setDevice(Main::device());
      static_cast<Debugger::GuiManager *>(Debugger::manager)->addRegisterView(*static_cast<Register::MainView *>(e));
      created = true;
    }
    break;
  }
  case Nb_EditorTypes: Q_ASSERT(false); break;
  }
  if ( e==0 ) return 0;
  if (created) {
    if ( !e->slotLoad() ) {
      delete e;
      return 0;
    }
    addEditor(e);
  } else showEditor(e);
  return e;
}
