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

/*These icons are placed on the icon toolbar, usbpicprog.xpm is the main program
icon.*/

#if defined(__WXGTK__) || defined(__WXMOTIF__) /*GTK needs bigger icons than Windows*/
#include "../icons/refresh.xpm"
#include "../icons/blankcheck.xpm"
#include "../icons/program.xpm"
#include "../icons/erase.xpm"
#include "../icons/read.xpm"
#include "../icons/verify.xpm"
#include "../icons/usbpicprog.xpm"
#else   /*Icons for Windows and Mac*/
#include "../icons/win/refresh.xpm"
#include "../icons/win/blankcheck.xpm"
#include "../icons/win/program.xpm"
#include "../icons/win/erase.xpm"
#include "../icons/win/read.xpm"
#include "../icons/win/verify.xpm"
#include "../icons/win/usbpicprog.xpm"
#endif
#endif //INCLUDE_ICONS_H
