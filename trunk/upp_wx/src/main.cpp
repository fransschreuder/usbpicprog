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

/*This is the wxWidgets initialization function, but we only use it to call 
 *wxWidgets own OnInit(), because OnInitCmdLine () is being used*/
bool UsbPicProg::OnInit()
{
	return wxApp::OnInit();
}

/*Called by WxWidgets to clean up some stuff when the program exits*/
int UsbPicProg::OnExit()
{
	delete readHexFile;
	delete picType;
	delete hardware;
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
 
/*After command line is being processed, this function is being called
by wxWidgets, even if no arguments are given. This is the actual function
in which the real application initializes.*/
bool UsbPicProg::OnCmdLineParsed(wxCmdLineParser& parser)
{

    #ifndef UPP_VERSION
	wxString versionString=wxString(wxT("Usbpicprog rev: ")).Append(wxString::FromAscii(SVN_REVISION));
	#else
	wxString versionString=wxString(wxT("Usbpicprog: ")).Append(wxString::FromAscii(UPP_VERSION));
	#endif
    string output;
	/*If no command line arguments are passed, we open the main window  *
	 *Else, a command line application is started.						*
	 *Only the filename may be passed to the gui						*/
	if(!parser.Found(wxT("h"))&
	   !parser.Found(wxT("p"))&
	   !parser.Found(wxT("s"))&
	   !parser.Found(wxT("w"))&
	   !parser.Found(wxT("r"))&
	   !parser.Found(wxT("v"))&
	   !parser.Found(wxT("e"))&
	   !parser.Found(wxT("b"))&
	   !parser.Found(wxT("f")))
	{
		UppMainWindowCallBack *uppMainWindow = new UppMainWindowCallBack((wxFrame *)NULL, 10000, versionString,
									   wxPoint(50, 50), wxSize(800, 600));
		if(parser.GetParamCount()>0){uppMainWindow->upp_open_file(parser.GetParam(0));}	
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
		//command line -s or --silent passed?
		silent_mode = parser.Found(wxT("s"));	
		hardware=new Hardware();
		if(!hardware->connected())
		{
			cout<<"Usbpicprog not found"<<endl;
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
			cout<<picType->getCurrentPic().Name<<endl;
		}
		/* if -e is passed, bulk erase the entire pic*/
		if(parser.Found(wxT("e")))
		{
			hardware->bulkErase();
		}
		/* if -b is passed, check if the device is blank*/
		if(parser.Found(wxT("b")))
		{
			cout<<"Blankcheck not implemented yet"<<endl;
		}
		/* if -w is passed, open the hexfile and write it to the pic */
		if(parser.Found(wxT("w")))
		{
			if(parser.GetParamCount()==0){cout<<"Please specify a filename"<<endl;}
			else
			{
				readHexFile=new ReadHexFile();
				if(readHexFile->open(picType,parser.GetParam(0).mb_str(wxConvUTF8))<0)cout<<"Unable to open file"<<endl;
				if(!silent_mode)
				{
                    readHexFile->print(&output,picType);
                    cout<<output<<endl;
                }
				hardware->writeCode(readHexFile,picType);
				hardware->writeData(readHexFile,picType);
				hardware->writeConfig(readHexFile,picType);
				delete readHexFile;
			}
		}
		/* if -r is passed, read it to the pic and save it to the hexfile	 */
		if(parser.Found(wxT("r")))
		{
			if(parser.GetParamCount()==0){cout<<"Please specify a filename"<<endl;}
			else
			{
				readHexFile=new ReadHexFile();
				hardware->readCode(readHexFile,picType);
				hardware->readData(readHexFile,picType);
				hardware->readConfig(readHexFile,picType);
				if(readHexFile->saveAs(picType,parser.GetParam(0).mb_str(wxConvUTF8))<0)cout<<"Unable to save file"<<endl;
				if(!silent_mode)
				{
                    readHexFile->print(&output,picType);
                    cout<<output<<endl;
                }
				delete readHexFile;
			}
		}
		/* if -v is passed, open the hexfile, read it and compare the results*/
		if(parser.Found(wxT("v")))
		{
			if(parser.GetParamCount()==0){cout<<"Please specify a filename"<<endl;}
			else
			{
				cout<<"Verify not implemented yet!!!"<<endl;
			}
		}
		
		exit(0);
	}
    return true;
}
