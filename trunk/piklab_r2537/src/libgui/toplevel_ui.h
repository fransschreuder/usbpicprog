/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TOPLEVEL_UI_H
#define TOPLEVEL_UI_H

#include <qtoolbutton.h>
#include <kdockwidget.h>
#include <kaction.h>

#include "common/gui/key_gui.h"
namespace Programmer { class Group; }
namespace Tool { class Group; }

//----------------------------------------------------------------------------
class ProgrammerStatusWidget : public KeyPopupButton
{
Q_OBJECT
public:
  ProgrammerStatusWidget(QWidget *parent);

signals:
  void configure();
  void selected(const Programmer::Group &group);

private slots:
  void activatedSlot(const QString &name);
};

//----------------------------------------------------------------------------
class ToolStatusWidget : public KeyPopupButton
{
Q_OBJECT
public:
  ToolStatusWidget(QWidget *parent);

signals:
  void configureToolchain();
  void configure();
  void selected(const Tool::Group &group);

private slots:
  void activatedSlot(const QString &name);
};

//----------------------------------------------------------------------------
class ViewMenuAction : public KToggleAction
{
Q_OBJECT
public:
  ViewMenuAction(KDockWidget *widget);

signals:
  void activated(QWidget *);

private slots:
  virtual void slotActivated();

private:
  KDockWidget *_widget;
};

//----------------------------------------------------------------------------
class MenuBarButton : public QToolButton
{
Q_OBJECT
public:
  MenuBarButton(const QString &icon, QWidget *parent);
  virtual QSize sizeHint() const;
};

#endif
