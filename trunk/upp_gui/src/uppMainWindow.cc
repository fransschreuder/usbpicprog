/***************************************************************************
 *   Copyright (C) 2008 by Frans Schreuder                                 *
 *   usbpicprog.sourceforge.net                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "config.h"
#include "uppMainWindow.hh"
#include <gtkmm.h>
#include <iostream>
using namespace std;

void uppMainWindow::hexBufferAppend(string line)
{
	Gtk::TextIter textIter=hexBuffer->get_iter_at_offset(-1);
	hexBuffer->insert(textIter,line);
}

void uppMainWindow::printHexFile()
{
	int lineSize;
	char txt[256];
	hexBuffer->set_text("");
	
	hexBufferAppend("Code Memory\n");
	for(int i=0;i<readHexFile->getCodeMemory().size();i+=16)
	{
		sprintf(txt,"%00000008X::",i);
		hexBufferAppend(txt);
		if(i+16<readHexFile->getCodeMemory().size())
		{
			lineSize=16;
		}
		else
		{
			lineSize=readHexFile->getCodeMemory().size()-i;
		}
		for(int j=0;j<lineSize;j++)
		{
			sprintf(txt,"%02X",readHexFile->getCodeMemory()[i+j]);
			hexBufferAppend(txt);
		}
		hexBufferAppend("\n");	
	}
	hexBufferAppend("\nConfig Memory\n");
	for(int i=0;i<readHexFile->getConfigMemory().size();i+=16)
	{
		sprintf(txt,"%00000008X::",i+picType->getCurrentPic().ConfigAddress);
		hexBufferAppend(txt);
		if(i+16<readHexFile->getConfigMemory().size())
		{
			lineSize=16;
		}
		else
		{
			lineSize=readHexFile->getConfigMemory().size()-i;
		}
		for(int j=0;j<lineSize;j++)
		{
			sprintf(txt,"%02X",readHexFile->getConfigMemory()[i+j]);
			hexBufferAppend(txt);
		}
		hexBufferAppend("\n");
	}
	hexBufferAppend("\nData Memory\n");;
	for(int i=0;i<readHexFile->getDataMemory().size();i+=16)
	{
		sprintf(txt,"%00000008X::",i);
		hexBufferAppend(txt);
		if(i+16<readHexFile->getDataMemory().size())
		{
			lineSize=16;
		}
		else
		{
			lineSize=readHexFile->getDataMemory().size()-i;
		}
		for(int j=0;j<lineSize;j++)
		{
			sprintf(txt,"%02X",readHexFile->getDataMemory()[i+j]);
			hexBufferAppend(txt);
		}
		hexBufferAppend("\n");
	}
}


uppMainWindow::uppMainWindow()
{
	Gtk::TreeModel::Row picTypeComboRow;
	hexBuffer=hexView->get_buffer();
	picTypeComboStore = Gtk::ListStore::create(picTypeColumns);
    picTypeCombo->set_model(picTypeComboStore);
	
	
	hardware=new Hardware();
	if(hardware->connected())
		statusBar->push("Usbpicprog found");
	else
		statusBar->push("Usbpicprog not found");
	readHexFile=new ReadHexFile();
	
	picType=new PicType(hardware->autoDetectDevice());
	
	
	for(int i=0;i<picType->getPicNames().size();i++)
	{
		picTypeComboRow = *(picTypeComboStore->append());
    	picTypeComboRow[picTypeColumns.colName] =picType->getPicNames()[i];
		
	}
	picTypeCombo->pack_start(picTypeColumns.colName);
	for(int i=0;i<picType->getPicNames().size();i++)
	{
		if(picType->getCurrentPic().Name.compare(picType->getPicNames()[i])==0)
			picTypeCombo->set_active(i);
	}
	
	picTypeCombo->signal_changed().connect( sigc::mem_fun(*this, &uppMainWindow::on_combo_changed) );
	cout<<"Detected: "<<picType->getCurrentPic().Name<<endl;

		
}

void uppMainWindow::on_combo_changed()
{
  Gtk::TreeModel::iterator iter = picTypeCombo->get_active();
  if(iter)
  {
    Gtk::TreeModel::Row row = *iter;
    if(row)
    {
      
     	Glib::ustring name = row[picTypeColumns.colName];
		picType=new PicType(name);
      cout << "Name=" << picType->getCurrentPic().Name << std::endl;
    }
  }
  else
    std::cout << "invalid iter" << std::endl;
}


void uppMainWindow::on_new_activate()
{
	
	hexBuffer->set_text("");
	delete readHexFile;
	readHexFile=new ReadHexFile();
	
	
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
		readHexFile->open(picType,dialog.get_filename().c_str());
		printHexFile();
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
	readHexFile->reload(picType);
	printHexFile();
}

void uppMainWindow::on_save_activate()
{  
	readHexFile->save(picType);
}

void uppMainWindow::on_save_as_activate()
{  
	Gtk::FileChooserDialog dialog("Select a hex file",
          Gtk::FILE_CHOOSER_ACTION_SAVE);
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
		readHexFile->saveAs(picType,dialog.get_filename().c_str());
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

void uppMainWindow::on_quit_activate()
{  
	hide();
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
