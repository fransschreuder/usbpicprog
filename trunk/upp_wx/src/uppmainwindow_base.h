///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
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
#include "hexview.h"
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include "configview.h"
#include <wx/hyperlink.h>
#include <wx/statbox.h>
#include <wx/choice.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include "packageview.h"
#include <wx/notebook.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

#include "formbuilder_includes.h"

///////////////////////////////////////////////////////////////////////////

#define wxID_PACKAGE_VARIANT 1000
#define wxID_EXAMPLES 1001
#define wxID_IO_TEST 1002

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
		UppHexViewGrid* m_pCodeGrid;
		wxPanel* m_pDataPanel;
		wxStaticText* m_staticText3;
		UppHexViewGrid* m_pDataGrid;
		wxPanel* m_pConfigPanel;
		wxStaticText* m_staticText2;
		UppConfigViewBook* m_pConfigListbook;
		wxPanel* m_pInfoPanel;
		wxHyperlinkCtrl* m_pDatasheetLink;
		wxStaticText* m_pVPPText;
		wxStaticText* m_pVDDText;
		wxStaticText* m_pFrequencyText;
		wxStaticText* m_pDeviceIDText;
		wxStaticText* m_pCodeMemoryText;
		wxStaticText* m_pDataMemoryText;
		wxStaticText* m_pTypeText;
		wxStaticText* m_staticText7;
		wxChoice* m_pPackageVariants;
		
		wxBitmapButton* m_pZoomInButton;
		wxBitmapButton* m_pZoomOutButton;
		wxBitmapButton* m_pZoomFitButton;
		UppPackageViewWindow* m_pPackageWin;
		wxStaticText* m_staticText11;
		wxMenuBar* m_pMenuBar;
		wxMenu* m_pMenuFile;
		wxMenu* m_pMenuEdit;
		wxMenu* m_pMenuOptions;
		wxMenu* m_pMenuHelp;
		wxStatusBar* m_pStatusBar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void on_new( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_open( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_examples( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_refresh( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_save( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_save_as( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_exit( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_copy( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_selectall( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_preferences( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_io_test( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_help( wxCommandEvent& event ) { event.Skip(); }
		virtual void on_about( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		UppMainWindowBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Usbpicprog"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 891,534 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~UppMainWindowBase();
	
};

#endif //__uppmainwindow_base__
