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

// NOTE: to avoid lots of warnings with MSVC 2008 about deprecated CRT functions
//       it's important to include wx/defs.h before STL headers
#include <wx/defs.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>
#include <wx/apptrait.h>

#include "main.h"
#include "../svn_revision.h"

// NOW we can include the <usb.h> header without compiling problems
#include <libusb.h>

#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // useful to catch memory leaks when compiling under MSVC 
    #include <windows.h>
#endif

#ifndef __WXMSW__
extern const char* libusb_strerror(enum libusb_error errcode);
#endif

using namespace std;
IMPLEMENT_APP(UsbPicProg)


// ----------------------------------------------------------------------------
// UsbPicProg
// ----------------------------------------------------------------------------

void UsbPicProg::MacOpenFile(const wxString &fileName)
{
    static_cast<UppMainWindow*>(GetTopWindow())->upp_open_file(fileName);
}

bool UsbPicProg::OnInit()
{
#ifdef __WXMSW__
    wxLocale::AddCatalogLookupPathPrefix(_T("po"));
#endif
#ifdef __WXMAC__
	wxLocale::AddCatalogLookupPathPrefix(((wxStandardPaths &)wxStandardPaths::Get()).GetExecutablePath() + _T("/po"));
    //wxLocale::AddCatalogLookupPathPrefix(wxString(wxApp::argv[0]).BeforeLast('/') + _T("/po"));
#endif


//Bosnian language not defined (at least not in wxWidgets 2.9.0), added this ticket for it: http://trac.wxwidgets.org/ticket/12016
//Hopefully, Bosnian will be added in 2.9.1, in that case the following lines can be removed.
#if wxCHECK_VERSION(2, 9, 1)
#else // replacement code for old version
#define wxLANGUAGE_BOSNIAN (wxLANGUAGE_USER_DEFINED+1)
	struct 
#ifdef __WIN32__
	WXDLLEXPORT
#endif
	wxLanguageInfo Bosnian=
	{
		wxLANGUAGE_BOSNIAN,                   // wxLanguage id
		"bs",         // Canonical name, e.g. fr_FR
#ifdef __WIN32__
		LANG_BOSNIAN, SUBLANG_BOSNIAN_BOSNIA_HERZEGOVINA_LATIN,
		                                // (LANG_xxxx, SUBLANG_xxxx)
#endif
		"Bosnian"           // human-readable name of the language
	};
	wxLocale::AddLanguage(Bosnian);
#endif //wxCHECK_VERSION(2,9,1)

    // init the locale
    m_locale = new wxLocale(wxLANGUAGE_DEFAULT);

    m_locale->AddCatalog("usbpicprog");

    // init the PNG handler
    wxImage::AddHandler( new wxPNGHandler );

    // we're not interested to timestamps; they only take away space in the log dialog
    wxLog::DisableTimestamp();

#if !defined(__WXMSW__) && !defined(__WXMAC__)
    ((wxStandardPaths&)wxStandardPaths::Get()).SetInstallPrefix(PACKAGE_PREFIX);
#endif
#if defined(__WXMSW__) && !defined(_WIN64)
    if (wxIsPlatform64Bit())
    {
        wxLogError(_("This is a 32bit application running under a 64bit Window version. UsbPicProg won't be able to connect to the hardware!\nPlease uninstall this program and download the UsbPicProg installer for 64bit operating systems."));
        delete m_locale;  // OnExit() won't be called if we fail inside OnInit()
        return false;
    }
#endif
#if defined(__WXMSW__)
    if (wxGetOsVersion() != wxOS_WINDOWS_NT)    // wxOS_WINDOWS_NT includes XP, Vista, 7
    {
        wxLogError(_("Unsupported Windows version. UsbPicProg runs only on Windows XP or later."));
        delete m_locale;  // OnExit() won't be called if we fail inside OnInit()
        return false;
    }
#endif

    // init the supported PIC types
    if (!PicType::Init())
    {
        wxLogError(_("Cannot load XML PIC data."));
        delete m_locale;  // OnExit() won't be called if we fail inside OnInit()
        return false;
    }
	
    // init libusb library
    // IMPORTANT: this should be done _before_ OnCmdLineParsed() (and possibly CmdLineMain())
    //            is called!
    int err;
    if ((err=libusb_init(NULL)) != LIBUSB_SUCCESS)
    {
        
#ifdef __WXMSW__
        if (err == LIBUSB_ERROR_OTHER)
            wxLogError(_("Could not initialize libusb. This may happen because of:\n1) an incorrect installation of the UsbPicProg driver; check if this is the case running the Windows Device Manager with the programmer attached;\n2) An incorrect installation of cfgmgr32.dll, winusb.dll or hid.dll;\n\nPlease re-install UsbPicProg and ensure that Windows recognizes the UsbPicProg hardware when it's attached to the USB bus. If Windows asks for a driver please point it to the 'UsbPicProg\\driver' directory which has been installed in your Programs folder. If you still have problems please drop a mail on the UsbPicProg mailing list including your 'DPINST.LOG' file (which is located in your 'Windows\\System32' folder)."));
        else    // avoid multiple wxLogError() calls to avoid the message gets ellipsized
#endif
        wxLogError(_("Could not initialize libusb: %s"), libusb_strerror((libusb_error)err));

        delete m_locale;  // OnExit() won't be called if we fail inside OnInit()
        return false;
    }

    // wxApp::OnInit() will call UsbPicProg::OnInitCmdLine() and UsbPicProg::OnCmdLineParsed()
    if (!wxApp::OnInit())
    {
        delete m_locale;  // OnExit() won't be called if we fail inside OnInit()
        return false;
    }   

    SetVendorName(("UsbPicProgrammer"));

    return true;        // wxWidgets will now call OnRun()
}

