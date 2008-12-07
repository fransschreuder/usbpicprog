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

UppMainWindow *uppMainWindow;

/*This is the wxWidgets initialization function, but we only use it to call
*wxWidgets own OnInit(), because OnInitCmdLine () is being used*/
bool UsbPicProg::OnInit()
{
    return wxApp::OnInit();
}

/*Called by WxWidgets to clean up some stuff when the program exits*/
int UsbPicProg::OnExit()
{
    if (hexFile != NULL) delete hexFile;
    if (picType != NULL) delete picType;
    if (hardware != NULL) delete hardware;
    delete m_locale;
    return 0;
}

/*The "main" loop start here, but wxApp::OnRun calls OnInit and OnInitCmdLine*/
int UsbPicProg::OnRun()
{
    return wxApp::OnRun();
}

/*Initialization function if command line is being used*/
void UsbPicProg::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetDesc (g_cmdLineDesc);
    parser.SetSwitchChars (wxT("-"));
}

/*Mac-specific open file function*/
void UsbPicProg::MacOpenFile(const wxString &fileName)
{
    uppMainWindow->upp_open_file(fileName);
}

/*After command line is being processed, this function is being called
by wxWidgets, even if no arguments are given. This is the actual function
in which the real application initializes.*/
bool UsbPicProg::OnCmdLineParsed(wxCmdLineParser& parser)
{
    hexFile = NULL;
    picType = NULL;
    hardware = NULL;

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
        // start a GUI app
        uppMainWindow = new UppMainWindow((wxFrame *)NULL, wxID_ANY);
        SetTopWindow(uppMainWindow);

        uppMainWindow->Show(true);
    }
    else	//start a command line app
    {
        return CmdLineMain(parser);
        // CmdLineMain() calls exit() itself
    }

    return true;
}

bool UsbPicProg::CmdLineMain(wxCmdLineParser& parser)
{
    string output;

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
    silent_mode = parser.Found(wxT("s"));
    hardware=new Hardware();
    if(!hardware->connected())
    {
        cerr<<"Usbpicprog not found"<<endl;
        exit(-1);
    }
    wxString picTypeStr;

    /* check if -p <str> is passed, else autodetect the device
    */
    if(parser.Found(wxT("p"),&picTypeStr))picType=new PicType(string(picTypeStr.mb_str(wxConvUTF8)));
    else
    {
        int devId=hardware->autoDetectDevice();
        picType=new PicType(devId);
        hardware->setPicType(picType);
        if(devId>0)cout<<"Detected: ";
        else cerr<<"Detection failed, setting pictype to default: ";
        cout<<picType->getCurrentPic().Name<<endl;
    }

    /* if -e is passed, bulk erase the entire pic*/
    if(parser.Found(wxT("e")))
    {
        cout<<"Bulk erase..."<<endl;
        if(hardware->bulkErase(picType)<0)cerr<<"Error during erase"<<endl;
    }

    /* if -b is passed, check if the device is blank*/
    if(parser.Found(wxT("b")))
    {
        cout<<"Blankcheck..."<<endl;
        string typeText;
        VerifyResult res=hardware->blankCheck(picType);
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
                    res.Address+((res.DataType==TYPE_CONFIG)+picType->getCurrentPic().ConfigAddress),
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

    /* if -w is passed, open the hexfile and write it to the pic */
    if(parser.Found(wxT("w")))
    {
        cout<<"Write..."<<endl;
        if(parser.GetParamCount()==0){cerr<<"Please specify a filename"<<endl;}
        else
        {
            hexFile=new HexFile();
            if(hexFile->open(picType,parser.GetParam(0).mb_str(wxConvUTF8))<0)cerr<<"Unable to open file"<<endl;
            if(!silent_mode)
            {
                hexFile->print(&output,picType);
                cout<<output<<endl;
            }
            if(hardware->writeCode(hexFile,picType)<0)cerr<<"Error writing Code"<<endl;
            if(hardware->writeData(hexFile,picType)<0)cerr<<"Error writing Data"<<endl;
            if(hardware->writeConfig(hexFile,picType)<0)cerr<<"Error writing Config"<<endl;
            delete hexFile;
        }
    }

    /* if -r is passed, read it to the pic and save it to the hexfile    */
    if(parser.Found(wxT("r")))
    {
        cout<<"Read..."<<endl;
        if(parser.GetParamCount()==0){cerr<<"Please specify a filename"<<endl;}
        else
        {
            hexFile=new HexFile();
            if(hardware->readCode(hexFile,picType)<0)cerr<<"Error reading Code"<<endl;
            if(hardware->readData(hexFile,picType)<0)cerr<<"Error reading Data"<<endl;
            if(hardware->readConfig(hexFile,picType)<0)cerr<<"Error reading Config"<<endl;
            hexFile->trimData(picType);
            if(hexFile->saveAs(picType,parser.GetParam(0).mb_str(wxConvUTF8))<0)cerr<<"Unable to save file"<<endl;
            if(!silent_mode)
            {
                hexFile->print(&output,picType);
                cout<<output<<endl;
            }
            delete hexFile;
        }
    }

    /* if -v is passed, open the hexfile, read it and compare the results*/
    if(parser.Found(wxT("v")))
    {
        cout<<"Verify..."<<endl;
        if(parser.GetParamCount()==0){cerr<<"Please specify a filename"<<endl;}
        else
        {
            hexFile=new HexFile();
            if(hexFile->open(picType,parser.GetParam(0).mb_str(wxConvUTF8))<0)cerr<<"Unable to open file"<<endl;
            if(!silent_mode)
            {
                hexFile->print(&output,picType);
                cout<<output<<endl;
            }
            string typeText;
            VerifyResult res=hardware->verify(hexFile,picType);
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
                        res.Address+((res.DataType==TYPE_CONFIG)+picType->getCurrentPic().ConfigAddress),
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
            delete hexFile;
        }
    }

    exit(0);
}
