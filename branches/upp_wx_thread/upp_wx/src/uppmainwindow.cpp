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
#include <wx/toolbar.h>
#include <wx/choice.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/event.h>
#include <wx/msgdlg.h>

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

wxString wxGetPicName(PicType* pt)
{
    return wxString::FromAscii(pt->getCurrentPic().Name.c_str());
}



// UPPMAINWINDOW
// =============================================================================


/*Do the basic initialization of the main window*/
UppMainWindow::UppMainWindow(wxWindow* parent, wxWindowID id)
    : UppMainWindowBase( parent, id, wxEmptyString /* will be set later */,
                         wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL ),
      m_history(4), m_picType(0)
{
    // load settings
    uppConfig=new wxConfig(wxT("usbpicprog"));
    uppConfig->SetPath(wxT("/"));
    if ( uppConfig->Read(wxT("DefaultPath"), &defaultPath) ) {}	else {defaultPath=wxT("");}
    if ( uppConfig->Read(wxT("ConfigProgramCode"), &configFields.ConfigProgramCode)){} else {configFields.ConfigProgramCode=true;}
    if ( uppConfig->Read(wxT("ConfigProgramConfig"), &configFields.ConfigProgramConfig)){} else {configFields.ConfigProgramConfig=true;}
    if ( uppConfig->Read(wxT("ConfigProgramData"), &configFields.ConfigProgramData)){} else {configFields.ConfigProgramData=true;}
    if ( uppConfig->Read(wxT("ConfigVerifyCode"), &configFields.ConfigVerifyCode)){} else {configFields.ConfigVerifyCode=true;}
    if ( uppConfig->Read(wxT("ConfigVerifyConfig"), &configFields.ConfigVerifyConfig)){} else {configFields.ConfigVerifyConfig=false;}
    if ( uppConfig->Read(wxT("ConfigVerifyData"), &configFields.ConfigVerifyData)){} else {configFields.ConfigVerifyData=true;}
    if ( uppConfig->Read(wxT("ConfigEraseBeforeProgramming"), &configFields.ConfigEraseBeforeProgramming)){} else {configFields.ConfigEraseBeforeProgramming=true;}
    m_history.Load(*uppConfig);

    // non-GUI init:
    //m_picType=NULL;       // means that there is no known PIC connected!
    m_hardware=NULL;      // upp_connect() will allocate it

    // GUI init:
    CompleteGUICreation();

    upp_connect();
}

UppMainWindow::~UppMainWindow()
{
    // save settings
    uppConfig->SetPath(wxT("/"));
    uppConfig->Write(wxT("DefaultPath"), defaultPath);
    uppConfig->Write(wxT("ConfigProgramCode"), configFields.ConfigProgramCode);
    uppConfig->Write(wxT("ConfigProgramConfig"), configFields.ConfigProgramConfig);
    uppConfig->Write(wxT("ConfigProgramData"), configFields.ConfigProgramData);
    uppConfig->Write(wxT("ConfigVerifyCode"), configFields.ConfigVerifyCode);
    uppConfig->Write(wxT("ConfigVerifyConfig"), configFields.ConfigVerifyConfig);
    uppConfig->Write(wxT("ConfigVerifyData"), configFields.ConfigVerifyData);
    uppConfig->Write(wxT("ConfigEraseBeforeProgramming"), configFields.ConfigEraseBeforeProgramming);
    m_history.Save(*uppConfig);

    delete uppConfig;
}

void UppMainWindow::UpdateTitle()
{
    wxString str;

    #ifndef UPP_VERSION
    str = wxString(_("Usbpicprog rev: ")).Append(wxString::FromAscii(SVN_REVISION));
    #else
    str = wxString(_("Usbpicprog: ")).Append(wxString::FromAscii(UPP_VERSION));
    #endif

    if (!m_hexFile.hasFileName())
        str += wxT(" - [") + wxString(_("untitled"));
    else
        str += wxT(" - [") + wxString::FromAscii(m_hexFile.getFileName());

    if (m_hexFile.wasModified())
        str += wxT(" *");

    SetTitle(str + wxT("]"));
}

