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

#ifndef INCLUDE_ICONS_H
#define INCLUDE_ICONS_H

/*These icons are placed on the icon toolbar, usbpicprog.xpm is the main program icon.*/

extern const char* refresh_xpm[];
extern const char* blankcheck_xpm[];
extern const char* program_xpm[];
extern const char* erase_xpm[];
extern const char* read_xpm[];
extern const char* verify_xpm[];
extern const char* usbpicprog_xpm[];


// IDs for the toolbuttons in the toolbar
enum
{
    wxID_PROGRAM = wxID_HIGHEST+1,
    wxID_READ,
    wxID_VERIFY,
    wxID_ERASE,
    wxID_BLANKCHECK,
    wxID_AUTODETECT,
    wxID_CONNECT,
    wxID_CONNECT_BOOT,
    wxID_DISCONNECT
};

#endif //INCLUDE_ICONS_H
