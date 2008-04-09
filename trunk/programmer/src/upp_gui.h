/***************************************************************************
 *   Copyright (C) 2008 by Frans Schreuder   *
 *   usbpicprog.sourceforge.net   *
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

#ifndef UPP_GUI_H
#define UPP_GUI_H


#include <vector>
#include <string>
#include "pictype.h"
#include "read_hexfile.h"

#include <gtkmm.h>

using namespace std;
using namespace Gtk;

class UppGui: public Window
{
	public:
		UppGui();
		virtual ~UppGui();
		
	private:
		Gtk::Frame m_Frame;
		
};

#endif //UPP_GUI_H