/* returns a bitmap suitable for UppMainWindow menu items */
wxBitmap UppMainWindow::GetMenuBitmap(const char* xpm_data[])
{
    wxImage tmp(xpm_data);

#if wxCHECK_VERSION(2,9,0)
    wxSize sz = wxArtProvider::GetNativeSizeHint(wxART_MENU);
    tmp.Rescale(sz.GetWidth(), sz.GetHeight());
#else
    tmp.Rescale(16,16);
#endif

    return wxBitmap(tmp);
}

/* completes UppMainWindow GUI creation started by wxFormBuilder-generated code */
void UppMainWindow::CompleteGUICreation()
{
    // create the actions menu with rescaled icons
    wxMenu* pMenuActions = new wxMenu();
    wxMenuItem* pMenuProgram;
    pMenuProgram = new wxMenuItem( pMenuActions, wxID_PROGRAM, wxString( _("&Program...") ) + wxT('\t') + wxT("F7"),
                                   _("Program the device"), wxITEM_NORMAL );
    pMenuProgram->SetBitmap(GetMenuBitmap( program_xpm ));
    pMenuActions->Append( pMenuProgram );

    wxMenuItem* pMenuRead;
    pMenuRead = new wxMenuItem( pMenuActions, wxID_READ, wxString( _("&Read...") ) + wxT('\t') + wxT("F8"),
                                _("Read the device"), wxITEM_NORMAL );
    pMenuRead->SetBitmap(GetMenuBitmap( read_xpm ));
    pMenuActions->Append( pMenuRead );

    wxMenuItem* pMenuVerify;
    pMenuVerify = new wxMenuItem( pMenuActions, wxID_VERIFY, wxString( _("&Verify...") ),
                                  _("Verify the device"), wxITEM_NORMAL );
    pMenuVerify->SetBitmap(GetMenuBitmap( verify_xpm ));
    pMenuActions->Append( pMenuVerify );

    wxMenuItem* pMenuErase;
    pMenuErase = new wxMenuItem( pMenuActions, wxID_ERASE, wxString( _("&Erase...") ),
                                 _("Erase the device"), wxITEM_NORMAL );
    pMenuErase->SetBitmap(GetMenuBitmap( erase_xpm ));
    pMenuActions->Append( pMenuErase );

    wxMenuItem* pMenuBlankCheck;
    pMenuBlankCheck = new wxMenuItem( pMenuActions, wxID_BLANKCHECK, wxString( _("&Blankcheck...") ),
                                      _("Blankcheck the device"), wxITEM_NORMAL );
    pMenuBlankCheck->SetBitmap(GetMenuBitmap( blankcheck_xpm ));
    pMenuActions->Append( pMenuBlankCheck );

    wxMenuItem* pMenuAutoDetect;
    pMenuAutoDetect = new wxMenuItem( pMenuActions, wxID_AUTODETECT, wxString( _("&Autodetect...") ),
                                      _("Detect the device"), wxITEM_NORMAL );
    pMenuAutoDetect->SetBitmap(GetMenuBitmap( blankcheck_xpm ));
    pMenuActions->Append( pMenuAutoDetect );

    m_pMenuBar->Insert(2, pMenuActions, _("&Actions") );

    this->Connect( wxID_PROGRAM, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_program ) );
    this->Connect( wxID_READ, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_read ) );
    this->Connect( wxID_VERIFY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_verify ) );
    this->Connect( wxID_ERASE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_erase ) );
    this->Connect( wxID_BLANKCHECK, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_blankcheck ) );
    this->Connect( wxID_AUTODETECT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_autodetect ) );

    // create the most-recently-used section in File menu
    m_history.UseMenu(m_pMenuFile);
    m_history.AddFilesToMenu();

    this->Connect( wxID_FILE1, wxID_FILE9, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_mru ) );

    // complete creation of the GUI creating the toolbar;
    // we can't let wxFormBuilder do it because it does not support wxArtProvider's usage
    wxToolBar* toolbar = this->CreateToolBar( wxTB_DOCKABLE|wxTB_HORIZONTAL, wxID_ANY );

    toolbar->AddTool( wxID_NEW, _("new"), wxArtProvider::GetBitmap(wxART_NEW,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("new"),
                      GetMenuBar()->FindItem(wxID_NEW)->GetHelp() );
    toolbar->AddTool( wxID_OPEN, _("open"), wxArtProvider::GetBitmap(wxART_FILE_OPEN,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("open"),
                      GetMenuBar()->FindItem(wxID_OPEN)->GetHelp() );
    toolbar->AddTool( wxID_REFRESH, _("reload"), wxIcon(refresh_xpm), wxNullBitmap, wxITEM_NORMAL, _("reload"),
                      GetMenuBar()->FindItem(wxID_REFRESH)->GetHelp() );
    toolbar->AddTool( wxID_SAVE, _("save"), wxArtProvider::GetBitmap(wxART_FILE_SAVE,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("save"),
                      GetMenuBar()->FindItem(wxID_SAVE)->GetHelp() );
    toolbar->AddTool( wxID_SAVEAS, _("save as"), wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("save as"),
                      GetMenuBar()->FindItem(wxID_SAVEAS)->GetHelp() );
    toolbar->AddSeparator();
    toolbar->AddTool( wxID_PROGRAM, _("program"), wxIcon( program_xpm ), wxNullBitmap, wxITEM_NORMAL, _("program"),
                      GetMenuBar()->FindItem(wxID_PROGRAM)->GetHelp() );
    toolbar->AddTool( wxID_READ, _("read"), wxIcon( read_xpm ), wxNullBitmap, wxITEM_NORMAL, _("read"),
                      GetMenuBar()->FindItem(wxID_READ)->GetHelp() );
    toolbar->AddTool( wxID_VERIFY, _("verify"), wxIcon( verify_xpm ), wxNullBitmap, wxITEM_NORMAL, _("verify"),
                      GetMenuBar()->FindItem(wxID_VERIFY)->GetHelp() );
    toolbar->AddTool( wxID_ERASE, _("erase"), wxIcon( erase_xpm ), wxNullBitmap, wxITEM_NORMAL, _("erase"),
                      GetMenuBar()->FindItem(wxID_ERASE)->GetHelp() );
    toolbar->AddTool( wxID_BLANKCHECK, _("blankcheck"), wxIcon( blankcheck_xpm ), wxNullBitmap, wxITEM_NORMAL, _("blankcheck"),
                      GetMenuBar()->FindItem(wxID_BLANKCHECK)->GetHelp() );
    toolbar->AddSeparator();

    m_pPICChoice = new wxChoice(toolbar, wxID_PIC_CHOICE, wxDefaultPosition, wxSize(120,-1));
    this->Connect( wxID_PIC_CHOICE, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UppMainWindow::on_choice_changed ) );

    toolbar->AddControl( m_pPICChoice );

/* FIXME
    toolbar->AddSeparator();
    m_radioButton_upp = new wxRadioButton( toolbar, wxID_ANY, _("Usbpicprog"), wxDefaultPosition, wxDefaultSize, 0);
    toolbar->AddControl(m_radioButton_upp);
    m_radioButton_boot = new wxRadioButton( toolbar, wxID_ANY, _("Bootloader"), wxDefaultPosition, wxDefaultSize, 0);
    toolbar->AddControl(m_radioButton_boot);
    */
    toolbar->Realize();

    // by default show code page at startup
    m_pNotebook->ChangeSelection(PAGE_CODE);

    // make 2nd pane wide the half of the 1st pane:
    const int widths[] = { -2, -1 };
    m_pStatusBar->SetStatusWidths(2, widths);

    // append all PIC names to the choice control
    vector<string> arr = m_picType.getPicNames();
    for(unsigned int i=0;i<arr.size();i++)
        m_pPICChoice->Append(wxString::FromAscii(arr[i].c_str()));

    this->SetIcon(wxIcon( usbpicprog_xpm ));
    this->SetSizerAndFit(m_pSizer);

    // misc event handlers
    this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( UppMainWindow::on_close ) );
    this->Connect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( UppMainWindow::on_cell_changed ), NULL, this );

    // set default title name
    UpdateTitle();

    // show default stuff
    UpdateGrids();
}

