/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "icd1_group_ui.h"

#include "progs/gui/prog_config_widget.h"
#include "progs/base/prog_group.h"
#include "progs/icd1/base/icd1_prog.h"

//----------------------------------------------------------------------------
Icd1::AdvancedDialog::AdvancedDialog(ProgrammerBase &base, QWidget *parent)
  : ::Programmer::PicAdvancedDialog(base, parent, "icd1_advanced_dialog")
{
  uint row = _selfTestContainer->numRows();
  QLabel *label = new QLabel(i18n("Result:"), _selfTestContainer);
  _selfTestContainer->addWidget(label, row, 0);
  _selfTestLabel = new QLabel(_selfTestContainer);
  _selfTestContainer->addWidget(_selfTestLabel, row, 1);
}

void Icd1::AdvancedDialog::updateDisplay()
{
  ::Programmer::PicAdvancedDialog::updateDisplay();
  _selfTestLabel->setText(i18n(::Programmer::RESULT_TYPE_LABELS[base().selfTestResult()]));
}

//----------------------------------------------------------------------------
::Programmer::ConfigWidget *Icd1::GroupUI::createConfigWidget(QWidget *parent) const
{
  return new ::Programmer::ConfigWidget(static_cast<const Group &>(group()), parent, "icd1_config_widget");
}

::Programmer::AdvancedDialog *Icd1::GroupUI::createAdvancedDialog(::Programmer::Base &base, QWidget *parent) const
{
  return new AdvancedDialog(static_cast<ProgrammerBase &>(base), parent);
}
