/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "bootloader_config_widget.h"

#include "progs/bootloader/base/bootloader_prog.h"
#include "progs/bootloader/base/bootloader_tbl.h"
#include "progs/bootloader/base/bootloader_picdem.h"
#include "common/port/serial.h"

//-----------------------------------------------------------------------------
TinyBootloader::ConfigWidget::ConfigWidget(QWidget *parent)
  : GridConfigWidget(parent, "tiny_bootloader_config_widget")
{
  uint row = _topLayout->numRows();

  QLabel *label = new QLabel(i18n("Port Speed:"), this);
  _topLayout->addWidget(label, row, 0);
  _speed = new KComboBox(this);
  for (uint i=0; i<Port::Serial::Nb_Speeds; i++) {
    if ( Port::Serial::SPEED_VALUES[i]==0 || !Port::Serial::SPEED_DATA[i].supported ) continue;
    _speed->insertItem(QString::number(Port::Serial::SPEED_VALUES[i]));
  }
  _topLayout->addWidget(_speed, row, 1);
  row++;

  label = new QLabel(i18n("Timeout (ms):"), this);
  _topLayout->addWidget(label, row, 0);
  _timeout = new KIntNumInput(this);
  _timeout->setMinValue(0);
  _topLayout->addWidget(_timeout, row, 1);
  row++;

  label = new QLabel(i18n("No of Retries:"), this);
  _topLayout->addWidget(label, row, 0);
  _retries = new KIntNumInput(this);
  _retries->setMinValue(0);
  _topLayout->addWidget(_retries, row, 1);
  row++;

  _topLayout->setColStretch(2, 1);
}

void TinyBootloader::ConfigWidget::saveConfig()
{
  Config config;
  uint k = 0;
  for (uint i=0; i<Port::Serial::Nb_Speeds; i++) {
    if ( Port::Serial::SPEED_VALUES[i]==0 || !Port::Serial::SPEED_DATA[i].supported ) continue;
    if ( uint(_speed->currentItem())==k ) {
      config.writeSpeed(Port::Serial::Speed(i));
      break;
    }
    k++;
  }
  config.writeTimeout(_timeout->value());
  config.writeRetries(_retries->value());
}

void TinyBootloader::ConfigWidget::loadConfig()
{
  Config config;
  Port::Serial::Speed speed = config.readSpeed();
  uint k = 0;
  for (uint i=0; i<Port::Serial::Nb_Speeds; i++) {
    if ( Port::Serial::SPEED_VALUES[i]==0 || !Port::Serial::SPEED_DATA[i].supported ) continue;
    if ( i==uint(speed) ) break;
    k++;
  }
  _speed->setCurrentItem(k);
  _timeout->setValue(config.readTimeout());
  _retries->setValue(config.readRetries());
}

//-----------------------------------------------------------------------------
PicdemBootloader::ConfigWidget::ConfigWidget(QWidget *parent)
  : GridConfigWidget(parent, "picdem_bootloader_config_widget")
{
  uint row = _topLayout->numRows();

  QLabel *label = new QLabel(i18n("USB Vendor Id:"), this);
  _topLayout->addWidget(label, row, 0);
  _vendorId = new HexWordEditor(4, this);
  _topLayout->addWidget(_vendorId, row, 1);
  row++;

  label = new QLabel(i18n("USB Product Id:"), this);
  _topLayout->addWidget(label, row, 0);
  _productId = new HexWordEditor(4, this);
  _topLayout->addWidget(_productId, row, 1);
  row++;

  _topLayout->setColStretch(2, 1);
}

void PicdemBootloader::ConfigWidget::saveConfig()
{
  Config config;
  config.writeVendorId(_vendorId->value());
  config.writeProductId(_productId->value());
}

void PicdemBootloader::ConfigWidget::loadConfig()
{
  Config config;
  _vendorId->setValue(config.readVendorId());
  _productId->setValue(config.readProductId());
}

//-----------------------------------------------------------------------------
::Programmer::ConfigWidget *Bootloader::GroupUI::createConfigWidget(QWidget *parent) const
{
  return new ConfigWidget(static_cast<const Group &>(group()), parent);
}

//-----------------------------------------------------------------------------
Bootloader::ConfigWidget::ConfigWidget(const Group &group, QWidget *parent)
  : ::Programmer::ConfigWidget(group, parent, "bootloader_config_widget")
{
  uint row = _topLayout->numRows();

  _eeprom = new QCheckBox(i18n("Program EEPROM (if supported)"), this);
  _topLayout->addMultiCellWidget(_eeprom, row,row, 0,1);
  row++;

  _cwidget = createConfigWidget(group.type());
  _cwidget->loadConfig();
  _topLayout->addMultiCellWidget(_cwidget, row,row, 0, 1);
  row++;

  _topLayout->setColStretch(2, 1);
}

ConfigWidget *Bootloader::ConfigWidget::createConfigWidget(Type type)
{
  switch (type) {
    case TinyBootloader: return new ::TinyBootloader::ConfigWidget(this);
    case Pickit2Bootloader: return new ::Pickit2Bootloader::ConfigWidget(this);
    case PicdemBootloader: return new ::PicdemBootloader::ConfigWidget(this);
    case Nb_Types: break;
  }
  Q_ASSERT(false);
  return 0;
}

void Bootloader::ConfigWidget::saveConfig()
{
  ::Programmer::ConfigWidget::saveConfig();
  Config config(static_cast<const Group &>(_group));
  config.writeProgramEeprom(_eeprom->isChecked());
  _cwidget->saveConfig();
}

void Bootloader::ConfigWidget::loadConfig()
{
  ::Programmer::ConfigWidget::loadConfig();
  Config config(static_cast<const Group &>(_group));
  _eeprom->setChecked(config.readProgramEeprom());
  _cwidget->loadConfig();
}
