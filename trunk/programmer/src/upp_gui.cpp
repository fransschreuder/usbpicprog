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

#include <gtkmm.h>
#include "upp_gui.h"
#include <iostream>
#include <cstdlib>

using namespace Gtk;
using namespace std;
UppGui::UppGui()
{
	/* Set some window properties */
	set_title("Usbpicprog 0.1");
	set_size_request(800, 600);

	/* Sets the border width of the window. */
	set_border_width(2);

	add(m_Frame);

	/* Set the frames label */
	m_Frame.set_label("Hex data");

	/* Align the label at the right of the frame */
  //m_Frame.set_label_align(Gtk::ALIGN_RIGHT, Gtk::ALIGN_TOP);

	/* Set the style of the frame */
	m_Frame.set_shadow_type(Gtk::SHADOW_IN);

	show_all_children();

}

UppGui::~UppGui()
{
}
