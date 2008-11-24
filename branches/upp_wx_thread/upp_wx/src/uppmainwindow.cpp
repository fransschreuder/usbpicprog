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


#include <wx/artprov.h>
#include "uppmainwindow_base.h"
#include "uppmainwindow.h"
#include "hexview.h"
#include "../svn_revision.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__) /*GTK needs bigger icons than Windows*/
#include "../icons/refresh.xpm"
#include "../icons/blankcheck.xpm"
#include "../icons/program.xpm"
#include "../icons/erase.xpm"
#include "../icons/read.xpm"
#include "../icons/verify.xpm"
#include "../icons/usbpicprog.xpm"
#else   /*Icons for Windows and Mac*/
#include "../icons/win/refresh.xpm"
#include "../icons/win/blankcheck.xpm"
#include "../icons/win/program.xpm"
#include "../icons/win/erase.xpm"
#include "../icons/win/read.xpm"
#include "../icons/win/verify.xpm"
#include "../icons/win/usbpicprog.xpm"
#endif


static const wxChar *FILETYPES = _T(
    "Hex files|*.hex|"
    "All files|*.*"
);

/*Do the basic initialization of the main window*/
UppMainWindow::UppMainWindow(wxWindow* parent, wxWindowID id, const wxString& title,
                             const wxPoint& pos, const wxSize& size, long style )
    : UppMainWindowBase( parent, id, title, pos, size, style )
{
    CompleteGUICreation();

    // non-GUI init:
    readHexFile=new ReadHexFile();
    picType=NULL;
    hardware=NULL;
    upp_connect();

    // append all PIC names to the choice control
    for(int i=0;i<(signed)picType->getPicNames().size();i++)
        m_pPICChoice->Append(wxString::FromAscii(picType->getPicNames()[i].c_str()));
    m_pPICChoice->SetStringSelection(wxT("P18F2550"));
    m_pPICChoice->InvalidateBestSize();
    m_pPICChoice->SetColumns(3);

    uppConfig=new wxConfig(wxT("usbpicprog"));

    if ( uppConfig->Read(wxT("DefaultPath"), &defaultPath) ) {}	else {defaultPath=wxT("");}
    if ( uppConfig->Read(wxT("ConfigProgramCode"), &configFields.ConfigProgramCode)){} else {configFields.ConfigProgramCode=true;}
    if ( uppConfig->Read(wxT("ConfigProgramConfig"), &configFields.ConfigProgramConfig)){} else {configFields.ConfigProgramConfig=true;}
    if ( uppConfig->Read(wxT("ConfigProgramData"), &configFields.ConfigProgramData)){} else {configFields.ConfigProgramData=true;}
    if ( uppConfig->Read(wxT("ConfigVerifyCode"), &configFields.ConfigVerifyCode)){} else {configFields.ConfigVerifyCode=true;}
    if ( uppConfig->Read(wxT("ConfigVerifyConfig"), &configFields.ConfigVerifyConfig)){} else {configFields.ConfigVerifyConfig=false;}
    if ( uppConfig->Read(wxT("ConfigVerifyData"), &configFields.ConfigVerifyData)){} else {configFields.ConfigVerifyData=true;}
    if ( uppConfig->Read(wxT("ConfigEraseBeforeProgramming"), &configFields.ConfigEraseBeforeProgramming)){} else {configFields.ConfigEraseBeforeProgramming=true;}

    fileOpened=false;
}

UppMainWindow::~UppMainWindow()
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

wxBitmap UppMainWindow::GetMenuBitmap(const char* xpm_data[])
{
    wxImage tmp(xpm_data);

#if wxCHECK_VERSION(2,9,0)
    wxSize sz = wxArtProvider::GetNativeSizeHint(wxART_MENU);
    tmp.Rescale(sz);
#else
    tmp.Rescale(16,16);
#endif

    return wxBitmap(tmp);
}

