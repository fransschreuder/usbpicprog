/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "hardware_config_widget.h"

#include <qlabel.h>
#include <kpushbutton.h>

#include "progs/base/prog_config.h"
#include "devices/base/device_group.h"
#include "common/gui/misc_gui.h"

//-----------------------------------------------------------------------------
Hardware::HConfigWidget::HConfigWidget(::Programmer::Base &base, QWidget *parent, bool edit)
  : QFrame(parent, "hardware_config_widget"), _edit(edit), _connected(false), _base(base)
{
  _hardware = 0;

  QHBoxLayout *top = new QHBoxLayout(this, 0, 10);
  _mainVBox = new QVBoxLayout(top);

  if (edit) {
    _editVBox = new QVBoxLayout(top);
    top->setStretchFactor(_editVBox, 1);
  } else _editVBox = 0;
}

//-----------------------------------------------------------------------------
Hardware::EditDialog::EditDialog(ConfigWidget *cwidget, const QString &name, const Port::Description &pd, Data *data)
  : KDialogBase(Plain, i18n("Edit and test hardware"), Ok|Cancel, Cancel, cwidget, "hardware_edit_dialog", true, true),
    _cwidget(cwidget), _savedName(name), _oldData(data)
{
  setButtonOK(i18n("Save"));
  setButtonCancel(i18n("Close"));

  QGridLayout *grid = new QGridLayout(plainPage(), 1, 1, 0, 10);
  grid->setColStretch(2, 1);

  QLabel *label = new QLabel(i18n("Hardware name:"), plainPage());
  grid->addWidget(label, 0, 0);
  _name = new QLineEdit(name, plainPage());
  grid->addWidget(_name, 0, 1);

  label = new QLabel(i18n("%1 at %2:").arg(i18n(Port::DATA[pd.type].label)).arg(pd.device), plainPage());
  grid->addWidget(label, 1, 0);
  label = new QLabel(plainPage());
  grid->addWidget(label, 1, 1);

  _hc = cwidget->createHardwareConfigWidget(plainPage(), true);
  grid->addMultiCellWidget(_hc, 2,2, 0,2);

  grid->setRowStretch(3, 1);

  bool ok = _hc->set(pd, *data);
  label->setText(ok ? i18n("Connected") : i18n("Not Connected"));
}

void Hardware::EditDialog::slotOk()
{
  if ( _name->text().isEmpty() ) {
    MessageBox::sorry(i18n("Could not save configuration: hardware name is empty."), Log::Show);
    return;
  }
  if ( _cwidget->_config->isStandardHardware(_name->text()) ) {
    MessageBox::sorry(i18n("The hardware name is already used for a standard hardware."), Log::Show);
    return;
  }
  QStringList names = _cwidget->_config->hardwareNames(Port::Nb_Types); // all hardwares
  if ( names.contains(_name->text()) ) {
    if ( !MessageBox::askContinue(i18n("Do you want to overwrite this custom hardware \"%1\"?").arg(_name->text()),
                                      KStdGuiItem::save()) ) return;
  }
  delete _oldData;
  _oldData = _hc->data();
  _cwidget->_config->writeCustomHardware(_name->text(), *_oldData);
  _savedName = _name->text();
  KDialogBase::accept();
}

void Hardware::EditDialog::slotCancel()
{
  Data *data = _hc->data();
  bool equal = _oldData->isEqual(*data);
  delete data;
  if ( !equal ) {
    if ( !MessageBox::askContinue(i18n("Closing will discard changes you have made. Close anyway?"),
                                      KStdGuiItem::close()) ) return;
    KDialogBase::reject();
  }
  KDialogBase::accept();
}

//-----------------------------------------------------------------------------
Hardware::ConfigWidget::ConfigWidget(::Programmer::Base *base, Config *config,
                                        QWidget *parent, const char *name)
  : ::Programmer::ConfigWidget(base->group(), parent, name), _base(base), _config(config), _hc(0)
{
// programmer combo
  uint row = _topLayout->numRows();
  _configCombo = new QComboBox(this);
  connect(_configCombo, SIGNAL(activated(int)), SLOT(configChanged(int)));
  _topLayout->addWidget(_configCombo, row, 0);
  _comment = new QLabel(this);
  _topLayout->addWidget(_comment, row, 1);
  row++;

// hardware config
  QHBoxLayout *hbox = new QHBoxLayout(10);
  _hbox = new QHBoxLayout(10);
  hbox->addLayout(_hbox);
  _topLayout->addMultiCellLayout(hbox, row,row, 0,1);

// buttons
  QVBoxLayout *vbox  = new QVBoxLayout(hbox);
  QPushButton *button = new KPushButton(i18n("Edit/Test..."), this);
  connect(button, SIGNAL(clicked()), SLOT(editClicked()));
  vbox->addWidget(button);
  _deleteButton = new KPushButton(i18n("Delete"), this);
  connect(_deleteButton, SIGNAL(clicked()), SLOT(deleteClicked()));
  vbox->addWidget(_deleteButton);
  vbox->addStretch(1);
}

void Hardware::ConfigWidget::saveConfig()
{
  ::Programmer::ConfigWidget::saveConfig();
  _config->writeCurrentHardware(_hc->portDescription().type, _names[_configCombo->currentItem()]);
}

void Hardware::ConfigWidget::configChanged(int i)
{
  set(_hc->portDescription(), i);
}

bool Hardware::ConfigWidget::set(const Port::Description &pd, uint i)
{
  Data *hd = _config->hardwareData(_names[i]);
  if ( _hc==0 ) {
    _hc = createHardwareConfigWidget(this, false);
    _hc->show();
    _hbox->addWidget(_hc);
  }
  bool ok = _hc->set(pd, *hd);
  delete hd;
  _comment->setText(_config->comment(_names[i]));
  _deleteButton->setEnabled(!_config->isStandardHardware(_names[i]));
  return ok;
}

bool Hardware::ConfigWidget::setPort(const ::Programmer::HardwareDescription &hd)
{
  updateList(hd.port.type);
  int i = _names.findIndex(_config->currentHardware(hd.port.type));
  if ( i!=-1 ) _configCombo->setCurrentItem(i);
  return set(hd.port, _configCombo->currentItem());
}

void Hardware::ConfigWidget::updateList(Port::Type type)
{
  _configCombo->clear();
  _names = _config->hardwareNames(type);
  for (uint i=0; i<_names.count(); i++) {
    bool standard = _config->isStandardHardware(_names[i]);
    QString s = (standard ? _config->label(_names[i]) : i18n("%1 <custom>").arg(_names[i]));
    _configCombo->insertItem(s);
  }
}

void Hardware::ConfigWidget::editClicked()
{
  QString name = _names[_configCombo->currentItem()];
  if ( _config->isStandardHardware(name) ) name = QString::null;
  Port::Description pd = _hc->portDescription();
  EditDialog *hcd = new EditDialog(this, name, pd, _hc->data());
  hcd->exec();
  updateList(pd.type);
  if ( !hcd->savedName().isEmpty() ) _configCombo->setCurrentItem(_names.findIndex(hcd->savedName()));
  configChanged(_configCombo->currentItem());
}

void Hardware::ConfigWidget::deleteClicked()
{
  QString name = _names[_configCombo->currentItem()];
  if ( !MessageBox::askContinue(i18n("Do you want to delete custom hardware \"%1\"?").arg(name),
                                KStdGuiItem::del()) ) return;
  _config->deleteCustomHardware(name);
  updateList(_hc->portDescription().type);
  configChanged(_configCombo->currentItem());
}
