/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "watch_view.h"

#include <qheader.h>
#include <qcombobox.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kiconloader.h>

#include "main_global.h"
#include "register_view.h"
#include "devices/base/device_group.h"
#include "devices/gui/device_group_ui.h"
#include "gui_debug_manager.h"
#include "editor_manager.h"

Register::WatchView::WatchView(QWidget *parent)
  : ListView(parent, "watch_view"), GenericView(Register::list()),
    _popup(0), _formatPopup(0), _data(0), _base(Number::Hex)
{
  connect(this, SIGNAL(mouseButtonClicked(int, QListViewItem *, const QPoint &, int)),
          SLOT(itemClicked(int, QListViewItem *, const QPoint &, int)));
  connect(this, SIGNAL(contextMenuRequested(QListViewItem *, const QPoint &, int)),
          SLOT(contextMenu(QListViewItem *, const QPoint &, int)));
  header()->hide();
  setSorting(-1);
  addColumn(QString::null);
  addColumn(QString::null);
  addColumn(QString::null);
  setFullWidth(true);
  setRootIsDecorated(false);
  setAllColumnsShowFocus(true);
  _root = new KListViewItem(this, i18n("Registers"));
  KIconLoader loader;
  _root->setPixmap(0, loader.loadIcon("piklab_chip", KIcon::Small));
  _root->setSelectable(false);
  _root->setOpen(true);
}

KPopupMenu *insertFormatMenu(QPopupMenu *popup, uint offset)
{
  KPopupMenu *pop = new KPopupMenu;
  pop->insertTitle(i18n("Format"));
  for (uint i=0; i<Number::Nb_Bases; i++)
    pop->insertItem(i18n(Number::DATA[i].label), offset+i);
  KIconLoader loader;
  QPixmap icon = loader.loadIcon("fonts", KIcon::Small);
  popup->insertItem(icon, i18n("Format"), pop);
  return pop;
}

void Register::WatchView::init(bool force)
{
  if ( !force && _data==Main::deviceData() ) return;
  _data = Main::deviceData();
  delete _popup;
  _popup = (Main::deviceData() ? Device::groupui(*_data).createWatchPopup(this, _ids) : 0);
  if (_popup) {
    _popup->insertSeparator();
    _formatPopup = insertFormatMenu(_popup, _ids.count());
    KIconLoader loader;
    QPixmap icon = loader.loadIcon("cancel", KIcon::Small);
    _popup->insertItem(icon, i18n("Clear"), Debugger::manager, SLOT(stopWatchAll()));
  }
  updateView();
}

void Register::WatchView::updateView()
{
  // delete items not watched anymore
  QValueVector<ListViewItem *> items;
  for (QListViewItem *item=_root->firstChild(); item; item=item->nextSibling()) {
    ListViewItem *ritem = static_cast<ListViewItem *>(item);
    if ( Register::list().isWatched(ritem->data()) ) continue;
    items.append(ritem);
  }
  for (uint i=0; i<items.count(); i++) delete items[i];
  // add new items
  QValueList<Register::TypeData> watched = Register::list().watched();
  items.resize(watched.count(), 0);
  for (QListViewItem *item=_root->firstChild(); item; item=item->nextSibling()) {
    ListViewItem *ritem = static_cast<ListViewItem *>(item);
    for (uint k=0; k<watched.count(); k++) {
      if ( ritem->data()==watched[k] ) {
        items[k] = ritem;
        break;
      }
    }
  }
  // take and insert in right order
  for (int k=watched.count()-1; k>=0; k--) {
    if ( items[k]==0 ) {
      items[k] = Device::groupui(*Main::deviceData()).createWatchItem(watched[k], _root);
      items[k]->setBase(_base);
    } else items[k]->updateView();
    _root->takeItem(items[k]);
    _root->insertItem(items[k]);
  }
}

void Register::WatchView::itemClicked(int button, QListViewItem *item, const QPoint &, int col)
{
  if ( item==0 || button!=LeftButton ) return;
  if ( item==_root ) Main::editorManager().openEditor(EditorManager::RegisterEditor);
  else if ( item->rtti()==RegisterRtti ) {
    if ( col==2 && Debugger::manager->isActive() ) static_cast<ListViewItem *>(item)->startRename();
    else item->setOpen(!item->isOpen());
  }
}

void Register::WatchView::contextMenu(QListViewItem *item, const QPoint &p, int)
{
  if ( item==0 ) return;
  if ( item==_root ) {
    if ( _popup==0 ) return;
    for (uint i=0; i<Number::Nb_Bases; i++)
      _formatPopup->setItemChecked(_ids.count()+i, _base==Number::Base(i));
    int res = _popup->exec(p);
    if ( res<0 ) return;
    if ( res<int(_ids.count()) ) Debugger::manager->setRegisterWatched(_ids[res], true);
    else {
      _base = Number::Base(res-_ids.count());
      for (QListViewItem *item=_root->firstChild(); item; item=item->nextSibling())
        static_cast<ListViewItem *>(item)->setBase(_base);
    }
  } else {
    if ( item->rtti()==Register::PortBitRtti ) return;
    Register::ListViewItem *ritem = static_cast<ListViewItem *>(item);
    KPopupMenu *pop = new KPopupMenu;
    pop->insertTitle(ritem->label());
    QPopupMenu *fpop = insertFormatMenu(pop, 0);
    for (uint i=0; i<Number::Nb_Bases; i++)
      fpop->setItemChecked(i, ritem->base()==Number::Base(i));
    pop->insertSeparator();
    KIconLoader loader;
    QPixmap icon = loader.loadIcon("edit", KIcon::Small);
    int editId = pop->insertItem(icon, i18n("Edit"));
    pop->setItemEnabled(editId, Debugger::manager->isActive());
    icon = loader.loadIcon("cancel", KIcon::Small);
    int removeId = pop->insertItem(icon, i18n("Remove"));
    int res = pop->exec(p);
    if ( res==editId ) ritem->startRename();
    else if ( res==removeId ) Debugger::manager->setRegisterWatched(ritem->data(), false);
    else if ( res>=0 ) ritem->setBase(Number::Base(res));
    delete pop;
  }
}

QString Register::WatchView::tooltip(const QListViewItem &item, int col) const
{
  if ( item.rtti()==Register::PortBitRtti ) return static_cast<const PortBitListViewItem &>(item).tooltip(col);
  if ( item.rtti()==Register::RegisterRtti ) return static_cast<const ListViewItem &>(item).tooltip(col);
  return QString::null;
}
