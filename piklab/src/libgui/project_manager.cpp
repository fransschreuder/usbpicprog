/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "project_manager.h"

#include <qdragobject.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qtimer.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kaction.h>
#include <kapplication.h>

#include "project.h"
#include "project_wizard.h"
#include "project_editor.h"
#include "toplevel.h"
#include "editor_manager.h"
#include "object_view.h"
#include "devices/list/device_list.h"
#include "tools/list/compile_config.h"
#include "register_view.h"
#include "hex_editor.h"
#include "main_global.h"
#include "common/gui/purl_gui.h"
#include "device_gui.h"

//----------------------------------------------------------------------------
void PopupMenu::insertItem(const QString &icon, const QString &label, QObject *receiver, const char *slot)
{
  KIconLoader loader;
  QPixmap pixmap = loader.loadIcon(icon, KIcon::Small);
  if (receiver) KPopupMenu::insertItem(pixmap, label, receiver, slot, 0, _index);
  else KPopupMenu::insertItem(pixmap, label, _index);
  _index++;
}

//----------------------------------------------------------------------------
const char *ProjectManager::HeaderItem::GROUP_LABELS[Nb_Groups] = {
  I18N_NOOP("Device"), I18N_NOOP("Headers"), I18N_NOOP("Linker Script"),
  I18N_NOOP("Sources"), I18N_NOOP("Objects"), I18N_NOOP("Views"),
  I18N_NOOP("Generated"), I18N_NOOP("Included")
};

ProjectManager::Group ProjectManager::group(PURL::FileType type)
{
  switch (PURL::DATA[type].group) {
    case PURL::Source: return SourceGroup;
    case PURL::Header: return HeaderGroup;
    case PURL::LinkerScript: return LinkerScriptGroup;
    case PURL::LinkerObject: return LinkerObjectGroup;
    case PURL::Nb_FileGroups: break;
  }
  return ViewGroup;
}

ProjectManager::RootItem::RootItem(KListView *listview)
  : UrlItem(listview)
{
  setSelectable(false);
  setPixmap(0, PURL::icon(PURL::Project));
  set(PURL::Url(), true);
}

void ProjectManager::RootItem::set(const PURL::Url &url, bool standalone)
{
  _url = url;
  _standalone = standalone;
  if ( _url.isEmpty() ) setText(0, i18n("<no project>"));
  else if (_standalone) setText(0, i18n("Standalone File"));
  else setText(0, _url.basename());
}

ProjectManager::HeaderItem::HeaderItem(RootItem *item, Group group)
  : KListViewItem(item), _group(group)
{
  if ( group!=DeviceGroup) setSelectable(false);
  setText(0, i18n(GROUP_LABELS[group]));
}

QListViewItem *ProjectManager::HeaderItem::lastChild() const
{
  QListViewItem *item = firstChild();
  if ( item==0 ) return 0;
  for (;;) {
    if ( item->nextSibling()==0 ) break;
    item = item->nextSibling();
  }
  return item;
}

ProjectManager::FileItem::FileItem(HeaderItem *item, PURL::FileType ftype,
                                   const PURL::Url &url, bool external)
  : UrlItem(item), _ftype(ftype), _external(external)
{
  setPixmap(0, PURL::icon(ftype));
  set(url);
}

void ProjectManager::FileItem::set(const PURL::Url &url)
{
  _url = url;
  switch (_ftype) {
    case PURL::Hex:  setText(0, i18n("Hex File")); break;
    case PURL::Coff: setText(0, i18n("Disassembly Listing")); break;
    case PURL::Lst:  setText(0, i18n("List")); break;
    case PURL::Map:  setText(0, i18n("Memory Map")); break;
    case PURL::Project:
    case PURL::Nb_FileTypes: Q_ASSERT(false); break;
    default: {
      QString s = url.filename();
      if ( _external && group(_ftype)==LinkerScriptGroup )
        s += i18n(" (default)");
      setText(0, s); break;
    }
  }
}

