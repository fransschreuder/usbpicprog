#include "uppmainwindow_callback.h"
#include "uppmainwindow.h"
#include "../svn_revision.h"


static const wxChar *FILETYPES = _T(
			"Hex files|*.hex|"
			"All files|*.*"
			);



void UppMainWindowCallBack::printHexFile()
{
    string output;
	uppHexEdit->Clear();
	uppHexEdit->Freeze();
	
	readHexFile->print(&output,picType);
	uppHexEdit->AppendText(wxString::FromAscii(output.c_str()));
    uppHexEdit->Thaw();
}


void UppMainWindowCallBack::upp_new()
{
	uppHexEdit->Clear();
	delete readHexFile;
	readHexFile=new ReadHexFile();
	fileOpened=false;
}

void UppMainWindowCallBack::upp_open()
{
	wxFileDialog* openFileDialog =
		new wxFileDialog( this, wxT("Open hexfile"), wxT(""), wxT(""), FILETYPES,
		                  wxOPEN, wxDefaultPosition);
 
	if ( openFileDialog->ShowModal() == wxID_OK )
	{
		upp_open_file(openFileDialog->GetPath());
	}
}


void UppMainWindowCallBack::upp_open_file(wxString path)
{
	
 	if(readHexFile->open(picType,path.mb_str(wxConvUTF8))<0)
    {
        SetStatusText(wxT("Unable to open file"));
        wxMessageDialog(this, wxT("Unable to open file"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
    }
 	else
 	{
    	printHexFile();
    	fileOpened=true;
    }
}


void UppMainWindowCallBack::upp_refresh()
{
	if(readHexFile->reload(picType)<0)
    {
        SetStatusText(wxT("Unable to open file"));
        wxMessageDialog(this, wxT("Unable to open file"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
    }
	else printHexFile();
}

void UppMainWindowCallBack::upp_save()
{
	if(fileOpened)
    {
        if(readHexFile->save(picType)<0)
        {
            SetStatusText(wxT("Unable to save file"));
            wxMessageDialog(this, wxT("Unable to save file"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        }
    }
	else upp_save_as();
}

void UppMainWindowCallBack::upp_save_as()
{
	wxFileDialog* openFileDialog =
		new wxFileDialog( this, wxT("Save hexfile"), wxT(""), wxT(""), FILETYPES,
		                  wxSAVE, wxDefaultPosition);
	if ( openFileDialog->ShowModal() == wxID_OK )
	{
		if(readHexFile->saveAs(picType,openFileDialog->GetPath().mb_str(wxConvUTF8))<0)
		{
            SetStatusText(wxT("Unable to save file"));
            wxMessageDialog(this, wxT("Unable to save file"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        }
	}
}

void UppMainWindowCallBack::upp_exit()
{
	Close();
}

void UppMainWindowCallBack::upp_program()
{
	hardware->writeCode(readHexFile,picType);
	hardware->writeData(readHexFile,picType);
	hardware->writeConfig(readHexFile,picType);
}

void UppMainWindowCallBack::upp_read()
{
	hardware->readCode(readHexFile,picType);
	hardware->readData(readHexFile,picType);
	hardware->readConfig(readHexFile,picType);
	printHexFile();
}

void UppMainWindowCallBack::upp_verify()
{
	SetStatusText(wxT("Verify not implemented yet"));
           
}

void UppMainWindowCallBack::upp_erase()
{
	hardware->bulkErase();
}

void UppMainWindowCallBack::upp_blankcheck()
{
	SetStatusText(wxT("Blankcheck not implemented yet"));
}

bool UppMainWindowCallBack::upp_autodetect()
{
    int devId=hardware->autoDetectDevice();
	picType=new PicType(devId);
	hardware->setPicType(picType);
	m_comboBox1->SetValue(wxString::FromAscii(picType->getCurrentPic().Name.c_str()));
	if(devId!=0)SetStatusText(wxString(wxT("Detected: ")).Append(wxString::FromAscii(picType->getCurrentPic().Name.c_str())));
	else SetStatusText(wxT("No pic detected!"));
	return (devId!=0);
}

bool UppMainWindowCallBack::upp_connect()
{
	hardware=new Hardware();
	if(hardware->connected())
	{
		upp_autodetect();
    }
	else
	{
     	picType=new PicType(0);
    	hardware->setPicType(picType);
	    m_comboBox1->SetValue(wxString::FromAscii(picType->getCurrentPic().Name.c_str()));
		SetStatusText(wxT("Usbpicprog not found"));
    }
    return hardware->connected();

}

void UppMainWindowCallBack::upp_disconnect()
{
	if(hardware->connected())
	   {
		   delete hardware;
		   SetStatusText(wxT("Disconnected usbpicprog"));
	   }
	   else
	   {
		   SetStatusText(wxT("Already disconnected"));
	   }
}

void UppMainWindowCallBack::upp_help()
{
	wxLaunchDefaultBrowser(wxT("http://usbpicprog.sourceforge.net/"));
}

void UppMainWindowCallBack::upp_about()
{
#ifdef HAVE_WXABOUTBOX
	wxAboutDialogInfo aboutInfo;
	aboutInfo.SetName(wxT("Usbpicprog"));
	#ifndef UPP_VERSION
	aboutInfo.SetVersion(wxString(wxT("(SVN) ")).Append(wxString::FromAscii(SVN_REVISION)));
	#else
	aboutInfo.SetVersion(wxString::FromAscii(UPP_VERSION));
	#endif
	aboutInfo.SetDescription(wxT("An open source USB pic programmer"));
	aboutInfo.SetCopyright(wxT("(C) 2008 http://usbpicprog.sourceforge.net/"));
	wxAboutBox(aboutInfo);
#endif
}


void UppMainWindowCallBack::upp_combo_changed()
{
		picType=new PicType(string(m_comboBox1->GetValue().mb_str(wxConvUTF8)));
		hardware->setPicType(picType);
    	upp_new();
}



UppMainWindowCallBack::UppMainWindowCallBack(wxWindow* parent, wxWindowID id , const wxString& title , const wxPoint& pos , const wxSize& size , long style ) : UppMainWindow( parent, id, title, pos, size, style )
{
	upp_connect();
	readHexFile=new ReadHexFile();
	for(int i=0;i<(signed)picType->getPicNames().size();i++)
	{
		m_comboBox1->Append(wxString::FromAscii(picType->getPicNames()[i].c_str()));
		
	}

	
	fileOpened=false;
}
