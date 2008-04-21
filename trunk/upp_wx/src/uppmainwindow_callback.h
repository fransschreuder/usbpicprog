#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <wx/wx.h>
#include <iostream>
using namespace std;

#include "uppmainwindow.h"

class UppMainWindowCallBack: public UppMainWindow
{
	
	public:
		 
		UppMainWindowCallBack(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("usbpicprog"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 634,361 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL  ) ;
		void on_tool1_clicked(wxCommandEvent& event);
};