/* returns a pointer to the grid currently open */
UppHexViewGrid* UppMainWindow::GetCurrentGrid() const
{
    switch (m_pNotebook->GetSelection())
    {
    case PAGE_CODE:
        return m_pCodeGrid;
    case PAGE_CONFIG:
        return m_pConfigGrid;
    case PAGE_DATA:
        return m_pDataGrid;

    default:
        return NULL;
    }
}

/*Update the progress bar; this function is called by m_hardware */
void UppMainWindow::updateProgress(int value)
{
    //uppProgressBar->SetValue(value); FIXME
    Update(); //refresh the gui, also when busy
}

/*Put the contents of the hex file in the text area*/
void UppMainWindow::UpdateGrids()
{
    m_pCodeGrid->ShowHexFile(&m_hexFile,&m_picType);
    m_pConfigGrid->ShowHexFile(&m_hexFile,&m_picType);
    m_pDataGrid->ShowHexFile(&m_hexFile,&m_picType);
}

bool UppMainWindow::ShoudContinueIfUnsaved()
{
    if (!m_hexFile.wasModified())
        return true;    // continue

    wxString msg = wxString::Format(
        _("The HEX file '%s' has not been saved... its content will be lost; continue?"),
        m_hexFile.hasFileName() ? m_hexFile.getFileName() : "untitled");

    if ( wxMessageBox(msg, _("Please confirm"),
                        wxICON_QUESTION | wxYES_NO, this) == wxYES )
        return true;    // continue...

    // don't continue!
    return false;
}



