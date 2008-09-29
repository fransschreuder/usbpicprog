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