int UsbPicProg::OnRun()
{
    if (!m_console)
    {
        // start a GUI app
        UppMainWindow *uppMainWindow = new UppMainWindow(m_hardware, (wxFrame *)NULL, wxID_ANY);
        SetTopWindow(uppMainWindow);

        uppMainWindow->Show(true);

        // let wxWidgets start the main loop event handler and proceed in GUI mode:
        return wxApp::OnRun();
    }
    else
    {
        // the console app was started in OnCmdLineParsed(); here we only need to
        // return the code computed there
        return m_nRetCode;
    }
}

int UsbPicProg::OnExit()
{
    // cleanup everything was initialized by OnInit()
    delete m_locale;
    PicType::CleanUp();
    libusb_exit(NULL);

    return wxApp::OnExit();
}

void UsbPicProg::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetDesc (g_cmdLineDesc);
    parser.SetSwitchChars (("-"));
}

bool UsbPicProg::OnCmdLineParsed(wxCmdLineParser& parser)
{
    wxString tmp;
    if (!parser.Found(("h")) &&
        !parser.Found(("V")) &&
        !parser.Found(("p"), &tmp) &&
        !parser.Found(("s")) &&
        !parser.Found(("w")) &&
        !parser.Found(("r")) &&
        !parser.Found(("v")) &&
        !parser.Found(("e")) &&
        !parser.Found(("b")) &&
        !parser.Found(("f"), &tmp)&&
        !parser.Found(("d"))&&
	    !parser.Found(("RO"), &tmp)&&
	    !parser.Found(("RB"), &tmp)&&
	    !parser.Found(("RT"))&&
	    !parser.Found(("ST")))
    {
        // no command line arguments (except maybe the filename with option "-f")
        // were given: open the main window
        m_console = false;
    }
    else    // start a command line app
    {
        m_console = true;

        // NOTE: the console app is started from here and not from OnInit()
        //       (which would be a more natural place!) because here we have
        //       a reference to the wxCmdLineParser instance:
        CmdLineMain(parser);
    }

    return true;
}

