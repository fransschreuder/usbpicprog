/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TOOLCHAIN_CONFIG_WIDGET_H
#define TOOLCHAIN_CONFIG_WIDGET_H

#include <qcombobox.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <klocale.h>
#include <kpushbutton.h>

#include "common/global/process.h"
#include "progs/gui/config_widget.h"
#include "tools/list/compile.h"
class Container;
namespace PURL { class DirectoryWidget; }
namespace Tool { class Group; }

//----------------------------------------------------------------------------
class ToolchainConfigWidget : public GridConfigWidget
{
  Q_OBJECT
public:
  ToolchainConfigWidget(const Tool::Group &group, QWidget *parent);
  virtual ~ToolchainConfigWidget();
  void init();

public slots:
  void detect();
  void forceDetect();
  virtual void loadConfig();
  virtual void saveConfig();

protected:
  bool withWine() const;
  virtual void addCustomExecutableOptions(Container *) {}
  virtual QString baseExecutable(Tool::Category category) const;
  virtual ::Process::LineOutput *checkExecutableProcess(Tool::Category category) const;
  virtual ::Process::LineOutput *checkDevicesProcess(uint i) const;

protected slots:
  void showDetails();
  void showDeviceDetails();
  void checkExecutableDone();
  void checkDevicesDone();

protected:
  const Tool::Group &_group;
  Compile::Config   *_config;
  bool               _dirty;
  QComboBox         *_execType;
  QLabel            *_devicesLabel;
  PURL::DirectoryWidget *_dirs[Compile::Nb_DirectoryTypes];
  class ExecData {
  public:
    ExecData() : label(0), button(0), process(0) {}
    ~ExecData() { delete process; }
    QLabel *label;
    QString command;
    QStringList checkLines;
    KPushButton *button;
    ::Process::LineOutput *process;
  };
  QValueVector<ExecData> _data;
  class DevicesData {
  public:
    DevicesData() : process(0) {}
    bool done;
    QString command;
    QStringList checkLines;
    ::Process::LineOutput *process;
  };
  QValueVector<DevicesData> _devicesData;
};

#endif
