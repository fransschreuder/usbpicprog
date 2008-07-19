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

	readHexFile=new ReadHexFile();
	picType=NULL;
	hardware=NULL;
	upp_connect();

	for(int i=0;i<(signed)picType->getPicNames().size();i++)
	{
		m_comboBox1->Append(wxString::FromAscii(picType->getPicNames()[i].c_str()));

	}
	uppConfig=new wxConfig(wxT("usbpicprog"));
	
  	if ( uppConfig->Read(wxT("DefaultPath"), &defaultPath) ) {}	else {defaultPath=wxT("");}
    if ( uppConfig->Read(wxT("ConfigProgramCode"), &configFields.ConfigProgramCode)){} else {configFields.ConfigProgramCode=true;}
    if ( uppConfig->Read(wxT("ConfigProgramConfig"), &configFields.ConfigProgramConfig)){} else {configFields.ConfigProgramConfig=true;}    
    if ( uppConfig->Read(wxT("ConfigProgramData"), &configFields.ConfigProgramData)){} else {configFields.ConfigProgramData=true;}    
    if ( uppConfig->Read(wxT("ConfigVerifyCode"), &configFields.ConfigVerifyCode)){} else {configFields.ConfigVerifyCode=true;}
    if ( uppConfig->Read(wxT("ConfigVerifyConfig"), &configFields.ConfigVerifyConfig)){} else {configFields.ConfigVerifyConfig=true;}
    if ( uppConfig->Read(wxT("ConfigVerifyData"), &configFields.ConfigVerifyData)){} else {configFields.ConfigVerifyData=true;}
    if ( uppConfig->Read(wxT("ConfigEraseBeforeProgramming"), &configFields.ConfigEraseBeforeProgramming)){} else {configFields.ConfigEraseBeforeProgramming=true;}
	fileOpened=false;
}

UppMainWindowCallBack::~UppMainWindowCallBack()
{
	uppConfig->Write(wxT("DefaultPath"), defaultPath);
	uppConfig->Write(wxT("ConfigProgramCode"), configFields.ConfigProgramCode);
    uppConfig->Write(wxT("ConfigProgramConfig"), configFields.ConfigProgramConfig);
    uppConfig->Write(wxT("ConfigProgramData"), configFields.ConfigProgramData);
    uppConfig->Write(wxT("ConfigVerifyCode"), configFields.ConfigVerifyCode);
    uppConfig->Write(wxT("ConfigVerifyConfig"), configFields.ConfigVerifyConfig);
    uppConfig->Write(wxT("ConfigVerifyData"), configFields.ConfigVerifyData);
    uppConfig->Write(wxT("ConfigEraseBeforeProgramming"), configFields.ConfigEraseBeforeProgramming);
	delete uppConfig;
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
#ifdef USE_UPPHEXVIEW
	uppHexEdit->putHexFile(readHexFile,picType);
	#if defined(__WXMSW__) || defined(__WXMAC__)
    int w=GetSize().GetWidth();
    int h=GetSize().GetHeight();
    SetSize(wxSize(w-1,h-1));
    SetSize(wxSize(w,h));
    #endif
#else
	string output;
	uppHexEdit->Freeze();
	uppHexEdit->Clear();	
	readHexFile->print(&output,picType);
	uppHexEdit->AppendText(wxString::FromAscii(output.c_str()));
	uppHexEdit->SetFont(wxFont(12, wxMODERN, wxNORMAL,wxNORMAL));
    uppHexEdit->Thaw();
	uppHexEdit->SetSelection(0,0); // cursor to top*/
#endif
}

/*clear the hexfile*/
void UppMainWindowCallBack::upp_new()
{

	readHexFile->newFile(picType);
	fileOpened=false;
	printHexFile();
}

