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
        Called by wxWidgets to initialize the application: here we initialize
        libusb, the locale, the PIC database and a few other details.

        Note that this function calls OnCmdLineParsed(); see OnCmdLineParsed()
        docs for more info about the console mode.

        If this function is successful, wxWidgets calls OnRun().
    */
    virtual bool OnInit();

    /**
        Called by wxWidgets to clean up the stuff initialized by OnInit(): 
        libusb, locale, etc.
    */
    virtual int OnExit();

    /**
        Initialization function for the command-line parser.
        Called by wxApp::OnInit().
    */
    virtual void OnInitCmdLine(wxCmdLineParser& parser);

    /**
        After processing of the command line (i.e. the creation of a wxCmdLineParser object), 
        this function is called by wxWidgets, even if no arguments are given. 

        Note that if the user provided command-line options, this function will call
        our CmdLineMain() private function which is the main() of the console mode.
        After CmdLineMain() execution the return code is saved and execution continues.
    */
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
    
    /**
        Runs the application in GUI mode if #m_console is false.

        If the application did run in console mode during OnCmdLineParsed() execution, then
        this function just returns the #m_nRetCode value.
    */
    virtual int OnRun();

    /**
        Open the given file (mac specific).
    */
    virtual void MacOpenFile(const wxString &fileName);

private:        // only for the command-line mode

    /** 
        The main() of the command line application. This function is called by OnCmdLineParsed()
        if command-line options were passed.

        It stores the return code in #m_nRetCode.
    */
    void CmdLineMain(wxCmdLineParser& parser);

    /** Return code of the CmdLineMain() function. */
    int m_nRetCode;

private:        // stuff common to both GUI and command-line modes

    /** Did we start as a console-only app? */
    bool m_console;

    /** Application locale */
    wxLocale* m_locale;
    
    /** 
        Instance of the class which connects the hardware and manages the 
        communication between this program and the real hardware through libusb
    */
    Hardware m_hardware;
    
};

DECLARE_APP(UsbPicProg)


/* Command line parameters and help text */
static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
	{ wxCMD_LINE_SWITCH, "h", _("help"),        _("displays help on the command line parameters"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
	{ wxCMD_LINE_SWITCH, "V", _("version"),     _("displays version information of usbpicprog") },
	{ wxCMD_LINE_OPTION, "p", _("pictype"),     _("specify the pic type (eg -p=18F2550)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "s", _("silent"),      _("do not display the hex file") },
	{ wxCMD_LINE_SWITCH, "w", _("write"),       _("write the device") },
	{ wxCMD_LINE_SWITCH, "r", _("read"),        _("read the device") },
	{ wxCMD_LINE_SWITCH, "v", _("verify"),      _("verify the device") },
	{ wxCMD_LINE_SWITCH, "e", _("erase"),       _("bulk erase the device") },
	{ wxCMD_LINE_SWITCH, "b", _("blankcheck"),  _("blankcheck the device") },
	{ wxCMD_LINE_SWITCH, "d", _("debug"),       _("debug the hardware") },
	{ wxCMD_LINE_SWITCH, "RT", _("run"),        _("run target device") },
	{ wxCMD_LINE_SWITCH, "ST", _("stop"),       _("stop target device") },
	{ wxCMD_LINE_SWITCH, "RST", _("reset"),     _("reset/reboot firmware/bootloader") },
	{ wxCMD_LINE_SWITCH, "BOOT", _("boot"),     _("tell firmware to reboot in bootloader mode") },
	{ wxCMD_LINE_OPTION, "f", _("file"),        _("hexfile"),wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "RO",		    _("restore-osccal"), _("Restore Osccal Register"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "RB",	 	    _("restore-bandgap"), _("Restore BandGap Register"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "VDDDIS", _("disable-vdd"),     _("Device is self powered, disable VDD") },
	{ wxCMD_LINE_SWITCH, "LIMVPP", _("limit-vpp"),     _("Limit VPP to 5V") },
	{ wxCMD_LINE_SWITCH, "LIMPGDPGC", _("limit-pgd-pgc"),     _("Limit PGD and PGC to 3.3V") },

	{ wxCMD_LINE_NONE }
};

#endif //MAIN_H