void ProjectManager::FileItem::paintCell(QPainter *p, const QColorGroup &cg,
                                         int column, int width, int align)
{
  QFont f = p->font();
  f.setItalic(group(_ftype)!=ViewGroup && _external);
  p->setFont(f);
  KListViewItem::paintCell(p, cg, column, width, align);
}

ProjectManager::RegisterItem::RegisterItem(HeaderItem *item)
  : KListViewItem(item)
{
  KIconLoader loader;
  QPixmap chip = loader.loadIcon("piklab_chip", KIcon::Small);
  setPixmap(0, chip);
  setText(0, i18n("Registers"));
}

ProjectManager::DeviceItem::DeviceItem(HeaderItem *item)
  : EditListViewItem(item)
{}

QWidget *ProjectManager::DeviceItem::editWidgetFactory(int) const
{
  QComboBox *combo = new DeviceChooser::ComboBox(Main::project()==0, 0);
  QString device = Main::device();
  if ( device!=Device::AUTO_DATA.name ) combo->setCurrentText(device);
  QObject::connect(combo, SIGNAL(activated(int)), listView(), SLOT(finishRenaming()));
  return combo;
}

void ProjectManager::DeviceItem::updateText()
{
  QString device = Main::device();
  if ( device==Device::AUTO_DATA.name ) {
    const Device::Data *data = Main::deviceData();
    if (data) device = data->name() + " " + i18n(Device::AUTO_DATA.label);
    else device = i18n(Device::AUTO_DATA.label);
  }
  setText(0, device);
}

ProjectManager::LinkerScriptItem::LinkerScriptItem(HeaderItem *item)
  : UrlItem(item)
{
  init();
}

void ProjectManager::LinkerScriptItem::init()
{
  _url = PURL::Url();
  PURL::Url lkr;
  if ( Main::project() ) lkr = Main::project()->customLinkerScript();
  setText(0, lkr.isEmpty() ? i18n("<default>") : lkr.filename());
  setPixmap(0, lkr.isEmpty() ? QPixmap() : PURL::icon(PURL::Lkr));
}

void ProjectManager::LinkerScriptItem::set(const PURL::Url &url)
{
  _url = url;
  QString s = url.filename();
  PURL::Url lkr;
  if ( Main::project() ) lkr = Main::project()->customLinkerScript();
  if ( lkr.isEmpty() ) s += i18n(" (default)");
  setText(0, s);
  setPixmap(0, PURL::icon(PURL::Lkr));
}

PURL::Url ProjectManager::LinkerScriptItem::url() const
{
  if ( !_url.isEmpty() ) return _url;
  if ( Main::project() ) return Main::project()->customLinkerScript();
  return PURL::Url();
}

//----------------------------------------------------------------------------
ProjectManager::View::View(QWidget *parent)
  : ListView(parent, "project_manager"), _project(0), _modified(false)
{
  connect(this, SIGNAL(mouseButtonClicked(int, QListViewItem *, const QPoint &, int)),
          SLOT(clicked(int, QListViewItem *, const QPoint &, int)));
  connect(this, SIGNAL(contextMenuRequested(QListViewItem *, const QPoint &, int)),
          SLOT(contextMenu(QListViewItem *, const QPoint &, int)));
  connect(this, SIGNAL(itemRenamed(QListViewItem *, int, const QString &)),
          SLOT(renamed(QListViewItem *, int, const QString &)));
  connect(this, SIGNAL(moved()), SLOT(filesReordered()));

  header()->hide();
  setSorting(-1);
  addColumn(QString::null, 170);
  setFullWidth(true);
  setRootIsDecorated(false);
  setAcceptDrops(true);
  setDragEnabled(true);
  setDropVisualizer(true);
  QTimer::singleShot(0, this, SLOT(init()));;
}

ProjectManager::View::~View()
{
  delete _project;
}