// UPPMAINWINDOW - event handlers
// =============================================================================

/*Open a hexfile using the most-recently-used menu items*/
void UppMainWindow::on_mru(wxCommandEvent& event)
{
    upp_open_file(m_history.GetHistoryFile(event.GetId() - wxID_FILE1));
}

void UppMainWindow::on_close(wxCloseEvent& event)
{
    if ( event.CanVeto() )
        if (!ShoudContinueIfUnsaved())
        {
            // user replied "do not continue"
            event.Veto();
            return;
        }

    Destroy();
}



// UPPMAINWINDOW - event handlers without event argument
// =============================================================================

/*The user touched one of the code/data/config grids*/
void UppMainWindow::upp_cell_changed()
{
    // m_hexFile has been automatically modified by the UppHexViewGrid!
    UpdateTitle();
}

/*clear the hexfile*/
void UppMainWindow::upp_new()
{
    if (!ShoudContinueIfUnsaved())
        return;

    m_hexFile.newFile(&m_picType);

    UpdateGrids();
    UpdateTitle();
}

/*Open a hexfile using a file dialog*/
void UppMainWindow::upp_open()
{
    wxFileDialog* openFileDialog =
        new wxFileDialog( this, _("Open hexfile"), defaultPath, wxT(""),
                          FILETYPES, wxFD_OPEN, wxDefaultPosition);

    if ( openFileDialog->ShowModal() == wxID_OK )
    {
        // get the folder of the opened file, without the name&extension
        defaultPath=wxFileName(openFileDialog->GetPath()).GetPath();

        upp_open_file(openFileDialog->GetPath());
    }
}

