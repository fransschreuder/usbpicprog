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

#include "main.h"
#include "../svn_revision.h"

using namespace std;
IMPLEMENT_APP(UsbPicProg)

#ifdef __WXMSW__
#include <windows.h>
#endif


/*Mac-specific open file function*/
void UsbPicProg::MacOpenFile(const wxString &fileName)
{
    static_cast<UppMainWindow*>(GetTopWindow())->upp_open_file(fileName);
}

/*This is the wxWidgets initialization function, but we only use it to call
*wxWidgets own OnInit(), because OnInitCmdLine () is being used*/
bool UsbPicProg::OnInit()
{
    // wxApp::OnInit() will call UsbPicProg::OnInitCmdLine and UsbPicProg::OnCmdLineParsed
    if (!wxApp::OnInit())
        return false;

    SetVendorName(wxT("UsbPicProgrammer"));

    if (!m_console)
    {
        // start a GUI app
        UppMainWindow *uppMainWindow = new UppMainWindow((wxFrame *)NULL, wxID_ANY);
        SetTopWindow(uppMainWindow);

        uppMainWindow->Show(true);
    }

    return true;
}

/*Called by WxWidgets to clean up some stuff when the program exits*/
int UsbPicProg::OnExit()
{
    if (m_hexFile != NULL) delete m_hexFile;
    if (m_picType != NULL) delete m_picType;
    if (m_hardware != NULL) delete m_hardware;
    delete m_locale;

    return wxApp::OnExit();
}

/*Initialization function if command line is being used*/
void UsbPicProg::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetDesc (g_cmdLineDesc);
    parser.SetSwitchChars (wxT("-"));
}

