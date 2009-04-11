///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __preferences_base__
#define __preferences_base__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class UppPreferencesDialogBase
///////////////////////////////////////////////////////////////////////////////
class UppPreferencesDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText12;
		wxCheckBox* m_pCheckboxProgramCode;
		wxCheckBox* m_pCheckboxProgramConfig;
		wxCheckBox* m_pCheckboxProgramData;
		wxCheckBox* m_pCheckboxEraseBeforeProgramming;
		wxCheckBox* m_pCheckboxVerifyAfterProgramming;
		wxCheckBox* m_pCheckboxVerifyCode;
		wxCheckBox* m_pCheckboxVerifyConfig;
		wxCheckBox* m_pCheckboxVerifyData;
		wxCheckBox* m_pCheckboxShowPopups;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
	
	public:
		UppPreferencesDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~UppPreferencesDialogBase();
	
};

#endif //__preferences_base__
