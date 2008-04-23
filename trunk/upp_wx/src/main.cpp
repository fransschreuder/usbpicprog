#include "main.h"
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
   
	if(parser.GetParamCount()==0)
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
		for (int i = 0; i < parser.GetParamCount(); i++)
		{
				cout<<parser.GetParam(i)<<endl;
		}
		if(parser.Found(wxT("P")))picType=new PicType(string(parser.GetParam(0)));
		else picType=new PicType(0);
	 
		// and other command line parameters
	 
		// then do what you need with them.
	}
    return true;
}
