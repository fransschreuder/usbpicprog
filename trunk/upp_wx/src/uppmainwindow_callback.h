#ifndef UPPMAINWINDOW_CALLBACK_H
#define UPPMAINWINDOW_CALLBACK_H
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <wx/wx.h>

#if wxCHECK_VERSION(2,7,1) //about dialog only implemented from wxWidgets v2.7.1
#include <wx/aboutdlg.h>
#else
#warning About dialog not implemented, use a newer wxWidgets version!
#endif
//#include <wx/utils.h>
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
		
		void updateProgress(int value);
		void printHexFile();
		void on_new( wxCommandEvent& event ){upp_new();}
		void on_open( wxCommandEvent& event ){upp_open();};
		void on_refresh( wxCommandEvent& event ){upp_refresh();};
		void on_save( wxCommandEvent& event ){upp_save();};
		void on_save_as( wxCommandEvent& event ){upp_save_as();};
		void on_exit( wxCommandEvent& event ){upp_exit();};
		void on_program( wxCommandEvent& event ){upp_program();};
		void on_read( wxCommandEvent& event ){upp_read();};
		void on_verify( wxCommandEvent& event ){upp_verify();};
		void on_erase( wxCommandEvent& event ){upp_erase();};
		void on_blankcheck( wxCommandEvent& event ){upp_blankcheck();};
		void on_autodetect( wxCommandEvent& event ){upp_autodetect();};
		void on_connect( wxCommandEvent& event ){upp_connect();};
		void on_disconnect( wxCommandEvent& event ){upp_disconnect();};
		void on_help( wxCommandEvent& event ){upp_help();};
		void on_about( wxCommandEvent& event ){upp_about();};
		void on_combo_changed( wxCommandEvent& event ){upp_combo_changed();};
		void upp_open_file(wxString path);
	private:
		void upp_new();
		void upp_open();
		void upp_refresh();
		void upp_save();
		void upp_save_as();
		void upp_exit();
		void upp_program();
		void upp_read();
		void upp_verify();
		void upp_erase();
		void upp_blankcheck();
		bool upp_autodetect();
		bool upp_connect();
		void upp_disconnect();
		void upp_help();
		void upp_about();
		void upp_combo_changed();
		ReadHexFile* readHexFile;
		PicType* picType;
		Hardware* hardware;
		bool fileOpened;
		
};
#endif