void UppMainWindow::CompleteGUICreation()
{
    // complete creation of the GUI creating the toolbar;
    // we can't let wxFormBuilder do it because it does not support wxArtProvider's usage
    wxToolBar* toolbar = this->CreateToolBar( wxTB_DOCKABLE|wxTB_HORIZONTAL, wxID_ANY );

    toolbar->AddTool( wxID_NEW, _("new"), wxArtProvider::GetBitmap(wxART_NEW,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("new"), _("Clear open hex file") );
    toolbar->AddTool( wxID_OPEN, _("open"), wxArtProvider::GetBitmap(wxART_FILE_OPEN,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("open"),  _("Open a hex file") );
    toolbar->AddTool( wxID_REFRESH, _("reload"), wxIcon(refresh_xpm), wxNullBitmap, wxITEM_NORMAL, _("reload"),  _("Reload the hex file") );
    toolbar->AddTool( wxID_SAVE, _("save"), wxArtProvider::GetBitmap(wxART_FILE_SAVE,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("save"), _("Save the hex file") );
    toolbar->AddTool( wxID_SAVE_AS, _("save as"), wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("save as"), _("Save the hex file as") );
    toolbar->AddSeparator();
    toolbar->AddTool( wxID_PROGRAM, _("program"), wxIcon( program_xpm ), wxNullBitmap, wxITEM_NORMAL, _("program"), _("Program the device") );
    toolbar->AddTool( wxID_READ, _("read"), wxIcon( read_xpm ), wxNullBitmap, wxITEM_NORMAL, _("read"), _("Read the device") );
    toolbar->AddTool( wxID_VERIFY, _("verify"), wxIcon( verify_xpm ), wxNullBitmap, wxITEM_NORMAL, _("verify"), _("Verify the device") );
    toolbar->AddTool( wxID_ERASE, _("erase"), wxIcon( erase_xpm ), wxNullBitmap, wxITEM_NORMAL, _("erase"), _("Erase the device") );
    toolbar->AddTool( wxID_BLANKCHECK, _("blankcheck"), wxIcon( blankcheck_xpm ), wxNullBitmap, wxITEM_NORMAL, _("blankcheck"), _("Blankcheck the device") );
    toolbar->AddSeparator();
    m_pPICChoice = new wxChoice(toolbar, wxID_ANY);
    toolbar->AddControl( m_pPICChoice );
/*
    toolbar->AddSeparator();
    m_radioButton_upp = new wxRadioButton( toolbar, wxID_ANY, _("Usbpicprog"), wxDefaultPosition, wxDefaultSize, 0);
    toolbar->AddControl(m_radioButton_upp);
    m_radioButton_boot = new wxRadioButton( toolbar, wxID_ANY, _("Bootloader"), wxDefaultPosition, wxDefaultSize, 0);
    toolbar->AddControl(m_radioButton_boot);
    */
    toolbar->Realize();

    // complete creation of the actions menu, by adding rescaled icons
    wxMenu* pMenuActions = new wxMenu();
    wxMenuItem* pMenuProgram;
    pMenuProgram = new wxMenuItem( pMenuActions, wxID_ANY, wxString( _("Program") ) + wxT('\t') + wxT("F7"), wxEmptyString, wxITEM_NORMAL );
    pMenuProgram->SetBitmap(GetMenuBitmap( program_xpm ));
    pMenuActions->Append( pMenuProgram );

    wxMenuItem* pMenuRead;
    pMenuRead = new wxMenuItem( pMenuActions, wxID_ANY, wxString( _("Read") ) + wxT('\t') + wxT("F8"), wxEmptyString, wxITEM_NORMAL );
    pMenuRead->SetBitmap(GetMenuBitmap( read_xpm ));
    pMenuActions->Append( pMenuRead );

    wxMenuItem* pMenuVerify;
    pMenuVerify = new wxMenuItem( pMenuActions, wxID_ANY, wxString( _("Verify") ) , wxEmptyString, wxITEM_NORMAL );
    pMenuVerify->SetBitmap(GetMenuBitmap( verify_xpm ));
    pMenuActions->Append( pMenuVerify );

    wxMenuItem* pMenuErase;
    pMenuErase = new wxMenuItem( pMenuActions, wxID_ANY, wxString( _("Erase") ) , wxEmptyString, wxITEM_NORMAL );
    pMenuErase->SetBitmap(GetMenuBitmap( erase_xpm ));
    pMenuActions->Append( pMenuErase );

    wxMenuItem* pMenuBlankCheck;
    pMenuBlankCheck = new wxMenuItem( pMenuActions, wxID_ANY, wxString( _("Blankcheck") ) , wxEmptyString, wxITEM_NORMAL );
    pMenuBlankCheck->SetBitmap(GetMenuBitmap( blankcheck_xpm ));
    pMenuActions->Append( pMenuBlankCheck );

    wxMenuItem* pMenuAutoDetect;
    pMenuAutoDetect = new wxMenuItem( pMenuActions, wxID_ANY, wxString( _("Autodetect") ) , wxEmptyString, wxITEM_NORMAL );
    pMenuAutoDetect->SetBitmap(GetMenuBitmap( blankcheck_xpm ));
    pMenuActions->Append( pMenuAutoDetect );

    m_pMenuBar->Insert(2, pMenuActions, _("Actions") );

    this->Connect( pMenuProgram->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_program ) );
    this->Connect( pMenuRead->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_read ) );
    this->Connect( pMenuVerify->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_verify ) );
    this->Connect( pMenuErase->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_erase ) );
    this->Connect( pMenuBlankCheck->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_blankcheck ) );
    this->Connect( pMenuAutoDetect->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_autodetect ) );

    this->SetIcon(wxIcon( usbpicprog_xpm ));
    this->SetSizerAndFit(m_pSizer);
}

