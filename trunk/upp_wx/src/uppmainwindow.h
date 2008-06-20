///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include <iostream>
using namespace std;
#ifndef __uppmainwindow__
#define __uppmainwindow__

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/combobox.h>
#include <wx/toolbar.h>
#include <wx/frame.h>
#include <wx/artprov.h>
#include <wx/gauge.h>
#include <wx/radiobut.h>
#include <wx/grid.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_SAVE_AS 1000

#define STATUS_FIELD_HARDWARE 0
#define STATUS_FIELD_OTHER 1
#define STATUS_FIELD_PROGRESS 2
#define STATUS_FIELD_SIDE 3

#ifdef __WXMAC__
#define STATUS_FIELD_SIDE_WIDTH 15
#else //__WXMAC__
#define STATUS_FIELD_SIDE_WIDTH 0
#endif //__WXMAC__

#include "hexview.h"

///////////////////////////////////////////////////////////////////////////////
/// Class UppMainWindow
///////////////////////////////////////////////////////////////////////////////
class UppMainWindow : public wxFrame 
{
	
	protected:
        wxBoxSizer* bSizer;
		//UppHexview* uppHexEdit;
		wxTextCtrl* uppHexEdit;
		wxMenuBar* uppMenuBar;
		wxMenu* uppMenuFile;
		wxMenu* uppMenuActions;
		wxMenu* uppMenuHelp;
		wxStatusBar* m_statusBar1;
		wxToolBar* m_toolBar1;
		wxComboBox* m_comboBox1;
		wxGauge* uppProgressBar;
		wxRadioButton* m_radioButton_upp;
		wxRadioButton* m_radioButton_boot;
		
		// Virtual event handlers, overide them in your derived class
		virtual void on_new( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_open( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_refresh( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_save( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_save_as( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_exit( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_program( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_read( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_verify( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_erase( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_blankcheck( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_autodetect( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_connect( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_connect_boot( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_disconnect( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_help( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_about( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_combo_changed( wxCommandEvent& event ){ event.Skip(); }
		
		
	
	public:
		UppMainWindow( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("usbpicprog"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 891,534 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~UppMainWindow();
	private:
        virtual void OnSize(wxSizeEvent& event)
        {
            wxRect rect;
            if(IsShown())
            {
        	m_statusBar1->GetFieldRect(STATUS_FIELD_PROGRESS, rect);

                uppProgressBar->SetPosition(rect.GetPosition());
            	uppProgressBar->SetSize(rect.GetSize());
            }
            event.Skip();

        }
    	DECLARE_EVENT_TABLE()
	
};

#endif //__uppmainwindow__
