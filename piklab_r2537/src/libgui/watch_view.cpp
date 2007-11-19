/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "watch_view.h"

#include <qheader.h>
#include <qcombobox.h>
#include <kiconloader.h>

#include "main_global.h"
#include "register_view.h"
#include "devices/base/device_group.h"
#include "devices/gui/device_group_ui.h"
#include "gui_debug_manager.h"
#include "editor_manager.h"
#include "common/gui/list_container.h"

//-----------------------------------------------------------------------------
Register::BaseListView::BaseListView(QWidget *parent)
  : ListView(parent), _root(0)
{
  header()->hide();
  setSorting(-1);
  setFullWidth(true);
  setRootIsDecorated(false);
  setAllColumnsShowFocus(true);
  connect(this, SIGNAL(mouseButtonClicked(int, QListViewItem *, const QPoint &, int)),
          SLOT(itemClicked(int, QListViewItem *, const QPoint &, int)));
  connect(this, SIGNAL(contextMenuRequested(QListViewItem *, const QPoint &, int)),
          SLOT(contextMenu(QListViewItem *, const QPoint &, int)));
}

//-----------------------------------------------------------------------------
Register::RegisterListView::RegisterListView(QWidget *parent)
  : BaseListView(parent)
{
  addColumn(QString::null);
}

void Register::RegisterListView::init(const Device::Data *data)
{
  delete _root;
  _root = new ListViewItemContainer(i18n("Registers"), this);
  KIconLoader loader;
  _root->setPixmap(0, loader.loadIcon("piklab_chip", KIcon::Small));
  _root->setSelectable(false);
  _root->setOpen(true);
  if (data) Device::groupui(*data).fillWatchListContainer(_root, _ids);
}

void Register::RegisterListView::updateView()
{
  QListViewItemIterator it(_root);
  for (; it.current(); ++it) {
    int id = _root->id(it.current());
    if ( id==-1 || _ids[id].type()==Invalid ) continue;
    bool watched = Register::list().isWatched(_ids[id]);
    static_cast<QCheckListItem *>(it.current())->setOn(watched);
  }
}

void Register::RegisterListView::itemClicked(int button, QListViewItem *item, const QPoint &, int)
{
  if ( item==0 || button!=LeftButton ) return;
  if ( item==firstChild() ) Main::editorManager().openEditor(EditorManager::RegisterEditor);
  int id = _root->id(item);
  if ( id==-1 || _ids[id].type()==Invalid ) return;
  bool watched = Register::list().isWatched(_ids[id]);
  static_cast<QCheckListItem *>(item)->setOn(!watched);
  Debugger::manager->setRegisterWatched(_ids[id], !watched);
}

//-----------------------------------------------------------------------------
Register::WatchedListView::WatchedListView(QWidget *parent)
  : BaseListView(parent), _popup(0), _base(NumberBase::Hex)
{
  setSorting(0);
  addColumn(QString::null);
  addColumn(QString::null);
  addColumn(QString::null);

  _root = new ListViewItemContainer(i18n("Watched"), this);
  KIconLoader loader;
  _root->setPixmap(0, loader.loadIcon("viewmag", KIcon::Small));
  _root->setSelectable(false);
  _root->setOpen(true);
}

KPopupMenu *Register::WatchedListView::appendFormatMenu(KPopupMenu *parent, uint offset)
{
  KIconLoader loader;
  QPixmap icon = loader.loadIcon("fonts", KIcon::Small);
  KPopupMenu *popup = new KPopupMenu;
  popup->insertTitle(i18n("Format"));
  FOR_EACH(NumberBase, base) popup->insertItem(i18n(base.label()), offset + base.type());
  parent->insertItem(icon, i18n("Format"), popup);
  return popup;
}

void Register::WatchedListView::init(const Device::Data *data)
{
  delete _popup;
  _popup = 0;
  if ( data==0 ) return;
  _popup = new PopupContainer(i18n("Watch Register"), this);
  Device::groupui(*data).fillWatchListContainer(_popup, _ids);
  _popup->insertSeparator();
  _formatPopup = appendFormatMenu(_popup, _ids.count());
  KIconLoader loader;
  QPixmap icon = loader.loadIcon("cancel", KIcon::Small);
  _popup->insertItem(icon, i18n("Clear"), Debugger::manager, SLOT(stopWatchAll()));
}

