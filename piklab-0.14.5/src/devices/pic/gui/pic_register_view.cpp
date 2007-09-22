/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_register_view.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpopupmenu.h>

#include <klocale.h>
#include <kiconloader.h>

#include "libgui/main_global.h"
#include "devices/gui/hex_word_editor.h"
#include "common/gui/misc_gui.h"
#include "devices/pic/base/pic.h"
#include "progs/base/generic_prog.h"
#include "progs/base/generic_debug.h"
#include "progs/base/prog_group.h"
#include "libgui/gui_debug_manager.h"
#include "coff/base/text_coff.h"

//-----------------------------------------------------------------------------
Pic::BankWidget::BankWidget(uint i, QWidget *parent)
  : QFrame(parent, "bank_widget"), _bindex(i), _bankCombo(0)
{
  setFrameStyle(WinPanel | Sunken);
  QGridLayout *top = new QGridLayout(this, 1, 1, 5, 0);
  top->setColSpacing(1, 4);
  QFont f("courier", font().pointSize());

  const Pic::Data &data = static_cast<const Pic::Data &>(*Main::deviceData());
  const Pic::RegistersData &rdata = data.registersData();
  bool debugging = Main::programmerGroup().isDebugger();
  uint row = 0;
  if ( rdata.nbBanks!=1 ) {
    if ( data.is18Family() ) {
      if ( (i/2)==0 ) {
        QString title = ((i%2)==0 ? i18n("Access Bank (low)") : i18n("Access Bank (high)"));
        QLabel *label = new QLabel(title, this);
        label->setAlignment(AlignCenter);
        top->addMultiCellWidget(label, row,row, 0,6, AlignHCenter);
      } else {
        _bankCombo = new QComboBox(this);
        for (uint k=1; k<2*rdata.nbBanks-1; k++) {
	  _bankCombo->insertItem((k%2)==0 ? i18n("Bank %1 (low)").arg(k/2) : i18n("Bank %1 (high)").arg(k/2));
	}
        if ( _bindex==3 ) _bankCombo->setCurrentItem(1);
        connect(_bankCombo, SIGNAL(activated(int)), SLOT(bankChanged()));
        top->addMultiCellWidget(_bankCombo, row,row, 0,6, AlignHCenter);
      }
    } else {
      QLabel *label = new QLabel(i18n("Bank %1").arg(i), this);
      label->setAlignment(AlignCenter);
      top->addMultiCellWidget(label, row,row, 0,6, AlignHCenter);
    }
    row++;
    top->setRowSpacing(row, 5);
    row++;
  }

  KIconLoader loader;
  QPixmap readIcon = loader.loadIcon("viewmag", KIcon::Small);
  QPixmap editIcon = loader.loadIcon("edit", KIcon::Small);
  uint nb;
  if ( !data.is18Family() ) nb = rdata.nbRegistersPerBank();
  else nb = kMax(rdata.accessBankSplit, rdata.nbRegistersPerBank() - rdata.accessBankSplit);
  _registers.resize(nb);
  for (uint k=0; k<nb; k++) {
    _registers[k].alabel = new QLabel(this);
    _registers[k].alabel->setFont(f);
    top->addWidget(_registers[k].alabel, row, 0);
    if (debugging) {
      _registers[k].button = new PopupButton(this);
      _registers[k].button->appendItem(i18n("Read"), readIcon, ReadId);
      _registers[k].button->appendItem(i18n("Edit"), editIcon, EditId);
      _registers[k].button->appendItem(i18n("Watch"), WatchId);
      connect(_registers[k].button, SIGNAL(activated(int)), SLOT(buttonActivated(int)));
      top->addWidget(_registers[k].button, row, 2);
      _registers[k].edit = new Register::LineEdit(this);
      connect(_registers[k].edit, SIGNAL(modified()), SLOT(write()));
      _registers[k].edit->setFont(f);
      top->addWidget(_registers[k].edit, row, 6);
    } else {
      _registers[k].label = new QLabel(this);
      top->addWidget(_registers[k].label, row, 2);
    }
    row++;
  }

  if (debugging) {
    top->setColSpacing(3, 5);
    top->setColSpacing(5, 5);
  }
  top->setRowStretch(row, 1);

  updateRegisterAddresses();
}

void Pic::BankWidget::bankChanged()
{
  updateRegisterAddresses();
  updateView();
}