void ProjectManager::View::init()
{
  clear();
  _rootItem  = new RootItem(this);
  setCurrentItem(_rootItem);
  KListViewItem *item = new RegisterItem(headerItem(DeviceGroup));
  ensureItemVisible(item);
  _deviceItem = new DeviceItem(headerItem(DeviceGroup));
  ensureItemVisible(_deviceItem);
  _linkerScriptItem = 0;
  (void)headerItem(SourceGroup);
}

ProjectManager::HeaderItem *ProjectManager::View::findHeaderItem(Group group) const
{
  QListViewItemIterator it(const_cast<View *>(this));
  for(; it.current(); ++it) {
    if ( it.current()->rtti()!=HeaderRtti ) continue;
    HeaderItem *hi = static_cast<HeaderItem *>(it.current());
    if ( hi->group()==group ) return hi;
  }
  return 0;
}

ProjectManager::HeaderItem *ProjectManager::View::headerItem(Group group)
{
  HeaderItem *item = findHeaderItem(group);
  if (item) return item;
  item = new HeaderItem(_rootItem, group);
  // reorder groups...
  HeaderItem *items[Nb_Groups];
  for (uint i=0; i<Nb_Groups; i++) {
    items[i] = findHeaderItem(Group(i));
    if (items[i]) _rootItem->takeItem(items[i]);
  }
  for (int i=Nb_Groups-1; i>=0; i--) {
    if ( items[i]==0 ) continue;
    _rootItem->insertItem(items[i]);
  }
  return item;
}

ProjectManager::FileItem *ProjectManager::View::findFileItem(const PURL::Url &url) const
{
  QListViewItemIterator it(const_cast<View *>(this));
  for(; it.current(); ++it) {
    if ( it.current()->rtti()!=FileRtti ) continue;
    FileItem *fi = static_cast<FileItem *>(it.current());
    if ( fi->url()==url ) return fi;
  }
  return 0;
}

ProjectManager::FileItem *ProjectManager::View::findFileItem(PURL::FileType type) const
{
  QListViewItemIterator it(const_cast<View *>(this));
  for(; it.current(); ++it) {
    if ( it.current()->rtti()!=FileRtti ) continue;
    FileItem *fi = static_cast<FileItem *>(it.current());
    if ( fi->ftype()==type ) return fi;
  }
  return 0;
}

QListViewItem *ProjectManager::View::findItem(const QString &tag) const
{
  QListViewItemIterator it(const_cast<View *>(this));
  for(; it.current(); ++it) {
    switch (Rtti(it.current()->rtti())) {
      case RootRtti:
      case FileRtti:
      case LinkerScriptRtti:
      case DeviceRtti: continue;
      case HeaderRtti:
      case RegisterRtti: break;
    }
    if ( it.current()->text(0)==tag ) return it.current();
  }
  return 0;
}

void ProjectManager::View::select(const Editor *e)
{
  QListViewItem *item = 0;
  if ( e->url().isEmpty() ) item = findItem(e->tag());
  else item = findFileItem(e->url());
  if (item) setSelected(item, true);
  else clearSelection();
}

