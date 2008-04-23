#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <wx/wx.h>
#include <iostream>
using namespace std;

#include "uppmainwindow_callback.h"

class UsbPicProg : public wxApp
{
  public:
    virtual bool OnInit();
};

IMPLEMENT_APP(UsbPicProg)



bool UsbPicProg::OnInit()
{
  UppMainWindowCallBack *uppMainWindow = new UppMainWindowCallBack((wxFrame *)NULL, 10000, wxT("usbpicprog v0.1"),
                               wxPoint(50, 50), wxSize(800, 600));
	
	
  uppMainWindow->Show(TRUE);
	
  return TRUE;
}




