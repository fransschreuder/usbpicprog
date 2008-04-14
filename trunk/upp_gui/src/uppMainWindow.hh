// generated 2008/4/11 17:29:11 CEST by frans@ubuntu.(none)
// using glademm V2.6.0
//
// newer (non customized) versions of this file go to uppMainWindow.hh_new

// you might replace
//    class foo : public foo_glade { ... };
// by
//    typedef foo_glade foo;
// if you didn't make any modifications to the widget

#ifndef _UPPMAINWINDOW_HH
#  include "uppMainWindow_glade.hh"
#  define _UPPMAINWINDOW_HH

#include "read_hexfile.h"
#include "pictype.h"

#include <gtkmm.h>
using namespace Gtk;

class uppMainWindow : public uppMainWindow_glade
{  
	public:
		uppMainWindow();
		
        
        void on_new_activate();
        void on_open_activate();
        void on_reload_activate();
        void on_save_activate();
        void on_save_as_activate();
        void on_quit_activate();
        void on_program_activate();
        void on_bulkerase_activate();
        void on_read_activate();
        void on_verify_activate();
        void on_about_activate();
        void on_newButton_clicked();
        void on_openButton_clicked();
        void on_reloadButton_clicked();
        void on_saveButton_clicked();
		void on_saveAsButton_clicked();
	private:
		Glib::RefPtr<TextBuffer> hexBuffer;
		PicType *picType;
		ReadHexFile *readHexFile;

};
#endif
