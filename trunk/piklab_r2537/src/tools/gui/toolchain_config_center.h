/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2004 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TOOLCHAIN_CONFIG_CENTER_H
#define TOOLCHAIN_CONFIG_CENTER_H

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qwidgetstack.h>

#include "tools/gui/tool_config_widget.h"
#include "common/gui/dialog.h"
class ToolchainConfigWidget;

class ToolchainsConfigCenter : public TreeListDialog
{
Q_OBJECT
public:
  ToolchainsConfigCenter(const Tool::Group &group, QWidget *parent);

public slots:
  virtual void slotOk();
  virtual void slotApply();
  virtual void slotUser1();
  virtual void slotUser2();

private slots:
  void aboutToShowPageSlot(QWidget *page);
  void showInformationDialog();

private:
  KPushButton *_infoButton;
  QMap<QWidget *, ToolchainConfigWidget *> _pages;

  ToolchainConfigWidget *current() const;
};

#endif
