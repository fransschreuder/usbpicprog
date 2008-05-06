#ifndef MAIN_H
#define MAIN_H
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <wx/wxprec.h>
#include <wx/cmdline.h>
#include <wx/wx.h>
#include <iostream>


#include "uppmainwindow_callback.h"
#include "hardware.h"
#include "pictype.h"
#include "read_hexfile.h"


class UsbPicProg : public wxApp
{
  public:
    virtual bool OnInit();
	
	virtual int OnExit();
    virtual int OnRun();
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
 
private:
    bool silent_mode;
	ReadHexFile* readHexFile;
	PicType* picType;
	Hardware* hardware;
};

DECLARE_APP(UsbPicProg)

static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
     { wxCMD_LINE_SWITCH, wxT("h"), wxT("help"),    wxT("displays help on the command line parameters"),wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
	 { wxCMD_LINE_OPTION, wxT("p"), wxT("pictype"), wxT("specify the pic type (eg -p=P18F2550)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
     { wxCMD_LINE_SWITCH, wxT("s"), wxT("silent"),  wxT("do not display the hex file") },
	{ wxCMD_LINE_SWITCH, wxT("w"), wxT("write"),  wxT("write the device") },
	{ wxCMD_LINE_SWITCH, wxT("r"), wxT("read"),  wxT("read the device") },
	{ wxCMD_LINE_SWITCH, wxT("v"), wxT("verify"),  wxT("verify the device") },
	{ wxCMD_LINE_SWITCH, wxT("e"), wxT("erase"),  wxT("bulk erase the device") },
	{ wxCMD_LINE_SWITCH, wxT("b"), wxT("blankcheck"),  wxT("blankcheck the device") },
	{ wxCMD_LINE_PARAM, wxT("f"), wxT("file"),  wxT("hexfile"),wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL },
 
     { wxCMD_LINE_NONE }
};
#endif //MAIN_H
