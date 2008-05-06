#include "main.h"
#include <wx/wx.h>
#include "svn_revision.h"
using namespace std;
IMPLEMENT_APP(UsbPicProg)

#ifdef WIN32
#include <windows.h>
#endif

bool UsbPicProg::OnInit()
{
	if (!wxApp::OnInit())
        return false;
  	
	
  	return TRUE;
}




int UsbPicProg::OnExit()
{
    // clean up
    return 0;
}
 
int UsbPicProg::OnRun()
{
    return wxApp::OnRun();
}
 
void UsbPicProg::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetDesc (g_cmdLineDesc);
    // must refuse '/' as parameter starter or cannot use "/path" style paths
    parser.SetSwitchChars (wxT("-"));
}
 
bool UsbPicProg::OnCmdLineParsed(wxCmdLineParser& parser)
{
    #ifdef WIN32
    if( AttachConsole((DWORD)-1) )
    {
      freopen( "CON", "w", stdout );
      freopen( "CON", "w", stderr );
    }
    #endif
    #ifndef UPP_VERSION
	wxString versionString=wxString(wxT("Usbpicprog rev: ")).Append(wxString::FromAscii(SVN_REVISION));
	#else
	wxString versionString=wxString(wxT("Usbpicprog: ")).Append(wxString::FromAscii(UPP_VERSION));
	#endif
    string output;
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
	else
	{
		silent_mode = parser.Found(wxT("s"));
		wxString picTypeStr;
		hardware=new Hardware();
		if(hardware->connected())
			cout<<"Usbpicprog found"<<endl;
		else
			cout<<"Usbpicprog not found"<<endl;
		if(parser.Found(wxT("p"),&picTypeStr))picType=new PicType(string(picTypeStr.mb_str(wxConvUTF8)));
		else 
		{
			int devId=hardware->autoDetectDevice();
	        picType=new PicType(devId);
			hardware->setPicType(picType);
			cout<<picType->getCurrentPic().Name<<endl;
		}
		
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
			}
		}
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
			}
		}
		if(parser.Found(wxT("v")))
		{
			if(parser.GetParamCount()==0){cout<<"Please specify a filename"<<endl;}
			else
			{
				cout<<"Verify not implemented yet!!!"<<endl;
			}
		}
		if(parser.Found(wxT("e")))
		{
			hardware->bulkErase();
		}
		if(parser.Found(wxT("b")))
		{
			cout<<"Blankcheck not implemented yet"<<endl;
		}
		exit(0);
	}
    return true;
}