uint Pic::BankWidget::bank() const
{
  const Pic::Data &data = static_cast<const Pic::Data &>(*Main::deviceData());
  if ( !data.is18Family() ) return _bindex;
  if ( _bindex==0 ) return 0;
  const Pic::RegistersData &rdata = data.registersData();
  if ( _bindex==1 ) return rdata.nbBanks - 1;
  return (_bankCombo->currentItem()+1)/2;
}

uint Pic::BankWidget::nbRegisters() const
{
  const Pic::Data &data = static_cast<const Pic::Data &>(*Main::deviceData());
  const Pic::RegistersData &rdata = data.registersData();
  if ( !data.is18Family() ) return rdata.nbRegistersPerBank();
  if ( _bindex==0 || (_bankCombo && _bankCombo->currentItem()==2*int(rdata.nbBanks)-3) ) return rdata.accessBankSplit;
  if ( _bindex==1 || (_bankCombo && _bankCombo->currentItem()==0) ) return rdata.nbRegistersPerBank() - rdata.accessBankSplit;
  return rdata.nbRegistersPerBank() / 2;
}

uint Pic::BankWidget::indexOffset() const
{
  const Pic::Data &data = static_cast<const Pic::Data &>(*Main::deviceData());
  const Pic::RegistersData &rdata = data.registersData();
  uint offset = bank() * rdata.nbRegistersPerBank();
  if ( !data.is18Family() ) return offset;
  if ( _bindex==0 || (_bankCombo && (_bankCombo->currentItem()%2)==1) ) return offset;
  if ( _bindex==1 || (_bankCombo && _bankCombo->currentItem()==0) ) return offset + rdata.accessBankSplit;
  return offset + rdata.nbRegistersPerBank()/2;
}

void Pic::BankWidget::updateRegisterAddresses()
{
  const Pic::Data &data = static_cast<const Pic::Data &>(*Main::deviceData());
  const Pic::RegistersData &rdata = data.registersData();
  uint nbChars = rdata.nbCharsAddress();
  uint nb = nbRegisters();
  uint offset = indexOffset();
  for (uint k=0; k<_registers.count(); k++) {
    if ( k<nb ) {
      _registers[k].alabel->show();
      _registers[k].address = rdata.addressFromIndex(offset + k);
      _registers[k].alabel->setText(toHexLabel(_registers[k].address, nbChars) + ":");
    } else _registers[k].alabel->hide();
  }
}

void Pic::BankWidget::buttonActivated(int id)
{
  const Pic::Data &data = static_cast<const Pic::Data &>(*Main::deviceData());
  const Pic::RegistersData &rdata = data.registersData();
  for (uint i=0; i<_registers.count(); i++) {
    if ( sender()!=_registers[i].button ) continue;
    Register::TypeData rtd(_registers[i].address, rdata.nbChars());
    switch (id) {
      case ReadId: Debugger::manager->readRegister(rtd); break;
      case EditId:
        _registers[i].edit->selectAll();
        _registers[i].edit->setFocus();
        break;
      case WatchId: {
        bool isWatched = Register::list().isWatched(rtd);
        Debugger::manager->setRegisterWatched(rtd, !isWatched);
        break;
      }
    }
    break;
  }
}

void Pic::BankWidget::write()
{
  const Pic::Data &data = static_cast<const Pic::Data &>(*Main::deviceData());
  const Pic::RegistersData &rdata = data.registersData();
  for (uint i=0; i<_registers.count(); i++) {
    if ( sender()!=_registers[i].edit ) continue;
    Register::TypeData rtd(_registers[i].address, rdata.nbChars());
    Debugger::manager->writeRegister(rtd, _registers[i].edit->value());
    break;
  }
}

