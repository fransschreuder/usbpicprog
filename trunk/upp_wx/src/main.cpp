#include "main.h"
#include <wx/wx.h>
using namespace std;
IMPLEMENT_APP(UsbPicProg)

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
    int exitcode = wxApp::OnRun();
    //wxTheClipboard->Flush();
    if (exitcode!=0)
        return exitcode;
}
 
void UsbPicProg::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetDesc (g_cmdLineDesc);
    // must refuse '/' as parameter starter or cannot use "/path" style paths
    parser.SetSwitchChars (wxT("-"));
}
 
bool UsbPicProg::OnCmdLineParsed(wxCmdLineParser& parser)
{
   
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
		UppMainWindowCallBack *uppMainWindow = new UppMainWindowCallBack((wxFrame *)NULL, 10000, wxT("usbpicprog v0.1"),
									   wxPoint(50, 50), wxSize(800, 600));
			
			
		cout<<"no command line passed"<<endl;
		uppMainWindow->Show(TRUE);
	}
	else
	{
		silent_mode = parser.Found(wxT("s"));
		cout<<"command line passed"<<endl;
		// to get at your unnamed parameters use
		//wxArrayString files;
		wxString picTypeStr;
		if(parser.Found(wxT("p"),&picTypeStr))picType=new PicType(string(picTypeStr.mb_str(wxConvUTF8)));
		else picType=new PicType(0);
		
		if(parser.Found(wxT("w")))
		{
			if(parser.GetParamCount()==0){cout<<"Please specify a filename"<<endl;}
		}
		if(parser.Found(wxT("r")))
		{
			if(parser.GetParamCount()==0){cout<<"Please specify a filename"<<endl;}
		}
		if(parser.Found(wxT("v")))
		{
			if(parser.GetParamCount()==0){cout<<"Please specify a filename"<<endl;}
		}
		if(parser.Found(wxT("e")))
		{
		
		}
		if(parser.Found(wxT("b")))
		{
		
		}
		
	 
		// and other command line parameters
	 
		// then do what you need with them.
	}
    return true;
}
