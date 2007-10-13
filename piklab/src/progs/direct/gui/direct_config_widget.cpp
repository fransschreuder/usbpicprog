/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "direct_config_widget.h"

#include <qtooltip.h>
#include <qcheckbox.h>
#include <qtimer.h>

#include "progs/direct/base/direct_prog_config.h"
#include "progs/direct/base/direct_prog.h"

//-----------------------------------------------------------------------------
::Programmer::ConfigWidget *Direct::GroupUI::createConfigWidget(QWidget *parent) const
{
  return new ConfigWidget(static_cast<const ::Programmer::Group &>(group()), parent);
}

//-----------------------------------------------------------------------------
const char * const INV_PIN_LABEL = I18N_NOOP("Check this option if your hardware uses negative logic for this pin.");
const char * const DELAY_LABEL = I18N_NOOP("Some programming cards need low clock rate:\nadding delay to clock pulses might help.");

Direct::HConfigWidget::HConfigWidget(::Programmer::Base &base, QWidget *parent, bool edit)
  : ::Hardware::HConfigWidget(base, parent, edit)
{
  // pins assignment
  QGroupBox *groupb = new QGroupBox(1, Horizontal, i18n("Pin assignment"), this);
  _mainVBox->addWidget(groupb);
  QWidget *w = new QWidget(groupb);
  QGridLayout *grid = new QGridLayout(w, 1, 1, 0, 10);
  if (edit) grid->setColStretch(5, 1);
  for (uint i=0; i<Nb_PinTypes; i++) {
    QLabel *label = new QLabel(i18n(PIN_DATA[i].label), w);
    QToolTip::add(label, PIN_DATA[i].comment);
    grid->addWidget(label, i, 0);
    _combos[i] = new QComboBox(w);
    _combos[i]->setEnabled(edit);
    connect(_combos[i], SIGNAL(activated(int)), SLOT(slotPinChanged()));
    QToolTip::add(_combos[i], PIN_DATA[i].comment);
    grid->addWidget(_combos[i], i, 1);
    _invcbs[i] = new QCheckBox(i18n("Inverted"), w);
    _invcbs[i]->setEnabled(edit);
    QToolTip::add(_invcbs[i], i18n(INV_PIN_LABEL));
    grid->addWidget(_invcbs[i], i, 2);
    if (edit) {
      _testcbs[i] = new QCheckBox(i18n("on"), w);
      QToolTip::add(_testcbs[i], PIN_DATA[i].testComment);
      connect(_testcbs[i], SIGNAL(clicked()), SLOT(slotTestPin()));
      grid->addWidget(_testcbs[i], i, 3);
      _testLabels[i] = new QLabel(w);
      QToolTip::add(_testcbs[i], PIN_DATA[i].testComment);
      grid->addWidget(_testLabels[i], i, 4);
      updateTestStatus(PinType(i), false);
    } else {
      _testcbs[i] = 0;
      _testLabels[i] = 0;
    }
  }

  QHBoxLayout *hbox = new QHBoxLayout(_mainVBox);
  QLabel *label = new QLabel(i18n("Clock delay"), this);
  QToolTip::add(label, i18n(DELAY_LABEL));
  hbox->addWidget(label);
  _delay = new KIntNumInput(0, Horizontal, this);
  _delay->setRange(0, 50, 5);
  _delay->setEnabled(edit);
  QToolTip::add(_delay, i18n(DELAY_LABEL));
  hbox->addWidget(_delay);

  if (edit) {
    _sendBitsButton = new QPushButton(i18n("Send 0xA55A"), this);
    _sendBitsButton->setToggleButton(true);
    connect(_sendBitsButton, SIGNAL(clicked()), SLOT(sendBits()));
    QToolTip::add(_sendBitsButton, i18n("Continuously send 0xA55A on \"Data out\" pin."));
    _editVBox->addWidget(_sendBitsButton);
    _editVBox->addStretch(1);
  } else _sendBitsButton = 0;

  // timer for sending bits
  _timerSendBits = new QTimer(this);
  connect(_timerSendBits, SIGNAL(timeout()), SLOT(slotSendBits()));

  // timer for automatically polling DataOut pin
  _timerPollDataOut = new QTimer(this);
  connect(_timerPollDataOut, SIGNAL(timeout()), SLOT(updateDataIn()));
}

void Direct::HConfigWidget::sendBits()
{
  if ( _timerSendBits->isActive() ) {
    _sendBitsButton->setText(i18n("Send 0xA55A"));
    _timerSendBits->stop();
    updateTestPin(DataOut);
  } else {
    _sendBitsButton->setText(i18n("Stop"));
    _timerSendBits->start(1);
  }
}

uint Direct::HConfigWidget::pin(PinType ptype) const
{
  return static_cast<const Hardware *>(_hardware)->pinForIndex(PIN_DATA[ptype].dir, _combos[ptype]->currentItem());
}

void Direct::HConfigWidget::slotPinChanged()
{
  for (uint i = 0; i<Nb_PinTypes; i++) {
    if ( sender()!=_combos[i] ) continue;
    updatePin(PinType(i));
  }
}

