/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DIALOG_H
#define DIALOG_H

#include <kdialogbase.h>
#include <klistview.h>

//-----------------------------------------------------------------------------
class Dialog : public KDialogBase
{
Q_OBJECT
public:
  Dialog(QWidget *parent, const char *name, bool modal,
         const QString &caption, int buttonMask, ButtonCode defaultButton, bool separator,
         const QSize &defaultSize = QSize());
  Dialog(DialogType type, const QString &caption,
         int buttonMask, ButtonCode defaultButton, QWidget *parent, const char *name,
         bool modal, bool separator, const QSize &defaultSize = QSize());
  virtual ~Dialog();

private slots:
  void updateSize();

private:
  QSize _defaultSize;
};

//-----------------------------------------------------------------------------
class TreeListDialog : public Dialog
{
Q_OBJECT
public:
  TreeListDialog(QWidget *parent, const char *name, bool modal,
                 const QString &caption, int buttonMask, ButtonCode defaultButton, bool separator);
  QWidget *addPage(const QStringList &labels, const QString &title);
  void showPage(QWidget *page);
  int activePageIndex() const;
  int pageIndex(QWidget *page) const;

private slots:
  void currentChanged(QListViewItem *item);
  void pageDestroyed(QObject *page);

private:
  KListView    *_listView;
  QLabel       *_label;
  QWidgetStack *_stack;

 class Item : public KListViewItem {
  public:
    Item(const QString &label, QWidget *page, const QString &title, QListView *listview);
    Item(const QString &label, QWidget *page, const QString &title, QListViewItem *item);
    QWidget *_page;
    QString  _title;
  };
};

//-----------------------------------------------------------------------------
class TextEditorDialog : public Dialog
{
Q_OBJECT
public:
  TextEditorDialog(const QString &text, const QString &caption, QWidget *parent);
};

#endif