void Register::WatchedListView::updateView()
{
  // delete items not watched anymore
  for (QListViewItem *item=_root->firstChild(); item;) {
    ListViewItem *ritem = static_cast<ListViewItem *>(item);
    item = item->nextSibling();
    if ( !Register::list().isWatched(ritem->data()) ) delete ritem;
  }
  // add new items
  bool added = false;
  QValueList<Register::TypeData> watched = Register::list().watched();
  QValueVector<ListViewItem *> items(watched.count());
  for (uint k=0; k<watched.count(); k++) {
    QListViewItem *item = _root->firstChild();
    for (; item; item=item->nextSibling())
      if ( static_cast<ListViewItem *>(item)->data()==watched[k] ) break;
    if (item) {
      items[k] = static_cast<ListViewItem *>(item);
      items[k]->updateView();
    } else {
      items[k] = Device::groupui(*Main::deviceData()).createWatchItem(watched[k], _root);
      items[k]->setBase(_base);
      added = true;
    }
  }
}

QString Register::WatchedListView::tooltip(const QListViewItem &item, int col) const
{
  if ( item.rtti()==Register::PortBitRtti ) return static_cast<const PortBitListViewItem &>(item).tooltip(col);
  if ( item.rtti()==Register::RegisterRtti ) return static_cast<const ListViewItem &>(item).tooltip(col);
  return QString::null;
}

void Register::WatchedListView::itemClicked(int button, QListViewItem *item, const QPoint &, int col)
{
  if ( item==0 || button!=LeftButton ) return;
  else if ( item->rtti()==RegisterRtti ) {
    if ( col==2 && Main::programmerState()==Programmer::Halted ) static_cast<ListViewItem *>(item)->startRename();
    else item->setOpen(!item->isOpen());
  }
}

void Register::WatchedListView::contextMenu(QListViewItem *item, const QPoint &p, int)
{
  if ( item==0 ) return;
  if ( item==firstChild() ) {
    if ( _popup==0 ) return;
    FOR_EACH(NumberBase, base) _formatPopup->setItemChecked(_ids.count()+base.type(), _base==base);
    int res = _popup->exec(p);
    if ( res<0 ) return;
    if ( res<int(_ids.count()) ) Debugger::manager->setRegisterWatched(_ids[res], true);
    else {
      _base = NumberBase::Type(res-_ids.count());
      for (QListViewItem *item=_root->firstChild(); item; item=item->nextSibling())
        static_cast<ListViewItem *>(item)->setBase(_base);
    }
  } else {
    if ( item->rtti()==Register::PortBitRtti ) return;
    Register::ListViewItem *ritem = static_cast<ListViewItem *>(item);
    KPopupMenu *pop = new KPopupMenu;
    pop->insertTitle(ritem->label());
    QPopupMenu *fpop = appendFormatMenu(pop, 0);
    FOR_EACH(NumberBase, base) fpop->setItemChecked(base.type(), ritem->base()==base);
    pop->insertSeparator();
    KIconLoader loader;
    QPixmap icon = loader.loadIcon("edit", KIcon::Small);
    int editId = pop->insertItem(icon, i18n("Edit"));
    pop->setItemEnabled(editId, Main::programmerState()==Programmer::Halted);
    icon = loader.loadIcon("cancel", KIcon::Small);
    int removeId = pop->insertItem(icon, i18n("Remove"));
    int res = pop->exec(p);
    if ( res==editId ) ritem->startRename();
    else if ( res==removeId ) Debugger::manager->setRegisterWatched(ritem->data(), false);
    else if ( res>=0 ) ritem->setBase(NumberBase::Type(res));
    delete pop;
  }
}

//-----------------------------------------------------------------------------
Register::WatchView::WatchView(QWidget *parent)
  : QWidget(parent, "watch_view"), GenericView(Register::list()), _data(0)
{
  QVBoxLayout *top = new QVBoxLayout(this);
  QValueList<int> sizes;
  sizes.append(50);
  Splitter *splitter = new Splitter(sizes, Qt::Vertical, this, "watch_window_splitter");
  top->addWidget(splitter);

  _registerListView = new RegisterListView(splitter);
  _watchedListView = new WatchedListView(splitter);
}

void Register::WatchView::init(bool force)
{
  if ( !force && _data==Main::deviceData() ) return;
  _data = Main::deviceData();
  _registerListView->init(_data);
  _watchedListView->init(_data);
  updateView();
}

void Register::WatchView::updateView()
{
  _registerListView->updateView();
  _watchedListView->updateView();
}