void Pic::BankWidget::updateView()
{
  const Pic::Data &data = static_cast<const Pic::Data &>(*Main::deviceData());
  const Pic::RegistersData &rdata = data.registersData();
  bool active = Debugger::manager->isActive();
  const Coff::Object *coff = Debugger::manager->coff();
  uint nb = nbRegisters();
  for (uint i=0; i<_registers.count(); i++) {
    uint address = _registers[i].address;
    Device::RegisterProperties rp = rdata.properties(address);
    QString label = rdata.label(address);
    Register::TypeData rtd(address, rdata.nbChars());
    bool isWatched = Register::list().isWatched(rtd);
    if (coff) {
      QString name = coff->variableName(address);
      if ( !name.isEmpty() ) label = "<" + name + ">";
    }
    if (_registers[i].button) {
      if ( i<nb ) {
	_registers[i].button->show();
	_registers[i].button->setText(label);
	if (isWatched) {
	  QFont f = _registers[i].button->font();
	  f.setBold(true);
	  _registers[i].button->setFont(f);
	} else _registers[i].button->unsetFont();
	_registers[i].button->popup()->setItemEnabled(ReadId, active && (rp & Device::Readable));
	_registers[i].button->popup()->setItemEnabled(EditId, active);
	_registers[i].button->popup()->changeItem(WatchId, isWatched ? i18n("Stop Watching") : i18n("Watch"));
	_registers[i].button->popup()->setItemEnabled(WatchId, rp & Device::Readable);
      } else _registers[i].button->hide();
    }
    if (_registers[i].label) {
      if ( i<nb ) {
	_registers[i].label->show();
	_registers[i].label->setText(label);
      } else _registers[i].label->hide();
    }
    if (_registers[i].edit) {
      if ( i<nb ) {
	_registers[i].edit->show();
	_registers[i].edit->setEnabled(active);
	uint value = Register::list().value(rtd);
	if ( value!=Register::list().oldValue(rtd) ) _registers[i].edit->setColor(red);
	else _registers[i].edit->unsetColor();
	_registers[i].edit->setValue(Number::Hex, value, rdata.nbChars());
      } else _registers[i].edit->hide();
    }
  }
}

//-----------------------------------------------------------------------------
Pic::RegisterView::RegisterView(QWidget *parent)
  : Register::View(parent, "pic_register_view"),
    _readAllButton(0), _clearAllButton(0)
{
  QVBoxLayout *vbox = new QVBoxLayout(this, 10, 10);
  QHBoxLayout *hbox = new QHBoxLayout(vbox);

  bool debugging = Main::programmerGroup().isDebugger();
  const Pic::Data &data = static_cast<const Pic::Data &>(*Main::deviceData());
  uint nb = data.registersData().nbBanks;
  if ( debugging && nb!=0 ) {
    QWidget *w = new QWidget(this);
    hbox->addWidget(w);
    QGridLayout *grid = new QGridLayout(w, 1, 1, 0, 10);
    _readAllButton = new QPushButton(i18n("Read All"), w);
    connect(_readAllButton, SIGNAL(clicked()), Debugger::manager, SLOT(readAllRegisters()));
    grid->addWidget(_readAllButton, 0, 0);
    _clearAllButton = new QPushButton(i18n("Clear all watching"), w);
    connect(_clearAllButton, SIGNAL(clicked()), SLOT(stopWatchAllRegisters()));
    grid->addWidget(_clearAllButton, 0, 1);
    grid->setColStretch(2, 1);
  }

  QHBoxLayout *hbox2 = 0;
  if ( nb==0 ) {
    QLabel *label = new QLabel(i18n("Registers information not available."), this);
    vbox->addWidget(label);
  } else {
    hbox = new QHBoxLayout(vbox);
    hbox2 = new QHBoxLayout(hbox);
    hbox->addStretch(1);
  }
  if ( data.is18Family() ) {
    nb = 2;
    for (uint k=1; k<data.registersData().nbBanks-1; k++) {
      if ( !data.registersData().isBankUsed(k) ) continue;
      nb += 2;
      break;
    }
  }
  _banks.resize(nb);
  for (uint i=0; i<nb; i++) {
    _banks[i] = new BankWidget(i, this);
    _banks[i]->show();
    hbox2->addWidget(_banks[i]);
  }
  vbox->addStretch(1);
}

void Pic::RegisterView::updateView()
{
  if (_readAllButton) _readAllButton->setEnabled(Debugger::manager->isActive());
  for (uint i=0; i<_banks.count(); i++) if (_banks[i]) _banks[i]->updateView();
}

void Pic::RegisterView::stopWatchAllRegisters()
{
  Debugger::manager->stopWatchAll();
}

//----------------------------------------------------------------------------
Pic::RegisterListViewItem::RegisterListViewItem(const Register::TypeData &data, KListViewItem *parent)
  : Register::ListViewItem(data, parent)
{}

uint Pic::RegisterListViewItem::nbCharsAddress() const
{
  const Pic::Data &data = static_cast<const Pic::Data &>(*Main::deviceData());
  return data.registersData().nbCharsAddress();
}

QString Pic::RegisterListViewItem::label() const
{
  if ( _data.type()!=Register::Regular ) return _data.name();
  const Coff::Object *coff = Debugger::manager->coff();
  if (coff) {
    QString name = coff->variableName(_data.address());
    if ( !name.isEmpty() ) return "<" + name + ">";
  }
  const Pic::Data &data = static_cast<const Pic::Data &>(*Main::deviceData());
  return data.registersData().label(_data.address());
}
