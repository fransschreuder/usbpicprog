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

#include "main.h"
#include "../svn_revision.h"

using namespace std;
IMPLEMENT_APP(UsbPicProg)

#ifdef __WXMSW__
#include <windows.h>
#endif


// ----------------------------------------------------------------------------
// UsbPicProg
// ----------------------------------------------------------------------------

/*Mac-specific open file function*/
void UsbPicProg::MacOpenFile(const wxString &fileName)
{
    static_cast<UppMainWindow*>(GetTopWindow())->upp_open_file(fileName);
}

/*This is the wxWidgets initialization function, but we only use it to call
*wxWidgets own OnInit(), because OnInitCmdLine () is being used*/
bool UsbPicProg::OnInit()
{
#ifdef __WXMSW__
    wxLocale::AddCatalogLookupPathPrefix(_T("po"));
#endif
#ifdef __WXMAC__
	wxLocale::AddCatalogLookupPathPrefix(((wxStandardPaths &)wxStandardPaths::Get()).GetExecutablePath() + _T("/po"));
    //wxLocale::AddCatalogLookupPathPrefix(wxString(wxApp::argv[0]).BeforeLast('/') + _T("/po"));
#endif
    // init the locale
    m_locale = new wxLocale(wxLANGUAGE_DEFAULT);
#if wxCHECK_VERSION(2,8,10)			
    m_locale->AddCatalog(("usbpicprog"));
#else	
	m_locale->AddCatalog(wxT("usbpicprog"));
#endif
    // init the PNG handler
    wxImage::AddHandler( new wxPNGHandler );

    // we're not interested to timestamps; they only take away space in the log dialog
#if wxCHECK_VERSION(2,9,0)		
    wxLog::DisableTimestamp();
#endif

#if !defined(__WXMSW__) && !defined(__WXMAC__)
	#if wxCHECK_VERSION(2,8,10)				
		((wxStandardPaths&)wxStandardPaths::Get()).SetInstallPrefix(PACKAGE_PREFIX);
	#else
		((wxStandardPaths&)wxStandardPaths::Get()).SetInstallPrefix(wxT(PACKAGE_PREFIX));
	#endif	
#endif
    // init the supported PIC types
    if (!PicType::Init())
    {
        wxLogError(_("Cannot load XML PIC data."));
        delete m_locale;  // OnExit() won't be called if we fail inside OnInit()
        return false;
    }
	
    // wxApp::OnInit() will call UsbPicProg::OnInitCmdLine and UsbPicProg::OnCmdLineParsed
    if (!wxApp::OnInit())
    {
        delete m_locale;  // OnExit() won't be called if we fail inside OnInit()
        return false;
    }
#if wxCHECK_VERSION(2,8,10)	
    SetVendorName(("UsbPicProgrammer"));
#else	
	SetVendorName(wxT("UsbPicProgrammer"));
#endif
    if (!m_console)
    {
        // start a GUI app
        UppMainWindow *uppMainWindow = new UppMainWindow((wxFrame *)NULL, wxID_ANY);
        SetTopWindow(uppMainWindow);

        uppMainWindow->Show(true);
    }
    //else: the console app was started in OnCmdLineParsed() where we have a
    //      reference to the command line parser

    return true;
}

/*Called by WxWidgets to clean up some stuff when the program exits*/
int UsbPicProg::OnExit()
{
    delete m_locale;
    
    PicType::CleanUp();

    return wxApp::OnExit();
}

/*Initialization function if command line is being used*/
void UsbPicProg::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetDesc (g_cmdLineDesc);
#if wxCHECK_VERSION(2,8,10)	
    parser.SetSwitchChars (("-"));
#else
	parser.SetSwitchChars (wxT("-"));
#endif	
}

/*After command line is being processed, this function is being called
by wxWidgets, even if no arguments are given. */
bool UsbPicProg::OnCmdLineParsed(wxCmdLineParser& parser)
{
    /*If no command line arguments are passed, we open the main window  *
     *Else, a command line application is started.						*
     *Only the filename may be passed to the gui						*/
    wxString tmp;
#if wxCHECK_VERSION(2,8,10)		
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
        !parser.Found(("d")))
#else
    if (!parser.Found(wxT("h")) &&
        !parser.Found(wxT("V")) &&
        !parser.Found(wxT("p"), &tmp) &&
        !parser.Found(wxT("s")) &&
        !parser.Found(wxT("w")) &&
        !parser.Found(wxT("r")) &&
        !parser.Found(wxT("v")) &&
        !parser.Found(wxT("e")) &&
        !parser.Found(wxT("b")) &&
        !parser.Found(wxT("f"), &tmp)&&
        !parser.Found(wxT("d")))
#endif
    {
        m_console = false;
    }
    else    //start a command line app
    {
        m_console = true;

        return CmdLineMain(parser);
            // CmdLineMain() calls exit() itself
    }

    return true;
}

