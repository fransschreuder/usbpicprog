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

#endif
