/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "editor.h"

#include <klocale.h>

#include "common/gui/purl_gui.h"
#include "common/gui/misc_gui.h"

Editor::Editor(const QString &title, const QString &tag, QWidget *parent, const char *name)
  : QWidget(parent, name), _title(title), _tag(tag)
{}

Editor::Editor(QWidget *parent, const char *name)
  : QWidget(parent, name)
{}

QSizePolicy Editor::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void Editor::setModified(bool m)
{
  setModifiedInternal(m);
  if (m) emit modified();
}

void Editor::setReadOnly(bool ro)
{
  setReadOnlyInternal(ro);
  emit guiChanged();
}

bool Editor::save()
{
  if ( url().isEmpty() ) return saveAs();
  if ( !save(url()) ) return false;
  setModified(false);
  emit guiChanged();
  return true;
}

bool Editor::saveAs()
{
  QString filter = PURL::filter(fileType());
  PURL::Url purl = PURL::getSaveUrl(":save_file_as", filter, this, i18n("Save File"), PURL::AskOverwrite);
  if ( purl.isEmpty() ) return false;
  if ( !save(purl) ) return false;
  setModified(false);
  emit guiChanged();
  return true;
}

bool Editor::slotLoad()
{
  bool readOnly = isReadOnly();
  if ( !open(url()) ) return false;
  setModified(false);
  setReadOnly(readOnly);
  statusChanged();
  emit guiChanged();
  return true;
}

QString Editor::filename() const
{
  return (url().isEmpty() ? "<" + _title + ">" : "\"" + url().filepath() + "\"");
}

bool Editor::checkSaved()
{
  if ( !isModified() ) return true;
  MessageBox::Result res = MessageBox::questionYesNoCancel(i18n("File %1 not saved.").arg(filename()),
                                                           KStdGuiItem::save(), KStdGuiItem::discard());
  if ( res==MessageBox::Cancel ) return false;
  if ( res==MessageBox::Yes ) save();
  return true;
}

bool Editor::reload()
{
  if ( !checkSaved() ) return false;
  return slotLoad();
}
