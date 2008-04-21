///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __uppmainwindow__
#define __uppmainwindow__

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/toolbar.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class UppMainWindow
///////////////////////////////////////////////////////////////////////////////
class UppMainWindow : public wxFrame 
{
	private:
	
	protected:
		wxTextCtrl* m_textCtrl3;
		wxMenuBar* m_menubar1;
		wxMenu* uppMenuFile;
		wxMenu* uppMenuActions;
		wxStatusBar* m_statusBar1;
		wxToolBar* m_toolBar1;
		wxButton* uppButtonNew;
		wxComboBox* m_comboBox1;
		
		// Virtual event handlers, overide them in your derived class
		virtual void on_tool1_clicked( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		UppMainWindow( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("usbpicprog"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 634,361 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~UppMainWindow();
	
};

#endif //__uppmainwindow__
