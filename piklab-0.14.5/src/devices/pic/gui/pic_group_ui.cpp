/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_group_ui.h"

#include <klocale.h>
#include <kpopupmenu.h>
#include <kaction.h>

#include "libgui/main_global.h"
#include "pic_hex_view.h"
#include "pic_register_view.h"
#include "pic_config_editor.h"
#include "coff/base/text_coff.h"
#include "libgui/gui_debug_manager.h"

Device::HexView *Pic::GroupUI::createHexView(const HexEditor &editor, QWidget *parent) const
{
  return new HexView(editor, parent);
}

Register::View *Pic::GroupUI::createRegisterView(QWidget *parent) const
{
  return new RegisterView(parent);
}

Device::MemoryEditor *Pic::GroupUI::createConfigEditor(Device::Memory &memory, QWidget *parent) const
{
  return new MemoryConfigEditorWidget(static_cast<Memory &>(memory), false, parent);
}

KPopupMenu *Pic::GroupUI::createWatchPopup(QWidget *parent, QValueVector<Register::TypeData> &ids) const
{
  ids.clear();
  const Pic::Data &data = static_cast<const Pic::Data &>(*Main::deviceData());
  const Pic::RegistersData &rdata = data.registersData();
  KPopupMenu *popup = new KPopupMenu(parent);
  popup->insertTitle(i18n("Watch Register"));
  KPopupMenu *pop = new KPopupMenu(popup);
  pop->insertTitle(i18n("SFRs"));
  QValueVector<Pic::RegisterNameData> list = Pic::sfrList(data);
  for (uint i=0; i<list.count(); i++) {
    pop->insertItem(list[i].label(), ids.count());
    ids.append(list[i].data());
  }
  popup->insertItem(i18n("SFRs"), pop);
  pop = new KPopupMenu(popup);
  pop->insertTitle(i18n("I/Os"));
  for (uint i=0; i<Device::MAX_NB_PORTS; i++) {
    if ( !rdata.hasPort(i) ) continue;
    QString name = rdata.portName(i);
    pop->insertItem(name, ids.count());
    ids.append(Register::TypeData(rdata.sfrs[name].address, rdata.nbChars()));
  }
  popup->insertItem(i18n("I/Os"), pop);
  pop = new KPopupMenu(popup);
  pop->insertTitle(i18n("GPRs"));
  const Coff::Object *coff = Debugger::manager->coff();
  list = Pic::gprList(data, coff);
  for (uint k=0; k<rdata.nbBanks; k++) {
    if ( !rdata.isBankUsed(k) ) continue;
    KPopupMenu *bpop = (rdata.nbBanks==1 ? pop : new KPopupMenu(pop));
    if ( rdata.nbBanks!=1 ) bpop->insertTitle(i18n("Bank %1").arg(k));
    uint nb = 0;
    for (uint i=0; i<list.count(); i++) {
      if ( rdata.bankFromAddress(list[i].data().address())!=k ) continue;
      bpop->insertItem(list[i].label(), ids.count());
      ids.append(list[i].data());
      nb++;
    }
    if ( rdata.nbBanks!=1 && nb!=0 ) pop->insertItem(i18n("Bank %1").arg(k), bpop);
  }
  popup->insertItem(i18n("GPRs"), pop);
  pop = new KPopupMenu(popup);
  pop->insertTitle(i18n("Variables"));
  if (coff) {
    list = Pic::variableList(data, *coff);
    if ( list.count()==0 ) {
      pop->insertItem(i18n("No variable"), ids.count());
      pop->setItemEnabled(ids.count(), false);
      ids.append(Register::TypeData());
    } else for (uint i=0; i<list.count(); i++) {
      pop->insertItem(list[i].label(), ids.count());
      ids.append(list[i].data());
    }
  } else {
    pop->insertItem(i18n("Please compile the current project"), ids.count());
    pop->setItemEnabled(ids.count(), false);
    ids.append(Register::TypeData());
  }
  popup->insertItem(i18n("Variables"), pop);
  return popup;
}

Register::ListViewItem *Pic::GroupUI::createWatchItem(const Register::TypeData &data, KListViewItem *parent) const
{
  return new Pic::RegisterListViewItem(data, parent);
}
