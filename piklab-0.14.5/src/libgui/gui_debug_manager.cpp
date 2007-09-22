/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "gui_debug_manager.h"

#include "toplevel.h"
#include "text_editor.h"
#include "project.h"
#include "tools/list/compile_config.h"
#include "register_view.h"
#include "watch_view.h"
#include "tools/list/compile.h"
#include "project_manager.h"
#include "editor_manager.h"
#include "main_global.h"
#include "coff/base/text_coff.h"
#include "progs/base/generic_prog.h"
#include "gui_prog_manager.h"
#include "breakpoint_view.h"

bool Debugger::GuiManager::addEditor(Editor &editor)
{
  if ( _editors.find(&editor)!=_editors.end() ) return false;
  connect(&editor, SIGNAL(destroyed()), SLOT(editorDestroyed()));
  _editors.append(&editor);
  return true;
}

void Debugger::GuiManager::addRegisterView(Register::MainView &view)
{
  if ( !addEditor(view) ) return;
  updateRegisters();
}

void Debugger::GuiManager::addTextEditor(TextEditor &editor)
{
  if ( !addEditor(editor) ) return;
  updateEditorMarks(editor);
  TextEditor *shown;
  bool hasCurrentPC;
  showPC(editor, shown, true, hasCurrentPC);
}

void Debugger::GuiManager::clearEditors()
{
  QValueList<Editor *>::iterator it = _editors.begin();
  for (; it!=_editors.end(); ++it) (*it)->disconnect(this);
  _editors.clear();
}

void Debugger::GuiManager::editorDestroyed()
{
  QValueList<Editor *>::iterator it = _editors.begin();
  for (; it!=_editors.end(); ++it) {
    if ( (*it)!=sender() ) continue;
    _editors.remove(it);
    break;
  }
}

void Debugger::GuiManager::updateDevice()
{
  Manager::updateDevice();
  Main::watchView().init(false);
}

PURL::Url Debugger::GuiManager::coffUrl() const
{
  return Main::projectManager().projectUrl().toFileType(PURL::Coff);
}

bool Debugger::GuiManager::internalInit()
{
  if ( !Manager::internalInit() ) return false;
  if ( !Main::projectManager().contains(coffUrl()) )
    Main::projectManager().addExternalFile(coffUrl(), ProjectManager::Generated);
  Main::watchView().init(true);
  if ( registerView() ) registerView()->view()->updateView();
  return true;
}

Breakpoint::Data Debugger::GuiManager::currentBreakpointData()
{
  const Breakpoint::Data *data = Main::breakpointsView().currentData();
  if (data) return *data;
  TextEditor *editor = ::qt_cast<TextEditor *>(Main::currentEditor());
  Q_ASSERT(editor);
  return Breakpoint::Data(editor->url(), editor->cursorLine());
}

void Debugger::GuiManager::toggleBreakpoint()
{
  Breakpoint::Data data = currentBreakpointData();
  if ( Breakpoint::list().contains(data) ) {
    Breakpoint::list().remove(data);
    return;
  }
  uint address;
  if ( !checkBreakpoint(data, false, address) ) return;
  Breakpoint::list().append(data);
  Breakpoint::list().setAddress(data, address);
  if (_coff) toggleEnableBreakpoint();
}

void Debugger::GuiManager::toggleEnableBreakpoint()
{
  Q_ASSERT(_coff);
  Breakpoint::Data data = currentBreakpointData();
  bool isActive = ( Breakpoint::list().state(data)==Breakpoint::Active );
  if ( Breakpoint::list().address(data)==Device::INVALID ) {
    if ( !isActive ) MessageBox::sorry(i18n("Breakpoint at non-code line cannot be activated."), Log::Show);
  } else {
    if ( !isActive ) {
      freeActiveBreakpoint();
      Breakpoint::list().setState(data, Breakpoint::Active);
    } else Breakpoint::list().setState(data, Breakpoint::Disabled);
  }
}