/*After command line is being processed, this function is being called
by wxWidgets, even if no arguments are given. This is the actual function
in which the real application initializes.*/
bool UsbPicProg::OnCmdLineParsed(wxCmdLineParser& parser)
{
    m_hexFile = NULL;
    m_picType = NULL;
    m_hardware = NULL;
#ifdef __WXMSW__
    wxLocale::AddCatalogLookupPathPrefix(_T("po"));
#endif
#ifdef __WXMAC__
    wxLocale::AddCatalogLookupPathPrefix(wxString(wxApp::argv[0]).BeforeLast('/') + _T("/po"));
#endif
    // init the locale
    m_locale = new wxLocale(wxLANGUAGE_DEFAULT);
    m_locale->AddCatalog(wxT("usbpicprog"));

    /*If no command line arguments are passed, we open the main window  *
    *Else, a command line application is started.						*
    *Only the filename may be passed to the gui						*/
    wxString tmp;
    if (!parser.Found(wxT("h")) &&
        !parser.Found(wxT("V")) &&
        !parser.Found(wxT("p"), &tmp) &&
        !parser.Found(wxT("s")) &&
        !parser.Found(wxT("w")) &&
        !parser.Found(wxT("r")) &&
        !parser.Found(wxT("v")) &&
        !parser.Found(wxT("e")) &&
        !parser.Found(wxT("b")) &&
        !parser.Found(wxT("f"), &tmp))
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
    string output;
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

    if(parser.Found(wxT("V")))
    {
#ifndef UPP_VERSION
        cerr<<string("usbpicprog (SVN) ").append(SVN_REVISION)<<endl;
#else
        cerr<<string("usbpicprog ").append(UPP_VERSION)<<endl;
#endif
        return EXIT_SUCCESS;
    }

    //command line -s or --silent passed?
    m_silent_mode = parser.Found(wxT("s"));
    m_hardware = new Hardware();
    if(!m_hardware->connected())
    {
        cerr<<"Usbpicprog not found"<<endl;
        exit(-1);
    }
    wxString m_picTypeStr;

    /* check if -p <str> is passed, else autodetect the device
    */
    if(parser.Found(wxT("p"),&m_picTypeStr))m_picType=new PicType(string(m_picTypeStr.mb_str(wxConvUTF8)));
    else
    {
        int devId=m_hardware->autoDetectDevice();
        m_picType=new PicType(devId);
        m_hardware->setPicType(m_picType);
        if(devId>0)cout<<"Detected: ";
        else cerr<<"Detection failed, setting m_picType to default: ";
        cout<<m_picType->getCurrentPic().Name<<endl;
    }

    /* if -e is passed, bulk erase the entire pic*/
    if(parser.Found(wxT("e")))
    {
        cout<<"Bulk erase..."<<endl;
        if(m_hardware->bulkErase(m_picType)<0)cerr<<"Error during erase"<<endl;
    }

    /* if -b is passed, check if the device is blank*/
    if(parser.Found(wxT("b")))
    {
        cout<<"Blankcheck..."<<endl;
        string typeText;
        VerifyResult res=m_hardware->blankCheck(m_picType);
        switch(res.Result)
        {
            case VERIFY_SUCCESS:
                cout<<"Device is blank"<<endl;
                break;
            case VERIFY_MISMATCH:
                switch (res.DataType)
            {
                case TYPE_CODE: typeText=string("code");break;
                case TYPE_DATA: typeText=string("data");break;
                case TYPE_CONFIG: typeText=string("config");break;
                default: typeText=string("unknown");break;
            }
                fprintf(stderr,"Blankcheck %s failed at 0x%X. Read: 0x%02X, Expected: 0x%02X",
                        typeText.c_str(),
                        res.Address+((res.DataType==TYPE_CONFIG)+m_picType->getCurrentPic().ConfigAddress),
                        res.Read,
                        res.Expected);
                cout<<endl;
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

    /* if -w is passed, open the m_hexFile and write it to the pic */
    if(parser.Found(wxT("w")))
    {
        cout<<"Write..."<<endl;
        if(!parser.Found(wxT("f"),&filename)){cerr<<"Please specify a filename"<<endl;}
        else
        {
            m_hexFile=new HexFile();
            if(m_hexFile->open(m_picType,filename.mb_str(wxConvUTF8))<0)cerr<<"Unable to open file"<<endl;
            if(!m_silent_mode)
            {
                m_hexFile->print(&output,m_picType);
                cout<<output<<endl;
            }
            if(m_hardware->writeCode(m_hexFile,m_picType)<0)cerr<<"Error writing Code"<<endl;
            if(m_hardware->writeData(m_hexFile,m_picType)<0)cerr<<"Error writing Data"<<endl;
            if(m_hardware->writeConfig(m_hexFile,m_picType)<0)cerr<<"Error writing Config"<<endl;
            delete m_hexFile;
        }
    }

    /* if -r is passed, read it to the pic and save it to the m_hexFile    */
    if(parser.Found(wxT("r")))
    {
        cout<<"Read..."<<endl;
        if(!parser.Found(wxT("f"),&filename)){cerr<<"Please specify a filename"<<endl;}
        else
        {
            m_hexFile=new HexFile();
            if(m_hardware->readCode(m_hexFile,m_picType)<0)cerr<<"Error reading Code"<<endl;
            if(m_hardware->readData(m_hexFile,m_picType)<0)cerr<<"Error reading Data"<<endl;
            if(m_hardware->readConfig(m_hexFile,m_picType)<0)cerr<<"Error reading Config"<<endl;
            m_hexFile->trimData(m_picType);
            if(m_hexFile->saveAs(m_picType,filename.mb_str(wxConvUTF8))<0)cerr<<"Unable to save file"<<endl;
            if(!m_silent_mode)
            {
                m_hexFile->print(&output,m_picType);
                cout<<output<<endl;
            }
            delete m_hexFile;
        }
    }

    /* if -v is passed, open the m_hexFile, read it and compare the results*/
    if(parser.Found(wxT("v")))
    {
        cout<<"Verify..."<<endl;
        if(parser.GetParamCount()==0){cerr<<"Please specify a filename"<<endl;}
        else
        {
            m_hexFile=new HexFile();
            if(m_hexFile->open(m_picType,parser.GetParam(0).mb_str(wxConvUTF8))<0)cerr<<"Unable to open file"<<endl;
            if(!m_silent_mode)
            {
                m_hexFile->print(&output,m_picType);
                cout<<output<<endl;
            }
            string typeText;
            VerifyResult res=m_hardware->verify(m_hexFile,m_picType);
            switch(res.Result)
            {
                case VERIFY_SUCCESS:
                    cout<<"Verify successful"<<endl;
                    break;
                case VERIFY_MISMATCH:
                    switch (res.DataType)
                {
                    case TYPE_CODE: typeText=string("code");break;
                    case TYPE_DATA: typeText=string("data");break;
                    case TYPE_CONFIG: typeText=string("config");break;
                    default: typeText=string("unknown");break;
                }
                    fprintf(stderr,"Verify %s failed at 0x%X. Read: 0x%02X, Expected: 0x%02X",
                            typeText.c_str(),
                            res.Address+((res.DataType==TYPE_CONFIG)+m_picType->getCurrentPic().ConfigAddress),
                            res.Read,
                            res.Expected);
                    cerr<<endl;
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
            delete m_hexFile;
        }
    }

    exit(0);
}
