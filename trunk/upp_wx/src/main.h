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

#ifndef MAIN_H
#define MAIN_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <wx/cmdline.h>

#include <iostream>
#include <cstdlib>

#include "uppmainwindow.h"
#include "hardware.h"
#include "pictype.h"
#include "hexfile.h"

/**
    Implementation of main wxWidgets application.
    
    In the OnCmdLineParsed() function, either the GUI or the command line app
    is created.
*/
class UsbPicProg : public wxApp
{
public:

    /**
        This is the wxWidgets initialization function, but we only use it to call
        wxWidgets our own OnInit(), because OnInitCmdLine () is being used.
    */
    virtual bool OnInit();

    /**
        Called by WxWidgets to clean up some stuff when the program exits.
    */
    virtual int OnExit();

    /**
        Initialization function if command line is being used.
    */
    virtual void OnInitCmdLine(wxCmdLineParser& parser);

    /**
        After command line is being processed, this function is being called
        by wxWidgets, even if no arguments are given. This is the actual function
        in which the real application initializes.
    */
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

    /**
        Open the given file (mac specific).
    */
    virtual void MacOpenFile(const wxString &fileName);

private:        // stuff common to both GUI and command-line modes

    /** The main() of the command line application */
    bool CmdLineMain(wxCmdLineParser& parser);

    /** Did we start as a console-only app? */
    bool m_console;

    /** Application locale */
    wxLocale* m_locale;
};

DECLARE_APP(UsbPicProg)


/* Command line parameters and help text */
static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
    { wxCMD_LINE_SWITCH, "h", _("help"),       _("displays help on the command line parameters"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_SWITCH, "V", _("version"),    _("displays version information of usbpicprog") },
    { wxCMD_LINE_OPTION, "p", _("pictype"),    _("specify the pic type (eg -p=P18F2550)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "s", _("silent"),     _("do not display the hex file") },
    { wxCMD_LINE_SWITCH, "w", _("write"),      _("write the device") },
    { wxCMD_LINE_SWITCH, "r", _("read"),       _("read the device") },
    { wxCMD_LINE_SWITCH, "v", _("verify"),     _("verify the device") },
    { wxCMD_LINE_SWITCH, "e", _("erase"),      _("bulk erase the device") },
    { wxCMD_LINE_SWITCH, "b", _("blankcheck"), _("blankcheck the device") },
	{ wxCMD_LINE_SWITCH, "d", _("debug"),       _("debug the hardware") },
    { wxCMD_LINE_OPTION, "f", _("file"),       _("hexfile"),wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "RO"	, _("restore-osccal"), _("Restore Osccal Register"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "RB"	, _("restore-bandgap"), _("Restore BandGap Register"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	
    { wxCMD_LINE_NONE }
};

#endif //MAIN_H