/*Update the progress bar*/
void UppMainWindow::updateProgress(int value)
{
    //uppProgressBar->SetValue(value);
    Update(); //refresh the gui, also when busy
}

/*Put the contents of the hex file in the text area*/
void UppMainWindow::printHexFile()
{
    m_pCodeGrid->ShowHexFile(readHexFile,readHexFile->getCodeMemory(),picType);

/*
    //#if defined(__WXMSW__) || defined(__WXMAC__)
    int w=GetSize().GetWidth();
    int h=GetSize().GetHeight();
    SetSize(wxSize(w-1,h-1));
    SetSize(wxSize(w,h));
    //#endif
    #else
    string output;
    uppHexEdit->Freeze();
    uppHexEdit->Clear();
    readHexFile->print(&output,picType);
    uppHexEdit->AppendText(wxString::FromAscii(output.c_str()));
    uppHexEdit->SetFont(wxFont(12, wxMODERN, wxNORMAL,wxNORMAL));
    uppHexEdit->Thaw();
    uppHexEdit->SetSelection(0,0); // cursor to top
    #endif*/
}

/*clear the hexfile*/
void UppMainWindow::upp_new()
{
    readHexFile->newFile(picType);
    fileOpened=false;
    printHexFile();
}

/*Open a hexfile using a file dialog*/
void UppMainWindow::upp_open()
{
    wxFileDialog* openFileDialog =
        new wxFileDialog( this, _("Open hexfile"), defaultPath, wxT(""),
                          FILETYPES, wxFD_OPEN, wxDefaultPosition);

    if ( openFileDialog->ShowModal() == wxID_OK )
    {
        upp_open_file(openFileDialog->GetPath());
        defaultPath=openFileDialog->GetDirectory();
    }
}