/*Open a hexfile by filename*/
bool UppMainWindow::upp_open_file(const wxString& path)
{
    if(m_hexFile.open(&m_picType,path.mb_str(wxConvUTF8))<0)
    {
        SetStatusText(_("Unable to open file"),STATUS_FIELD_OTHER);
        wxLogError(_("Unable to open file"));
        return false;
    }
    else
    {
        UpdateGrids();
        UpdateTitle();
        m_history.AddFileToHistory(path);

        return true;
    }
}

/*re-open the hexfile*/
void UppMainWindow::upp_refresh()
{
    if(!m_hexFile.hasFileName())
    {
        SetStatusText(_("No file to refresh"),STATUS_FIELD_OTHER);
        return;
    }

    if(m_hexFile.reload(&m_picType)<0)
    {
        SetStatusText(_("Unable to open file"),STATUS_FIELD_OTHER);
        wxLogError(_("Unable to open file"));
    }
    else
    {
        UpdateGrids();
        UpdateTitle();
    }
}

/*save the hexfile when already open, else perform a save_as*/
void UppMainWindow::upp_save()
{
    if(m_hexFile.hasFileName())
    {
        if(m_hexFile.save(&m_picType)<0)
        {
            SetStatusText(_("Unable to save file"),STATUS_FIELD_OTHER);
            wxLogError(_("Unable to save file"));
        }
        else
            UpdateTitle();
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
        // get the folder of the opened file, without the name&extension
        defaultPath=wxFileName(openFileDialog->GetPath()).GetPath();

        if(m_hexFile.saveAs(&m_picType,openFileDialog->GetPath().mb_str(wxConvUTF8))<0)
        {
            SetStatusText(_("Unable to save file"),STATUS_FIELD_OTHER);
            wxLogError(_("Unable to save file"));
        }
        else
            UpdateTitle();
    }
}

void UppMainWindow::upp_exit()
{
    Close();
}

void UppMainWindow::upp_copy()
{
    UppHexViewGrid *grid = GetCurrentGrid();
    if (grid)
        grid->Copy();
}

void UppMainWindow::upp_selectall()
{
    UppHexViewGrid *grid = GetCurrentGrid();
    if (grid)
        grid->SelectAll();
}