void ProjectManager::View::contextMenu(QListViewItem *item, const QPoint &p, int)
{
  if ( item==0 ) return;

  PopupMenu pop;
  Group group = Nb_Groups;
  if ( item->rtti()==HeaderRtti) group = static_cast<const HeaderItem *>(item)->group();

  if ( item->rtti()==LinkerScriptRtti || group==LinkerScriptGroup ) {
    if ( _project==0 || Main::toolGroup().linkerScriptType()==PURL::Nb_FileTypes ) return;
    pop.insertTitle(i18n("Linker Script"));
    pop.insertItem("piklab_addfile", i18n("Set Custom..."));
    if ( !_project->customLinkerScript().isEmpty() ) pop.insertItem("editdelete", i18n("Set Default"));
    switch( pop.exec(p) ) {
      case 1: {
        PURL::Url url = PURL::getOpenUrl(":custom_linker_script", PURL::filter(Main::toolGroup().linkerScriptType()), this, i18n("Select Linker Script"));
        if ( !url.isEmpty() ) _project->setCustomLinkerScript(url);
        break;
      }
      case 2: _project->setCustomLinkerScript(PURL::Url()); break;
    }
    _linkerScriptItem->init();
  } else if ( item->rtti()==RootRtti ) {
    RootItem *ri = static_cast<RootItem *>(item);
    if ( _project==0 ) {
      if ( ri->url().isEmpty() ) {
        pop.insertItem("piklab_createproject", i18n("New Project..."), &Main::toplevel(), SLOT(newProject()));
        pop.insertItem("piklab_openproject", i18n("Open Project..."), &Main::toplevel(), SLOT(openProject()));
        pop.exec(p);
      } else {
        pop.insertTitle(i18n("Standalone File"));
        pop.insertItem("configure", i18n("Configure..."));
        if ( pop.exec(p)==1 ) Main::toplevel().configure(ConfigCenter::Standalone);
      }
    } else {
      pop.insertTitle(i18n("Project"));
      pop.insertItem("configure", i18n("Options..."), &Main::toplevel(), SLOT(configureProject()));
      pop.insertItem("find", i18n("Find Files..."),   &Main::toplevel(), SLOT(runKfind()));
      pop.insertSeparator();
      pop.insertItem("piklab_compile", i18n("Build Project"), &Main::toplevel(), SLOT(buildProject()));
      pop.insertItem("trashcan_empty", i18n("Clean Project"), &Main::toplevel(), SLOT(cleanBuild()));
      pop.insertSeparator();
      pop.insertItem("filenew", i18n("New Source File..."), &Main::toplevel(), SLOT(newSourceFile()));
      pop.insertItem("piklab_addfile", i18n("Add Source Files..."), this, SLOT(insertSourceFiles()));
      pop.insertItem("piklab_addfile", i18n("Add Object Files..."), this, SLOT(insertObjectFiles()));
      if ( Main::currentEditor() ) pop.insertItem("piklab_addcurrentfile", i18n("Add Current File"), this, SLOT(insertCurrentFile()));
      pop.exec(p);
    }
  } else if ( item->rtti()==FileRtti ) {
    if ( _project==0 ) return;
    FileItem *fi = static_cast<FileItem *>(item);
    if ( isExternalFile(fi->url()) ) return;
    pop.insertTitle(item->text(0));
    pop.insertItem("editdelete", i18n("Remove From Project"));
    if ( pop.exec(p)==1 ) removeFile(fi->url());
  } else if ( item->rtti()==HeaderRtti ) {
    if ( _project==0 ) return;
    if ( group==LinkerObjectGroup ) {
      pop.insertTitle(i18n("Objects"));
      pop.insertItem("piklab_addfile", i18n("Add Object Files..."), this, SLOT(insertObjectFiles()));
      pop.exec(p);
    } else if ( group==SourceGroup || group==HeaderGroup ) {
      pop.insertTitle(i18n("Sources"));
      pop.insertItem("filenew", i18n("New File..."), &Main::toplevel(), SLOT(newSourceFile()));
      pop.insertItem("piklab_addfile", i18n("Add Source Files..."), this, SLOT(insertSourceFiles()));
      pop.exec(p);
    } else if ( group==DeviceGroup ) {
      pop.insertItem("filenew", i18n("Select Device..."), &Main::toplevel(), SLOT(showDeviceInfo()));
      pop.exec(p);
    }
  }
}

