#include "config.h"
#include "uppMainWindow.hh"
#include <gtkmm.h>
#include <iostream>
#include "read_hexfile.h"
#include "pictype.h"
using namespace std;

uppMainWindow::uppMainWindow()
{
	hexBuffer=hexView->get_buffer();
		picType=new PicType("P18F2550");
		cout<<picType->getCurrentPic().Name<<endl;

		
}

void uppMainWindow::on_new_activate()
{
	
	hexBuffer->set_text("");
	
	
}

void uppMainWindow::on_open_activate()
{  
	Gtk::FileChooserDialog dialog("Select a hex file",
          Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);

  //Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

  //Add filters, so that only certain file types can be selected:
  Gtk::FileFilter filter_hex;
  filter_hex.set_name("Hex files");
  filter_hex.add_pattern("*.hex");
  dialog.add_filter(filter_hex);
	
  Gtk::FileFilter filter_any;
  filter_any.set_name("Any files");
  filter_any.add_pattern("*");
  dialog.add_filter(filter_any);

  //Show the dialog and wait for a user response:
  int result = dialog.run();

  //Handle the response:
  switch(result)
  {
    case(Gtk::RESPONSE_OK):
    {
      std::cout << "Open clicked." << std::endl;
		readHexFile=new ReadHexFile(picType,dialog.get_filename().c_str());
      //Notice that this is a std::string, not a Glib::ustring.
      break;
    }
    case(Gtk::RESPONSE_CANCEL):
    {
      std::cout << "Cancel clicked." << std::endl;
      break;
    }
    default:
    {
      std::cout << "Unexpected button clicked." << std::endl;
      break;
    }
  }	
}


void uppMainWindow::on_reload_activate()
{  
}

void uppMainWindow::on_save_activate()
{  
}

void uppMainWindow::on_save_as_activate()
{  
}

void uppMainWindow::on_quit_activate()
{  
}

void uppMainWindow::on_program_activate()
{  
}

void uppMainWindow::on_bulkerase_activate()
{  
}

void uppMainWindow::on_read_activate()
{  
}

void uppMainWindow::on_verify_activate()
{  
}

void uppMainWindow::on_about_activate()
{  
}

void uppMainWindow::on_newButton_clicked()
{  
	on_new_activate();
}

void uppMainWindow::on_openButton_clicked()
{  
	on_open_activate();
}

void uppMainWindow::on_reloadButton_clicked()
{  
	on_reload_activate();
}

void uppMainWindow::on_saveButton_clicked()
{  
	on_save_activate();
}


void uppMainWindow::on_saveAsButton_clicked()
{  
	on_save_as_activate();
}
