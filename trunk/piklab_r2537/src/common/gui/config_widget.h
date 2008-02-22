/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2004 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CONFIG_WIDGET_H
#define CONFIG_WIDGET_H

#include <qframe.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qcheckbox.h>

#include "common/global/global.h"
#include "common/common/key_enum.h"

//-----------------------------------------------------------------------------
class ConfigWidget : public QFrame
{
Q_OBJECT
public:
  ConfigWidget(QWidget *parent = 0, const char *name = 0) : QFrame(parent, name) {}
  virtual QString title() const { return QString::null; }
  virtual QString header() const { return QString::null; }
  virtual QPixmap pixmap() const { return QPixmap(); }

public slots:
  virtual void loadConfig() = 0;
  virtual void saveConfig() = 0;
};

//-----------------------------------------------------------------------------
class GridConfigWidget : public ConfigWidget
{
Q_OBJECT
public:
  GridConfigWidget(QWidget *parent = 0, const char *name = 0);
  QGridLayout *topLayout() { return _topLayout; }

protected:
  QGridLayout *_topLayout;
};

//-----------------------------------------------------------------------------
class TabConfigWidget : public ConfigWidget
{
Q_OBJECT
public:
  TabConfigWidget(QWidget *parent = 0, const char *name = 0);

protected:
  QTabWidget *_tabWidget;
};

//-----------------------------------------------------------------------------
template <typename Type>
class BaseConfigWidget
{
public:
  BaseConfigWidget(GridConfigWidget *parent) {
    _widgets.resize(Type::Nb_Types);
    for(Type type; type<Type::Nb_Types; ++type)
      _widgets[type.type()] = createWidget(type, parent->topLayout(), parent);
  }
  void loadConfig() {
    for(Type type; type<Type::Nb_Types; ++type) load(type, _widgets[type.type()]);
  }
  void saveConfig() {
    for(Type type; type<Type::Nb_Types; ++type) save(type, _widgets[type.type()]);
  }
  void setDefault() {
    for(Type type; type<Type::Nb_Types; ++type) setDefault(type, _widgets[type.type()]);
  }

private:
  QValueVector<QWidget *> _widgets;

  static QWidget *createWidget(Type type, QGridLayout *grid, QWidget *parent) {
    QWidget *w = 0;
    int row = grid->numRows();
    switch (type.data().defValue.type()) {
      case QVariant::Bool:
        w = new QCheckBox(i18n(type.label()), parent);
        grid->addMultiCellWidget(w, row,row, 0,1);
        break;
      default: Q_ASSERT(false); break;
    }
    return w;
  }
  void load(Type type, QWidget *widget) const {
    switch (type.data().defValue.type()) {
      case QVariant::Bool:
        static_cast<QCheckBox *>(widget)->setChecked(readConfigEntry(type).toBool());
        break;
      default: Q_ASSERT(false); break;
    }
  }
  void save(Type type, QWidget *widget) {
    switch (type.data().defValue.type()) {
      case QVariant::Bool:
        writeConfigEntry(type, QVariant(static_cast<QCheckBox *>(widget)->isChecked(), 0));
        break;
      default: Q_ASSERT(false); break;
    }
  }
  void setDefault(Type type, QWidget *widget) const {
    switch (type.data().defValue.type()) {
      case QVariant::Bool:
        static_cast<QCheckBox *>(widget)->setChecked(type.data().defValue.toBool());
        break;
      default: Q_ASSERT(false); break;
    }
  }

};

//-----------------------------------------------------------------------------
#define BEGIN_DECLARE_CONFIG_WIDGET(ConfigType, ConfigWidgetType) \
class ConfigWidgetType : public GridConfigWidget, public BaseConfigWidget<ConfigType> \
{ \
public: \
  ConfigWidgetType() : BaseConfigWidget<ConfigType>(this) {} \
  virtual void loadConfig() { BaseConfigWidget<ConfigType>::loadConfig(); } \
  virtual void saveConfig() { BaseConfigWidget<ConfigType>::saveConfig(); }

#define END_DECLARE_CONFIG_WIDGET \
};

#endif
