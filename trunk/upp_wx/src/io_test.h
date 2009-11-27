#ifndef __io_test__
#define __io_test__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/radiobut.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include "io_test_base.h"
#include "hardware.h"


#ifdef __WXMAC__
    #define EVENT_FIX
#else //__WXMAC__
    #define EVENT_FIX event.Skip();
#endif //__WXMAC__

class IOTest : public IOTestBase
{
	private:

		Hardware* m_hardware;
		wxTimer* m_timer;
	protected:

		void on_vpp( wxCommandEvent& event ){ upp_vpp() ; EVENT_FIX }
		void on_vdd( wxCommandEvent& event ){ upp_vdd() ; EVENT_FIX }
		void on_pgd( wxCommandEvent& event ){ upp_pgd() ; EVENT_FIX }
		void on_pgc( wxCommandEvent& event ){ upp_pgc() ; EVENT_FIX }
		void on_btn_close( wxCommandEvent& event ){ upp_btn_close() ; EVENT_FIX }
		void on_timer(wxTimerEvent& event){upp_timer(); EVENT_FIX }

		
		void upp_vpp();
		void upp_vdd();
		void upp_pgd();
		void upp_pgc();
		void upp_btn_close();
		void upp_timer();

	
	public:
		IOTest(Hardware* hardware, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Usbpicprog IO Test"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~IOTest();
	
};

#endif //__io_test__
