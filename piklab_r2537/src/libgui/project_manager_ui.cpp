/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "project_manager_ui.h"

#include <qpainter.h>
#include <kiconloader.h>

#include "project.h"
#include "devices/list/device_list.h"
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
  switch (type.data().group) {
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
  switch (_ftype.type()) {
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
