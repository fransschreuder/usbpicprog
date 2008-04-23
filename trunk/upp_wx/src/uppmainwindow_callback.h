#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <wx/wx.h>
#include <iostream>
using namespace std;

#include "uppmainwindow.h"
#include "hardware.h"
#include "pictype.h"
#include "read_hexfile.h"


class UppMainWindowCallBack: public UppMainWindow
{
	
	public:
		 
		UppMainWindowCallBack(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("usbpicprog"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 634,361 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL  ) ;
		
		void on_new( wxCommandEvent& event );
		void on_open( wxCommandEvent& event );
		void on_refresh( wxCommandEvent& event );
		void on_save( wxCommandEvent& event );
		void on_save_as( wxCommandEvent& event );
		void on_exit( wxCommandEvent& event );
		void on_program( wxCommandEvent& event );
		void on_read( wxCommandEvent& event );
		void on_verify( wxCommandEvent& event );
		void on_erase( wxCommandEvent& event );
		void on_blankcheck( wxCommandEvent& event );
		void on_autodetect( wxCommandEvent& event );
		void on_connect( wxCommandEvent& event );
		void on_disconnect( wxCommandEvent& event );
		void on_help( wxCommandEvent& event );
		void on_about( wxCommandEvent& event );
		void on_combo_changed( wxCommandEvent& event );
	private:
		ReadHexFile* readHexFile;
		PicType* picType;
		Hardware* hardware;
};