/*Write everything to the device*/
void UppMainWindow::upp_program()
{
    if (m_hardware == NULL) return;

    if (!m_hardware->connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    if(configFields.ConfigEraseBeforeProgramming)
    {
        switch(m_hardware->bulkErase(&m_picType))
        {
        case 1:
            SetStatusText(_("Erase OK"),STATUS_FIELD_OTHER);
            break;
        default:
            SetStatusText(_("Error erasing the device"),STATUS_FIELD_OTHER);
            wxLogError(_("Error erasing the device"));
            break;
        }
    }

    if(configFields.ConfigProgramCode)
    {
        switch(m_hardware->writeCode(&m_hexFile,&m_picType))
        {
        case 0:
            SetStatusText(_("Write Code memory OK"),STATUS_FIELD_OTHER);
            break;
        case -1:
            SetStatusText(_("m_hardware should say OK"),STATUS_FIELD_OTHER);
            wxLogError(_("m_hardware should say OK"));
            break;
        case -2:
            SetStatusText(_("m_hardware should ask for next block"),STATUS_FIELD_OTHER);
            wxLogError(_("m_hardware should ask for next block"));
            break;
        case -3:
            SetStatusText(_("write code not implemented for current PIC"),STATUS_FIELD_OTHER);
            wxLogError(_("write code not implemented for current PIC"));
            break;
        case -4:
            SetStatusText(_("Verify error while writing code memory"),STATUS_FIELD_OTHER);
            wxLogError(_("Verify error while writing code memory"));
            break;
        case -5:
            SetStatusText(_("USB error while writing code memory"),STATUS_FIELD_OTHER);
            wxLogError(_("USB error while writing code memory"));
            break;
        default:
            SetStatusText(_("Error programming code memory"),STATUS_FIELD_OTHER);
            wxLogError(_("Error programming code memory"));
            break;
        }
    }

    if(configFields.ConfigProgramConfig)
    {
        switch(m_hardware->writeData(&m_hexFile,&m_picType))
        {
        case 0:
            SetStatusText(_("Write Data memory OK"),STATUS_FIELD_OTHER);
            break;
        case -1:
            SetStatusText(_("m_hardware should say OK"),STATUS_FIELD_OTHER);
            wxLogError(_("m_hardware should say OK"));
            break;
        case -2:
            SetStatusText(_("m_hardware should ask for next block"),STATUS_FIELD_OTHER);
            wxLogError(_("m_hardware should ask for next block"));
            break;
        case -3:
            SetStatusText(_("write data not implemented for current PIC"),STATUS_FIELD_OTHER);
            wxLogError(_("write data not implemented for current PIC"));
            break;
        case -4:
            SetStatusText(_("USB error while writing code memory"),STATUS_FIELD_OTHER);
            wxLogError(_("USB error while writing code memory"));
            break;
        default:
            SetStatusText(_("Error programming data memory"),STATUS_FIELD_OTHER);
            wxLogError(_("Error programming data memory"));
            break;
        }
    }

    if(configFields.ConfigProgramData)
    {
        switch(m_hardware->writeConfig(&m_hexFile,&m_picType))
        {
        case 0:
            SetStatusText(_("Write Config memory OK"),STATUS_FIELD_OTHER);
            break;
        case -1:
            SetStatusText(_("m_hardware should say OK"),STATUS_FIELD_OTHER);
            wxLogError(_("m_hardware should say OK"));
            break;
        case -2:
            SetStatusText(_("m_hardware should ask for next block"),STATUS_FIELD_OTHER);
            wxLogError(_("m_hardware should ask for next block"));
            break;
        case -3:
            SetStatusText(_("write config not implemented for current PIC"),STATUS_FIELD_OTHER);
            wxLogError(_("write config not implemented for current PIC"));
            break;
        case -4:
            SetStatusText(_("USB error while writing code memory"),STATUS_FIELD_OTHER);
            wxLogError(_("USB error while writing code memory"));
            break;
        default:
            SetStatusText(_("Error programming config memory"),STATUS_FIELD_OTHER);
            wxLogError(_("Error programming config memory"));
            break;
        }
    }

    updateProgress(100);
}

/*read everything from the device*/
void UppMainWindow::upp_read()
{
    if (m_hardware == NULL) return;

    if (!m_hardware->connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    // this command overwrites current code/config/data HEX... so ask to the user
    // before proceeding:
    if (!ShoudContinueIfUnsaved())
        return;

    // reset current contents:
    m_hexFile.newFile(&m_picType);

    if(m_hardware->readCode(&m_hexFile,&m_picType)<0)
    {
        SetStatusText(_("Error reading code memory"),STATUS_FIELD_OTHER);
        wxLogError(_("Error reading code memory"));
        //return;
    }
    if(m_hardware->readData(&m_hexFile,&m_picType)<0)
    {
        SetStatusText(_("Error reading data memory"),STATUS_FIELD_OTHER);
        wxLogError(_("Error reading data memory"));
        //return;
    }
    if(m_hardware->readConfig(&m_hexFile,&m_picType)<0)
    {
        SetStatusText(_("Error reading config memory"),STATUS_FIELD_OTHER);
        wxLogError(_("Error reading config memory"));
        //return;
    }

    m_hexFile.trimData(&m_picType);

    UpdateGrids();
    UpdateTitle();

    updateProgress(100);
}

/*verify the device with the open hexfile*/
void UppMainWindow::upp_verify()
{
    if (m_hardware == NULL) return;

    if (!m_hardware->connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    wxString verifyText;
    wxString typeText;
    VerifyResult res=
        m_hardware->verify(&m_hexFile,&m_picType,configFields.ConfigVerifyCode,
                         configFields.ConfigVerifyConfig,configFields.ConfigVerifyData);

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
                res.Address+((res.DataType==TYPE_CONFIG)*m_picType.getCurrentPic().ConfigAddress),
                res.Read,
                            res.Expected);
            verifyText.Prepend(typeText);
            SetStatusText(verifyText,STATUS_FIELD_OTHER);
            wxLogError(verifyText);
            break;
        case VERIFY_USB_ERROR:
            SetStatusText(_("USB error during verify"),STATUS_FIELD_OTHER);
            wxLogError(_("USB error during verify"));
            break;
        case VERIFY_OTHER_ERROR:
            SetStatusText(_("Unknown error during verify"),STATUS_FIELD_OTHER);
            wxLogError(_("Unknown error during verify"));
            break;
        default:
            SetStatusText(_("I'm sorry for being stupid"),STATUS_FIELD_OTHER);
            wxLogError(_("I'm sorry for being stupid"));
            break;
    }
    updateProgress(100);
}

/*perform a bulk-erase on the current PIC*/
void UppMainWindow::upp_erase()
{
    if (m_hardware == NULL) return;

    if (!m_hardware->connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    // this command overwrites current code/config/data HEX... so ask to the user
    // before proceeding:
    if (!ShoudContinueIfUnsaved())
        return;

    // reset current contents:
    m_hexFile.newFile(&m_picType);

    if(m_hardware->bulkErase(&m_picType)<0)
    {
        SetStatusText(_("Error erasing the device"),STATUS_FIELD_OTHER);
        wxLogError(_("Error erasing the device"));
        return;
    }

    UpdateGrids();
    UpdateTitle();

    updateProgress(100);
}

/*Check if the device is erased successfully*/
void UppMainWindow::upp_blankcheck()
{
    if (m_hardware == NULL) return;

    if (!m_hardware->connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    wxString verifyText;
    string typeText;
    VerifyResult res=m_hardware->blankCheck(&m_picType);
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
                res.Address+((res.DataType==TYPE_CONFIG)+m_picType.getCurrentPic().ConfigAddress),
                res.Read,
                res.Expected);
            SetStatusText(verifyText,STATUS_FIELD_OTHER);
            wxLogError(verifyText);
            break;
        case VERIFY_USB_ERROR:
            SetStatusText(_("USB error during blankcheck"),STATUS_FIELD_OTHER);
            wxLogError(_("USB error during blankcheck"));
            break;
        case VERIFY_OTHER_ERROR:
            SetStatusText(_("Unknown error during blankcheck"),STATUS_FIELD_OTHER);
            wxLogError(_("Unknown error during blankcheck"));
            break;
        default:
            SetStatusText(_("I'm sorry for being stupid"),STATUS_FIELD_OTHER);
            wxLogError(_("I'm sorry for being stupid"));
            break;
    }

    updateProgress(100);
}

/*Detect which PIC is connected and select it in the choicebox and the m_hardware*/
bool UppMainWindow::upp_autodetect()
{
    if (m_hardware == NULL) return false;

    if (!m_hardware->connected())
    {
        wxLogError(_("The programmer is not connected"));
        return false;
    }

    int devId=m_hardware->autoDetectDevice();
    cout<<"autodetected ID: 0x"<<hex<<devId<<dec<<endl;

    m_picType=PicType(devId);
    m_hardware->setPicType(&m_picType);

    wxString picName = wxGetPicName(&m_picType);
    m_pPICChoice->SetStringSelection(picName);

    if(devId>0)
        SetStatusText(wxString(_("Detected: ")) + picName,STATUS_FIELD_HARDWARE);
    else
        SetStatusText(_("No pic detected!"),STATUS_FIELD_HARDWARE);

    upp_new();

    return (devId>0);
}

/*Connect upp_wx to the upp programmer*/
bool UppMainWindow::upp_connect()
{
    // recreate the hw class
    if (m_hardware != NULL) delete m_hardware;
    m_hardware=new Hardware(this, HW_UPP);

    if(m_hardware->connected())
    {
        upp_autodetect();       // already calls upp_new();

        char msg[64];
        if(m_hardware->getFirmwareVersion(msg)<0)
            SetStatusText(_("Unable to read firmware version"),STATUS_FIELD_HARDWARE);
        else
            SetStatusText(wxString::FromAscii(msg).Trim().Append(_(" Connected")),STATUS_FIELD_HARDWARE);
    }
    else
    {
        m_picType=PicType(0);   // select default PIC
        m_hardware->setPicType(&m_picType);
        m_pPICChoice->SetStringSelection(wxGetPicName(&m_picType));

        SetStatusText(_("Usbpicprog programmer not found"),STATUS_FIELD_HARDWARE);

        upp_new();
    }

    upp_update_hardware_type();

    return m_hardware->connected();
}

/*Connect upp_wx to the bootloader*/
bool UppMainWindow::upp_connect_boot()
{
    if (m_hardware != NULL) delete m_hardware;
    m_hardware=new Hardware(this, HW_BOOTLOADER);

    if(m_hardware->connected())
    {
        upp_autodetect();       // already calls upp_new();

        char msg[64];
        if(m_hardware->getFirmwareVersion(msg)<0)
            SetStatusText(_("Unable to read version"),STATUS_FIELD_HARDWARE);
        else
            SetStatusText(wxString::FromAscii(msg).Trim().Append(_(" Connected")),STATUS_FIELD_HARDWARE);
    }
    else
    {
        m_picType=PicType(0);     // select default PIC
        m_hardware->setPicType(&m_picType);
        m_pPICChoice->SetStringSelection(wxGetPicName(&m_picType));

        SetStatusText(_("Bootloader not found"),STATUS_FIELD_HARDWARE);

        upp_new();
    }

    upp_update_hardware_type();

    return m_hardware->connected();
}

/*disconnect the m_hardware*/
void UppMainWindow::upp_disconnect()
{
    if(m_hardware != NULL)
    {
        if (m_hardware->connected())
        {
            delete m_hardware;
            m_hardware = NULL;
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
    PreferencesDialog dlg(this, wxID_ANY, _("Preferences"));

    dlg.SetConfigFields(configFields);
    if (dlg.ShowModal() == wxID_OK)
        configFields = dlg.GetResult();
}

/*load a browser with the usbpicprog website*/
void UppMainWindow::upp_help()
{
    wxLaunchDefaultBrowser(wxT("http://usbpicprog.org/"));
}

/*show an about box (only supported from wxWidgets 2.8.something+) */
void UppMainWindow::upp_about()
{
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
}

/*if the combo changed, also change it in the m_hardware*/
void UppMainWindow::upp_choice_changed()
{
    if (m_hardware != NULL)
    {
        if(m_hardware->getCurrentHardware()==HW_BOOTLOADER)
        {
            // revert selection to the default type
            m_pPICChoice->SetStringSelection(wxT("P18F2550"));
            return;
        }

        m_picType=PicType(string(m_pPICChoice->GetStringSelection().mb_str(wxConvUTF8)));
        m_hardware->setPicType(&m_picType);

        upp_new();
    }
    else
    {
        m_picType=PicType(string(m_pPICChoice->GetStringSelection().mb_str(wxConvUTF8)));

        upp_new();
    }

}

void UppMainWindow::upp_update_hardware_type()
{
    /* FIXME
    if (m_hardware != NULL && m_hardware->getm_hardwareType() != HW_NONE)
    {
        if (m_hardware->getm_hardwareType() == HW_UPP)
        {
            m_radioButton_upp->SetValue(true);
            m_radioButton_boot->SetValue(false);
        }
        else if (m_hardware->getm_hardwareType() == HW_BOOTLOADER)
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