void Direct::HConfigWidget::updatePin(PinType ptype)
{
  bool ground = hardware()->isGroundPin(pin(ptype));
  _invcbs[ptype]->setEnabled(_edit);
  if (ground) _invcbs[ptype]->hide();
  else _invcbs[ptype]->show();
  if (_edit) {
    _testcbs[ptype]->setEnabled(PIN_DATA[ptype].dir==Port::Out && _connected);
    _testLabels[ptype]->setEnabled(_connected);
    if (ground) {
      _testcbs[ptype]->hide();
      _testLabels[ptype]->hide();
    } else {
      _testcbs[ptype]->show();
      _testLabels[ptype]->show();
    }
  }
}

void Direct::HConfigWidget::slotTestPin()
{
  for (uint i = 0; i<Nb_PinTypes; i++) {
    if ( sender()!=_testcbs[i] ) continue;
    updateTestPin(PinType(i));
    break;
  }
}

void Direct::HConfigWidget::updateTestPin(PinType ptype)
{
  Q_ASSERT( _connected && ptype!=DataIn );
  bool on = _testcbs[ptype]->isChecked();
  hardware()->setPin(ptype, on);
  updateTestStatus(ptype, on);
  if ( ptype==Vpp ) updateDataIn();
}

void Direct::HConfigWidget::updateTestStatus(PinType ptype, bool on)
{
  if (on) _testLabels[ptype]->setText(i18n(PIN_DATA[ptype].onLabel));
  else _testLabels[ptype]->setText(i18n(PIN_DATA[ptype].offLabel));
}

void Direct::HConfigWidget::updateDataIn()
{
  bool on = hardware()->readBit();
  updateTestStatus(DataIn, on);
  _testcbs[DataIn]->setChecked(on);
}

void Direct::HConfigWidget::sendBits(uint d, int nbb)
{
  Q_ASSERT(_connected);
  hardware()->setWrite();
  for (; nbb; --nbb) {
    hardware()->setPin(Clock, High);
    if ( d & 0x01 ) hardware()->setPin(DataOut, High);
    else hardware()->setPin(DataOut, Low);
    hardware()->setPin(Clock, Low);
    d >>= 1;       // Move the data over 1 bit
  }
  hardware()->setPin(DataOut, Low);
  hardware()->setRead();
}

void Direct::HConfigWidget::slotSendBits()
{
    sendBits(0xA55A, 16);
}

bool Direct::HConfigWidget::set(const Port::Description &pd, const ::Hardware::Data &data)
{
  // connect port
  _timerPollDataOut->stop();
  if ( _timerSendBits->isActive() ) sendBits(); // stop
  delete _hardware;
  const HardwareData &hdata = static_cast<const HardwareData &>(data);
  if ( pd.type==Port::SerialType ) _hardware = new SerialHardware(_base, pd.device, hdata);
  else _hardware = new ParallelHardware(_base, pd.device, hdata);
  bool ok = _hardware->connectHardware();
  if ( !_edit) _hardware->disconnectHardware();
  else _connected = ok;

  // update GUI
  if (_edit) {
    for (uint i=0; i<Nb_PinTypes; i++) {
      _testcbs[i]->setEnabled(_connected);
      updateTestStatus(PinType(i), false);
    }
    if ( _connected ) _timerPollDataOut->start(100);
    _sendBitsButton->setEnabled(_connected);
  }

  // update pins
  for (uint i=0; i<Nb_PinTypes; i++) {
    _combos[i]->clear();
    Port::IODir dir = PIN_DATA[i].dir;
    for (uint k=0; k<hardware()->nbPins(dir); k++)
      _combos[i]->insertItem(hardware()->pinLabelForIndex(dir, k));
    if (PIN_DATA[i].canBeGround) _combos[i]->insertItem("GND");
    _combos[i]->setCurrentItem(hardware()->indexForPin(dir, hdata.data.pins[i]));
    _invcbs[i]->setChecked(hdata.data.pins[i]<0);
    updatePin(PinType(i));
  }
  _delay->setValue(hdata.data.clockDelay);

  return ok;
}

Hardware::Data *Direct::HConfigWidget::data() const
{
  HardwareData *hdata = new HardwareData;
  hdata->portType = _hardware->portDescription().type;
  for (uint i=0; i<Nb_PinTypes; i++) {
    hdata->data.pins[i] = pin(PinType(i));
    if ( _invcbs[i]->isChecked() ) hdata->data.pins[i] = -hdata->data.pins[i];
  }
  hdata->data.clockDelay = _delay->value();
  return hdata;
}

//-----------------------------------------------------------------------------
Direct::ConfigWidget::ConfigWidget(const ::Programmer::Group &group, QWidget *parent)
  : ::Hardware::ConfigWidget(new ::Direct::PicBase(group, 0), new Config, parent, "direct_config_widget")
{}

Hardware::HConfigWidget *Direct::ConfigWidget::createHardwareConfigWidget(QWidget *parent, bool edit) const
{
  return new HConfigWidget(*_base, parent, edit);
}
