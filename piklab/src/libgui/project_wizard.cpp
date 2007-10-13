/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "project_wizard.h"

#include <qheader.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qstyle.h>
#include <kfiledialog.h>
#include <kiconloader.h>

#include "common/gui/purl_gui.h"
#include "device_gui.h"
#include "tools/list/tool_list.h"
#include "tools/list/compile_config.h"
#include "main_global.h"
#include "project.h"
#include "tools/list/compile.h"
#include "common/gui/editlistbox.h"
#include "devices/list/device_list.h"

//-----------------------------------------------------------------------------
FileListItem::FileListItem(KListView *view)
  : KListViewItem(view), _group(0), _copy(true)
{
  KIconLoader loader;
  _pixmap = loader.loadIcon("button_ok", KIcon::Small);
}

void FileListItem::toggle()
{
  _copy = !_copy;
  repaint();
}

PURL::FileGroup FileListItem::fileGroup() const
{
  if ( _group==0 ) return PURL::Nb_FileGroups;
  PURL::Url url = PURL::Url::fromPathOrUrl(text(1));
  switch (url.data().group) {
  case PURL::Source: {
    FOR_EACH(PURL::ToolType, type)
      if ( _group->implementationType(type)==url.fileType() ) return PURL::Source;
    break;
  }
  case PURL::Header: return PURL::Header;
  case PURL::LinkerScript: {
    if ( _group->linkerScriptType()==url.fileType() ) return PURL::LinkerScript;
    break;
  }
  case PURL::LinkerObject: return PURL::LinkerObject;
  case PURL::Nb_FileGroups: break;
  }
  return PURL::Nb_FileGroups;
}

const QPixmap *FileListItem::pixmap(int column) const
{

  if ( column==0 && _copy ) return &_pixmap;
  return KListViewItem::pixmap(column);
}

//-----------------------------------------------------------------------------
FileListBox::FileListBox(QWidget *parent)
  : EditListBox(2, 0, 0, parent, "file_list_box")
{
  _listView->header()->show();
  _listView->header()->setClickEnabled(false);
  _listView->header()->setResizeEnabled(false);
  _listView->header()->setMovingEnabled(false);
  _listView->setColumnText(0, i18n("Copy"));
  int spacing = style().pixelMetric(QStyle::PM_HeaderMargin);
  QFontMetrics fm(font());
  _listView->header()->resizeSection(0, fm.width(i18n("Copy")) + 2*spacing); // hack
  _listView->setColumnText(1, i18n("Filename"));
  _listView->setAllColumnsShowFocus(true);
  connect(_listView, SIGNAL(clicked(QListViewItem *, const QPoint &, int)), SLOT(clicked(QListViewItem *, const QPoint &, int)));
}

void FileListBox::addItem()
{
  PURL::UrlList urls = PURL::getOpenUrls(_directory.path(), QString::null, this, i18n("Select Files"));
  PURL::UrlList::const_iterator it;
  for (it=urls.begin(); it!=urls.end(); ++it) EditListBox::addItem((*it).pretty());
}

QListViewItem *FileListBox::createItem()
{
  return new FileListItem(_listView);
}

void FileListBox::clicked(QListViewItem *item, const QPoint &, int column)
{
  if ( item==0 || column!=0 ) return;
  static_cast<FileListItem *>(item)->toggle();
}

void FileListBox::setToolGroup(const Tool::Group &group)
{
  QListViewItem *item = _listView->firstChild();
  for (; item; item = item->nextSibling()) static_cast<FileListItem *>(item)->setToolGroup(group);
}

//-----------------------------------------------------------------------------
ProjectWizard::ProjectWizard(QWidget *parent)
  : KWizard(parent, "project_wizard"), _project(0)
{
  // first page
  _first = new QWidget(this);
  addPage(_first, i18n("New Project: Basic Settings"));
  setHelpEnabled(_first, false);
  QGridLayout *grid = new QGridLayout(_first, 1, 1, 10, 10);
  QLabel *label = new QLabel(i18n("Name:"), _first);
  grid->addWidget(label, 0, 0);
  _name = new KLineEdit(_first);
  _name->setFocus();
  grid->addMultiCellWidget(_name, 0,0, 1,2);
  label = new QLabel(i18n("Directory:"), _first);
  grid->addWidget(label, 1, 0);
  _directory = new PURL::DirectoryWidget(":new_project", _first);
  grid->addMultiCellWidget(_directory, 1,1, 1,2);
  label = new QLabel(i18n("Device:"), _first);
  grid->addWidget(label, 2, 0);
  _device = new DeviceChooser::Button(false, _first);
  _device->setDevice(Main::device());
  grid->addWidget(_device, 2, 1);
  label = new QLabel(i18n("Toolchain:"), _first);
  grid->addWidget(label, 3, 0);
  _toolchain = new KeyComboBox(_first);
  Tool::Lister::ConstIterator it;
  for (it=Tool::lister().begin(); it!=Tool::lister().end(); ++it)
    _toolchain->appendItem(it.key(), it.data()->label());
  _toolchain->setCurrentItem(Main::toolGroup().name());
  grid->addWidget(_toolchain, 3, 1);
  grid->setColStretch(2, 1);
  grid->setRowStretch(4, 1);

  // second page
  _second = new QWidget(this);
  addPage(_second, i18n("New Project: Source Files"));
  grid = new QGridLayout(_second, 1, 1, 10, 10);
  _bgroup = new QVButtonGroup(i18n("Add Source Files"), _second);
  QRadioButton *button = new QRadioButton(i18n("Create template source file."), _bgroup);
  button->setChecked(true);
  (void)new QRadioButton(i18n("Add existing files."), _bgroup);
  (void)new QRadioButton(i18n("Do not add files now."), _bgroup);
  connect(_bgroup, SIGNAL(clicked(int)), SLOT(buttonClicked(int)));
  grid->addWidget(_bgroup, 0, 0);

  // third page
  _third = new QWidget(this);
  addPage(_third, i18n("New Project: Add Files"));
  setHelpEnabled(_third, false);
  setFinishEnabled(_third, true);
  grid = new QGridLayout(_third, 1, 1, 10, 10);
  _files = new FileListBox(_third);
  grid->addWidget(_files, 0, 0);
}

