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
#include <wx/wx.h>
#include "../svn_revision.h"
using namespace std;
IMPLEMENT_APP(UsbPicProg)

#ifdef __WXMSW__
#include <windows.h>
#endif

UppMainWindowCallBack *uppMainWindow;

/*This is the wxWidgets initialization function, but we only use it to call 
 *wxWidgets own OnInit(), because OnInitCmdLine () is being used*/
bool UsbPicProg::OnInit()
{
	return wxApp::OnInit();
}

/*Called by WxWidgets to clean up some stuff when the program exits*/
int UsbPicProg::OnExit()
{
	if (readHexFile != NULL) delete readHexFile;
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
    parser.SetSwitchChars (_("-"));
}

void UsbPicProg::MacOpenFile(const wxString &fileName)
{
	uppMainWindow->upp_open_file(fileName);
}

/*After command line is being processed, this function is being called
by wxWidgets, even if no arguments are given. This is the actual function
in which the real application initializes.*/
bool UsbPicProg::OnCmdLineParsed(wxCmdLineParser& parser)
{
	readHexFile = NULL;
	picType = NULL;
	hardware = NULL;
	m_locale = new wxLocale(wxLANGUAGE_DEFAULT);
	m_locale->AddCatalog(wxT("usbpicprog"));
	
    #ifndef UPP_VERSION
	wxString versionString=wxString(_("Usbpicprog rev: ")).Append(wxString::FromAscii(SVN_REVISION));
	#else
	wxString versionString=wxString(_("Usbpicprog: ")).Append(wxString::FromAscii(UPP_VERSION));
	#endif
    string output;
	/*If no command line arguments are passed, we open the main window  *
	 *Else, a command line application is started.						*
	 *Only the filename may be passed to the gui						*/
	if(!parser.Found(_("h"))&
		!parser.Found(_("V"))&
		!parser.Found(_("p"))&
		!parser.Found(_("s"))&
		!parser.Found(_("w"))&
		!parser.Found(_("r"))&
		!parser.Found(_("v"))&
		!parser.Found(_("e"))&
		!parser.Found(_("b"))&
		!parser.Found(_("f")))
	{
		uppMainWindow = new UppMainWindowCallBack((wxFrame *)NULL, 10000, versionString, wxPoint(50, 50), wxSize(20, 20));
		// creating the window with 20x20 dimensions: workaround for minimum statusbar size, we'll resize it later to 800x600
		if(parser.GetParamCount()>0){uppMainWindow->upp_open_file(parser.GetParam(0));}
		else{uppMainWindow->upp_new();}
		uppMainWindow->Show(TRUE);
	}
	else	//start a command line app
	{
		/*when using Windows, wxWidgets takes over the terminal, *
		 *but we want to have it for cout and cerr			     */
		#ifdef __WXMSW__	
		if( AttachConsole((DWORD)-1) )
		{
		  freopen( "CON", "w", stdout );
		  freopen( "CON", "w", stderr );
		}
		#endif
		if(parser.Found(_("V")))
		{
			#ifndef UPP_VERSION
				cerr<<string("usbpicprog (SVN) ").append(SVN_REVISION)<<endl;
			#else
				cerr<<string("usbpicprog ").append(UPP_VERSION)<<endl;
			#endif
			return EXIT_SUCCESS;
		}
		//command line -s or --silent passed?
		silent_mode = parser.Found(_("s"));	
		hardware=new Hardware();
		if(!hardware->connected())
		{
			cerr<<"Usbpicprog not found"<<endl;
			exit(-1);
		}
		wxString picTypeStr;
		/* check if -p <str> is passed, else autodetect the device
		 */
		if(parser.Found(_("p"),&picTypeStr))picType=new PicType(string(picTypeStr.mb_str(wxConvUTF8)));
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
		if(parser.Found(_("e")))
		{
            cout<<"Bulk erase..."<<endl;
			if(hardware->bulkErase(picType)<0)cerr<<"Error during erase"<<endl;
		}
		/* if -b is passed, check if the device is blank*/
		if(parser.Found(_("b")))
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
		if(parser.Found(_("w")))
		{
            cout<<"Write..."<<endl;
			if(parser.GetParamCount()==0){cerr<<"Please specify a filename"<<endl;}
			else
			{
				readHexFile=new ReadHexFile();
				if(readHexFile->open(picType,parser.GetParam(0).mb_str(wxConvUTF8))<0)cerr<<"Unable to open file"<<endl;
				if(!silent_mode)
				{
                    readHexFile->print(&output,picType);
                    cout<<output<<endl;
                }
				if(hardware->writeCode(readHexFile,picType)<0)cerr<<"Error writing Code"<<endl;
				if(hardware->writeData(readHexFile,picType)<0)cerr<<"Error writing Data"<<endl;
				if(hardware->writeConfig(readHexFile,picType)<0)cerr<<"Error writing Config"<<endl;
				delete readHexFile;
			}
		}
		/* if -r is passed, read it to the pic and save it to the hexfile	 */
		if(parser.Found(_("r")))
		{
            cout<<"Read..."<<endl;
			if(parser.GetParamCount()==0){cerr<<"Please specify a filename"<<endl;}
			else
			{
				readHexFile=new ReadHexFile();
				if(hardware->readCode(readHexFile,picType)<0)cerr<<"Error reading Code"<<endl;
				if(hardware->readData(readHexFile,picType)<0)cerr<<"Error reading Data"<<endl;
				if(hardware->readConfig(readHexFile,picType)<0)cerr<<"Error reading Config"<<endl;
				readHexFile->trimData(picType);
				if(readHexFile->saveAs(picType,parser.GetParam(0).mb_str(wxConvUTF8))<0)cerr<<"Unable to save file"<<endl;
				if(!silent_mode)
				{
                    readHexFile->print(&output,picType);
                    cout<<output<<endl;
                }
				delete readHexFile;
			}
		}
		/* if -v is passed, open the hexfile, read it and compare the results*/
		if(parser.Found(_("v")))
		{
            cout<<"Verify..."<<endl;
			if(parser.GetParamCount()==0){cerr<<"Please specify a filename"<<endl;}
			else
			{
                readHexFile=new ReadHexFile();
				if(readHexFile->open(picType,parser.GetParam(0).mb_str(wxConvUTF8))<0)cerr<<"Unable to open file"<<endl;
				if(!silent_mode)
				{
                    readHexFile->print(&output,picType);
                    cout<<output<<endl;
                }
                string typeText;
                VerifyResult res=hardware->verify(readHexFile,picType);
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
                delete readHexFile;
			}
		}
		exit(0);
	}
    return true;
}