bool UsbPicProg::CmdLineMain(wxCmdLineParser& parser)
{
    bool silent_mode = false;
    HexFile* hexFile = NULL;
    PicType* picType = NULL;
    Hardware* hardware = NULL;

    wxString output;
    wxString filename;
    /*when using Windows, wxWidgets takes over the terminal, *
    *but we want to have it for cout and cerr                */
#ifdef __WXMSW__
    if( AttachConsole((DWORD)-1) )
    {
        freopen( "CON", "w", stdout );
        freopen( "CON", "w", stderr );
    }
#endif

#if wxCHECK_VERSION(2,8,10)		
    if(parser.Found(("V")))
#else
	if(parser.Found(wxT("V")))	
#endif		
    {
#ifndef UPP_VERSION
        cerr<<"usbpicprog (SVN) "<<(SVN_REVISION)<<endl;
#else
        cerr<<"usbpicprog "<<(UPP_VERSION)<<endl;
#endif
        return EXIT_SUCCESS;
    }
	
    //command line -s or --silent passed?
#if wxCHECK_VERSION(2,8,10)		
    silent_mode = parser.Found(("s"));
#else
	silent_mode = parser.Found(wxT("s"));
#endif	
    hardware = new Hardware();
    if(!hardware->connected())
    {
        cerr<<"Usbpicprog not found"<<endl;
        exit(-1);
    }
    wxString picTypeStr;


	
    /* check if -p <str> is passed, else autodetect the device
    */
#if wxCHECK_VERSION(2,8,10)		
    if(parser.Found(("p"),&picTypeStr))
#else		
	if(parser.Found(wxT("p"),&picTypeStr))		
#endif		
	{
        picType=new PicType(PicType::FindPIC(picTypeStr));
		hardware->setPicType(picType);
	}
    else
    {
        int devId=hardware->autoDetectDevice();
        picType=new PicType(PicType::FindPIC(devId));
        hardware->setPicType(picType);
        if(devId>0)cout<<"Detected: ";
        else cerr<<"Detection failed, setting picType to default: ";
#if wxCHECK_VERSION(2,8,10)	
        cout<<(string)picType->Name<<endl;
#else
		cout<<(string)picType->Name.mb_str(wxConvUTF8)<<endl;
#endif
    }

#if wxCHECK_VERSION(2,8,10)		
	if(parser.Found("d"))
#else
	if(parser.Found(wxT("d")))		
#endif		
	{
		hardware->debug();
		exit( 0 );
	}

	
    /* if -e is passed, bulk erase the entire pic*/
#if wxCHECK_VERSION(2,8,10)	
    if(parser.Found(("e")))
#else
	if(parser.Found(wxT("e")))		
#endif		
    {
        cout<<"Bulk erase..."<<endl;
        if(hardware->bulkErase(picType)<0)cerr<<"Error during erase"<<endl;
    }

    /* if -b is passed, check if the device is blank*/
#if wxCHECK_VERSION(2,8,10)	
    if(parser.Found(("b")))
#else
	if(parser.Found(wxT("b")))		
#endif		
    {
        cout<<"Blankcheck..."<<endl;
        wxString typeText,tempStr;
        VerifyResult res=hardware->blankCheck(picType);
        switch(res.Result)
        {
            case VERIFY_SUCCESS:
                cout<<"Device is blank"<<endl;
                break;
            case VERIFY_MISMATCH:
                switch (res.DataType)
                {
#if wxCHECK_VERSION(2,8,10)					
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
#else
				case TYPE_CODE: typeText=wxT("code");break;
                    case TYPE_DATA: typeText=wxT("data");break;
                    case TYPE_CONFIG: typeText=wxT("config");break;
                    default: typeText=wxT("unknown");break;

                }
				tempStr.Printf(wxT("Blankcheck %s failed at 0x%X. Read: 0x%02X, Expected: 0x%02X"),
                        typeText.c_str(),
                        res.Address+((res.DataType==TYPE_CONFIG)+picType->ConfigAddress),
                        res.Read,
                        res.Expected);

                cerr<<(string)tempStr.mb_str(wxConvUTF8)<<endl;
#endif				
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

    /* if -w is passed, open the hexFile and write it to the pic */
#if wxCHECK_VERSION(2,8,10)
    if(parser.Found(("w")))
#else
	if(parser.Found(wxT("w")))
#endif
    {
        cout<<"Write..."<<endl;
#if wxCHECK_VERSION(2,8,10)		
        if(!parser.Found(("f"),&filename)){cerr<<"Please specify a filename"<<endl;}
#else		
		if(!parser.Found(wxT("f"),&filename)){cerr<<"Please specify a filename"<<endl;}
#endif		
        else
        {
            hexFile=new HexFile();
            if(!hexFile->open(picType,filename.mb_str(wxConvUTF8)))
                cerr<<"Unable to open file"<<endl;
            if(!silent_mode)
            {
                hexFile->print(&output,picType);
#if wxCHECK_VERSION(2,8,10)					
                cout<<(string)output<<endl;
#else
				cout<<(string)output.mb_str(wxConvUTF8)<<endl;
#endif				
            }
            if(hardware->write(TYPE_CODE,hexFile,picType)<0)cerr<<"Error writing Code"<<endl;
            if(hardware->write(TYPE_DATA,hexFile,picType)<0)cerr<<"Error writing Data"<<endl;
            if(hardware->write(TYPE_CONFIG,hexFile,picType)<0)cerr<<"Error writing Config"<<endl;
            delete hexFile;
        }
    }

    /* if -r is passed, read it to the pic and save it to the hexFile    */
#if wxCHECK_VERSION(2,8,10)	
    if(parser.Found(("r")))
#else
	if(parser.Found(wxT("r")))
#endif
    {
        cout<<"Read..."<<endl;
#if wxCHECK_VERSION(2,8,10)		
        if(!parser.Found(("f"),&filename)){cerr<<"Please specify a filename"<<endl;}
#else		
		if(!parser.Found(wxT("f"),&filename)){cerr<<"Please specify a filename"<<endl;}
#endif		
        else
        {
            hexFile=new HexFile();
            if(hardware->read(TYPE_CODE,hexFile,picType)<0)cerr<<"Error reading Code"<<endl;
            if(hardware->read(TYPE_DATA,hexFile,picType)<0)cerr<<"Error reading Data"<<endl;
            if(hardware->read(TYPE_CONFIG,hexFile,picType)<0)cerr<<"Error reading Config"<<endl;
            hexFile->trimData(picType);
            if(!hexFile->saveAs(picType,filename.mb_str(wxConvUTF8)))
                cerr<<"Unable to save file"<<endl;
            if(!silent_mode)
            {
                hexFile->print(&output,picType);
#if wxCHECK_VERSION(2,8,10)					
                cout<<(string)output<<endl;
#else				
				cout<<(string)output.mb_str(wxConvUTF8)<<endl;
#endif				
            }
            delete hexFile;
        }
    }

    /* if -v is passed, open the hexFile, read it and compare the results*/
#if wxCHECK_VERSION(2,8,10)
    if(parser.Found(("v")))
#else
	if(parser.Found(wxT("v")))
#endif
    {
        cout<<"Verify..."<<endl;
#if wxCHECK_VERSION(2,8,10)		
        if(!parser.Found(("f"),&filename)){cerr<<"Please specify a filename"<<endl;}
#else
		if(!parser.Found(wxT("f"),&filename)){cerr<<"Please specify a filename"<<endl;}
#endif		
        else
        {
            hexFile=new HexFile();
            if(!hexFile->open(picType,filename.mb_str(wxConvUTF8)))
                cerr<<"Unable to open file"<<endl;
            if(!silent_mode)
            {
                hexFile->print(&output,picType);
#if wxCHECK_VERSION(2,8,10)				
                cout<<(string)output<<endl;
#else
		      	cout<<(string)output.mb_str(wxConvUTF8)<<endl;		
#endif				
            }
            wxString typeText,tempStr;
            VerifyResult res=hardware->verify(hexFile,picType);
            switch(res.Result)
            {
                case VERIFY_SUCCESS:
                    cout<<"Verify successful"<<endl;
                    break;
                case VERIFY_MISMATCH:
                    switch (res.DataType)
                    {
#if wxCHECK_VERSION(2,8,10)							
                        case TYPE_CODE: typeText="code";break;
                        case TYPE_DATA: typeText="data";break;
                        case TYPE_CONFIG: typeText="config";break;
                        default: typeText="unknown";break;
                    }
                    tempStr.Printf("Verify %s failed at 0x%X. Read: 0x%02X, Expected: 0x%02X",
                            typeText.c_str(),
                            res.Address+((res.DataType==TYPE_CONFIG)+picType->ConfigAddress),
                            res.Read,
                            res.Expected);
                    cerr<<(string)tempStr<<endl;						
#else
						case TYPE_CODE: typeText=wxT("code");break;
                        case TYPE_DATA: typeText=wxT("data");break;
                        case TYPE_CONFIG: typeText=wxT("config");break;
                        default: typeText=wxT("unknown");break;
                    }
                    tempStr.Printf(wxT("Verify %s failed at 0x%X. Read: 0x%02X, Expected: 0x%02X"),
                            typeText.c_str(),
                            res.Address+((res.DataType==TYPE_CONFIG)+picType->ConfigAddress),
                            res.Read,
                            res.Expected);
                    cerr<<(string)tempStr.mb_str(wxConvUTF8)<<endl;					
#endif						

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

    exit(0);
}
