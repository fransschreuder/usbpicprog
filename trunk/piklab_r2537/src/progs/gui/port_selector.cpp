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
#include <ktextbrowser.h>

#include "common/port/serial.h"
#include "common/port/parallel.h"
#include "common/global/about.h"
#include "common/gui/purl_gui.h"
#include "progs/base/prog_config.h"

const char * const PortSelector::LABELS[PortType::Nb_Types] = {
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
  FOR_EACH(PortType, type) {
    _combos[type.type()] = 0;
    _status[type.type()] = 0;
    if ( !group.isPortSupported(type) ) continue;
    Port::Description pd(type, Programmer::GroupConfig::portDevice(group, type));
    addPortType(pd);
  }
  _bgroup->setButton(Programmer::GroupConfig::portType(group).type());
  _main->show();
}

void PortSelector::addPortType(const Port::Description &pd)
{
  QString noDeviceMessage, notAvailableMessage;
  switch (pd.type.type()) {
  case PortType::Serial:
    noDeviceMessage = i18n("Your computer might not have any serial port.");
    break;
  case PortType::Parallel:
    noDeviceMessage = i18n("Your computer might not have any parallel port "
                           "or the /dev/parportX device has not been created.<br>"
                           "Use \"mknod /dev/parport0 c 99 0\" to create it<br>"
                           "and \"chmod a+rw /dev/parport0\" to make it RW enabled.");
    notAvailableMessage = i18n("Piklab has been compiled without support for parallel port.");
    break;
  case PortType::USB:
    notAvailableMessage = i18n("Piklab has been compiled without support for USB port.");
    break;
  case PortType::Nb_Types: Q_ASSERT(false); break;
  }

  QRadioButton *rb = new QRadioButton(i18n(LABELS[pd.type.type()]), _main);
  _bgroup->insert(rb, pd.type.type());
  if ( _bgroup->count()==1 ) _bgroup->setButton(pd.type.type());
  _grid->addWidget(rb, 3*(_bgroup->count()-1), 0);
  _status[pd.type.type()] = new QLabel(_main);
  _grid->addWidget(_status[pd.type.type()], 3*(_bgroup->count()-1), 2);

  QStringList list = Port::probedDeviceList(pd.type);
  bool noDevice = ( list.isEmpty() && pd.type.data().withDevice );
  bool notAvailable = !Port::isAvailable(pd.type);
  rb->setEnabled(!notAvailable);
  if ( noDevice || notAvailable ) {
    KTextBrowser *comment = new KTextBrowser(_main);
    QString text = (notAvailable ? notAvailableMessage : noDeviceMessage);
    text += "<p>";
    text += i18n("<a href=\"%1\">See Piklab homepage for help</a>.").arg(Piklab::URLS[Piklab::Support]);
    comment->setText(text);
    _grid->addMultiCellWidget(comment, 3*(_bgroup->count()-1)+1,3*(_bgroup->count()-1)+1, 0,3);
  }

  if (pd.type.data().withDevice) {
    _combos[pd.type.type()] = new QComboBox(true, _main);
    for (uint i=0; i<list.count(); i++) _combos[pd.type.type()]->insertItem(list[i]);
    if ( !pd.device.isEmpty() && !list.contains(pd.device) ) _combos[pd.type.type()]->insertItem(pd.device);
    _combos[pd.type.type()]->setCurrentText(pd.device);
    connect(_combos[pd.type.type()], SIGNAL(activated(int)), SIGNAL(changed()));
    connect(_combos[pd.type.type()], SIGNAL(textChanged(const QString &)), SLOT(textChanged()));
    _grid->addWidget(_combos[pd.type.type()], 3*(_bgroup->count()-1), 1);
  }
}

void PortSelector::setStatus(PortType ptype, const QString &message)
{
  _pending = false;
  FOR_EACH(PortType, type) {
    if ( _status[type.type()]==0 ) continue;
    if ( type!=ptype ) _status[type.type()]->hide();
    else {
      _status[type.type()]->show();
      _status[type.type()]->setText(message);
    }
  }
}

QString PortSelector::device(PortType type) const
{
  if ( type==PortType::Nb_Types || _combos[type.type()]==0 || !type.data().withDevice ) return QString::null;
  return _combos[type.type()]->currentText();
}

void PortSelector::saveConfig()
{
  Programmer::GroupConfig::writePortType(*_group, type());
  FOR_EACH(PortType, type) {
    if ( !_group->isPortSupported(type) ) continue;
    Programmer::GroupConfig::writePortDevice(*_group, type, device(type));
  }
}

PortType PortSelector::type() const
{
  if ( _bgroup->count()==0 ) return PortType::Nb_Types;
  return PortType::Type(_bgroup->selectedId());
}

void PortSelector::textChanged()
{
  if (_pending) return;
  _status[type().type()]->hide();
  _pending = true;
  QTimer::singleShot(1000, this, SIGNAL(changed()));
}
