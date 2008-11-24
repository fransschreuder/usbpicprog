///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __uppmainwindow_base__
#define __uppmainwindow_base__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

#include "formbuilder_includes.h"

///////////////////////////////////////////////////////////////////////////

#define wxID_SAVE_AS 1000

///////////////////////////////////////////////////////////////////////////////
/// Class UppMainWindowBase
///////////////////////////////////////////////////////////////////////////////
class UppMainWindowBase : public wxFrame 
{
	private:
	
	protected:
		wxBoxSizer* m_pSizer;
		wxNotebook* m_pNotebook;
		wxPanel* m_pCodePanel;
		wxStaticText* m_staticText1;
		wxGrid* m_pCodeGrid;
		wxPanel* m_pConfigPanel;
		wxStaticText* m_staticText2;
		wxGrid* m_pConfigGrid;
		wxPanel* m_pDataPanel;
		wxStaticText* m_staticText3;
		wxGrid* m_pDataGrid;
		wxMenuBar* m_pMenuBar;
		wxMenu* m_pMenuFile;
		wxMenu* m_pMenuEdit;
		wxMenu* uppMenuHelp;
		wxStatusBar* m_pStatusBar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void on_new( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_open( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_refresh( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_save( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_save_as( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_exit( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_help( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_about( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		UppMainWindowBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Usbpicprog"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 891,534 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~UppMainWindowBase();
	
};

#endif //__uppmainwindow_base__