void UsbPicProg::CmdLineMain(wxCmdLineParser& parser)
{
    bool silent_mode = false;
    HexFile* hexFile = NULL;
    PicType* picType = NULL;
    wxString output;
    wxString filename;
	VerifyResult res;

#ifdef __WXMSW__
    // when using Windows, wxWidgets takes over the terminal but we want to have it for cout and cerr
    if(AttachConsole(ATTACH_PARENT_PROCESS))
    {
        freopen( "CON", "w", stdout );
        freopen( "CON", "w", stderr );
    }
#endif

    if(parser.Found(("V")))
    {
#ifndef UPP_VERSION
        cerr<<"usbpicprog (SVN) "<<(SVN_REVISION)<<endl;
#else
        cerr<<"usbpicprog "<<(UPP_VERSION)<<endl;
#endif
        m_nRetCode = 0;
        return;
    }
	
    // before proceeding, test if the Hardware class is able to connect to
    // the hardware interface:
    m_hardware.connect(NULL, HW_UPP);
	if(!m_hardware.connected())	//no usbpicprog found? try the bootloader
		m_hardware.connect(NULL, HW_BOOTLOADER); 
    if(!m_hardware.connected()) //no bootloader found either...
    {
        cerr<<"Usbpicprog not found"<<endl;
        m_nRetCode = -1;
        return;
    }

    // command line option -s or --silent passed?
    silent_mode = parser.Found(("s"));
    
    // Run Target Device
    if(parser.Found(("RT")))
    {
        cout<<"Running target device"<<endl;
        if(!m_hardware.runTarget())cout<<"Unable to run target device"<<endl;
        m_hardware.disconnect();
        return;
    }
	
	// Stop Target Device
    if(parser.Found(("ST")))
    {
        cout<<"Stopping target device"<<endl;
        if(!m_hardware.stopTarget())cout<<"Unable to stop target device"<<endl;
        m_hardware.disconnect();
        return;
    }
	
    // check if -p <str> was given, else autodetect the device
    wxString picTypeStr;
    if(parser.Found(("p"),&picTypeStr))
	{
        picType=new PicType(PicType::FindPIC(picTypeStr));
		m_hardware.setPicType(picType);
	}
    else
    {
        int devId=m_hardware.autoDetectDevice();
        picType=new PicType(PicType::FindPIC(devId));
        m_hardware.setPicType(picType);
        if(devId>0)cout<<"Detected: ";
        else cerr<<"Detection failed, setting picType to default: ";
        cout<<(string)picType->Name<<endl;
    }

	if(parser.Found("d"))
	{
		m_hardware.debug();
		m_nRetCode = 0;
        return;
	}

	// if -RO and -RB are both passed, restore those registers to P12F629 devices
	wxString BandGap, OscCal;	
	if((parser.Found(("RB"), &BandGap)
	    ||(picType->picFamily==P12F508)
	    ||(picType->picFamily==P10F200)
	    ||(picType->picFamily==P10F202))&&parser.Found(("RO"), &OscCal))
	{
		int iSelectedOscCal;
		sscanf(OscCal.c_str(),"%4X",&iSelectedOscCal);
		int minOscCal, maxOscCal;
		if(picType->picFamily==P10F200||picType->picFamily==P10F202)
		{
			minOscCal = 0xC00;
			maxOscCal = 0xCFF;
		}
		else
		{
			minOscCal = 0x3400;
			maxOscCal = 0x37FF;
		}
		if((iSelectedOscCal<minOscCal)|(iSelectedOscCal>maxOscCal))
    	{
			cout<<wxString(_("Please specify an Oscal Value between ")+wxString::Format("%X",minOscCal)+_(" and ")+ wxString::Format("%X",maxOscCal)).mb_str(wxConvUTF8)<<endl;
			exit ( -1 );
		}
		int iSelectedBandGap=0;
		if(picType->picFamily==P12F629)
		{
			sscanf(BandGap.c_str(),"%4X",&iSelectedBandGap);
			if((iSelectedBandGap<0x0)|(iSelectedBandGap>0x3))
    		{
				cout<<_("Please specify a BandGap Value between 0 and 3").mb_str(wxConvUTF8)<<endl;
				exit ( -1 );
			}
		}
		if (m_hardware.bulkErase(picType,false)<0)
            cout<<_("Error erasing the device").mb_str(wxConvUTF8)<<endl;
		if (m_hardware.restoreOscCalBandGap(picType, iSelectedOscCal, iSelectedBandGap)<0)
            cout<<_("Error restoring Calibration Registers").mb_str(wxConvUTF8)<<endl;
	}
	
	if(picType->picFamily==P12F629&&
		((parser.Found(("RB"), &BandGap)&&(!parser.Found(("RO"), &OscCal)))||
	    (!(parser.Found(("RB"), &BandGap))&&(parser.Found(("RO"), &OscCal)))))
    {
		cout<<"-RB must be passed together with RO"<<endl;
		m_nRetCode = -1;
        return;
	}

    // if -e is passed, bulk erase the entire pic
    if(parser.Found(("e")))
    {
        cout<<"Bulk erase..."<<endl;
        if(m_hardware.bulkErase(picType, true)<0)cerr<<"Error during erase"<<endl;
    }

    // if -b is passed, check if the device is blank
    if(parser.Found(("b")))
    {
        cout<<"Blankcheck..."<<endl;
        wxString typeText,tempStr;
        VerifyResult res=m_hardware.blankCheck(picType);
        switch(res.Result)
        {
            case VERIFY_SUCCESS:
                cout<<"Device is blank"<<endl;
                break;
            case VERIFY_MISMATCH:
                switch (res.DataType)
                {
                    case TYPE_CODE: typeText="code";break;
                    case TYPE_DATA: typeText="data";break;
                    case TYPE_CONFIG: typeText="config";break;
                    default: typeText="unknown";break;

                }
				tempStr.Printf("Blankcheck %s failed at 0x%X. Read: 0x%02X, Expected: 0x%02X",
                        typeText.c_str(),
                        res.Address+((res.DataType==TYPE_CONFIG)+picType->ConfigAddress),
                        res.Read,
                        res.Expected);

                cerr<<(string)tempStr<<endl;
                break;
            case VERIFY_USB_ERROR:
                cerr<<"USB error during blankcheck"<<endl;
                break;
            case VERIFY_OTHER_ERROR:
                cerr<<"Unknown error during blankcheck"<<endl;
                break;
            default:
                cerr<<"I'm sorry for being stupid"<<endl;
                break;
        }
    }
	res.Result=VERIFY_SUCCESS;
    // if -w is passed, open the hexFile and write it to the pic
    if(parser.Found(("w")))
    {
        cout<<"Write..."<<endl;
        if(!parser.Found(("f"),&filename)){cerr<<"Please specify a filename"<<endl;}
        else
        {
            hexFile=new HexFile();
            if(!hexFile->open(picType,filename.mb_str(wxConvUTF8)))
                cerr<<"Unable to open file"<<endl;
            if(!silent_mode)
            {
                hexFile->print(&output,picType);
                cout<<(string)output<<endl;
            }
			if((picType->picFamily==P12F629)||(picType->picFamily==P12F508))
			{
				m_hardware.backupOscCalBandGap(picType);
				hexFile->putOscCalBandGap (picType);
			}
            if(m_hardware.write(TYPE_CODE,hexFile,picType)<0)cerr<<"Error writing Code"<<endl;
				
            if(m_hardware.write(TYPE_DATA,hexFile,picType)<0)cerr<<"Error writing Data"<<endl;
			if(parser.Found(("v")))
			{
				res=m_hardware.verify(hexFile,picType, true, false, true); //verify code and data before programming config memory, due to possible code protection
			}
            if(m_hardware.write(TYPE_CONFIG,hexFile,picType)<0)cerr<<"Error writing Config"<<endl;
            delete hexFile;
        }
    }

    // if -r is passed, read it to the pic and save it to the hexFile
    if(parser.Found(("r")))
    {
        cout<<"Read..."<<endl;
        if(!parser.Found(("f"),&filename)){cerr<<"Please specify a filename"<<endl;}
        else
        {
            hexFile=new HexFile();
            if(m_hardware.read(TYPE_CODE,hexFile,picType,picType->CodeSize)<0)cerr<<"Error reading Code"<<endl;
            if(m_hardware.read(TYPE_DATA,hexFile,picType,picType->DataSize)<0)cerr<<"Error reading Data"<<endl;
            if(m_hardware.read(TYPE_CONFIG,hexFile,picType,picType->ConfigSize)<0)cerr<<"Error reading Config"<<endl;
            hexFile->trimData(picType);
            if(!hexFile->saveAs(picType,filename.mb_str(wxConvUTF8)))
                cerr<<"Unable to save file"<<endl;
            if(!silent_mode)
            {
                hexFile->print(&output,picType);
                cout<<(string)output<<endl;
            }
            delete hexFile;
        }
    }

    // if -v is passed, open the hexFile, read it and compare the results
    if(parser.Found(("v")))
    {
        cout<<"Verify..."<<endl;
        if(!parser.Found(("f"),&filename)){cerr<<"Please specify a filename"<<endl;}
        else
        {
            hexFile=new HexFile();
            if(!hexFile->open(picType,filename.mb_str(wxConvUTF8)))
                cerr<<"Unable to open file"<<endl;
            if(!silent_mode)
            {
                hexFile->print(&output,picType);
                cout<<(string)output<<endl;
            }
            wxString typeText,tempStr;
			if(res.Result==VERIFY_SUCCESS)
			{
	            res=m_hardware.verify(hexFile,picType, !parser.Found("w"), true, !parser.Found("w")); //only verify code and data here if not programmed here. Else this has to be done before config due to possible code protection
			}
            switch(res.Result)
            {
                case VERIFY_SUCCESS:
                    cout<<"Verify successful"<<endl;
                    break;
                case VERIFY_MISMATCH:
                    switch (res.DataType)
                    {
                        case TYPE_CODE: typeText="code";break;
                        case TYPE_DATA: typeText="data";break;
                        case TYPE_CONFIG: typeText="config";break;
                        default: typeText="unknown";break;
                    }
                    tempStr.Printf("Verify %s failed at 0x%X. Read: 0x%02X, Expected: 0x%02X",
                            typeText.c_str(),
                            res.Address+((res.DataType==TYPE_CONFIG)*picType->ConfigAddress),
                            res.Read,
                            res.Expected);
                    cerr<<(string)tempStr<<endl;						
			

                    break;
                case VERIFY_USB_ERROR:
                    cerr<<"USB error during verify"<<endl;
                    break;
                case VERIFY_OTHER_ERROR:
                    cerr<<"Unknown error during verify"<<endl;
                    break;
                default:
                    cerr<<"I'm sorry for being stupid"<<endl;
                    break;
            }
            delete hexFile;
        }
    }
    m_hardware.disconnect();

    m_nRetCode = 0;
}
