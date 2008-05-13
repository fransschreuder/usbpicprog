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
        SetStatusText(wxT("Unable to open file"),STATUS_FIELD_OTHER);
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
        SetStatusText(wxT("No file to refresh"),STATUS_FIELD_OTHER);
        return;
    }
	if(readHexFile->reload(picType)<0)
    {
        SetStatusText(wxT("Unable to open file"),STATUS_FIELD_OTHER);
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
            SetStatusText(wxT("Unable to save file"),STATUS_FIELD_OTHER);
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
            SetStatusText(wxT("Unable to save file"),STATUS_FIELD_OTHER);
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
	if(hardware->writeCode(readHexFile,picType)<0)
	{
        SetStatusText(wxT("Error programming code memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error programming code memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
	if(hardware->writeData(readHexFile,picType)<0)
	{
        SetStatusText(wxT("Error programming data memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error programming data memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
	if(hardware->writeConfig(readHexFile,picType)<0)
	{
        SetStatusText(wxT("Error programming config memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error programming config memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
}

/*read everything from the device*/
void UppMainWindowCallBack::upp_read()
{
	if(hardware->readCode(readHexFile,picType)<0)
	{
        SetStatusText(wxT("Error reading code memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error reading code memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
	if(hardware->readData(readHexFile,picType)<0)
	{
        SetStatusText(wxT("Error reading data memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error reading data memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
	if(hardware->readConfig(readHexFile,picType)<0)
	{
        SetStatusText(wxT("Error reading config memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error reading config memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
	printHexFile();
}

/*verify the device with the open hexfile*/
void UppMainWindowCallBack::upp_verify()
{
    wxString verifyText;
    ReadHexFile *verifyHexFile = new ReadHexFile;
    if(hardware->readCode(verifyHexFile,picType)<0)
	{
        SetStatusText(wxT("Error reading code memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error reading code memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
	if(hardware->readData(verifyHexFile,picType)<0)
	{
        SetStatusText(wxT("Error reading data memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error reading data memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
	if(hardware->readConfig(verifyHexFile,picType)<0)
	{
        SetStatusText(wxT("Error reading config memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error reading config memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
    if ((readHexFile->getCodeMemory().size()+
        readHexFile->getDataMemory().size()+
        readHexFile->getConfigMemory().size())>0)
    {
        for(int i=0;i<(signed)readHexFile->getCodeMemory().size();i++)
        {
            if((signed)verifyHexFile->getCodeMemory().size()<(i+1))
            {
                SetStatusText(wxT("Read code smaller than in file"),STATUS_FIELD_OTHER);
                wxMessageDialog(this, wxT("Read code smaller than in file"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
                return;
            }
            if(verifyHexFile->getCodeMemory()[i]!=readHexFile->getCodeMemory()[i])
            {
                verifyText.Printf(wxT("Verify code failed at 0x%X. Read: 0x%02X, File: 0x%02X"),i,verifyHexFile->getCodeMemory()[i],readHexFile->getCodeMemory()[i]);
                SetStatusText(verifyText,STATUS_FIELD_OTHER);
                wxMessageDialog(this, verifyText, wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
                return;
            }
            
        }
        for(int i=0;i<(signed)readHexFile->getDataMemory().size();i++)
        {
            if((signed)verifyHexFile->getDataMemory().size()<(i+1))
            {
                SetStatusText(wxT("Read data smaller than in file"),STATUS_FIELD_OTHER);
                wxMessageDialog(this, wxT("Read data smaller than in file"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
                return;
            }
            if(verifyHexFile->getDataMemory()[i]!=readHexFile->getDataMemory()[i])
            {
                verifyText.Printf(wxT("Verify data failed at 0x%X. Read: 0x%02X, File: 0x%02X"),i,verifyHexFile->getDataMemory()[i],readHexFile->getDataMemory()[i]);
                SetStatusText(verifyText,STATUS_FIELD_OTHER);
                wxMessageDialog(this, verifyText, wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
                return;
            }

        }
        for(int i=0;i<(signed)readHexFile->getConfigMemory().size();i++)
        {
            if((signed)verifyHexFile->getConfigMemory().size()<(i+1))
            {
                SetStatusText(wxT("Read config smaller than in file"),STATUS_FIELD_OTHER);
                wxMessageDialog(this, wxT("Read config smaller than in file"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
                return;
            }
            if(verifyHexFile->getConfigMemory()[i]!=readHexFile->getConfigMemory()[i])
            {
                verifyText.Printf(wxT("Verify config failed at 0x%X. Read: 0x%02X, File: 0x%02X"),picType->getCurrentPic().ConfigAddress+i,
                    verifyHexFile->getConfigMemory()[i],
                    readHexFile->getConfigMemory()[i]);
                SetStatusText(verifyText,STATUS_FIELD_OTHER);
                wxMessageDialog(this, verifyText, wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
                return;
            }

        }
        SetStatusText(wxT("Verified successfully"),STATUS_FIELD_OTHER);
        
    }
    else
    {
        SetStatusText(wxT("Nothing to verify"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Nothing to verify"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
    }
           
}

/*perform a bulk-erase on the current PIC*/
void UppMainWindowCallBack::upp_erase()
{
	if(hardware->bulkErase()<0)
	{
        SetStatusText(wxT("Error erasing the device"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error erasing the device"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
}

/*Check if the device is erased successfully*/
void UppMainWindowCallBack::upp_blankcheck()
{
    int readWord,verifyWord;
    if((picType->getCurrentPic().Name.find("P10")==0)|
        (picType->getCurrentPic().Name.find("P12")==0)|
        (picType->getCurrentPic().Name.find("P16")==0))verifyWord=0x3FFF;
    else  verifyWord=0xFFFF;
	
    wxString verifyText;
    ReadHexFile *verifyHexFile = new ReadHexFile;
    if(hardware->readCode(verifyHexFile,picType)<0)
	{
        SetStatusText(wxT("Error reading code memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error reading code memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
	if(hardware->readData(verifyHexFile,picType)<0)
	{
        SetStatusText(wxT("Error reading data memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error reading data memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
	if(hardware->readConfig(verifyHexFile,picType)<0)
	{
        SetStatusText(wxT("Error reading config memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error reading config memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
    if ((verifyHexFile->getCodeMemory().size()+
        verifyHexFile->getDataMemory().size()+
        verifyHexFile->getConfigMemory().size())>0)
    {
        for(int i=0;i<(signed)verifyHexFile->getCodeMemory().size();i+=2)
        {
            readWord=verifyHexFile->getCodeMemory()[i]|(verifyHexFile->getCodeMemory()[i+1]<<8);
            if(readWord!=verifyWord)
            {
                verifyText.Printf(wxT("Verify code failed at 0x%X. Read: 0x%04X, Expected: 0x%04X"),i,readWord,verifyWord);
                SetStatusText(verifyText,STATUS_FIELD_OTHER);
                wxMessageDialog(this, verifyText, wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
                return;
            }
        }
        for(int i=0;i<(signed)verifyHexFile->getDataMemory().size();i++)
        {
            if(verifyHexFile->getDataMemory()[i]!=0xFF)
            {
                verifyText.Printf(wxT("Verify data failed at 0x%X. Read: 0x%02X, Expected: 0x%02X"),i,verifyHexFile->getDataMemory()[i],0xFF);
                SetStatusText(verifyText,STATUS_FIELD_OTHER);
                wxMessageDialog(this, verifyText, wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
                return;
            }
        }
        SetStatusText(wxT("Device is blank"),STATUS_FIELD_OTHER);
    }
    else
    {
        SetStatusText(wxT("Error performing a blankcheck"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error performing a blankcheck"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
    }
}

/*Detect which PIC is connected and select it in the combobox and the hardware*/
bool UppMainWindowCallBack::upp_autodetect()
{
    int devId=hardware->autoDetectDevice();
	picType=new PicType(devId);
	hardware->setPicType(picType);
	m_comboBox1->SetValue(wxString::FromAscii(picType->getCurrentPic().Name.c_str()));
	if(devId>0)SetStatusText(wxString(wxT("Detected: ")).Append(wxString::FromAscii(picType->getCurrentPic().Name.c_str())),STATUS_FIELD_HARDWARE);
	else SetStatusText(wxT("No pic detected!"),STATUS_FIELD_HARDWARE);
	return (devId>0);
}

/*Connect usbpicprog to the usb port*/
bool UppMainWindowCallBack::upp_connect()
{
	hardware=new Hardware(this);
	if(hardware->connected())
	{
		upp_autodetect();
		char msg[64];
		if(hardware->getFirmwareVersion(msg)<0)
            SetStatusText(wxT("Unable to read firmware version"),STATUS_FIELD_HARDWARE);
        else
    		SetStatusText(wxString::FromAscii(msg).Append(wxT(" Connected")),STATUS_FIELD_HARDWARE);
    }
	else
	{
     	picType=new PicType(0);
    	hardware->setPicType(picType);
	    m_comboBox1->SetValue(wxString::FromAscii(picType->getCurrentPic().Name.c_str()));
		SetStatusText(wxT("Usbpicprog not found"),STATUS_FIELD_HARDWARE);
    }
    return hardware->connected();

}

/*disconnect the hardware*/
void UppMainWindowCallBack::upp_disconnect()
{
	if(hardware->connected())
	   {
		   delete hardware;
		   SetStatusText(wxT("Disconnected usbpicprog"),STATUS_FIELD_HARDWARE);
	   }
	   else
	   {
		   SetStatusText(wxT("Already disconnected"),STATUS_FIELD_HARDWARE);
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
#ifdef _WX_ABOUTDLG_H_
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
#else//_WX_ABOUTDLG_H_
	#ifndef UPP_VERSION
	wxMessageDialog(this, wxString(wxT("Usbpicprog (SVN) ")).Append(wxString::FromAscii(SVN_REVISION)), wxT("About"),  wxOK | wxICON_INFORMATION,  wxDefaultPosition).ShowModal();
	#else
	wxMessageDialog(this, wxString(wxT("Usbpicprog ")).Append(wxString::FromAscii(UPP_VERSION)), wxT("About"),  wxOK | wxICON_INFORMATION,  wxDefaultPosition).ShowModal();
	#endif
#endif//_WX_ABOUTDLG_H_
}

/*if the combo changed, also change it in the hardware*/
void UppMainWindowCallBack::upp_combo_changed()
{
		picType=new PicType(string(m_comboBox1->GetValue().mb_str(wxConvUTF8)));
		hardware->setPicType(picType);
    	upp_new();
}




