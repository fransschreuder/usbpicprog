/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "breakpoint_view.h"

#include <qlayout.h>
#include <klocale.h>
#include <qpopupmenu.h>

#include "main_global.h"
#include "editor_manager.h"
#include "coff/base/text_coff.h"
#include "gui_debug_manager.h"

//----------------------------------------------------------------------------
void Breakpoint::updateActions(const Data *data)
{
  bool hasBreakpoint = (data ? Breakpoint::list().contains(*data) : false);
  Main::action("toggle_breakpoint")->setText(hasBreakpoint ? i18n("Remove breakpoint") : i18n("Set breakpoint"));
  Main::action("toggle_breakpoint")->setEnabled(data);
  bool isActive = (hasBreakpoint ? Breakpoint::list().state(*data)==Breakpoint::Active : false);
  Main::action("enable_breakpoint")->setText(!isActive ? i18n("Enable breakpoint") : i18n("Disable breakpoint"));
  Main::action("enable_breakpoint")->setEnabled(Debugger::manager->coff() && hasBreakpoint);
}

//----------------------------------------------------------------------------
Breakpoint::ListViewItem::ListViewItem(ListView *parent, const Data &data)
  : KListViewItem(parent), _data(data)
{}

//----------------------------------------------------------------------------
Breakpoint::View::View(QWidget *parent)
  : QWidget(parent, "breakpoints_view"), GenericView(Breakpoint::list()),
    _currentData(0)
{
  QVBoxLayout *top = new QVBoxLayout(this);
  _listview = new ListView(this);
  connect(_listview, SIGNAL(clicked(QListViewItem *)), SLOT(itemClicked(QListViewItem *)));
  connect(_listview, SIGNAL(contextMenuRequested(QListViewItem *, const QPoint &, int)),
          SLOT(contextMenu(QListViewItem *, const QPoint &, int)));
  _listview->setAllColumnsShowFocus(true);
  _listview->addColumn(i18n("Status"));
  _listview->addColumn(i18n("Location"));
  _listview->addColumn(i18n("Address"));
  top->addWidget(_listview);
}

void Breakpoint::View::updateView()
{
  // #### flickering...
  _listview->clear();
  for (uint i=0; i<Breakpoint::list().count(); i++) {
    const Data &data = Breakpoint::list().data(i);
    KListViewItem *item = new ListViewItem(_listview, data);
    item->setPixmap(0, TextEditor::pixmap(Debugger::manager->breakpointType(data)));
    item->setText(1, data.url.filename() + ":" + QString::number(data.line));
    uint address = Breakpoint::list().address(data);
    if ( address!=Device::INVALID ) item->setText(2, toHexLabel(address, nbChars(address)));
    else if ( Debugger::manager->coff() ) item->setText(2, i18n("Non-code breakpoint"));
    else item->setText(2, "---");
  }
}

void Breakpoint::View::itemClicked(QListViewItem *item)
{
  if ( item==0 ) return;
  const Data &data = static_cast<ListViewItem *>(item)->data();
  uint address = Breakpoint::list().address(data);
  TextEditor *editor = ::qt_cast<TextEditor *>(Main::currentEditor());
  const Coff::TextObject *coff = Debugger::manager->coff();
  int line = -1;
  if ( coff && editor && editor->fileType()==PURL::Coff && address!=Device::INVALID )
    line = coff->lineForAddress(editor->url(), address);
  if ( line==-1 ) {
    editor = ::qt_cast<TextEditor *>(Main::editorManager().openEditor(data.url));
    line = data.line;
  }
  if ( editor==0 ) return;
  editor->show();
  editor->setCursor(line, 0);
}

void Breakpoint::View::contextMenu(QListViewItem *item, const QPoint &pos, int)
{
  _currentData = (item ? &static_cast<ListViewItem *>(item)->data() : 0);
  updateActions(_currentData);
  Main::popup("breakpoint_context_menu").exec(pos);
  _currentData = 0;
}
