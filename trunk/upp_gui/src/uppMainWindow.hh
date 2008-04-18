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

#ifndef _UPPMAINWINDOW_HH
#  include "uppMainWindow_glade.hh"
#  define _UPPMAINWINDOW_HH

#include "read_hexfile.h"
#include "pictype.h"
#include "hardware.h"

#include <gtkmm.h>
using namespace Gtk;

class uppMainWindow : public uppMainWindow_glade
{  
	public:
		uppMainWindow();
		void on_detect_device();
		void on_connect_hardware();
		void on_disconnect_hardware();
        void on_combo_changed();
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
		void printHexFile();
		void hexBufferAppend(string line);
		Glib::RefPtr<TextBuffer> hexBuffer;
		  
		//Columns for the picTypeCombo
		class PicTypeColumns : public Gtk::TreeModel::ColumnRecord
		{
		public:
			PicTypeColumns(){ add(colName); }
			
			Gtk::TreeModelColumn<Glib::ustring> colName;
		};

		PicTypeColumns picTypeColumns;		  
		Glib::RefPtr<Gtk::ListStore> picTypeComboStore;
		
		
		PicType *picType;
		ReadHexFile *readHexFile;
		Hardware *hardware;

};
#endif
