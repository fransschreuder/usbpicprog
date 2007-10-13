/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "text_editor.h"

#include <qfile.h>
#include <qtextedit.h>
#include <qlayout.h>

#include <klibloader.h>
#include <kpopupmenu.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kapplication.h>
#include <ktexteditor/markinterface.h>

#include "main_global.h"
#include "gui_debug_manager.h"
#include "editor_manager.h"
#include "global_config.h"
#include "toplevel.h"

//-----------------------------------------------------------------------------
const TextEditor::MarkTypeData TextEditor::MARK_TYPE_DATA[Breakpoint::Nb_MarkTypes] = {
  { KTextEditor::MarkInterface::Execution,          "piklab_program_counter"          },
  { KTextEditor::MarkInterface::markType08,         "piklab_program_counter_disabled" },
  { KTextEditor::MarkInterface::BreakpointActive,   "piklab_breakpoint_active"        },
  { KTextEditor::MarkInterface::BreakpointDisabled, "piklab_breakpoint_disabled"      },
  { KTextEditor::MarkInterface::BreakpointReached,  "piklab_breakpoint_reached"       },
  { KTextEditor::MarkInterface::Error,              "piklab_breakpoint_invalid"       }
};

QPixmap TextEditor::pixmap(Breakpoint::MarkType type)
{
  return SmallIcon(MARK_TYPE_DATA[type].pixmap);
}

TextEditor::TextEditor(bool withDebugger, QWidget *parent, const char *name)
  : Editor(parent, name), _view(0)
{
  KLibFactory *factory = KLibLoader::self()->factory("libkatepart");
  if ( factory==0 ) qFatal("Could not find katepart");
  _document = static_cast<Kate::Document *>(factory->create(this, "kate", "KTextEditor::Document"));
  _oldModified = _document->isModified();
  _oldReadOnly = !_document->isReadWrite();

  QVBoxLayout *top = new QVBoxLayout(this, 0, 10);
  _split = new QSplitter(this);
  _split->setFrameStyle(QFrame::TabWidgetPanel |  QFrame::Sunken);
  top->addWidget(_split);

  connect(_document, SIGNAL(hlChanged()), SLOT(highlightChanged()));
  setAcceptDrops(true);
  addView();

  if (withDebugger) {
    for (uint i=0; i<Breakpoint::Nb_MarkTypes; i++)
      _document->setPixmap(KTextEditor::MarkInterface::MarkTypes(MARK_TYPE_DATA[i].value), pixmap(Breakpoint::MarkType(i)));
    QTimer::singleShot(0, this, SLOT(addToDebugManager()));
  }
}

bool TextEditor::open(const PURL::Url &url)
{
  setReadOnly(PURL::DATA[url.fileType()].properties & PURL::ReadOnly);
  if ( !_document->openURL(url.kurl()) ) return false;
  _view->setEol(url.isDosFile() ? Dos : Unix);
  highlightChanged();
  return true;
}

void TextEditor::addToDebugManager()
{
  static_cast<Debugger::GuiManager *>(Debugger::manager)->addTextEditor(*this);
}

void TextEditor::addView()
{
    KTextEditor::View *v = _document->createView(_split);
    if ( _view==0 ) _view = static_cast<Kate::View *>(v);
    Q_ASSERT(v);
    connect(v, SIGNAL(gotFocus(Kate::View *)), SLOT(gotFocus(Kate::View *)));
    connect(v, SIGNAL(cursorPositionChanged()), SLOT(statusChanged()));
    connect(v, SIGNAL(dropEventPass(QDropEvent *)), SIGNAL(dropEventPass(QDropEvent *)));
    connect(v, SIGNAL(newStatus()), SLOT(statusChanged()));
    v->show();
    v->setFocus();
    v->installEventFilter(this);
    KTextEditor::PopupMenuInterface *pmi = KTextEditor::popupMenuInterface(v);
    pmi->installPopup(&Main::popup("text_editor_context_menu"));

    // dispatch available space between views
    QValueList<int> list = _split->sizes();
    QValueList<int>::Iterator it;
    int sum = 0;
    for (it = list.begin(); it!= list.end(); ++it) sum += *it;
    sum /= list.size();
    for (it = list.begin(); it!=list.end(); ++it) *it = sum;
    _split->setSizes(list);

    emit guiChanged();
}

void TextEditor::gotFocus(Kate::View *v)
{
  _view = v;
  setFocusProxy(v);
  statusChanged();
  emit guiChanged();
}

void TextEditor::addGui()
{
  Main::toplevel().guiFactory()->addClient(_view);
}

void TextEditor::removeGui()
{
  Main::toplevel().guiFactory()->removeClient(_view);
}

void TextEditor::removeCurrentView()
{
  delete _view;
  _view = static_cast<Kate::View *>(_document->views().last());
  _document->views().last()->setFocus();
  emit guiChanged();
}

bool TextEditor::isReadOnly() const
{
  return !_document->isReadWrite();
}

void TextEditor::setReadOnlyInternal(bool ro)
{
  _oldReadOnly = ro;
  _document->setReadWrite(!ro);
}

bool TextEditor::isModified() const
{
  return _document->isModified();
}

void TextEditor::setModifiedInternal(bool m)
{
  _oldModified = m;
  _document->setModified(m);
}

void TextEditor::statusChanged()
{
  uint line, col;
  _view->cursorPosition(&line, &col) ;
  QString text = i18n("Line: %1  Col: %2").arg(line+1).arg(col+1);
  if( isReadOnly() ) text += "  " + i18n("R/O");
  emit statusTextChanged(" " + text + " ");
  if ( isReadOnly()!=_oldReadOnly || isModified()!=_oldModified ) emit guiChanged();
  if ( isModified()!=_oldModified ) emit modified();
  _oldModified = isModified();
  _oldReadOnly = isReadOnly();
  Breakpoint::Data data(url(), line);
  Breakpoint::updateActions(&data);
}