void Debugger::GuiManager::updateEditorMarks(TextEditor &editor) const
{
  editor.clearMarks();
  // update breakpoints
  bool reached = false;
  for (uint i=0; i<Breakpoint::list().count(); i++) {
    const Breakpoint::Data &data = Breakpoint::list().data(i);
    int line = -1;
    uint address = Breakpoint::list().address(data);
    if ( _coff && address!=Device::INVALID ) line = _coff->lineForAddress(editor.url(), address);
    else if ( data.url==editor.url() ) line = data.line;
    Breakpoint::MarkType type = breakpointType(data);
    if ( line!=-1 ) editor.setMark(line, type);
    if ( type==Breakpoint::BreakpointReached ) reached = true;
  }
  // update pc
  bool isDebugging = ( _coff && programmer() && programmer()->isInDebugging() );
  int pcline = (isDebugging && pc()!=Device::INVALID ? _coff->lineForAddress(editor.url(), pc()) : -1);
  if ( pcline!=-1 && !reached ) {
    if ( programmer()->state()==Programmer::Halted ) editor.setMark(pcline, Breakpoint::ProgramCounterActive);
    else editor.setMark(pcline, Breakpoint::ProgramCounterDisabled);
  }

}

void Debugger::GuiManager::clear()
{
  Manager::clear();
  Main::watchView().init(true);
}

bool Debugger::GuiManager::update()
{
  bool on = (programmer() ? programmer()->isTargetPowerOn() : false);
  static_cast<KToggleAction *>(Main::action("prog_power"))->setChecked(on);
  return Manager::update();
}

void Debugger::GuiManager::showPC(TextEditor &editor, TextEditor *&shown,
                                  bool gotoPC, bool &currentHasPC)
{
  if ( _coff==0 ) return;
  int pcline = _coff->lineForAddress(editor.url(), pc());
  if ( pcline==-1 ) return;
  shown = &editor;
  if (gotoPC) editor.setCursor(pcline, 0);
  if ( (&editor)==Main::currentEditor() ) currentHasPC = true;
}

void Debugger::GuiManager::showPC()
{
  QValueList<Editor *>::iterator it = _editors.begin();
  TextEditor *shown = 0;
  bool currentHasPC = false;
  for (; it!=_editors.end(); ++it) {
    TextEditor *e = ::qt_cast<TextEditor *>(*it);
    if (e) showPC(*e, shown, true, currentHasPC);
  }
  if ( shown==0 ) {
    PURL::Url url = _coff->urlForAddress(pc());
    if ( !url.probablyExists() ) return;
    TextEditor *e = ::qt_cast<TextEditor *>(Main::editorManager().openEditor(url));
    if (e) showPC(*e, shown, true, currentHasPC);
  }
  if ( shown && !currentHasPC ) Main::editorManager().showEditor(shown);
}

void Debugger::GuiManager::updateView(bool gotoPC)
{
  Main::breakpointsView().updateView();
  bool currentHasPC = false;
  TextEditor *shown = 0;
  QValueList<Editor *>::iterator it = _editors.begin();
  for (; it!=_editors.end(); ++it) {
    TextEditor *e = ::qt_cast<TextEditor *>(*it);
    if (e) {
      updateEditorMarks(*e);
      showPC(*e, shown, gotoPC, currentHasPC);
    }
  }
  if ( programmer() && programmer()->state()==Programmer::Halted ) {
    if ( shown==0 && _coff && pc()!=Device::INVALID ) {
      PURL::Url url = _coff->urlForAddress(pc());
      if ( !url.probablyExists() ) return;
      TextEditor *e = ::qt_cast<TextEditor *>(Main::editorManager().openEditor(url));
      if (e) {
        updateEditorMarks(*e);
        showPC(*e, shown, gotoPC, currentHasPC);
      }
    }
    if ( shown && !currentHasPC && gotoPC ) Main::editorManager().showEditor(shown);
  }
}

Register::MainView *Debugger::GuiManager::registerView() const
{
  QValueList<Editor *>::const_iterator it = _editors.begin();
  for (; it!=_editors.end(); ++it) {
    Register::MainView *rv = ::qt_cast<Register::MainView *>(*it);
    if (rv) return rv;
  }
  return 0;
}
