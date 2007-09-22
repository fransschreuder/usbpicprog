/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "port_selector.h"

#include <qtimer.h>
#include <klocale.h>

#include "common/port/serial.h"
#include "common/port/parallel.h"
#include "common/global/about.h"
#include "common/gui/purl_gui.h"
#include "progs/base/prog_config.h"

const char * const PortSelector::LABELS[Port::Nb_Types] = {
  I18N_NOOP("Serial"), I18N_NOOP("Parallel"), I18N_NOOP("USB")
};

PortSelector::PortSelector(QWidget *parent)
  : QFrame(parent, "port_selector"), _group(0), _main(0)
{
  _top = new QGridLayout(this, 1, 1, 0, 10);
  _top->setRowStretch(1, 1);

  _bgroup = new QButtonGroup;
  connect(_bgroup, SIGNAL(clicked(int)), SIGNAL(changed()));
}

void PortSelector::setGroup(const Programmer::Group &group)
{
  _group = &group;
  delete _main;
  _main = new QWidget(this);
  _top->addWidget(_main, 0, 0);
  _grid = new QGridLayout(_main, 1, 1, 0, 10);
  _grid->setColStretch(3, 1);
  Programmer::GroupConfig config(group);
  for (uint i=0; i<Port::Nb_Types; i++) {
    _combos[i] = 0;
    _status[i] = 0;
    if ( !group.isPortSupported(Port::Type(i)) ) continue;
    Port::Type ptype = Port::Type(i);
    Port::Description pd(ptype, config.portDevice(ptype));
    addPortType(pd);
  }
  _bgroup->setButton(config.portType());
  _main->show();
}

void PortSelector::addPortType(const Port::Description &pd)
{
  QString noDeviceMessage, notAvailableMessage;
  switch (pd.type) {
  case Port::SerialType:
    noDeviceMessage = i18n("<qt>Your computer might not have any serial port.</qt>");
    break;
  case Port::ParallelType:
    noDeviceMessage = i18n("<qt>Your computer might not have any parallel port "
                           "or the /dev/parportX device has not been created.<br>"
                           "Use \"mknod /dev/parport0 c 99 0\" to create it<br>"
                           "and \"chmod a+rw /dev/parport0\" to make it RW enabled.</qt>");
    notAvailableMessage = i18n("<qt>Piklab has been compiled without support for parallel port.</qt>");
    break;
  case Port::USBType:
    notAvailableMessage = i18n("<qt>Piklab has been compiled without support for USB port.</qt>");
    break;
  case Port::Nb_Types: Q_ASSERT(false); break;
  }

  QRadioButton *rb = new QRadioButton(i18n(LABELS[pd.type]), _main);
  _bgroup->insert(rb, pd.type);
  if ( _bgroup->count()==1 ) _bgroup->setButton(pd.type);
  _grid->addWidget(rb, 3*(_bgroup->count()-1), 0);

  QStringList list = Port::probedDeviceList(pd.type);
  bool noDevice = ( list.isEmpty() && Port::DATA[pd.type].withDevice );
  bool notAvailable = !Port::isAvailable(pd.type);
  rb->setEnabled(!notAvailable);
  if ( noDevice || notAvailable ) {
    QLabel *label = new QLabel(notAvailable ? notAvailableMessage : noDeviceMessage, _main);
    _grid->addMultiCellWidget(label, 3*(_bgroup->count()-1)+1,3*(_bgroup->count()-1)+1, 0,3);
    PURL::Label *ul = new PURL::Label(Piklab::URLS[Piklab::Support], i18n("See Piklab homepage for help."), _main);
    _grid->addMultiCellWidget(ul, 3*(_bgroup->count()-1)+2,3*(_bgroup->count()-1)+2, 0,3);
  }
  _status[pd.type] = new QLabel(_main);
  _grid->addWidget(_status[pd.type], 3*(_bgroup->count()-1), 2);

  if ( Port::DATA[pd.type].withDevice ) {
    _combos[pd.type] = new QComboBox(true, _main);
    for (uint i=0; i<list.count(); i++) _combos[pd.type]->insertItem(list[i]);
    if ( !pd.device.isEmpty() && !list.contains(pd.device) ) _combos[pd.type]->insertItem(pd.device);
    _combos[pd.type]->setCurrentText(pd.device);
    connect(_combos[pd.type], SIGNAL(activated(int)), SIGNAL(changed()));
    connect(_combos[pd.type], SIGNAL(textChanged(const QString &)), SLOT(textChanged()));
    _grid->addWidget(_combos[pd.type], 3*(_bgroup->count()-1), 1);
  }
}

void PortSelector::setStatus(Port::Type type, const QString &message)
{
  _pending = false;
  for (uint i=0; i<Port::Nb_Types; i++) {
    if ( _status[i]==0 ) continue;
    if ( Port::Type(i)!=type ) _status[i]->hide();
    else {
      _status[i]->show();
      _status[i]->setText(message);
    }
  }
}

QString PortSelector::device(Port::Type type) const
{
  if ( type==Port::Nb_Types || _combos[type]==0 || !Port::DATA[type].withDevice ) return QString::null;
  return _combos[type]->currentText();
}

void PortSelector::saveConfig()
{
  Programmer::GroupConfig config(*_group);
  config.writePortType(type());
  for (uint i=0; i<Port::Nb_Types; i++) {
    if ( !_group->isPortSupported(Port::Type(i)) ) continue;
    Port::Type ptype = Port::Type(i);
    config.writePortDevice(ptype, device(ptype));
  }
}

Port::Type PortSelector::type() const
{
  if ( _bgroup->count()==0 ) return Port::Nb_Types;
  return Port::Type(_bgroup->selectedId());
}

void PortSelector::textChanged()
{
  if (_pending) return;
  _status[type()]->hide();
  _pending = true;
  QTimer::singleShot(1000, this, SIGNAL(changed()));
}
