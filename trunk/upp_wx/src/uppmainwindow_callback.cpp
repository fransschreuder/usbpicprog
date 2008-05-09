#include "uppmainwindow_callback.h"
#include "uppmainwindow.h"
#include "../svn_revision.h"



static const wxChar *FILETYPES = _T(
			"Hex files|*.hex|"
			"All files|*.*"
			);

/*Do the basic initialization of the main window*/
UppMainWindowCallBack::UppMainWindowCallBack(wxWindow* parent, wxWindowID id , const wxString& title , const wxPoint& pos , const wxSize& size , long style ) : UppMainWindow( parent, id, title, pos, size, style )
{
	upp_connect();
	readHexFile=new ReadHexFile();
	for(int i=0;i<(signed)picType->getPicNames().size();i++)
	{
		m_comboBox1->Append(wxString::FromAscii(picType->getPicNames()[i].c_str()));

	}

	uppProgressBar->SetValue(100);
	printHexFile();
	fileOpened=false;
}

/*Update the progress bar*/
void UppMainWindowCallBack::updateProgress(int value)
{
	uppProgressBar->SetValue(value);
	Update(); //refresh the gui, also when busy
}
/*Put the contents of the hex file in the text area*/
void UppMainWindowCallBack::printHexFile()
{
    string output;
	uppHexEdit->Freeze();
	uppHexEdit->Clear();	
	readHexFile->print(&output,picType);
	uppHexEdit->AppendText(wxString::FromAscii(output.c_str()));
    uppHexEdit->Thaw();
}

/*clear the hexfile*/
void UppMainWindowCallBack::upp_new()
{
	uppHexEdit->Clear();
	delete readHexFile;
	readHexFile=new ReadHexFile();
	fileOpened=false;
	printHexFile();
}

/*Open a hexfile using a file dialog*/
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

/*Open a hexfile by filename*/
void UppMainWindowCallBack::upp_open_file(wxString path)
{
	
 	if(readHexFile->open(picType,path.mb_str(wxConvUTF8))<0)
    {
        SetStatusText(wxT("Unable to open file"));
        wxMessageDialog(this, wxT("Unable to open file"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
    }
 	else
 	{
    	fileOpened=true;
/*Now you would ask: why twice? well, I have no idea but sometimes the 
first time doesn't completely put everything in the text area in Windows...*/    	
   	   	printHexFile(); 
   	   	printHexFile();
    }
}

/*re-open the hexfile*/
void UppMainWindowCallBack::upp_refresh()
{
    if(!fileOpened)
    {
        SetStatusText(wxT("No file to refresh"));
        return;
    }
	if(readHexFile->reload(picType)<0)
    {
        SetStatusText(wxT("Unable to open file"));
        wxMessageDialog(this, wxT("Unable to open file"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
    }
	else printHexFile();
}

/*save the hexfile when already open, else perform a save_as*/
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

/*save the hex file with a file dialog*/
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

/*Write everything to the device*/
void UppMainWindowCallBack::upp_program()
{
	hardware->writeCode(readHexFile,picType);
	hardware->writeData(readHexFile,picType);
	hardware->writeConfig(readHexFile,picType);
}

/*read everything from the device*/
void UppMainWindowCallBack::upp_read()
{
	hardware->readCode(readHexFile,picType);
	hardware->readData(readHexFile,picType);
	hardware->readConfig(readHexFile,picType);
	printHexFile();
}

/*verify the device with the open hexfile*/
void UppMainWindowCallBack::upp_verify()
{
	SetStatusText(wxT("Verify not implemented yet"));
           
}

/*perform a bulk-erase on the current PIC*/
void UppMainWindowCallBack::upp_erase()
{
	hardware->bulkErase();
}

/*Check if the device is erased successfully*/
void UppMainWindowCallBack::upp_blankcheck()
{
	SetStatusText(wxT("Blankcheck not implemented yet"));
}

/*Detect which PIC is connected and select it in the combobox and the hardware*/
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

/*Connect usbpicprog to the usb port*/
bool UppMainWindowCallBack::upp_connect()
{
	hardware=new Hardware(this);
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

/*disconnect the hardware*/
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

/*load a browser with the usbpicprog website*/
void UppMainWindowCallBack::upp_help()
{
	wxLaunchDefaultBrowser(wxT("http://usbpicprog.sourceforge.net/"));
}

/*show an about box (only supported from wxWidgets 2.8.something+) */
void UppMainWindowCallBack::upp_about()
{
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
}

/*if the combo changed, also change it in the hardware*/
void UppMainWindowCallBack::upp_combo_changed()
{
		picType=new PicType(string(m_comboBox1->GetValue().mb_str(wxConvUTF8)));
		hardware->setPicType(picType);
    	upp_new();
}