void ProjectManager::View::closeProject()
{
  if ( _project==0 && projectUrl().isEmpty() ) return;
  if (_project) {
    // save opened files
    PURL::UrlList files = Main::editorManager().files();
    PURL::UrlList::const_iterator it = files.begin();
    PURL::UrlList opened;
    for (; it!=files.end(); ++it) {
      if ( !isExternalFile(*it) ) opened += *it;
      Main::editorManager().closeEditor(*it);
    }
    _project->setOpenedFiles(opened);
    // save watched registers
    _project->setWatchedRegisters(Register::list().watched());
    QString error;
    if ( !_project->save(error) )
      MessageBox::detailedSorry(i18n("Could not save project file \"%1\".").arg(_project->url().pretty()), error, Log::Show);
    delete _project;
    _project = 0;
  }
  _modified = false;
  init();
}

void ProjectManager::View::addExternalFiles()
{
  const QMap<PURL::Url, FileOrigin> &ext = projectData().externals;
  QMap<PURL::Url, FileOrigin>::const_iterator it;
  for (it=ext.begin(); it!=ext.end(); ++it) {
    if ( !it.key().exists() ) continue;
    addFile(it.key(), it.key().fileType(), it.data());
  }
}

void ProjectManager::View::setStandalone(const PURL::Url &url, PURL::FileType type)
{
  if ( projectUrl()==url ) return;
  closeProject();
  _rootItem->set(url, true);
  addFile(url, type, Intrinsic);
  _standaloneData[url].type = type;
  addExternalFiles();
}

PURL::Url ProjectManager::View::standaloneGenerator(const PURL::Url &url, PURL::FileType &type) const
{
  QMap<PURL::Url, ProjectData>::const_iterator it;
  for (it=_standaloneData.begin(); it!=_standaloneData.end(); ++it) {
    if ( !it.data().externals.contains(url) ) continue;
    if ( !it.key().exists() ) continue;
    type = it.data().type;
    return it.key();
  }
  type = PURL::Nb_FileTypes;
  return url;
}

void ProjectManager::View::insertSourceFiles()
{
  Q_ASSERT(_project);
  PURL::UrlList list = PURL::getOpenUrls(_project->directory().path(), PURL::sourceFilter(PURL::CompleteFilter), this, i18n("Select Source File"));
  if ( list.isEmpty() ) return;
  PURL::UrlList::const_iterator it;
  for (it=list.begin(); it!=list.end(); ++it) insertFile(*it);
}

void ProjectManager::View::insertObjectFiles()
{
  Q_ASSERT(_project);
  PURL::UrlList list = PURL::getOpenUrls(_project->directory().path(), PURL::objectFilter(PURL::CompleteFilter), this, i18n("Select Object File"));
  if ( list.isEmpty() ) return;
  PURL::UrlList::const_iterator it;
  for (it=list.begin(); it!=list.end(); ++it) insertFile(*it);
}

void ProjectManager::View::insertFile(const PURL::Url &url)
{
  if ( !url.exists() ) {
    MessageBox::detailedSorry(i18n("Could not find file."), i18n("File: %1").arg(url.pretty()), Log::Show);
    return;
  }
  PURL::Url purl = url;
  MessageBox::Result copy = MessageBox::No;
  if ( !url.isInto(_project->directory()) ) {
    copy = MessageBox::questionYesNoCancel(i18n("File \"%1\" is not inside the project directory. Do you want to copy the file to your project directory?").arg(url.pretty()),
                                          i18n("Copy and Add"), i18n("Add only"));
    if ( copy==MessageBox::Cancel ) return;
    if ( copy==MessageBox::Yes ) purl = PURL::Url(_project->directory(), url.filename());
  }
  if ( _project->absoluteFiles().contains(purl) ) {
    MessageBox::detailedSorry(i18n("File is already in the project."), i18n("File: %1").arg(purl.pretty()), Log::Show);
    return;
  }
  if ( copy==MessageBox::Yes ) {
    Log::StringView sview;
    if ( !url.copyTo(purl, sview) ) {
      MessageBox::detailedSorry(i18n("Copying file to project directory failed."), i18n("File: %1\n").arg(url.pretty()) + sview.string(), Log::Show);
      return;
    }
  }
  _project->addFile(purl);
  addFile(purl, purl.fileType(), Intrinsic);
}

