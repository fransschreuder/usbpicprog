/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "picdem_bootloader_ui.h"

#include "progs/picdem_bootloader/base/picdem_bootloader.h"

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
  config.writeVendorId(_vendorId->value().toUInt());
  config.writeProductId(_productId->value().toUInt());
}

void PicdemBootloader::ConfigWidget::loadConfig()
{
  Config config;
  _vendorId->setValue(config.readVendorId());
  _productId->setValue(config.readProductId());
}