/*Open a hexfile by filename*/
void UppMainWindow::upp_open_file(const wxString& path)
{
    if(readHexFile->open(picType,path.mb_str(wxConvUTF8))<0)
    {
        SetStatusText(_("Unable to open file"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, _("Unable to open file"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
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
void UppMainWindow::upp_refresh()
{
    if(!fileOpened)
    {
        SetStatusText(_("No file to refresh"),STATUS_FIELD_OTHER);
        return;
    }

    if(readHexFile->reload(picType)<0)
    {
        SetStatusText(_("Unable to open file"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, _("Unable to open file"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
    }
    else printHexFile();
}

/*save the hexfile when already open, else perform a save_as*/
void UppMainWindow::upp_save()
{
    if(fileOpened)
    {
        if(readHexFile->save(picType)<0)
        {
            SetStatusText(_("Unable to save file"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Unable to save file"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        }
    }
    else upp_save_as();
}

/*save the hex file with a file dialog*/
void UppMainWindow::upp_save_as()
{
    wxFileDialog* openFileDialog =
        new wxFileDialog( this, _("Save hexfile"), defaultPath, wxT(""),
                          FILETYPES, wxFD_SAVE, wxDefaultPosition);

    if ( openFileDialog->ShowModal() == wxID_OK )
    {
        if(readHexFile->saveAs(picType,openFileDialog->GetPath().mb_str(wxConvUTF8))<0)
        {
            SetStatusText(_("Unable to save file"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Unable to save file"), _("Error"),
                            wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            defaultPath=openFileDialog->GetDirectory();
        }
    }
}

void UppMainWindow::upp_exit()
{
    Close();
}

void UppMainWindow::upp_copy()
{
//    uppHexEdit->Copy();           FIXME
}

void UppMainWindow::upp_selectall()
{/* FIXME
    #ifdef USE_UPPHEXVIEW
    uppHexEdit->SelectAll();
    #else
    uppHexEdit->SetSelection(-1,-1);
    #endif */
}

/*Write everything to the device*/
void UppMainWindow::upp_program()
{
    if (hardware == NULL) return;
    if(configFields.ConfigEraseBeforeProgramming)
    {
        switch(hardware->bulkErase(picType))
        {
        case 1:
        SetStatusText(_("Erase OK"),STATUS_FIELD_OTHER);
        break;
        default:
        SetStatusText(_("Error erasing the device"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, _("Error erasing the device"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        break;

        }
    }
    if(configFields.ConfigProgramCode)
    {
        switch(hardware->writeCode(readHexFile,picType))
        {
        case 0:
                    SetStatusText(_("Write Code memory OK"),STATUS_FIELD_OTHER);
            break;
        case -1:
            SetStatusText(_("Hardware should say OK"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Hardware should say OK"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case -2:
            SetStatusText(_("Hardware should ask for next block"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Hardware should ask for next block"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case -3:
            SetStatusText(_("write code not implemented for current PIC"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("write code not implemented for current PIC"), wxT("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case -4:
            SetStatusText(_("Verify error while writing code memory"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Verify error while writing code memory"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case -5:
            SetStatusText(_("USB error while writing code memory"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("USB error while writing code memory"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        default:
            SetStatusText(_("Error programming code memory"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Error programming code memory"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        }
    }
    if(configFields.ConfigProgramConfig)
    {
        switch(hardware->writeData(readHexFile,picType))
        {
            case 0:
                    SetStatusText(_("Write Data memory OK"),STATUS_FIELD_OTHER);
            break;
        case -1:
            SetStatusText(_("Hardware should say OK"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Hardware should say OK"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case -2:
            SetStatusText(_("Hardware should ask for next block"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Hardware should ask for next block"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case -3:
            SetStatusText(_("write data not implemented for current PIC"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("write data not implemented for current PIC"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case -4:
            SetStatusText(_("USB error while writing code memory"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("USB error while writing code memory"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        default:
            SetStatusText(_("Error programming data memory"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Error programming data memory"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        }
    }
    if(configFields.ConfigProgramData)
    {
        switch(hardware->writeConfig(readHexFile,picType))
        {
        case 0:
                    SetStatusText(_("Write Config memory OK"),STATUS_FIELD_OTHER);
            break;
        case -1:
            SetStatusText(_("Hardware should say OK"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Hardware should say OK"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case -2:
            SetStatusText(_("Hardware should ask for next block"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Hardware should ask for next block"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case -3:
            SetStatusText(_("write config not implemented for current PIC"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("write config not implemented for current PIC"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case -4:
            SetStatusText(_("USB error while writing code memory"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("USB error while writing code memory"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        default:
            SetStatusText(_("Error programming config memory"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Error programming config memory"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        }
    }
    updateProgress(100);
    return;
}

/*read everything from the device*/
void UppMainWindow::upp_read()
{
    if (hardware == NULL) return;

    if(hardware->readCode(readHexFile,picType)<0)
    {
        SetStatusText(_("Error reading code memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, _("Error reading code memory"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        //return;
    }
    if(hardware->readData(readHexFile,picType)<0)
    {
        SetStatusText(_("Error reading data memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, _("Error reading data memory"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        //return;
    }
    if(hardware->readConfig(readHexFile,picType)<0)
    {
        SetStatusText(_("Error reading config memory"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, _("Error reading config memory"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        //return;
    }
    readHexFile->trimData(picType);
    printHexFile();
    updateProgress(100);
    return;
}

/*verify the device with the open hexfile*/
void UppMainWindow::upp_verify()
{
    if (hardware == NULL) return;

    wxString verifyText;
    wxString typeText;
    VerifyResult res=hardware->verify(readHexFile,picType,configFields.ConfigVerifyCode,configFields.ConfigVerifyConfig,configFields.ConfigVerifyData);
    switch(res.Result)
    {
        case VERIFY_SUCCESS:
            SetStatusText(_("Verify successful"),STATUS_FIELD_OTHER);
            break;
        case VERIFY_MISMATCH:

            switch (res.DataType)
            {
                case TYPE_CODE: typeText=_("Verify code");break;
                case TYPE_DATA: typeText=_("Verify data");break;
                case TYPE_CONFIG: typeText=_("Verify config");break;
                default: typeText=_("Verify unknown");break;
            }
            verifyText.Printf(_(" failed at 0x%X. Read: 0x%02X, Expected: 0x%02X"),
                res.Address+((res.DataType==TYPE_CONFIG)*picType->getCurrentPic().ConfigAddress),
                res.Read,
                            res.Expected);
            verifyText.Prepend(typeText);
            SetStatusText(verifyText,STATUS_FIELD_OTHER);
            wxMessageDialog(this, verifyText, _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case VERIFY_USB_ERROR:
            SetStatusText(_("USB error during verify"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("USB error during verify"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case VERIFY_OTHER_ERROR:
            SetStatusText(_("Unknown error during verify"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Unknown error during verify"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        default:
            SetStatusText(_("I'm sorry for being stupid"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("I'm sorry for being stupid"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
    }
    updateProgress(100);
    return;
}

/*perform a bulk-erase on the current PIC*/
void UppMainWindow::upp_erase()
{
    if (hardware == NULL) return;

    if(hardware->bulkErase(picType)<0)
    {
        SetStatusText(_("Error erasing the device"),STATUS_FIELD_OTHER);
        wxMessageDialog(this, _("Error erasing the device"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
        return;
    }
    updateProgress(100);
}

/*Check if the device is erased successfully*/
void UppMainWindow::upp_blankcheck()
{
    if (hardware == NULL) return;

    wxString verifyText;
    string typeText;
    VerifyResult res=hardware->blankCheck(picType);
    switch(res.Result)
    {
        case VERIFY_SUCCESS:
            SetStatusText(_("Device is blank"),STATUS_FIELD_OTHER);
            break;
        case VERIFY_MISMATCH:

            switch (res.DataType)
            {
                case TYPE_CODE: typeText=string("code");break;
                case TYPE_DATA: typeText=string("data");break;
                case TYPE_CONFIG: typeText=string("config");break;
                default: typeText=string("unknown");break;
            }
            verifyText.Printf(_("Blankcheck failed at 0x%X. Read: 0x%02X, Expected: 0x%02X"),
                res.Address+((res.DataType==TYPE_CONFIG)+picType->getCurrentPic().ConfigAddress),
                res.Read,
                res.Expected);
            SetStatusText(verifyText,STATUS_FIELD_OTHER);
            wxMessageDialog(this, verifyText, _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case VERIFY_USB_ERROR:
            SetStatusText(_("USB error during blankcheck"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("USB error during blankcheck"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        case VERIFY_OTHER_ERROR:
            SetStatusText(_("Unknown error during blankcheck"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("Unknown error during blankcheck"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
        default:
            SetStatusText(_("I'm sorry for being stupid"),STATUS_FIELD_OTHER);
            wxMessageDialog(this, _("I'm sorry for being stupid"), _("Error"),  wxOK | wxICON_ERROR,  wxDefaultPosition).ShowModal();
            break;
    }
    updateProgress(100);
}

/*Detect which PIC is connected and select it in the combobox and the hardware*/
bool UppMainWindow::upp_autodetect()
{
    if (hardware == NULL) return 0;

    int devId=hardware->autoDetectDevice();
    cout<<"ID: 0x"<<hex<<devId<<dec<<endl;
    picType=new PicType(devId);
    hardware->setPicType(picType);

    // FIXME: we know the index of the currently selected PIC; use SetSelection instead!
    m_pPICChoice->SetStringSelection(wxString::FromAscii(picType->getCurrentPic().Name.c_str()));

    if(devId>0)SetStatusText(wxString(_("Detected: ")).Append(wxString::FromAscii(picType->getCurrentPic().Name.c_str())),STATUS_FIELD_HARDWARE);
    else SetStatusText(_("No pic detected!"),STATUS_FIELD_HARDWARE);
    upp_new();
    return (devId>0);
}

/*Connect usbpicprog to the usb port*/
bool UppMainWindow::upp_connect()
{
    if (hardware != NULL) delete hardware;
    hardware=new Hardware(this, HW_UPP);
    if(hardware->connected())
    {
        upp_autodetect();
        char msg[64];
        if(hardware->getFirmwareVersion(msg)<0)
            SetStatusText(_("Unable to read version"),STATUS_FIELD_HARDWARE);
        else
            SetStatusText(wxString::FromAscii(msg).Trim().Append(_(" Connected")),STATUS_FIELD_HARDWARE);
        upp_new();
    }
    else
    {
        picType=new PicType(0);
        hardware->setPicType(picType);

        m_pPICChoice->SetStringSelection(wxString::FromAscii(picType->getCurrentPic().Name.c_str()));

        SetStatusText(_("Usbpicprog not found"),STATUS_FIELD_HARDWARE);
    }
    upp_update_hardware_type();
    return hardware->connected();
}

/*Connect bootloader to the usb port*/
bool UppMainWindow::upp_connect_boot()
{
    if (hardware != NULL) delete hardware;
    hardware=new Hardware(this, HW_BOOTLOADER);
    if(hardware->connected())
    {
        upp_autodetect();
        char msg[64];
        if(hardware->getFirmwareVersion(msg)<0)
            SetStatusText(_("Unable to read version"),STATUS_FIELD_HARDWARE);
        else
            SetStatusText(wxString::FromAscii(msg).Trim().Append(_(" Connected")),STATUS_FIELD_HARDWARE);
    }
    else
    {
        picType=new PicType(0);
        hardware->setPicType(picType);

        m_pPICChoice->SetStringSelection(wxString::FromAscii(picType->getCurrentPic().Name.c_str()));

        SetStatusText(_("Bootloader not found"),STATUS_FIELD_HARDWARE);
    }

    upp_update_hardware_type();
    return hardware->connected();
}

/*disconnect the hardware*/
void UppMainWindow::upp_disconnect()
{
    if(hardware != NULL)
    {
        if (hardware->connected())
        {
            delete hardware;
            hardware = NULL;
            SetStatusText(_("Disconnected usbpicprog"),STATUS_FIELD_HARDWARE);
        }
        else
        {
            SetStatusText(_("Already disconnected"),STATUS_FIELD_HARDWARE);
        }
    }
    else
    {
        SetStatusText(_("Already disconnected"),STATUS_FIELD_HARDWARE);
    }

    upp_update_hardware_type();
}

void UppMainWindow::upp_preferences()
{

    configFields.OkClicked=false;
    PreferencesDialog *preferencesDialog = new PreferencesDialog(this, wxID_ANY, _("Preferences"), wxPoint(-1,-1), wxSize(-1,-1));
    preferencesDialog->SetConfigFields(configFields);
    preferencesDialog->ShowModal();
    configFields=preferencesDialog->GetResult();

}

/*load a browser with the usbpicprog website*/
void UppMainWindow::upp_help()
{
    wxLaunchDefaultBrowser(wxT("http://usbpicprog.org/"));
}

/*show an about box (only supported from wxWidgets 2.8.something+) */
void UppMainWindow::upp_about()
{
#ifdef _WX_ABOUTDLG_H_
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName(wxT("Usbpicprog"));
    #ifndef UPP_VERSION
    aboutInfo.SetVersion(wxString(wxT("(SVN) ")).Append(wxString::FromAscii(SVN_REVISION)));
    #else
    aboutInfo.SetVersion(wxString::FromAscii(UPP_VERSION));
    #endif
    aboutInfo.SetDescription(_("An open source USB pic programmer"));
    aboutInfo.SetCopyright(wxT("(C) 2008 http://usbpicprog.org/"));
    wxAboutBox(aboutInfo);
#else//_WX_ABOUTDLG_H_
    #ifndef UPP_VERSION
    wxMessageDialog(this, wxString(wxT("Usbpicprog (SVN) ")).Append(wxString::FromAscii(SVN_REVISION)), _("About"),  wxOK | wxICON_INFORMATION,  wxDefaultPosition).ShowModal();
    #else
    wxMessageDialog(this, wxString(wxT("Usbpicprog ")).Append(wxString::FromAscii(UPP_VERSION)), _("About"),  wxOK | wxICON_INFORMATION,  wxDefaultPosition).ShowModal();
    #endif
#endif//_WX_ABOUTDLG_H_

}

/*if the combo changed, also change it in the hardware*/
void UppMainWindow::upp_combo_changed()
{
    if (hardware != NULL)
    {
        if(hardware->getCurrentHardware()==HW_BOOTLOADER)
        {
            //m_pPICChoice->Undo();  FIXME
            return;
        }
        picType=new PicType(string(m_pPICChoice->GetStringSelection().mb_str(wxConvUTF8)));
        hardware->setPicType(picType);
        upp_new();
    }
    else
    {
        picType=new PicType(string(m_pPICChoice->GetStringSelection().mb_str(wxConvUTF8)));
        upp_new();
    }

}

void UppMainWindow::upp_update_hardware_type()
{
    /* FIXME
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
    }*/
}

void UppMainWindow::OnSize(wxSizeEvent& event)
{
    wxRect rect;
    if(IsShown())
    {
        m_pStatusBar->GetFieldRect(STATUS_FIELD_PROGRESS, rect);

        // FIXME
        //uppProgressBar->SetPosition(rect.GetPosition());
        //uppProgressBar->SetSize(rect.GetSize());

    }
    this->Layout();
    event.Skip();
}