bool ProjectManager::View::openProject()
{
  PURL::Url url = PURL::getOpenUrl(":open_project", PURL::projectFilter(PURL::CompleteFilter), this, i18n("Select Project file"));
  return openProject(url);
}

void ProjectManager::View::addExternalFile(const PURL::Url &url, FileOrigin origin)
{
  Q_ASSERT( origin!=Intrinsic );
  addFile(url, url.fileType(), origin);
}

const ProjectManager::View::ProjectData &ProjectManager::View::projectData() const
{
  if ( _project==0 ) return _standaloneData[projectUrl()];
  return _projectData;
}

ProjectManager::View::ProjectData &ProjectManager::View::projectData()
{
  if ( _project==0 ) return _standaloneData[projectUrl()];
  return _projectData;
}

void ProjectManager::View::addFile(const PURL::Url &url, PURL::FileType type, FileOrigin origin)
{
  if ( contains(url) ) return;
  QMap<PURL::Url, FileOrigin> &ext = projectData().externals;
  if ( PURL::DATA[type].group==PURL::LinkerScript && _linkerScriptItem ) {
    _linkerScriptItem->set(url);
    ext[url] = Included;
    return;
  }
  PURL::FileProperties properties = PURL::DATA[type].properties;
  Group grp = Nb_Groups;
  switch (origin) {
    case Intrinsic: grp = group(type); break;
    case Generated: grp = GeneratedGroup; break;
    case Included:  grp = IncludedGroup; break;
  }
  HeaderItem *hitem = headerItem(grp);
  QListViewItem *item = new FileItem(hitem, type, url, origin!=Intrinsic);
  item->moveItem(hitem->lastChild());
  ensureItemVisible(item);
  if ( origin!=Intrinsic ) ext[url] = origin;
  if ( type==PURL::Hex && _project==0 ) {
    QString extension = PURL::extension(PURL::AsmGPAsm);
    PURL::Url durl = PURL::Url::fromPathOrUrl("<" + (url.isEmpty() ? i18n("Disassembly") : url.appendExtension(extension).filename()) + ">");
    if ( findFileItem(durl)==0 ) {
      (void)new FileItem(headerItem(ViewGroup), PURL::Coff, durl, true);
      ext[durl] = Generated;
    }
  }
  if ( _project && origin==Intrinsic ) _modified = true;
}

void ProjectManager::View::removeExternalFiles()
{
  QMap<PURL::Url, FileOrigin> &ext = projectData().externals;
  QMap<PURL::Url, FileOrigin>::const_iterator it;
  for (it=ext.begin(); it!=ext.end(); ++it) {
    Main::editorManager().closeEditor(it.key());
    removeFile(it.key());
  }
  ext.clear();
  if (_linkerScriptItem) _linkerScriptItem->init();
}

void ProjectManager::View::removeFile(const PURL::Url &url)
{
  if ( _project && !isExternalFile(url) ) _project->removeFile(url);
  FileItem *item = findFileItem(url);
  if ( item==0 ) return;
  HeaderItem *group = static_cast<HeaderItem *>(item->parent());
  delete item;
  if ( group->childCount()==0 ) delete group;
  _modified = true;
  emit guiChanged();
}