/*Open a hexfile using a file dialog*/
void UppMainWindowCallBack::upp_open()
{
	//wxOpen is not defined in wxWidgets 2.9
	wxFileDialog* openFileDialog =
		new wxFileDialog( this, wxT("Open hexfile"), defaultPath, wxT(""), FILETYPES,
		                  wxOPEN, wxDefaultPosition);
	
 
	if ( openFileDialog->ShowModal() == wxID_OK )
	{
		upp_open_file(openFileDialog->GetPath());
		defaultPath=openFileDialog->GetDirectory();
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
	//wxSAVE is not defined in wxWidgets 2.9
	wxFileDialog* openFileDialog =
		new wxFileDialog( this, wxT("Save hexfile"), defaultPath, wxT(""), FILETYPES,
		                  wxSAVE, wxDefaultPosition);
	if ( openFileDialog->ShowModal() == wxID_OK )
	{
		if(readHexFile->saveAs(picType,openFileDialog->GetPath().mb_str(wxConvUTF8))<0)
		{
            SetStatusText(wxT("Unable to save file"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, wxT("Unable to save file"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
			defaultPath=openFileDialog->GetDirectory();
        }
	}
}

void UppMainWindowCallBack::upp_exit()
{
	Close();
}

void UppMainWindowCallBack::upp_copy()
{
    uppHexEdit->Copy();
}

void UppMainWindowCallBack::upp_selectall()
{
    #ifdef USE_UPPHEXVIEW
    uppHexEdit->SelectAll();
    #else
    uppHexEdit->SetSelection(-1,-1);
    #endif
}

/*Write everything to the device*/
void UppMainWindowCallBack::upp_program()
{
	if (hardware == NULL) return;
	if(configFields.ConfigEraseBeforeProgramming)
	{
    	if(hardware->bulkErase()<0)
    	{
    		SetStatusText(wxT("Error erasing the device"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, wxT("Error erasing the device"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            return;
    	}
    }
    if(configFields.ConfigProgramCode)
	{
    	if(hardware->writeCode(readHexFile,picType)<0)
    	{
            SetStatusText(wxT("Error programming code memory"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, wxT("Error programming code memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            return;
        }
    }
    if(configFields.ConfigProgramConfig)
	{
    	if(hardware->writeData(readHexFile,picType)<0)
    	{
            SetStatusText(wxT("Error programming data memory"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, wxT("Error programming data memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            return;
        }
    }
    if(configFields.ConfigProgramData)
	{
    	if(hardware->writeConfig(readHexFile,picType)<0)
    	{
            SetStatusText(wxT("Error programming config memory"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, wxT("Error programming config memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            return;
        }
    }
	updateProgress(100);
	return;
}

/*read everything from the device*/
void UppMainWindowCallBack::upp_read()
{
	if (hardware == NULL) return;
	
	if(hardware->readCode(readHexFile,picType)<0)
	{
        SetStatusText(wxT("Error reading code memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error reading code memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        //return;
    }
	if(hardware->readData(readHexFile,picType)<0)
	{
        SetStatusText(wxT("Error reading data memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error reading data memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        //return;
    }
	if(hardware->readConfig(readHexFile,picType)<0)
	{
        SetStatusText(wxT("Error reading config memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error reading config memory"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        //return;
    }
	readHexFile->trimData(picType);
	printHexFile();
	updateProgress(100);
	return;
}

/*verify the device with the open hexfile*/
void UppMainWindowCallBack::upp_verify()
{
	if (hardware == NULL) return;
	
    wxString verifyText;
	wxString typeText;
    VerifyResult res=hardware->verify(readHexFile,picType,configFields.ConfigVerifyCode,configFields.ConfigVerifyConfig,configFields.ConfigVerifyData);
    switch(res.Result)
    {
        case VERIFY_SUCCESS:
            SetStatusText(wxT("Verify successful"),STATUS_FIELD_OTHER);
            break;
        case VERIFY_MISMATCH:
            
            switch (res.DataType)
            {
                case TYPE_CODE: typeText=wxT("Verify code");break;
                case TYPE_DATA: typeText=wxT("Verify data");break;
                case TYPE_CONFIG: typeText=wxT("Verify config");break;
                default: typeText=wxT("Verify unknown");break;
            }
            verifyText.Printf(wxT(" failed at 0x%X. Read: 0x%02X, Expected: 0x%02X"),
                res.Address+((res.DataType==TYPE_CONFIG)*picType->getCurrentPic().ConfigAddress),
                res.Read,
							  res.Expected);
			verifyText.Prepend(typeText);
            SetStatusText(verifyText,STATUS_FIELD_OTHER);
            wxMessageDialog(this, verifyText, wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case VERIFY_USB_ERROR:
            SetStatusText(wxT("USB error during verify"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, wxT("USB error during verify"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case VERIFY_OTHER_ERROR:
            SetStatusText(wxT("Unknown error during verify"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, wxT("Unknown error during verify"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        default:
            SetStatusText(wxT("I'm sorry for being stupid"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, wxT("I'm sorry for being stupid"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
    }
	updateProgress(100);
	return;
}

/*perform a bulk-erase on the current PIC*/
void UppMainWindowCallBack::upp_erase()
{
	if (hardware == NULL) return;
	
	if(hardware->bulkErase()<0)
	{
        SetStatusText(wxT("Error erasing the device"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, wxT("Error erasing the device"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
	updateProgress(100);
}

/*Check if the device is erased successfully*/
void UppMainWindowCallBack::upp_blankcheck()
{
	if (hardware == NULL) return;
	
    wxString verifyText;
    string typeText;
    VerifyResult res=hardware->blankCheck(picType);
    switch(res.Result)
    {
        case VERIFY_SUCCESS:
            SetStatusText(wxT("Device is blank"),STATUS_FIELD_OTHER);
            break;
        case VERIFY_MISMATCH:

            switch (res.DataType)
            {
                case TYPE_CODE: typeText=string("code");break;
                case TYPE_DATA: typeText=string("data");break;
                case TYPE_CONFIG: typeText=string("config");break;
                default: typeText=string("unknown");break;
            }
            verifyText.Printf(wxT("Blankcheck %s failed at 0x%X. Read: 0x%02X, Expected: 0x%02X"),
                typeText.c_str(),
                res.Address+((res.DataType==TYPE_CONFIG)+picType->getCurrentPic().ConfigAddress),
                res.Read,
                res.Expected);
            SetStatusText(verifyText,STATUS_FIELD_OTHER);
            wxMessageDialog(this, verifyText, wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case VERIFY_USB_ERROR:
            SetStatusText(wxT("USB error during blankcheck"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, wxT("USB error during blankcheck"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case VERIFY_OTHER_ERROR:
            SetStatusText(wxT("Unknown error during blankcheck"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, wxT("Unknown error during blankcheck"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        default:
            SetStatusText(wxT("I'm sorry for being stupid"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, wxT("I'm sorry for being stupid"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
    }
	updateProgress(100);
}

/*Detect which PIC is connected and select it in the combobox and the hardware*/
bool UppMainWindowCallBack::upp_autodetect()
{
	if (hardware == NULL) return 0;
	
	int devId=hardware->autoDetectDevice();
	cout<<"ID: 0x"<<hex<<devId<<dec<<endl;
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
	if (hardware != NULL) delete hardware;
	hardware=new Hardware(this, HW_UPP);
	if(hardware->connected())
	{
		upp_autodetect();
		char msg[64];
		if(hardware->getFirmwareVersion(msg)<0)
            SetStatusText(wxT("Unable to read version"),STATUS_FIELD_HARDWARE);
        else
    		SetStatusText(wxString::FromAscii(msg).Trim().Append(wxT(" Connected")),STATUS_FIELD_HARDWARE);
    }
	else
	{
     	picType=new PicType(0);
    	hardware->setPicType(picType);
	    m_comboBox1->SetValue(wxString::FromAscii(picType->getCurrentPic().Name.c_str()));
		SetStatusText(wxT("Usbpicprog not found"),STATUS_FIELD_HARDWARE);
    }
	
	upp_update_hardware_type();
    return hardware->connected();
}

/*Connect bootloader to the usb port*/
bool UppMainWindowCallBack::upp_connect_boot()
{
	if (hardware != NULL) delete hardware;
	hardware=new Hardware(this, HW_BOOTLOADER);
	if(hardware->connected())
	{
		upp_autodetect();
		char msg[64];
		if(hardware->getFirmwareVersion(msg)<0)
            SetStatusText(wxT("Unable to read version"),STATUS_FIELD_HARDWARE);
        else
    		SetStatusText(wxString::FromAscii(msg).Trim().Append(wxT(" Connected")),STATUS_FIELD_HARDWARE);
    }
	else
	{
     	picType=new PicType(0);
    	hardware->setPicType(picType);
	    m_comboBox1->SetValue(wxString::FromAscii(picType->getCurrentPic().Name.c_str()));
		SetStatusText(wxT("Bootloader not found"),STATUS_FIELD_HARDWARE);
    }
	
	upp_update_hardware_type();
    return hardware->connected();
}

/*disconnect the hardware*/
void UppMainWindowCallBack::upp_disconnect()
{
	if(hardware != NULL)
	{
		if (hardware->connected())
		{
			delete hardware;
			hardware = NULL;
			SetStatusText(wxT("Disconnected usbpicprog"),STATUS_FIELD_HARDWARE);	
		}
		else
		{
			SetStatusText(wxT("Already disconnected"),STATUS_FIELD_HARDWARE);
		}
	}
	else
	{
		SetStatusText(wxT("Already disconnected"),STATUS_FIELD_HARDWARE);
	}
	
	upp_update_hardware_type();
}

void UppMainWindowCallBack::upp_preferences()
{

    configFields.OkClicked=false;
    PreferencesDialog *preferencesDialog = new PreferencesDialog(this, wxID_ANY, wxT("Preferences"), wxPoint(-1,-1), wxSize(-1,-1));
    preferencesDialog->SetConfigFields(configFields);
    preferencesDialog->ShowModal();
    configFields=preferencesDialog->GetResult();
    
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
	if (hardware != NULL)
	{
		picType=new PicType(string(m_comboBox1->GetValue().mb_str(wxConvUTF8)));
		hardware->setPicType(picType);
    	upp_new();
	}
}

void UppMainWindowCallBack::upp_update_hardware_type()
{
	if (hardware != NULL && hardware->getHardwareType() != HW_NONE)
	{
		if (hardware->getHardwareType() == HW_UPP)
		{
			m_radioButton_upp->SetValue(true);
			m_radioButton_boot->SetValue(false);		
		}
		else if (hardware->getHardwareType() == HW_BOOTLOADER)
		{
			m_radioButton_boot->SetValue(true);
			m_radioButton_upp->SetValue(false);
		}
	}
	else
	{
		m_radioButton_boot->SetValue(false);
		m_radioButton_upp->SetValue(false);
	}
}

void UppMainWindowCallBack::OnSize(wxSizeEvent& event)
{
	wxRect rect;
	if(IsShown())
	{
		m_statusBar1->GetFieldRect(STATUS_FIELD_PROGRESS, rect);

		uppProgressBar->SetPosition(rect.GetPosition());
		uppProgressBar->SetSize(rect.GetSize());
		
	}
	this->Layout();
	//if(uppHexEdit->OnResize())
	//		uppHexEdit->putHexFile(readHexFile);
	event.Skip();

}

