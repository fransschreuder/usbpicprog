/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit2_group_ui.h"

#include <kfiledialog.h>

#include "common/global/purl.h"
#include "progs/gui/prog_config_widget.h"
#include "progs/base/prog_group.h"
#include "progs/pickit2/base/pickit2_prog.h"
#include "progs/pickit2/base/pickit2.h"

//----------------------------------------------------------------------------
Pickit::AdvancedDialog::AdvancedDialog(Base &base, QWidget *parent, const char *name)
  : ::Programmer::PicAdvancedDialog(base, parent, name)
{}

void Pickit::AdvancedDialog::regenerateCalibration()
{
  if ( !base().deviceHasOsccalRegeneration() ) {
    MessageBox::sorry(i18n("Osccal regeneration not available for the selected device."), Log::Show);
    return;
  }
  KFileDialog dialog(":open_autohex", PURL::filter(PURL::Hex), this, "autohex_dialog", true);
  dialog.setOperationMode(KFileDialog::Opening);
  dialog.setCaption(i18n("Open Calibration Firmware"));
  dialog.setMode(KFile::File);
  //dialog.ops->clearHistory();
  dialog.setSelection("autocal.hex");
  dialog.exec();
  PURL::Url url(dialog.selectedURL());
  if ( url.isEmpty() ) return;
  base().regenerateOsccal(url.filepath(false));
}

//----------------------------------------------------------------------------
::Programmer::ConfigWidget *Pickit2::GroupUI::createConfigWidget(QWidget *parent) const
{
  return new ::Programmer::ConfigWidget(static_cast<const ::Programmer::Group &>(group()), parent, "pickit2_config_widget");
}

::Programmer::AdvancedDialog *Pickit2::GroupUI::createAdvancedDialog(::Programmer::Base &base, QWidget *parent) const
{
  return new Pickit::AdvancedDialog(static_cast<Base &>(base), parent, "pickit2_advanced_dialog");
}
