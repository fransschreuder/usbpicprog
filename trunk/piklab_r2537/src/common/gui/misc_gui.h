/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MISC_GUI_H
#define MISC_GUI_H

#include <qlayout.h>
#include <qsplitter.h>
#include <qvaluevector.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qwidgetstack.h>

#include <klocale.h>
#include <kpushbutton.h>
#include <ktabwidget.h>
#include <ktabbar.h>
#include <kstdguiitem.h>
#include <klineedit.h>
class KAction;

#include "common/global/generic_config.h"
#include "common/global/log.h"
#include "common/common/number.h"

//-----------------------------------------------------------------------------
class BusyCursor
{
public:
  BusyCursor() { start(); }
  ~BusyCursor() { stop(); }
  static void start();
  static void stop();
  static void pause();
  static void restore();

private:
  static bool _overridePaused;
};

//-----------------------------------------------------------------------------
namespace MessageBox
{
extern void information(const QString &text, Log::ShowMode show, const QString &dontShowAgainName = QString::null);
extern void detailedSorry(const QString &text, const QString &details, Log::ShowMode show);
inline void sorry(const QString &text, Log::ShowMode show) { detailedSorry(text, QString::null, show); }
extern bool askContinue(const QString &text, const KGuiItem &continueButton = KStdGuiItem::cont(),
                        const QString &caption = i18n("Warning"));
extern bool questionYesNo(const QString &text, const KGuiItem &yesButton, const KGuiItem &noButton,
                          const QString &caption = i18n("Warning"));
enum Result { Yes, No, Cancel };
extern Result questionYesNoCancel(const QString &text, const KGuiItem &yesButton, const KGuiItem &noButton,
                                  const QString &caption = i18n("Warning"));
extern void text(const QString &text, Log::ShowMode show);
}

//----------------------------------------------------------------------------
class PopupButton : public KPushButton
{
Q_OBJECT
public:
  PopupButton(QWidget *parent = 0, const char *name = 0);
  PopupButton(const QString &text, QWidget *parent = 0, const char *name = 0);
  void appendAction(KAction *action);
  void appendAction(const QString &label, const QString &icon = QString::null,
                    QObject *receiver = 0, const char *slot = 0);
  int appendItem(const QString &label, uint id) { return appendItem(label, QPixmap(), id); }
  int appendItem(const QString &label, const QString &icon, int id = -1);
  int appendItem(const QString &label, const QPixmap &icon, int id = -1);
  void appendSeparator() { _separator = true; }

signals:
  void activated(int id);

private:
  bool _separator;

  void init();
};

//-----------------------------------------------------------------------------
class Splitter : public QSplitter
{
Q_OBJECT
public:
  Splitter(const QValueList<int> &defaultSizes, Orientation orientation,
           QWidget *parent, const char *name);
  virtual ~Splitter();

private slots:
  void updateSizes();

private:
  QValueList<int> _defaultSizes;
};

//-----------------------------------------------------------------------------
class GuiConfig : public GenericConfig
{
public:
  GuiConfig() : GenericConfig("gui") {}
};

//-----------------------------------------------------------------------------
class SeparatorWidget : public QFrame
{
Q_OBJECT
public:
  SeparatorWidget(QWidget *parent) : QFrame(parent, "separator") {
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setMargin(2);
    setFixedHeight(2*2);
  }
};

//-----------------------------------------------------------------------------
class TabBar : public KTabBar
{
Q_OBJECT
public:
  TabBar(QWidget *parent = 0, const char *name = 0);

protected:
  virtual void wheelEvent(QWheelEvent *e);

private:
  bool _ignoreWheelEvent;

  friend class TabWidget;
};

class TabWidget : public KTabWidget
{
Q_OBJECT
public:
  TabWidget(QWidget *parent = 0, const char *name = 0);
  void setIgnoreWheelEvent(bool ignore);

protected:
  virtual void wheelEvent(QWheelEvent *e);
  void setTabBar(TabBar *tabbar);
};

//-----------------------------------------------------------------------------
class ComboBox : public QComboBox
{
Q_OBJECT
public:
  ComboBox(QWidget *parent = 0, const char *name = 0);
  void setIgnoreWheelEvent(bool ignore) { _ignoreWheelEvent = ignore; }

protected:
  virtual void wheelEvent(QWheelEvent *e);

private:
  bool _ignoreWheelEvent;
};

#endif
