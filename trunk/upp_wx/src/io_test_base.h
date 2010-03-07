///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __io_test_base__
#define __io_test_base__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

#include "formbuilder_includes.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class IOTestDialogBase
///////////////////////////////////////////////////////////////////////////////
class IOTestDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText17;
		wxRadioButton* m_radioVPP12;
		wxStaticText* m_labelVPP;
		wxRadioButton* m_radioVPP5;
		wxStaticText* m_labelVPPVoltage;
		wxRadioButton* m_radioVPPFLOAT;
		wxStaticText* m_labelVPP2;
		wxRadioButton* m_radioVPPRESET;
		wxStaticText* m_labelVPPVoltage2;
		wxRadioButton* m_radioVDD5;
		wxStaticText* m_labelVDD;
		wxRadioButton* m_radioVDDFLOAT;
		wxStaticText* m_labelVDDVoltage;
		wxRadioButton* m_radioPGD5;
		wxStaticText* m_labelPGD;
		wxRadioButton* m_radioPGD3_3;
		wxStaticText* m_labelPGDVoltage;
		wxRadioButton* m_radioPGD0;
		
		wxRadioButton* m_radioPGDIn;
		
		wxRadioButton* m_radioPGC5;
		wxStaticText* m_labelPGC;
		wxRadioButton* m_radioPGC3_3;
		wxStaticText* m_labelPGCVoltage;
		wxRadioButton* m_radioPGC0;
		
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void on_vpp( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_vdd( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_pgd( wxCommandEvent& event ){ event.Skip(); }
		virtual void on_pgc( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		IOTestDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Input/output test"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~IOTestDialogBase();
	
};

#endif //__io_test_base__
