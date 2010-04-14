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

#ifndef IO_TEST_H
#define IO_TEST_H

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


/**
    Dialog window which allows the user to set voltages of the UPP hardware.
*/
class IOTestDialog : public IOTestDialogBase
{
public:
	IOTestDialog(Hardware* hardware, wxWindow* parent);
	~IOTestDialog();

private:
	Hardware* m_hardware;
	wxTimer* m_timer;

protected:
	void on_vpp( wxCommandEvent& event ){ upp_vpp(); EVENT_FIX }
	void on_vdd( wxCommandEvent& event ){ upp_vdd(); EVENT_FIX }
	void on_pgd( wxCommandEvent& event ){ upp_pgd(); EVENT_FIX }
	void on_pgc( wxCommandEvent& event ){ upp_pgc(); EVENT_FIX }
	void on_btn_close( wxCommandEvent& event ){ upp_btn_close(); EVENT_FIX }
	void on_timer(wxTimerEvent& event){upp_update_hw_status(); EVENT_FIX }

	void upp_vpp();
	void upp_vdd();
	void upp_pgd();
	void upp_pgc();
	void upp_btn_close();
	void upp_update_hw_status();
};

#endif // IO_TEST_H
