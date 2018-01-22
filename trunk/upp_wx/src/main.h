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
	{ wxCMD_LINE_SWITCH, "h", wxT_2("help"),        wxT_2("displays help on the command line parameters"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
	{ wxCMD_LINE_SWITCH, "V", wxT_2("version"),     wxT_2("displays version information of usbpicprog") },
	{ wxCMD_LINE_OPTION, "p", wxT_2("pictype"),     wxT_2("specify the pic type (case insensitive) (eg -p=18F2550)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "s", wxT_2("silent"),      wxT_2("do not display the hex file") },
	{ wxCMD_LINE_SWITCH, "w", wxT_2("write"),       wxT_2("write the device") },
	{ wxCMD_LINE_SWITCH, "r", wxT_2("read"),        wxT_2("read the device") },
	{ wxCMD_LINE_SWITCH, "v", wxT_2("verify"),      wxT_2("verify the device") },
	{ wxCMD_LINE_SWITCH, "e", wxT_2("erase"),       wxT_2("bulk erase the device") },
	{ wxCMD_LINE_SWITCH, "b", wxT_2("blankcheck"),  wxT_2("blankcheck the device") },
	{ wxCMD_LINE_SWITCH, "d", wxT_2("debug"),       wxT_2("debug the hardware") },
	{ wxCMD_LINE_SWITCH, "RT", wxT_2("run"),        wxT_2("run target device") },
	{ wxCMD_LINE_SWITCH, "ST", wxT_2("stop"),       wxT_2("stop target device") },
	{ wxCMD_LINE_SWITCH, "RST", wxT_2("reset"),     wxT_2("reset/reboot firmware/bootloader") },
	{ wxCMD_LINE_SWITCH, "BOOT", wxT_2("boot"),     wxT_2("tell firmware to reboot in bootloader mode") },
	{ wxCMD_LINE_OPTION, "f", wxT_2("file"),        wxT_2("hexfile"),wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "RO",		    wxT_2("restore-osccal"), wxT_2("Restore Osccal Register"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_OPTION, "RB",	 	    wxT_2("restore-bandgap"), wxT_2("Restore BandGap Register"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_SWITCH, "VDDDIS", wxT_2("disable-vdd"),     wxT_2("Device is self powered, disable VDD") },
	{ wxCMD_LINE_SWITCH, "LIMVPP", wxT_2("limit-vpp"),     wxT_2("Limit VPP to 5V") },
	{ wxCMD_LINE_SWITCH, "LIMPGDPGC", wxT_2("limit-pgd-pgc"),     wxT_2("Limit PGD and PGC to 3.3V") },

	{ wxCMD_LINE_NONE }
};

#endif //MAIN_H