void ProjectManager::View::clicked(int button, QListViewItem *item, const QPoint &, int)
{
  if ( item==0 ) return;
  if ( button!=LeftButton ) return;
  const Device::Data *data = Main::deviceData();
  Rtti rtti = Rtti(item->rtti());
  if ( data==0 && rtti!=DeviceRtti && rtti!=RootRtti ) {
    MessageBox::sorry(i18n("Cannot open without device specified."), Log::Show);
    return;
  }
  Editor *e = 0;
  ::BusyCursor bc;
  switch (rtti) {
    case RootRtti:
      Main::toplevel().configureProject();
      break;
    case HeaderRtti: {
      if ( static_cast<HeaderItem *>(item)->group()!=DeviceGroup ) break;
      e = Main::editorManager().openEditor(EditorManager::DeviceEditor);
      break;
    }
    case RegisterRtti:
      e = Main::editorManager().openEditor(EditorManager::RegisterEditor);
      break;
    case DeviceRtti: break;
    case LinkerScriptRtti: {
      PURL::Url url = Main::projectManager().linkerScriptUrl();
      if ( url.isEmpty() ) break;
      e = Main::editorManager().findEditor(url);
      if ( e==0 ) {
        e = Main::editorManager().createEditor(url.fileType(), url);
        if ( !e->open(url) ) {
	  delete e;
	  break;
	}
        if ( e && isExternalFile(url) ) e->setReadOnly(true);
        Main::editorManager().addEditor(e);
      } else Main::editorManager().showEditor(e);
      break;
    }
    case FileRtti: {
      FileItem *fi = static_cast<FileItem *>(item);
      if ( !(PURL::DATA[fi->ftype()].properties & PURL::Editable) ) break;
      e = Main::editorManager().findEditor(fi->url());
      if ( e==0 ) {
        if ( fi->ftype()==PURL::Coff && _project==0 && !fi->url().exists() ) {
          PURL::Url url = findFileItem(PURL::Hex)->url();
          if ( url.isEmpty() ) {
            HexEditor *he = ::qt_cast<HexEditor *>(Main::currentEditor());
            if ( he==0 ) break;
            e = new DisassemblyEditor(*he, *data, this);
          } else e = new DisassemblyEditor(url, *data, this);
          addExternalFile(fi->url(), Generated);
        } else e = Main::editorManager().createEditor(fi->url().fileType(), fi->url());
        if ( !e->open(fi->url()) ) {
	  delete e;
	  break;
	}
        if ( e && isExternalFile(fi->url()) ) e->setReadOnly(true);
        Main::editorManager().addEditor(e);
      } else Main::editorManager().showEditor(e);
      break;
    }
  }
}

void ProjectManager::View::insertCurrentFile()
{
  insertFile(Main::editorManager().currentEditor()->url());
}

bool ProjectManager::View::editProject()
{
  ProjectEditor dialog(*_project, this);
  if ( dialog.exec()!=QDialog::Accepted ) return false;
  _modified = true;
  if (_linkerScriptItem) _linkerScriptItem->init();
  return true;
}

bool ProjectManager::View::newProject()
{
  ProjectWizard wizard(this);
  if ( wizard.exec()==QDialog::Rejected ) return false;
  closeProject();
  QString error;
  if ( !wizard.project()->save(error) ) {
    MessageBox::detailedSorry(i18n("Failed to create new project file"), error, Log::Show);
    return false;
  }
  openProject(wizard.url());
  return true;
}

void ProjectManager::View::setProject(Project *project)
{
  closeProject();
  Main::editorManager().closeAllEditors();
  _project = project;
  _rootItem->set(project->url(), false);
  if ( project && Main::toolGroup().linkerScriptType()!=PURL::Nb_FileTypes ) {
    _linkerScriptItem = new LinkerScriptItem(headerItem(LinkerScriptGroup));
    ensureItemVisible(_linkerScriptItem);
  }
}

bool ProjectManager::View::openProject(const PURL::Url &url)
{
  if ( url.isEmpty() ) return false;
  bool reload = ( _project && _project->url()==url );
  if ( reload && !MessageBox::askContinue(i18n("Project already loaded. Reload?"), i18n(" Already Loaded"), i18n("Reload")) ) return false;
  static_cast< KRecentFilesAction *>(Main::action("project_open_recent"))->removeURL(url.kurl());
  Project *p = new Project(url);
  QString error;
  if ( !p->load(error) ) {
    MessageBox::detailedSorry(i18n("Could not open project file."), error, Log::Show);
    delete p;
    return false;
  }
  setProject(p);
  PURL::UrlList files = _project->absoluteFiles();
  PURL::UrlList::const_iterator it;
  for(it=files.begin(); it!=files.end(); ++it) addFile(*it, (*it).fileType(), Intrinsic);
  _projectData.type = PURL::Project;
  _projectData.externals.clear();
  static_cast<KRecentFilesAction *>(Main::action("project_open_recent"))->addURL(url.kurl());
  files = _project->openedFiles();
  for(it = files.begin(); it!=files.end(); ++it) Main::editorManager().openFile(*it);
  Register::list().init();
  QValueList<Register::TypeData> watched = _project->watchedRegisters();
  QValueList<Register::TypeData>::const_iterator wit;
  for (wit=watched.begin(); wit!=watched.end(); ++wit) Register::list().setWatched(*wit, true);
  return true;
}

