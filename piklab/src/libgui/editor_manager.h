/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef EDITOR_MANAGER_H
#define EDITOR_MANAGER_H

class QEvent;
#include <ktabbar.h>
#include <ktabwidget.h>
#include <klineedit.h>

#include "text_editor.h"
#include "common/gui/misc_gui.h"
#include "common/gui/dialog.h"

//-----------------------------------------------------------------------------
class SwitchToDialog : public Dialog
{
Q_OBJECT
public:
  SwitchToDialog(const QStringList &names, QWidget *parent);
  QString name() const { return _edit->text(); }

private:
  KLineEdit *_edit;
};

//-----------------------------------------------------------------------------
class EditorTabBar : public TabBar
{
Q_OBJECT
public:
  EditorTabBar(QWidget *parent) : TabBar(parent, "editor_tab_bar") {}
  void setReadOnly(uint index, bool readOnly) { _readOnly[tabAt(index)] = readOnly; }

private:
  QMap<QTab *, bool> _readOnly;
  virtual void paintLabel(QPainter *p, const QRect &br, QTab *t, bool has_focus) const;
};

//-----------------------------------------------------------------------------
class EditorHistory
{
public:
  EditorHistory() : _current(0) {}
  bool hasBack() const { return _current!=0; }
  bool hasForward() const { return (_current+1)<_names.count(); }
  void add(const QString &name);
  void closedLast();
  QString goBack();
  QString goForward();

private:
  uint _current;
  QValueVector<QString> _names;
};

//-----------------------------------------------------------------------------
class EditorManager : public TabWidget
{
Q_OBJECT
public:
  EditorManager(QWidget *parent);

  PURL::UrlList files() const;
  QValueList<Editor *> &editors() { return _editors; }
  uint nbEditors() const { return _editors.count(); }
  Editor *createEditor(PURL::FileType type, const PURL::Url &url);
  void addEditor(Editor *e);
  Editor *currentEditor() const { return _current; }
  Editor *findEditor(const PURL::Url &file);
  Editor *findEditor(const QString &tag);
  void showEditor(Editor *e);
  bool closeEditor(const PURL::Url &url);
  bool closeEditor(Editor *e, bool ask);
  bool openFile(const PURL::Url &url);
  Editor *openEditor(const PURL::Url &url);
  void connectEditor(Editor *editor);
  void disconnectEditor(Editor *editor);
  const EditorHistory &history() const { return _history; }
  enum EditorType { DeviceEditor = 0, RegisterEditor, Nb_EditorTypes };
  Editor *openEditor(EditorType type);

public slots:
  void updateTitles();
  void slotDropEvent(QDropEvent *e);
  void saveAllFiles();
  bool closeCurrentEditor();
  bool closeAllEditors();
  bool closeAllOtherEditors();
  void switchHeaderImplementation();
  void switchToEditor();
  void goBack();
  void goForward();

signals:
  void modified(const PURL::Url &url);
  void guiChanged();
  void statusChanged(const QString &);

private:
  void changeToEditor(Editor *e);
  void enableActions(bool enable);
  QString title(const Editor &e) const;
  QString name(const Editor &e) const;
  virtual void contextMenu(int i, const QPoint &p);

private slots:
  void showEditor(QWidget *w) { showEditor(static_cast<Editor *>(w)); }
  void closeRequest(int i);
  void modifiedSlot();

private:
  Editor *_current;
  QValueList<Editor *> _editors;
  EditorHistory _history;
  static const char * const EDITOR_TAGS[Nb_EditorTypes];
};

#endif
