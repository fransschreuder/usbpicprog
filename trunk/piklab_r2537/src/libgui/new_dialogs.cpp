/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "new_dialogs.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kpushbutton.h>

#include "common/gui/purl_gui.h"
#include "project.h"

//----------------------------------------------------------------------------
NewDialog::NewDialog(const QString &caption, QWidget *parent)
  : Dialog(parent, "new_dialog", true, caption, Ok|Cancel, Ok, false)
{
  _top = new QGridLayout(mainWidget(), 0, 0, 10, 10);
  _top->setColStretch(2, 1);

  _fLabel = new QLabel(mainWidget());
  _top->addWidget(_fLabel, 0, 0);
  _filename = new QLineEdit(mainWidget());
  connect(_filename, SIGNAL(textChanged(const QString &)), SLOT(changed()));
  _top->addMultiCellWidget(_filename, 0,0, 1,3);

  QLabel *label= new QLabel(i18n("Location:"), mainWidget());
  _top->addWidget(label, 1, 0);
  _dir = new QLineEdit(mainWidget());
  connect(_dir, SIGNAL(textChanged(const QString &)), SLOT(changed()));
  _top->addMultiCellWidget(_dir, 1,1, 1,2);
  KIconLoader loader;
  QIconSet iconset = loader.loadIcon("fileopen", KIcon::Toolbar);
  KPushButton *button = new KPushButton(iconset, QString::null, mainWidget());
  connect(button, SIGNAL(clicked()), SLOT(browse()));
  _top->addWidget(button, 1, 3);

  _filename->setFocus();
  enableButtonOK(false);
}

void NewDialog::changed()
{
  enableButtonOK(!_filename->text().isEmpty() && !_dir->text().isEmpty());
}

void NewDialog::browse()
{
  PURL::Directory dir = PURL::getExistingDirectory(startDir(), this, QString::null);
  if ( dir.isEmpty() ) return;
  _dir->setText(dir.path());
}

//----------------------------------------------------------------------------
NewFileDialog::NewFileDialog(Project *project, QWidget *parent)
  : NewDialog(i18n("Create New File"), parent), _project(project)
{
  _fLabel->setText(i18n("File Name:"));

  if (project) {
    _add = new QCheckBox(i18n("Add to project"), mainWidget());
    _add->setChecked(project);
    _top->addMultiCellWidget(_add, 2,2, 1,2);
    _top->setRowStretch(3, 1);
    _dir->setText(project->directory().path());
  }
}

QString NewFileDialog::startDir() const
{
  if (_project) return _project->directory().path();
  return ":new_file";
}

PURL::Url NewFileDialog::url() const
{
  return PURL::Url(PURL::Directory(_dir->text()), _filename->text());
}
