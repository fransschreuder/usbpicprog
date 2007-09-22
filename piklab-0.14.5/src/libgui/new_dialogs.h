/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef NEW_DIALOGS_H
#define NEW_DIALOGS_H

#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <kcombobox.h>

#include "common/global/purl.h"
#include "common/gui/dialog.h"
class Project;

//----------------------------------------------------------------------------
class NewDialog : public Dialog
{
Q_OBJECT
public:
  NewDialog(const QString &caption, QWidget *parent);

protected:
  QGridLayout *_top;
  QLabel      *_fLabel;
  QLineEdit   *_filename, *_dir;

  virtual QString startDir() const = 0;

private slots:
  void changed();
  void browse();
};

//----------------------------------------------------------------------------
class NewFileDialog : public NewDialog
{
Q_OBJECT
public:
  NewFileDialog(Project *project, QWidget *parent);
  PURL::Url url() const;
  bool addToProject() const { return (_project ? _add->isChecked() : false); }

private:
  Project   *_project;
  QCheckBox *_add;

  virtual QString startDir() const;
};

#endif
