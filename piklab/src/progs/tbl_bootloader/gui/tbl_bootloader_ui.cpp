/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "tbl_bootloader_ui.h"

#include "progs/tbl_bootloader/base/tbl_bootloader.h"
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