void ProjectWizard::next()
{
  if ( currentPage()==_first ) {
    QString name = _name->text().stripWhiteSpace();
    if ( name.isEmpty() ) {
      MessageBox::sorry(i18n("Project name is empty."), Log::Show);
      return;
    }
    PURL::Directory directory = _directory->directory();
    if ( directory.isEmpty() ) {
      MessageBox::sorry(i18n("Directory is empty."), Log::Show);
      return;
    }
    Log::StringView sview;
    if ( !directory.exists() ) {
      if ( !MessageBox::askContinue(i18n("Directory does not exists. Create it?")) ) return;
      if ( !_directory->directory().create(sview) ) {
        MessageBox::detailedSorry(i18n("Error creating directory."), sview.string(), Log::Show);
        return;
      }
    } else if ( url().exists() ) {
      if ( !MessageBox::askContinue(i18n("Project \"%1\"already exists. Overwrite it?").arg(url().filename())) ) return;
    }
    if ( !toolchain().check(device(), &Main::compileLog()) ) return;
    _files->setDirectory(_directory->directory());
    _files->setToolGroup(toolchain());
    buttonClicked(0);
  }
  if ( currentPage()==_third ) {
    uint nb = 0;
    for (uint i=0; i<_files->count(); i++)
      if ( static_cast<const FileListItem *>(_files->item(i))->fileGroup()==PURL::Source ) nb++;
    if ( toolchain().compileType()==Tool::SingleFile && nb>1 ) {
      if ( !MessageBox::askContinue(i18n("The selected toolchain can only compile a single source file and you have selected %1 source files. Continue anyway? ").arg(nb)) ) return;
    }
  }
  KWizard::next();
}

void ProjectWizard::done(int r)
{
  if ( r==Accepted ) {
    PURL::UrlList files;
    _project = new Project(url());
    Compile::Config::setDevice(_project, device());
    Compile::Config::setToolGroup(_project, toolchain());

    if ( _bgroup->selectedId()==0 ) {
      PURL::ToolType ttype = PURL::ToolType::Compiler;
      PURL::FileType ftype = PURL::Nb_FileTypes;
      FOR_EACH(PURL::ToolType, i) {
        PURL::FileType type = toolchain().implementationType(i);
        if ( type==PURL::Nb_FileTypes ) continue;
        ftype = type;
        ttype = i;
      }
      Q_ASSERT( ftype!=PURL::Nb_FileTypes );
      PURL::Url turl = url().toFileType(ftype);
      QString text;
      const Tool::SourceGenerator *generator = toolchain().sourceGenerator();
      if ( generator==0 ) text = i18n("Template source file generation not implemented yet for this toolchain...");
      else {
        bool ok = true;
        const Device::Data *data = Device::lister().data(device());
        SourceLine::List lines;
        SourceLine::List list = generator->templateSourceFile(ttype, *data, ok);
        if ( !ok ) lines.appendTitle(i18n("Template source file generation only partially implemented."));
        lines += list;
        text = SourceLine::text(PURL::DATA[ftype].sourceFamily, lines, 4);
      }
      Log::StringView sview;
      if ( turl.write(text, sview) ) files += turl;
      else MessageBox::detailedSorry(i18n("Error creating template file."), i18n("File: %1\n").arg(turl.pretty()) + sview.string(), Log::Show);
      _project->setOpenedFiles(files);
    } else {
      Log::StringView sview;
      for (uint i=0; i<_files->count(); i++) {
        PURL::Url furl = PURL::Url::fromPathOrUrl(_files->text(i));
        if ( static_cast<const FileListItem *>(_files->item(i))->copy() ) {
          PURL::Url to(url().directory(), furl.filename());
          if ( furl==to || furl.copyTo(to, sview) ) files += to;
        } else files += furl;
      }
    }
    PURL::UrlList::const_iterator it;
    for (it=files.begin(); it!=files.end(); ++it) _project->addFile(*it);
  }
  KWizard::done(r);
}

PURL::Url ProjectWizard::url() const
{
  return PURL::Url(PURL::Directory(_directory->directory()), _name->text(), PURL::Project);
}

const Tool::Group &ProjectWizard::toolchain() const
{
  return *Tool::lister().group(_toolchain->currentItem());
}

QString ProjectWizard::device() const
{
  return _device->device();
}

void ProjectWizard::buttonClicked(int id)
{
  setNextEnabled(_second, id==1);
  setFinishEnabled(_second, id!=1);
}