uint TextEditor::highlightMode(const QString &name) const
{
  uint mode = 0;
  for (; mode<_document->hlModeCount(); mode++)
    if ( _document->hlModeName(mode)==name ) break;
  return mode;
}

void TextEditor::highlightChanged()
{
  // managed by hand because of collisions with file extensions
  // used by other languages/softwares like other ASM and also PHP...
  const char *name = PURL::DATA[fileType()].highlightModeName;
  if ( name==0 ) return;
  uint mode = highlightMode(name);
  if ( mode>=_document->hlModeCount() || _document->hlMode()==mode ) return;
  _document->setHlMode(mode);
}

uint TextEditor::cursorLine() const
{
  uint line;
  _view->cursorPosition(&line, 0);
  return line;
}

void TextEditor::setMark(uint line, Breakpoint::MarkType type)
{
  _view->setIconBorder(true);
  _document->setMark(line, MARK_TYPE_DATA[type].value);
}

void TextEditor::clearMarks()
{
  _document->clearMarks();
}

#if 0
void TextEditor::slotChangedText()
{
    //This slot runs the instrucion set help bar,
    // finds the current command word and compares it to the set of instrucions
    //Found in the descript Qstringlist.


    QString currentword;
    QString testval;

    Kate::View *v =  currentView();
    currentword = v->currentTextLine();
    //prepare the string for compareing
    currentword = currentword.simplifyWhiteSpace();
    currentword = currentword.upper();

    for ( QStringList::Iterator it = descript.begin(); it != descript.end(); ++it )
    {
        testval = *it;
        if(testval.startsWith(currentword.left(5)))
        {
            //if (testval.left(5) == currentword.left(5) ) {
            lab_curword->setText(*it);
        }
        //      else {
        //              lab_curword->setText("Pic Instruction Help");
        //      }
    }


}
#endif
#if 0
void TextEditor::populateList()
{
    //Populate the qstringlist with the pic instruction set, and the text to go along with them

    descript    += "CLRF  : Clear F     OPERANDS: f";
    descript    += "ADDWF : Add W to F  OPERANDS: f,d";
    descript    += "ANDWF : AND W with F OPERANDS: f,d";
    descript    += "CLRW  : Clear W     OPERANDS: NONE";
    descript    += "COMF  : Complement F OPERANDS: f,d";
    descript    += "DECF  : Decrement  F OPERANDS: f,d";
    descript    += "DECSSZ: Decrement F, Skip 0 OPERANDS: f,d";
    descript    += "INCF  : Increment F  OPERANDS: f,d";
    descript    += "INCFSZ: Increment F, Skip 0 OPERANDS: f,d";
    descript    += "IORWF : Inclusive OR W with F OPERANDS: f,d";
    descript    += "MOVF  : Move F,  OPERANDS: f,d";
    descript    += "MOVWF : Move W to F OPERANDS: f,d";
    descript    += "NOP   : No Operation OPERANDS: NONE";
    descript    += "RLF   : Rotate Left F through Carry OPERANDS: f,d";
    descript    += "RRF   : Rotate Right F through Carry OPERANDS: f,d";
    descript    += "SUBWF : Subtract W from F OPERANDS: f,d";
    descript    += "SWAPF : Swap Nibbles in F OPERANDS: f,d";
    descript    += "XORWF : Exclusive OR W with F OPERANDS: f,s";
    descript    += "BCF   : Bit Clear F OPERANDS: f,b";
    descript    += "BSF   : Bit Set F   OPERANDS: f,b";
    descript    += "BTFSC : Bit Test F, Skip if Clear OPERANDS: f,b";
    descript    += "BTFSS : Bit Test F, Skip if Set   OPERANDS: f,b";
    descript    += "ADDLW : Add Literal and W OPERANDS: k";
    descript    += "ANDLW : And Literal and W OPERANDS: k";
    descript    += "CLRWDT: Clear Watchdog Timer OPERANDS: NONE";
    descript    += "CALL  : Call Subroutine OPERANDS: k";
    descript    += "GOTO  : Goto address OPERANDS: k";
    descript    += "IORLW : Inclusive OR literal with W OPERANDS: k";
    descript    += "MOVLW : Move Literal with W  OPERANDS: k";
    descript    += "RETFIE: Return From Interrupt OPERANDS: NONE";
    descript    += "RETLW : Return with Literal in W OPERANDS: k";
    descript    += "RETURN: Return from subroutine OPERANDS: NONE";
    descript    += "SLEEP : Go into Standby Mode OPERANDS: NONE";
    descript    += "SUBLW : Subtract W from Literal OPERANDS: k";
    descript    += "XORLW : Exclusive OR Literal with W OPERANDS: k";

}
#endif

//-----------------------------------------------------------------------------
SimpleTextEditor::SimpleTextEditor(bool withDebugger, PURL::FileType type, QWidget *parent, const char *name)
  : TextEditor(withDebugger, parent, name), _type(type), _file(_sview)
{}

SimpleTextEditor::SimpleTextEditor(bool withDebugger, QWidget *parent, const char *name)
  : TextEditor(withDebugger, parent, name), _type(PURL::Nb_FileTypes), _file(_sview)
{}

bool SimpleTextEditor::open(const PURL::Url &url)
{
  _type = url.fileType();
  return TextEditor::open(url);
}

bool SimpleTextEditor::setText(const QString &text)
{
  _file.openForWrite();
  _file.appendText(text);
  _file.close();
  if ( !_document->openURL(_file.url().kurl()) ) return false;
  highlightChanged();
  return true;
}
