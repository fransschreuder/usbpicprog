/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include <qsplitter.h>
#include <qstringlist.h>
class QSplitter;

#include <kate/view.h>
#include <kate/document.h>

#include "common/gui/pfile_ext.h"
#include "editor.h"
#include "progs/manager/breakpoint.h"

//-----------------------------------------------------------------------------
class TextEditor : public Editor
{
Q_OBJECT
public:
  TextEditor(bool withDebugger, QWidget *parent, const char *name = 0);
  virtual PURL::FileType fileType() const { return url().fileType(); }
  virtual PURL::Url url() const { return _document->url(); }
  virtual bool isModified() const;
  virtual bool isReadOnly() const;
  virtual void addGui();
  virtual void removeGui();
  virtual void setFocus() { _view->setFocus(); }
  static QPixmap pixmap(Breakpoint::MarkType type);
  void setMark(uint line, Breakpoint::MarkType type);
  void clearMarks();
  void setCursor(uint line, uint column) { _view->setCursorPosition(line, column); }
  uint cursorLine() const;
  virtual bool open(const PURL::Url &url);
  virtual bool save(const PURL::Url &url) { return _document->saveAs(url.kurl()); }

public slots:
  void addView();
  void removeCurrentView();
  void gotFocus(Kate::View *);
  void highlightChanged();
  virtual void statusChanged();
  void selectAll() { _document->selectAll(); }
  void deselect() { _document->clearSelection(); }
  void copy() { _view->copy(); }

protected:
  enum EolType { Dos = 1, Unix = 0, Mac = 2 };
  Kate::Document *_document;
  Kate::View     *_view;

private slots:
  void addToDebugManager();

private:
  QSplitter *_split;
  bool       _oldModified, _oldReadOnly;
  struct MarkTypeData {
    int value;
    const char *pixmap;
  };
  static const MarkTypeData MARK_TYPE_DATA[Breakpoint::Nb_MarkTypes];

  virtual void setModifiedInternal(bool modified);
  virtual void setReadOnlyInternal(bool readOnly);
  uint highlightMode(const QString &name) const;
};

//-----------------------------------------------------------------------------
class SimpleTextEditor : public TextEditor
{
Q_OBJECT
public:
  SimpleTextEditor(bool withDebugger, PURL::FileType type, QWidget *parent, const char *name = 0);
  SimpleTextEditor(bool withDebugger, QWidget *parent, const char *name = 0);
  void setFileType(PURL::FileType type) { _type = type; }
  virtual PURL::FileType fileType() const { return _type; }
  bool setText(const QString &text);
  virtual bool open(const PURL::Url &url);

protected:
  virtual bool load() { return false; }

private:
  Log::StringView _sview;
  PURL::FileType  _type;
  PURL::TempFile  _file;
};

#endif
