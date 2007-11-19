/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_group_ui.h"

#include <kaction.h>

#include "libgui/main_global.h"
#include "pic_hex_view.h"
#include "pic_register_view.h"
#include "pic_config_editor.h"
#include "coff/base/text_coff.h"
#include "libgui/gui_debug_manager.h"
#include "common/gui/list_container.h"

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

void Pic::GroupUI::fillWatchListContainer(ListContainer *container, QValueVector<Register::TypeData> &ids) const
{
  ids.clear();
  const Pic::Data &data = static_cast<const Pic::Data &>(*Main::deviceData());
  const Pic::RegistersData &rdata = data.registersData();
  ListContainer *branch = container->appendBranch(i18n("SFRs"));
  QValueVector<Pic::RegisterNameData> list = Pic::sfrList(data);
  for (uint i=0; i<list.count(); i++) {
    branch->appendItem(list[i].label(), ids.count(), ListContainer::UnChecked);
    ids.append(list[i].data());
  }
  branch = container->appendBranch(i18n("I/Os"));
  for (uint i=0; i<Device::MAX_NB_PORTS; i++) {
    if ( !rdata.hasPort(i) ) continue;
    QString name = rdata.portName(i);
    branch->appendItem(name, ids.count(), ListContainer::UnChecked);
    ids.append(Register::TypeData(rdata.sfrs[name].address, rdata.nbChars()));
  }
  branch = container->appendBranch(i18n("GPRs"));
  const Coff::Object *coff = Debugger::manager->coff();
  list = Pic::gprList(data, coff);
  for (uint k=0; k<rdata.nbBanks; k++) {
    if ( !rdata.isBankUsed(k) ) continue;
    ListContainer *bbranch = (rdata.nbBanks==1 ? branch : branch->appendBranch(i18n("Bank %1").arg(k)));
    uint nb = 0;
    for (uint i=0; i<list.count(); i++) {
      if ( rdata.bankFromAddress(list[i].data().address())!=k ) continue;
      bbranch->appendItem(list[i].label(), ids.count(), ListContainer::UnChecked);
      ids.append(list[i].data());
      nb++;
    }
  }
  branch = container->appendBranch(i18n("Variables"));
  if (coff) {
    list = Pic::variableList(data, *coff);
    if ( list.count()==0 ) {
      branch->appendItem(i18n("No variable"), ids.count(), ListContainer::Disabled);
      ids.append(Register::TypeData());
    } else for (uint i=0; i<list.count(); i++) {
      branch->appendItem(list[i].label(), ids.count(), ListContainer::UnChecked);
      ids.append(list[i].data());
    }
  } else {
    branch->appendItem(i18n("Please compile the current project"), ids.count(), ListContainer::Disabled);
    ids.append(Register::TypeData());
  }
}

Register::ListViewItem *Pic::GroupUI::createWatchItem(const Register::TypeData &data, KListViewItem *parent) const
{
  return new Pic::RegisterListViewItem(data, parent);
}
