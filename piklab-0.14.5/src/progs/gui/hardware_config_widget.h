/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef HARDWARE_CONFIG_WIDGET
#define HARDWARE_CONFIG_WIDGET

#include <qlineedit.h>
#include <qcombobox.h>
#include <kdialogbase.h>

#include "progs/base/hardware_config.h"
#include "progs/gui/prog_config_widget.h"

namespace HProgrammer
{

//-----------------------------------------------------------------------------
class HardwareConfigWidget : public QFrame
{
Q_OBJECT
public:
  HardwareConfigWidget(::Programmer::Base &base, QWidget *parent, bool edit);
  virtual ~HardwareConfigWidget();
  virtual bool set(const Port::Description &pd, const HardwareData &data) = 0;
  virtual HardwareData *data() const = 0;
  Port::Description portDescription() const { return _hardware->portDescription(); }

protected:
  ::Programmer::Hardware *_hardware;
  QVBoxLayout *_mainVBox, *_editVBox;
  bool _edit, _connected;
  ::Programmer::Base &_base;
};

//-----------------------------------------------------------------------------
class ConfigWidget;

class HardwareEditDialog : public KDialogBase
{
Q_OBJECT
public:
  HardwareEditDialog(ConfigWidget *parent, const QString &name, const Port::Description &pd, HardwareData *data);
  QString savedName() const { return _savedName; }

private slots:
  virtual void slotOk();
  virtual void slotCancel();

private:
  ConfigWidget         *_cwidget;
  QString               _savedName;
  HardwareData         *_oldData;
  HardwareConfigWidget *_hc;
  QLineEdit            *_name;
};

//-----------------------------------------------------------------------------
class ConfigWidget : public ::Programmer::ConfigWidget
{
Q_OBJECT
public:
  ConfigWidget(::Programmer::Base *base, Config *config, QWidget *parent, const char *name);
  virtual ~ConfigWidget() { delete _base; }
  virtual void saveConfig();
  virtual bool setPort(const Port::Description &pd);
  virtual HardwareConfigWidget *createHardwareConfigWidget(QWidget *parent, bool edit) const = 0;

private slots:
  void editClicked();
  void deleteClicked();
  void configChanged(int i);

protected:
  ::Programmer::Base *_base;

private:
  Config        *_config;
  QStringList    _names;
  HardwareConfigWidget *_hc;
  QPushButton    *_deleteButton;
  QComboBox      *_configCombo;
  QLabel         *_comment;
  QHBoxLayout    *_hbox;

  void updateList(Port::Type type);
  bool set(const Port::Description &pd, uint i);

  friend class HardwareEditDialog;
};

} // namespace

#endif
