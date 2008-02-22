/*  Copyright (C) 2000 David Faure <faure@kde.org>, Alexander Neundorf <neundorf@kde.org>
    Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#ifndef EDITLISTBOX_H
#define EDITLISTBOX_H

#include <qlayout.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <klistview.h>

#include "common/common/qflags.h"

//----------------------------------------------------------------------------
// modified KEditListBox (beyond recognition)
// * support for duplicated items
// * use KStdGuiItem for buttons
// * support for New, Clear, Reset buttons
// * use KListView
class EditListBox : public QFrame
{
Q_OBJECT
   public:
      enum Mode { DuplicatesDisallowed, DuplicatesAllowed, DuplicatesCheckedAtEntering };
      enum Button { Add = 1, Remove = 2, UpDown = 4, RemoveAll = 8, Reset = 16 };
      Q_DECLARE_FLAGS(Buttons, Button)

      EditListBox(uint nbColumns, QWidget *parent = 0, const char *name = 0, Mode mode = DuplicatesDisallowed,
                  Buttons buttons = Buttons(Add|Remove|RemoveAll|UpDown) );
      EditListBox(uint nbColumns, QWidget *view, KLineEdit *lineEdit, QWidget *parent = 0, const char *name = 0,
                  Mode mode = DuplicatesDisallowed, Buttons buttons = Buttons(Add|Remove|RemoveAll|UpDown) );
      void setTexts(const QStringList& items);
      QStringList texts() const;
      uint count() const;
      QString text(uint i) const { return item(i)->text(textColumn()); }
      const QListViewItem *item(uint i) const;
      Buttons buttons() const { return _buttons; }
      void setButtons(Buttons buttons);
      void setEditText(const QString &text);
      void addItem(const QString &text);

   signals:
      void reset();
      void changed();
      void added( const QString & text );
      void removed( const QString & text );

   public slots:
      void clear();

   protected slots:
      virtual void moveItemUp();
      virtual void moveItemDown();
      virtual void clearEdit();
      virtual void addItem();
      virtual void removeItem();
      void selectionChanged();
      void typedSomething(const QString& text);

   protected:
      KListView *_listView;

      virtual QListViewItem *createItem();
      virtual uint textColumn() const { return 0; }
      QString selectedText() const;

   private:
      Mode         _mode;
      Buttons      _buttons;
      QVBoxLayout *_buttonsLayout;
      KLineEdit   *m_lineEdit;
      KPushButton *_addButton, *_removeButton, *_moveUpButton, *_moveDownButton,
                  *_removeAllButton, *_resetButton;

      void init(uint nbColumns, QWidget *view);
      void updateButtons();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(EditListBox::Buttons)

#endif