bool ProjectManager::View::isExternalFile(const PURL::Url &url) const
{
  if ( projectUrl().isEmpty() ) return false;
  return projectData().externals.contains(url);
}

void ProjectManager::View::modified(const PURL::Url &url)
{
  FileItem *item = findFileItem(url);
  if ( item && !isExternalFile(url) ) _modified = true;
}

void ProjectManager::View::renamed(QListViewItem *item, int, const QString &text)
{
  Q_ASSERT ( item->rtti()==DeviceRtti );
  Main::toplevel().setDevice(text);
}

void ProjectManager::View::updateGUI()
{
  cancelRenaming();
  _deviceItem->updateText();
}

QDragObject *ProjectManager::View::dragObject()
{
  if ( currentItem()==0 || currentItem()->rtti()!=FileRtti ) return 0;
  const FileItem *item = static_cast<const FileItem *>(currentItem());
  const HeaderItem *hitem = static_cast<const HeaderItem *>(item->parent());
  if ( hitem->group()!=SourceGroup ) return 0;
  QStrList uris;
  uris.append(QUriDrag::localFileToUri(item->url().filepath()));
  return new QUriDrag(uris, viewport());
}

bool ProjectManager::View::acceptDrag(QDropEvent* e) const
{
  if ( e->source()!=viewport() ) return false;
  const QListViewItem *item = itemAt(e->pos());
  if ( item==0 || item->rtti()!=FileRtti ) return false;
  const HeaderItem *hitem = static_cast<const HeaderItem *>(item->parent());
  return ( hitem->group()==SourceGroup );
}

void ProjectManager::View::filesReordered()
{
  if ( _project==0 ) return;
  _project->clearFiles();
  QListViewItem *item = headerItem(SourceGroup)->firstChild();
  for (;item; item=item->nextSibling())
    _project->addFile(static_cast<FileItem *>(item)->url());
}

QString ProjectManager::View::tooltip(QListViewItem *item, int) const
{
  switch (Rtti(item->rtti())) {
    case RootRtti:
    case FileRtti:
    case LinkerScriptRtti: return static_cast<const UrlItem *>(item)->url().filepath();
    case DeviceRtti:
    case RegisterRtti:
    case HeaderRtti: break;
  }
  return QString::null;
}

PURL::Url ProjectManager::View::linkerScriptUrl() const
{
  QListViewItemIterator it(const_cast<View *>(this));
  for(; it.current(); ++it) {
    if ( it.current()->rtti()!=LinkerScriptRtti ) continue;
    return static_cast<LinkerScriptItem *>(it.current())->url();
  }
  return PURL::Url();
}

bool ProjectManager::View::needsRecompile() const
{
  // ### this could be perfected...
  PURL::Url output = projectUrl().toFileType(PURL::Hex);
  QDateTime outputLastModified;
  if ( !output.exists(&outputLastModified) ) return true;
  PURL::UrlList files = Main::project()->absoluteFiles();
  PURL::UrlList::const_iterator it;
  for (it=files.begin(); it!=files.end(); ++it) {
    QDateTime lastModified;
    if ( !(*it).exists(&lastModified) ) continue;
    if ( lastModified>outputLastModified ) return true;
  }
  return false;
}
