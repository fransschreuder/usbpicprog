/***************************************************************************
*   Copyright (C) 2008-2010 by Frans Schreuder, Francesco Montorsi        *
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

// NOTE: to avoid lots of warnings with MSVC 2008 about deprecated CRT functions
//       it's important to include wx/defs.h before STL headers

#include <wx/defs.h>
#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/toolbar.h>
#include <wx/choice.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/filesys.h>
#include "wx/fs_zip.h"
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/debug.h>
#include <wx/dataview.h>
#include <wx/dcmemory.h>
#include <wx/stdpaths.h>

#include <wx/persist/toplevel.h>
#include <wx/evtloop.h>

#include "uppmainwindow_base.h"
#include "uppmainwindow.h"
#include "hexview.h"
#include "packageview.h"
#include "configview.h"
#include "io_test.h"
#include "../svn_revision.h"
#include "../firmwareinfo.h"

#include <map>

#if defined(__WXGTK__) || defined(__WXMOTIF__) 
    /*GTK needs bigger icons than Windows*/
    #include "../icons/refresh.xpm"
    #include "../icons/blankcheck.xpm"
    #include "../icons/program.xpm"
    #include "../icons/erase.xpm"
    #include "../icons/read.xpm"
    #include "../icons/play.xpm"
    #include "../icons/stop.xpm"
    #include "../icons/verify.xpm"
    #include "../icons/usbpicprog.xpm"
#else   /*Icons for Windows and Mac*/
    #include "../icons/win/refresh.xpm"
    #include "../icons/win/blankcheck.xpm"
    #include "../icons/win/program.xpm"
    #include "../icons/win/erase.xpm"
    #include "../icons/win/read.xpm"
    #include "../icons/win/play.xpm"
    #include "../icons/win/stop.xpm"
    #include "../icons/win/verify.xpm"
    #include "../icons/win/usbpicprog.xpm"
    #include "../icons/win/zoomin.xpm"
    #include "../icons/win/zoomout.xpm"
    #include "../icons/win/zoomfit.xpm"
#endif

#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // useful to catch memory leaks when compiling under MSVC 
#endif

static const wxChar *FILETYPES = _T(
    "Hex files|*.hex;*.HEX|All files|*.*"
);

// see uppmainwindow_base.cpp
extern int g_handModificationsTrickSymbol;

wxDEFINE_EVENT( wxEVT_COMMAND_THREAD_UPDATE, wxThreadEvent );
wxDEFINE_EVENT( wxEVT_COMMAND_THREAD_COMPLETE, wxThreadEvent );


// ----------------------------------------------------------------------------
// UppMainWindow
// ----------------------------------------------------------------------------

UppMainWindow::UppMainWindow(Hardware& hardware, wxWindow* parent, wxWindowID id)
    : UppMainWindowBase( parent, id, wxEmptyString /* will be set later */,
                        wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL ),
    m_history(4),
    m_hardware(hardware)
{
    SetName("UppMainWindow");

    // load settings from config file or set a default value
    wxConfigBase* pCfg = wxConfig::Get();
    pCfg->SetPath(("/"));
    if ( !pCfg->Read(("m_defaultPath"), &m_defaultPath) )
        m_defaultPath=("");
    if ( !pCfg->Read(("ConfigProgramCode"), &m_cfg.ConfigProgramCode))
        m_cfg.ConfigProgramCode=true;
    if ( !pCfg->Read(("ConfigProgramConfig"), &m_cfg.ConfigProgramConfig))
        m_cfg.ConfigProgramConfig=true;
    if ( !pCfg->Read(("ConfigProgramData"), &m_cfg.ConfigProgramData))
        m_cfg.ConfigProgramData=true;
    if ( !pCfg->Read(("ConfigVerifyCode"), &m_cfg.ConfigVerifyCode))
        m_cfg.ConfigVerifyCode=true;
    if ( !pCfg->Read(("ConfigVerifyConfig"), &m_cfg.ConfigVerifyConfig))
        m_cfg.ConfigVerifyConfig=true;
    if ( !pCfg->Read(("ConfigVerifyData"), &m_cfg.ConfigVerifyData))
        m_cfg.ConfigVerifyData=true;
    if ( !pCfg->Read(("ConfigEraseBeforeProgramming"), &m_cfg.ConfigEraseBeforeProgramming))
        m_cfg.ConfigEraseBeforeProgramming=true;
    if ( !pCfg->Read(("ConfigVerifyAfterProgramming"), &m_cfg.ConfigVerifyAfterProgramming))
        m_cfg.ConfigShowPopups=false;
    if ( !pCfg->Read(("ConfigShowPopups"), &m_cfg.ConfigShowPopups))
        m_cfg.ConfigShowPopups=false;
    m_history.Load(*pCfg);

    // non-GUI init:
    m_dlgProgress=NULL;   // will be created when needed
	m_arrPICName=PicType::getSupportedPicNames();

	wxFileSystem::AddHandler(new wxZipFSHandler);
	m_HelpController =  new wxHtmlHelpController();

	m_HelpController->AddBook(((wxStandardPaths &)wxStandardPaths::Get()).GetDataDir()+wxT("/usbpicprog.htb"),true);
	
    // GUI init:
    CompleteGUICreation();    // also loads the saved pos&size of this frame

    // NOTE: select a PIC reading the last used one which was saved in wxConfig
    //       or just use the first one (0-th) as pCfg->Read() default value
    long lastPic=0;
    if ((lastPic=pCfg->Read(("SelectedPIC"), (long)0)) >= 0 && 
        lastPic < (int)m_arrPICName.size())
    {
        m_picType = PicType::FindPIC(m_arrPICName[lastPic]);

        // keep the choice box synchronized
        m_pPICChoice->SetStringSelection(m_arrPICName[lastPic]);
    }

    // find the hardware connected to the PC, if any:
    upp_connect();

	// PIC changed; reset the code/config/data grids
    Reset();
}

UppMainWindow::~UppMainWindow()
{
    if (m_hardware.connected())
    {
        m_hardware.disconnect();
    }

    // save settings
    wxConfigBase* pCfg = wxConfig::Get();
    pCfg->SetPath(("/"));
    pCfg->Write(("m_defaultPath"), m_defaultPath);
    pCfg->Write(("ConfigProgramCode"), m_cfg.ConfigProgramCode);
    pCfg->Write(("ConfigProgramConfig"), m_cfg.ConfigProgramConfig);
    pCfg->Write(("ConfigProgramData"), m_cfg.ConfigProgramData);
    pCfg->Write(("ConfigVerifyCode"), m_cfg.ConfigVerifyCode);
    pCfg->Write(("ConfigVerifyConfig"), m_cfg.ConfigVerifyConfig);
    pCfg->Write(("ConfigVerifyData"), m_cfg.ConfigVerifyData);
    pCfg->Write(("ConfigVerifyAfterProgramming"), m_cfg.ConfigVerifyAfterProgramming);
    pCfg->Write(("ConfigEraseBeforeProgramming"), m_cfg.ConfigEraseBeforeProgramming);
    pCfg->Write(("ConfigShowPopups"), m_cfg.ConfigShowPopups);
    pCfg->Write(("SelectedPIC"), m_pPICChoice->GetSelection());
    m_history.Save(*pCfg);
}

wxBitmap UppMainWindow::GetMenuBitmap(const char* xpm_data[])
{
    wxImage tmp(xpm_data);
    wxSize sz = wxArtProvider::GetNativeSizeHint(wxART_MENU);
    tmp.Rescale(sz.GetWidth(), sz.GetHeight());
    return wxBitmap(tmp);
}

void UppMainWindow::CompleteGUICreation()
{
#ifdef __WXMSW__
    // make the border around the wxNotebook to have the same background colour of the notebook bg colour
    SetOwnBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    // in wx 2.9.0 there is the more readable/sensed synonim wxSYS_COLOUR_FRAMEBK
#endif

    // create the actions menu with rescaled icons
    wxMenu* pMenuActions = new wxMenu();

    wxMenuItem* pMenuProgram;
    pMenuProgram = new wxMenuItem( pMenuActions, wxID_PROGRAM, wxString( _("&Program...") ) + ('\t') + ("F7"),
                                _("Program the PIC device"), wxITEM_NORMAL );

    wxMenuItem* pMenuRead;
    pMenuRead = new wxMenuItem( pMenuActions, wxID_READ, wxString( _("&Read...") ) + ('\t') + ("F8"),
                                _("Read the PIC device"), wxITEM_NORMAL );

    wxMenuItem* pMenuVerify;
    pMenuVerify = new wxMenuItem( pMenuActions, wxID_VERIFY, wxString( _("&Verify...") ),
                                _("Verify the PIC device"), wxITEM_NORMAL );

    wxMenuItem* pMenuErase;
    pMenuErase = new wxMenuItem( pMenuActions, wxID_ERASE, wxString( _("&Erase...") ),
                                _("Erase the PIC device"), wxITEM_NORMAL );
//This menuItem must be enabled / disabled, so let's make it global...
//	wxMenuItem* pMenuRestoreCal;
    
    m_pMenuRestoreCal = new wxMenuItem( pMenuActions, wxID_RESTORE, wxString( _("Restore Cal regs...") ),
                                _("Restore PIC's calibration registers"), wxITEM_NORMAL );

    wxMenuItem* pMenuBlankCheck;
    pMenuBlankCheck = new wxMenuItem( pMenuActions, wxID_BLANKCHECK, wxString( _("&Blankcheck...") ),
                                    _("Blankcheck the PIC device"), wxITEM_NORMAL );

    wxMenuItem* pMenuAutoDetect;
    pMenuAutoDetect = new wxMenuItem( pMenuActions, wxID_AUTODETECT, wxString( _("&Autodetect...") ),
                                    _("Detect the type of the PIC device"), wxITEM_NORMAL );
                                    

    wxMenuItem* pMenuRunTarget;
    pMenuRunTarget = new wxMenuItem( pMenuActions, wxID_RUN_TARGET, wxString(_("Run Target")),
                                    _("Set 5V to MCLR to run the target controller"), wxITEM_NORMAL );

    wxMenuItem* pMenuStopTarget;
    pMenuStopTarget = new wxMenuItem( pMenuActions, wxID_STOP_TARGET, wxString(_("Stop Target")),
                                    _("Set 0V to MCLR to stop the target controller"), wxITEM_NORMAL );
                                    
    wxMenuItem* pMenuConnect;
    pMenuConnect = new wxMenuItem( pMenuActions, wxID_CONNECT, wxString( _("&Connect...") ),
                                    _("Connect to the programmer"), wxITEM_NORMAL );

    wxMenuItem* pMenuDisconnect;
    pMenuDisconnect = new wxMenuItem( pMenuActions, wxID_DISCONNECT, wxString( _("&Disconnect...") ),
                                    _("Disconnect from the programmer"), wxITEM_NORMAL );

    wxMenu* pMenuSelectPIC;
    pMenuSelectPIC = new wxMenu( 0 );       // this is a menu with submenus

#ifdef __WXGTK__
    // on Windows all other menus have no bitmaps (because wxWidgets does not add stock icons
    // on platforms without native stock icons); it looks weird to have them only for the Actions menu...
    // NOTE: this needs to be done _before_ appending menu items or wxGTK won't like it
    pMenuProgram->SetBitmap(GetMenuBitmap( program_xpm ));
    pMenuRead->SetBitmap(GetMenuBitmap( read_xpm ));
    pMenuVerify->SetBitmap(GetMenuBitmap( verify_xpm ));
    pMenuErase->SetBitmap(GetMenuBitmap( erase_xpm ));
    pMenuBlankCheck->SetBitmap(GetMenuBitmap( blankcheck_xpm ));
    pMenuAutoDetect->SetBitmap(GetMenuBitmap( blankcheck_xpm ));
    
    pMenuRunTarget->SetBitmap(GetMenuBitmap( play_xpm ));
    pMenuStopTarget->SetBitmap(GetMenuBitmap( stop_xpm ));

    pMenuConnect->SetBitmap(wxArtProvider::GetBitmap(("gtk-connect"), wxART_MENU));
    pMenuDisconnect->SetBitmap(wxArtProvider::GetBitmap(("gtk-disconnect"), wxART_MENU));
#endif

    pMenuActions->Append( pMenuProgram );
    pMenuActions->Append( pMenuRead );
    pMenuActions->Append( pMenuVerify );
    pMenuActions->Append( pMenuErase );
    pMenuActions->Append( m_pMenuRestoreCal );
    pMenuActions->Append( pMenuBlankCheck );
    pMenuActions->Append( pMenuAutoDetect );                             
    pMenuActions->Append( pMenuRunTarget );
    pMenuActions->Append( pMenuStopTarget );
    pMenuActions->AppendSeparator();
    pMenuActions->Append( pMenuConnect );
    pMenuActions->Append( pMenuDisconnect );
    pMenuActions->AppendSeparator();
    pMenuActions->AppendSubMenu( pMenuSelectPIC, wxString( _("&Select PIC...") ),
                                _("Change the currently selected PIC") );
    // create a menu-item for each PIC
    map<wxString,wxMenu*> menus;
    for(unsigned int i=0;i<m_arrPICName.size();i++)
    {
        bool bFamilyFound = false;

        // the first 4 characters of the PIC name are the family:
        wxString family;
        if(m_arrPICName[i].substr(0, 4)=="24XX") family = wxString("I2C Eeprom");
        else family = wxString("PIC"+m_arrPICName[i].substr(0, 2));

        // is there a menu for this PIC family?
        for(map<wxString,wxMenu*>::const_iterator j=menus.begin();j!=menus.end();j++)
        {
            if (j->first == family)
            {
                // yes, there's one!
                wxMenu* pFamilyMenu = j->second;
                pFamilyMenu->Append(wxID_PIC_CHOICE_MENU+i,
                                    m_arrPICName[i]);
                bFamilyFound = true;
            }
        }

        if (!bFamilyFound)
        {
            // not yet; add it
            menus[family] = new wxMenu(0);
            i--; // repeat processing for this element
        }
    }

    // add a submenu for each PIC family
    for(map<wxString,wxMenu*>::const_iterator j=menus.begin();j!=menus.end();j++)
        pMenuSelectPIC->AppendSubMenu(j->second, j->first);
    m_pMenuBar->Insert(2, pMenuActions, _("&Actions") );

    this->Connect( wxID_PROGRAM, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_program ) );
    this->Connect( wxID_READ, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_read ) );
    this->Connect( wxID_VERIFY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_verify ) );
    this->Connect( wxID_ERASE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_erase ) );
    this->Connect( wxID_RESTORE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_restore ) );
    this->Connect( wxID_BLANKCHECK, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_blankcheck ) );
    this->Connect( wxID_AUTODETECT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_autodetect ) );
    this->Connect( wxID_RUN_TARGET, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_run_target ) );
    this->Connect( wxID_STOP_TARGET, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_stop_target ) );
    this->Connect( wxID_CONNECT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_connect ) );
    this->Connect( wxID_DISCONNECT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_disconnect ) );

    this->Connect( wxID_PIC_CHOICE_MENU, wxID_PIC_CHOICE_MENU+m_arrPICName.size(),
                wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_pic_choice_changed_bymenu ) );

    // create the most-recently-used section in File menu
    m_history.UseMenu(m_pMenuFile);
    m_history.AddFilesToMenu();

    this->Connect( wxID_FILE1, wxID_FILE9, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_mru ) );

    // complete creation of the GUI creating the toolbar;
    // we can't let wxFormBuilder do it because it does not support wxArtProvider's usage
    wxToolBar* toolbar = this->CreateToolBar( wxTB_DOCKABLE|wxTB_HORIZONTAL, wxID_ANY );

    wxSize sz = toolbar->GetToolBitmapSize();
    toolbar->AddTool( wxID_NEW, _("new"), wxArtProvider::GetBitmap(wxART_NEW,wxART_TOOLBAR,sz), wxNullBitmap, wxITEM_NORMAL, _("new"),
                    GetMenuBar()->FindItem(wxID_NEW)->GetHelp() );
    toolbar->AddTool( wxID_OPEN, _("open"), wxArtProvider::GetBitmap(wxART_FILE_OPEN,wxART_TOOLBAR,sz), wxNullBitmap, wxITEM_NORMAL, _("open"),
                    GetMenuBar()->FindItem(wxID_OPEN)->GetHelp() );
    toolbar->AddTool( wxID_REFRESH, _("reload"), wxIcon(refresh_xpm), wxNullBitmap, wxITEM_NORMAL, _("reload"),
                    GetMenuBar()->FindItem(wxID_REFRESH)->GetHelp() );
    toolbar->AddTool( wxID_SAVE, _("save"), wxArtProvider::GetBitmap(wxART_FILE_SAVE,wxART_TOOLBAR,sz), wxNullBitmap, wxITEM_NORMAL, _("save"),
                    GetMenuBar()->FindItem(wxID_SAVE)->GetHelp() );
    toolbar->AddTool( wxID_SAVEAS, _("save as"), wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS,wxART_TOOLBAR,sz), wxNullBitmap, wxITEM_NORMAL, _("save as"),
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
    toolbar->AddTool( wxID_AUTODETECT, _("autodetect"), wxIcon( blankcheck_xpm ), wxNullBitmap, wxITEM_NORMAL, _("autodetect"),
                    GetMenuBar()->FindItem(wxID_AUTODETECT)->GetHelp() );
    toolbar->AddSeparator();
    toolbar->AddTool( wxID_RUN_TARGET, _("Run Target"), wxIcon( play_xpm ), wxNullBitmap, wxITEM_NORMAL, _("Run Target"),
                    GetMenuBar()->FindItem(wxID_RUN_TARGET)->GetHelp() );
    toolbar->AddTool( wxID_STOP_TARGET, _("Stop Target"), wxIcon( stop_xpm ), wxNullBitmap, wxITEM_NORMAL, _("Stop Target"),
                    GetMenuBar()->FindItem(wxID_STOP_TARGET)->GetHelp() );
                    
    toolbar->AddSeparator();

    m_pPICChoice = new wxChoice(toolbar, wxID_PIC_CHOICE_COMBO, wxDefaultPosition, wxSize(120,-1));
    m_pPICChoice->SetToolTip(_("currently selected PIC type"));
    this->Connect( wxID_PIC_CHOICE_COMBO, wxEVT_COMMAND_CHOICE_SELECTED,
                wxCommandEventHandler( UppMainWindow::on_pic_choice_changed ) );

    toolbar->AddControl( m_pPICChoice );
    toolbar->Realize();

    // set bitmaps on the bitmap buttons
#ifdef __WXGTK__
    m_pZoomInButton->SetBitmapLabel(wxArtProvider::GetBitmap("gtk-zoom-in"));
    m_pZoomOutButton->SetBitmapLabel(wxArtProvider::GetBitmap("gtk-zoom-out"));
    m_pZoomFitButton->SetBitmapLabel(wxArtProvider::GetBitmap("gtk-zoom-fit"));
#else
    m_pZoomInButton->SetBitmapLabel( wxBitmap(zoomin_xpm) );
    m_pZoomOutButton->SetBitmapLabel( wxBitmap(zoomout_xpm) );
    m_pZoomFitButton->SetBitmapLabel( wxBitmap(zoomfit_xpm) );
#endif
    m_pZoomInButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(UppPackageViewWindow::OnZoomIn), NULL, m_pPackageWin);
    m_pZoomOutButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(UppPackageViewWindow::OnZoomOut), NULL, m_pPackageWin);
    m_pZoomFitButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(UppPackageViewWindow::OnZoomFit), NULL, m_pPackageWin);
    
    // by default show code page at startup
    m_pNotebook->ChangeSelection(PAGE_CODE);

    // make 2nd pane wide about the half of the 1st pane:
    const int widths[] = { -5, -3 };
    m_pStatusBar->SetStatusWidths(2, widths);

    // append all PIC names to the choice control
    m_pPICChoice->Append(m_arrPICName);

    this->SetIcon(wxIcon( usbpicprog_xpm ));

    // NOTE: these operations are normally done by code automatically generated
    //       by wxFormBuilder in UppMainWindowBase ctor; however because of the
    //       toolbar creation done above we have to do these operations here.
    this->SetSizerAndFit( m_pSizer );
	this->Layout();

    // NOTE2: because of the code above, we need to make sure that the hand modifications
    //        of uppmainwindow_base.cpp are preserved; we do it with a small trick which will
    //        trigger a linker error if the hand modifications get lost:
    g_handModificationsTrickSymbol = 1;
        // if you get a linker error about this symbol, please make sure to re-apply the
        // required hand modifications to uppmainwindow_base.cpp

    // eventually load position&size of this window from the wxConfig object:
    wxPersistenceManager::Get().RegisterAndRestore(this);

    // misc event handlers
    this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( UppMainWindow::on_close ) );
    this->Connect( wxEVT_SIZE, wxSizeEventHandler( UppMainWindow::on_size ) );
    this->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UppMainWindow::on_package_variant_changed ) );
    this->Connect( wxEVT_COMMAND_THREAD_UPDATE, wxThreadEventHandler( UppMainWindow::OnThreadUpdate ) );
    this->Connect( wxEVT_COMMAND_THREAD_COMPLETE, wxThreadEventHandler( UppMainWindow::OnThreadCompleted ) );

    // set default title name
    UpdateTitle();

    // show default stuff
    UpdatePicInfo();

    // this is to be sure that the package-view window gets updated by our upp_size_changed()
    // event handler to reflect the effective window size
    // (this somehow doesn't happen on wxGTK; probably because of deferred top-window resizing)
    PostSizeEvent();
}

UppHexViewGrid* UppMainWindow::GetCurrentGrid() const
{
    switch (m_pNotebook->GetSelection())
    {
    case PAGE_CODE:
        return m_pCodeGrid;
    case PAGE_DATA:
        return m_pDataGrid;

    default:
        return NULL;
    }
}

bool UppMainWindow::ShouldContinueIfUnsaved()
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

void UppMainWindow::checkFirmwareVersion(FirmwareVersion firmwareVersion)
{
    if (!firmwareVersion.stableRelease)
    {
        #ifdef UPP_VERSION
            wxLogMessage(_("You are using a development release of the firmware with a stable version of the software. Consider upgrading your firmware"));     
        #else
        if (firmwareVersion.release<DEV_VERSION)
            wxLogMessage(_("Your firmware is too old (latest version is %d); please consider upgrading it"),
                         DEV_VERSION);
        #endif
        return;
    }

    #ifdef UPP_VERSION
    // check major digit
    double stableVersion=(double)STABLE_VERSION_MAJOR+((double)STABLE_VERSION_MINOR)/10+((double)STABLE_VERSION_RELEASE)/100;
    double stableFirmwareVersion=(double)firmwareVersion.major+((double)firmwareVersion.minor)/10+((double)firmwareVersion.release)/100;
    if (stableVersion<stableFirmwareVersion)
        wxLogMessage(_("Firmware probably too new")); 
    if (stableVersion>stableFirmwareVersion)
        wxLogMessage(_("Your firmware is too old (latest version is %d.%d.%d); please consider upgrading it"),
                    STABLE_VERSION_MAJOR,STABLE_VERSION_MINOR,STABLE_VERSION_RELEASE);
    #else
    wxLogMessage(_("You are using a stable release of the firmware with a development version of the software. Consider upgrading your firmware"));     
    #endif
}



// UPPMAINWINDOW - update functions
// =============================================================================

void UppMainWindow::UpdateTitle()
{
    wxString str;

    #ifndef UPP_VERSION
    str = wxString(_("Usbpicprog rev: ")).Append(wxString::FromAscii(SVN_REVISION));
    #else
    str = wxString(_("Usbpicprog: ")).Append(wxString::FromAscii(UPP_VERSION));
    #endif

    if (!m_hexFile.hasFileName())
        str += (" - [") + wxString(_("untitled"));
    else
        str += (" - [") + wxString::FromAscii(m_hexFile.getFileName());

    if (m_hexFile.wasModified())
        str += (" *");

    SetTitle(str + ("]"));
}

void UppMainWindow::UpdatePicInfo()
{
    // reset grid contents
    m_pCodeGrid->SetHexFile(&m_hexFile);
    m_pDataGrid->SetHexFile(&m_hexFile);

    // reset config listbook contents
    m_pConfigListbook->SetHexFile(&m_hexFile, m_picType);
    
    // reset the PIC info page
    if (!m_picType.ok())
        return;

    const vector<ChipPackage>& pkg = m_picType.Package;

    // update the misc infos
    m_pDatasheetLink->SetLabel(wxString::Format(_("%s datasheet"), wxString(m_picType.GetExtName().c_str())));
    m_pDatasheetLink->SetURL(
        wxString::Format("http://www.google.com/search?q=%s%%2Bdatasheet&as_sitesearch=microchip.com", wxString(m_picType.GetExtName().c_str())));
    m_pDatasheetLink->SetVisited(false);

    m_pVPPText->SetLabel(
        wxString::Format(_("Programming voltage (Vpp):\n   Min=%.2fV\n   Nom=%.2fV\n   Max=%.2fV"),
                        m_picType.ProgVoltages[MINIMUM], m_picType.ProgVoltages[NOMINAL], m_picType.ProgVoltages[MAXIMUM]));
    m_pVDDText->SetLabel(
        wxString::Format(_("Supply voltage (Vdd):\n   Min=%.2fV\n   Nom=%.2fV\n   Max=%.2fV"),
                        m_picType.WorkVoltages[MINIMUM], m_picType.WorkVoltages[NOMINAL], m_picType.WorkVoltages[MAXIMUM]));
    m_pFrequencyText->SetLabel(wxString::Format(_("Frequency range:\n   Min=%.2fMhz\n   Max=%.2fMhz"), m_picType.MinFreq, m_picType.MaxFreq));
    m_pDeviceIDText->SetLabel(wxString::Format(_("Device ID: 0x%X"), m_picType.DevId&0xFFFF));
    m_pCodeMemoryText->SetLabel(wxString::Format(_("Code memory size: %d bytes"), m_picType.CodeSize));
    m_pDataMemoryText->SetLabel(wxString::Format(_("Data memory size: %d bytes"), m_picType.DataSize));
    m_pTypeText->SetLabel(wxString::Format(_("Type: %d-bit microcontroller"), m_picType.bitsPerWord()));

    m_pInfoPanel->Layout();
        // size may have changed: relayout the panel which is the parent of the windows
        // whose size has changed (do it after updating _all_ label texts)

    // update the package variants combobox
    m_pPackageVariants->Clear();
    for (unsigned int i=0; i<pkg.size(); i++)
        m_pPackageVariants->Append(
            wxString::Format(_("%s [%d pins]"), pkg[i].GetName(), pkg[i].GetPinCount()));
    m_pPackageVariants->SetSelection(0);

    // let's update the bitmap and the pin names:
    upp_package_variant_changed();
    if((m_picType.picFamily==P12F629)||(m_picType.picFamily==P12F508)||(m_picType.picFamily==P10F200)||(m_picType.picFamily==P10F202))
            m_pMenuRestoreCal->Enable(true);
    else
            m_pMenuRestoreCal->Enable(false);
            
}

void UppMainWindow::Reset()
{
    m_hexFile.newFile(&m_picType);
    m_hardware.backupOscCalBandGap(&m_picType);
    m_hexFile.putOscCalBandGap (&m_picType);
    UpdatePicInfo();
    UpdateTitle();
	
}


// UPPMAINWINDOW - event handlers
// =============================================================================

void UppMainWindow::on_mru(wxCommandEvent& event)
{
    upp_open_file(m_history.GetHistoryFile(event.GetId() - wxID_FILE1));
}

void UppMainWindow::on_close(wxCloseEvent& event)
{
    if ( event.CanVeto() )
        if (!ShouldContinueIfUnsaved())
        {
            // user replied "do not continue"
            event.Veto();
            return;
        }

    // wait for the thread
    if (GetThread())
        GetThread()->Wait();

    Destroy();
}



// UPPMAINWINDOW - thread-related functions
// =============================================================================

void UppMainWindow::updateProgress(int value)
{
    // NOTE: this function is not always executed in the secondary thread's context
    //       so that we cannot do: wxASSERT(!wxThread::IsMain() || !m_dlgProgress);

    // the following line will result in a call to UppMainWindow::OnThreadUpdate()
    // in the primary thread context
    if (wxEventLoopBase::GetActive() == NULL)
        return;     // this may happen at startup, when the Hardware class is initialized

    wxThreadEvent* ev = new wxThreadEvent(wxEVT_COMMAND_THREAD_UPDATE);
#ifdef __WXMAC__
	if(value>99)value=99;
#endif
	ev->SetInt(value);
    wxQueueEvent(this, ev);
}

void UppMainWindow::OnThreadUpdate(wxThreadEvent& evt)
{
    // NOTE: this function is executed in the primary thread's context!
    wxASSERT(wxThread::IsMain());

    if (m_dlgProgress)
    {
        wxCriticalSectionLocker lock(m_arrLogCS);



#ifdef __WXMAC__
		bool continueOperation =
		m_dlgProgress->Pulse( _("Please wait until the operations are completed:\n") +
							  wxJoin(m_arrLog, '\n'));
#else
		bool continueOperation =
		m_dlgProgress->Update(evt.GetInt(),
							  _("Please wait until the operations are completed:\n") +
							  wxJoin(m_arrLog, '\n'));
#endif
		
        // update to the new label size
        m_dlgProgress->Fit();

        // make sure the focused window is the progress dialog:
        m_dlgProgress->SetFocus();
        if (!continueOperation &&               // user clicked "abort"?
            !m_hardware.operationsAborted())   // is the hardware already aborting?
        {
            m_hardware.abortOperations(true);
			wxLogWarning(_("Operations aborted"));
        }
    }
}

void UppMainWindow::OnThreadCompleted(wxThreadEvent&)
{
    // NOTE: this function is executed in the primary thread's context!
    wxASSERT(wxThread::IsMain());

	
    // reset abort flag:
    m_hardware.abortOperations(false);

    if (m_dlgProgress)
    {
        m_dlgProgress->Destroy();
        m_dlgProgress = NULL;
    }

    // log all messages created by the Entry()
    // NOTE: the thread has ended, so that we can access m_arrLog safely
    //       without using m_arrLogCS
    bool success = true;
    for (unsigned int i=0; i<m_arrLog.GetCount(); i++)
    {
        success &= m_arrLogLevel[i] == wxLOG_Message;
        wxLog::OnLog(m_arrLogLevel[i], 
                    m_arrLog[i].StartsWith(PROGRESS_MESSAGE_PREFIX) ? 
                        m_arrLog[i].Mid(PROGRESS_MESSAGE_PREFIX.Length()) : m_arrLog[i], 
                    m_arrLogTimes[i]);
    }

    m_arrLog.clear();
    m_arrLogLevel.clear();
    m_arrLogTimes.clear();
    
    SetStatusText(_("All operations completed"), STATUS_FIELD_OTHER);
    if (m_cfg.ConfigShowPopups)
    {
        if (success)
            wxLogMessage(_("All operations completed"));
        else
            wxLogWarning(_("Operations completed with errors/warnings"));
    }

	
    // some of the operations performed by the secondary thread
    // require updating the title or the grids:
    switch (m_mode)
    {
    case THREAD_READ:
        UpdatePicInfo();
        UpdateTitle();
        break;

    case THREAD_ERASE:
        Reset();
        break;
    default:
        break;
    }

    // make sure all thread messages logged above are shown to the user _now_
    // (after updating the PIC info); sometimes in fact it may happen that the
    // idle event which triggers wxLog flushing is not generated and so the
    // log messages are shown only when the application is exited
    wxLog::FlushActive();
}

wxThread::ExitCode UppMainWindow::Entry()
{
    // NOTE: this function is the core of the secondary thread's context!
    wxASSERT(!wxThread::IsMain());

    bool exitCode=false;
    switch (m_mode)
    {
    case THREAD_PROGRAM:
        exitCode = upp_thread_program();
        break;

    case THREAD_READ:
        exitCode = upp_thread_read();
        break;

    case THREAD_VERIFY:
        exitCode = upp_thread_verify();
        break;

    case THREAD_ERASE:
        exitCode = upp_thread_erase();
        break;

    case THREAD_BLANKCHECK:
        exitCode = upp_thread_blankcheck();
        break;

    default:
        wxFAIL;
    }

    // signal the main thread we've completed our task; this will result
    // in a call to UppMainWindow::OnThreadCompleted done in the primary
    // thread context:

    wxQueueEvent(this, new wxThreadEvent(wxEVT_COMMAND_THREAD_COMPLETE));
	
    return (wxThread::ExitCode)exitCode;
}

void UppMainWindow::LogFromThread(wxLogLevel level, const wxString& str)
{
    // NOTE: this function is executed in the secondary thread context
    wxASSERT(!wxThread::IsMain());

    wxCriticalSectionLocker lock(m_arrLogCS);
    m_arrLog.push_back(PROGRESS_MESSAGE_PREFIX + str);
    m_arrLogLevel.push_back(level);
    m_arrLogTimes.push_back(time(NULL));
}

bool UppMainWindow::upp_thread_program()
{
    // NOTE: this function is executed in the secondary thread context
    wxASSERT(!wxThread::IsMain());

    if (m_cfg.ConfigEraseBeforeProgramming)
    {
        LogFromThread(wxLOG_Message, _("Erasing before programming..."));

        switch(m_hardware.bulkErase(&m_picType,true))
        {
        case 1:
            LogFromThread(wxLOG_Message, _("Erase OK"));
            break;
        default:
            LogFromThread(wxLOG_Error, _("Error erasing the device"));
            return false;
        }
    }
    if (GetThread()->TestDestroy())
        return false;   // stop the operation...

    if (m_cfg.ConfigProgramCode)
    {
        LogFromThread(wxLOG_Message, _("Programming the code area of the PIC..."));
        switch(m_hardware.write(TYPE_CODE, &m_hexFile, &m_picType))
        {
        case 1:
            LogFromThread(wxLOG_Message, _("Write Code memory OK"));
            break;
        case -1:
            LogFromThread(wxLOG_Error, _("The hardware should say OK"));
            return false;
        case -2:
            LogFromThread(wxLOG_Error, _("The hardware should ask for next block"));
            return false;
        case -3:
            LogFromThread(wxLOG_Error, _("Write code not implemented for current PIC"));
            return false;
        case -4:
            LogFromThread(wxLOG_Error, _("Verify error while writing code memory"));
            return false;
        case -5:
            LogFromThread(wxLOG_Error, _("USB error while writing code memory"));
            return false;
        default:
            LogFromThread(wxLOG_Error, _("Error programming code memory"));
            return false;
        }
    }
    if (GetThread()->TestDestroy())
        return false;   // stop the operation...

    if (m_cfg.ConfigProgramData)
    {
        LogFromThread(wxLOG_Message, _("Programming the data area of the PIC..."));

        switch(m_hardware.write(TYPE_DATA, &m_hexFile, &m_picType))
        {
        case 1:
            LogFromThread(wxLOG_Message, _("Write Data memory OK"));
            break;
        case -1:
            LogFromThread(wxLOG_Error, _("The hardware should say OK"));
            return false;
        case -2:
            LogFromThread(wxLOG_Error, _("The hardware should ask for next block"));
            return false;
        case -3:
            LogFromThread(wxLOG_Error, _("Write data not implemented for current PIC"));
            return false;
        case -4:
            LogFromThread(wxLOG_Error, _("USB error while writing code memory"));
            return false;
        default:
            LogFromThread(wxLOG_Error, _("Error programming data memory"));
            return false;
        }
    }

    if (GetThread()->TestDestroy())
        return false;   // stop the operation...

    if (m_cfg.ConfigProgramConfig)
    {
        LogFromThread(wxLOG_Message, _("Programming configuration area of the PIC..."));

        switch(m_hardware.write(TYPE_CONFIG, &m_hexFile, &m_picType))
        {
        case 1:
            LogFromThread(wxLOG_Message, _("Write Config memory OK"));
            break;
        case -1:
            LogFromThread(wxLOG_Error, _("The hardware should say OK"));
            return false;
        case -2:
            LogFromThread(wxLOG_Error, _("The hardware should ask for next block"));
            return false;
        case -3:
            LogFromThread(wxLOG_Error, _("Write config not implemented for current PIC"));
            return false;
        case -4:
            LogFromThread(wxLOG_Error, _("USB error while writing code memory"));
            return false;
        default:
            LogFromThread(wxLOG_Error, _("Error programming config memory"));
            return false;
        }
    }
    if (m_cfg.ConfigVerifyAfterProgramming)
    {
        upp_thread_verify();
    }
    return true;
}

bool UppMainWindow::upp_thread_read()
{
    // NOTE: this function is executed in the secondary thread context
    wxASSERT(!wxThread::IsMain());

    // reset current contents:
    m_hexFile.newFile(&m_picType);
    LogFromThread(wxLOG_Message, _("Reading the code area of the PIC..."));
    if (m_hardware.read(TYPE_CODE, &m_hexFile, &m_picType, m_picType.CodeSize)<0)
    {
        LogFromThread(wxLOG_Error, _("Error reading code memory"));
        m_hexFile.trimData(&m_picType);
        return false;
    }
    if (GetThread()->TestDestroy())
        return false;   // stop the operation...

    LogFromThread(wxLOG_Message, _("Reading the data area of the PIC..."));
    if (m_hardware.read(TYPE_DATA, &m_hexFile, &m_picType, m_picType.DataSize)<0)
    {
        LogFromThread(wxLOG_Error, _("Error reading data memory"));
        m_hexFile.trimData(&m_picType);
        return false;
    }

    if (GetThread()->TestDestroy())
        return false;   // stop the operation...

    LogFromThread(wxLOG_Message, _("Reading the configuration area of the PIC..."));
    if (m_hardware.read(TYPE_CONFIG, &m_hexFile, &m_picType, m_picType.ConfigSize)<0)
    {
        LogFromThread(wxLOG_Error, _("Error reading configuration memory"));
        m_hexFile.trimData(&m_picType);
        return false;
    }

	
    m_hexFile.trimData(&m_picType);
    return true;
}

bool UppMainWindow::upp_thread_verify()
{
    // NOTE: this function is executed in the secondary thread context
    wxASSERT(!wxThread::IsMain());

    LogFromThread(wxLOG_Message, _("Verifying all areas of the PIC..."));

    wxString verifyText;
    wxString typeText;

    // do the verify operation:
    VerifyResult res =
        m_hardware.verify(&m_hexFile, &m_picType, m_cfg.ConfigVerifyCode,
                        m_cfg.ConfigVerifyConfig, m_cfg.ConfigVerifyData);

    switch(res.Result)
    {
    case VERIFY_SUCCESS:
        LogFromThread(wxLOG_Message, _("Verify successful"));
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
                        res.Address+((res.DataType==TYPE_CONFIG)*m_picType.ConfigAddress),
                        res.Read, res.Expected);
        verifyText.Prepend(typeText);
        LogFromThread(wxLOG_Error, verifyText);
        break;
    case VERIFY_USB_ERROR:
        LogFromThread(wxLOG_Error, _("USB error during verify"));
        break;
    case VERIFY_OTHER_ERROR:
        LogFromThread(wxLOG_Error, _("Unknown error during verify"));
        break;
    default:
        LogFromThread(wxLOG_Error, _("I'm sorry for being stupid"));
        break;
    }

    return true;
}

bool UppMainWindow::upp_thread_erase()
{
    // NOTE: this function is executed in the secondary thread context
    wxASSERT(!wxThread::IsMain());

    LogFromThread(wxLOG_Message, _("Erasing all areas of the PIC..."));

    if (m_hardware.bulkErase(&m_picType,true)<0)
    {
        LogFromThread(wxLOG_Error, _("Error erasing the device"));
        return false;
    }

    return true;
}

bool UppMainWindow::upp_thread_blankcheck()
{
    // NOTE: this function is executed in the secondary thread context
    wxASSERT(!wxThread::IsMain());

    wxString verifyText;
    wxString typeText;

    LogFromThread(wxLOG_Message, _("Checking if the device is blank..."));

    // do the blankcheck:
    VerifyResult res = m_hardware.blankCheck(&m_picType);

    switch (res.Result)
    {
    case VERIFY_SUCCESS:
        LogFromThread(wxLOG_Message, _("Device is blank"));
        break;
    case VERIFY_MISMATCH:
        switch (res.DataType)
        {
            case TYPE_CODE: typeText="code";break;
            case TYPE_DATA: typeText="data";break;
            case TYPE_CONFIG: typeText="config";break;
            default: typeText="unknown";break;
            
        }

        verifyText.Printf(_("Blankcheck failed at 0x%X. Read: 0x%02X, Expected: 0x%02X"),
            res.Address+((res.DataType==TYPE_CONFIG)+m_picType.ConfigAddress),
            res.Read,
            res.Expected);
        LogFromThread(wxLOG_Message, verifyText);
        LogFromThread(wxLOG_Message, _("Device is not blank."));
        break;
    case VERIFY_USB_ERROR:
        LogFromThread(wxLOG_Error, _("USB error during blankcheck"));
        break;
    case VERIFY_OTHER_ERROR:
        LogFromThread(wxLOG_Error, _("Unknown error during blankcheck"));
        break;
    default:
        LogFromThread(wxLOG_Error, _("I'm sorry for being stupid"));
        break;
    }

    return true;
}

bool UppMainWindow::RunThread(UppMainWindowThreadMode mode)
{
    // NOTE: this function is executed in the primary thread context
    wxASSERT(wxThread::IsMain());

    // create the progress dialog to show while our secondary thread works
    // NOTE: we use 101 as maximum value and not 100 because updateProgress() 
    //       will often be called with values between 0 and 100 for each
    //       sub-operation performed inside the various upp_thread_* functions; 
    //       we however want the progress dialog to auto-hide _only_ when all 
    //       sub-operations have been completed, i.e. in OnThreadCompleted
    m_dlgProgress = new wxProgressDialog
                    (
                        _("Progress dialog"),
                        _("Initializing...")
#ifdef __WXMAC__
					 +"                                                      " //due to a bug in wxmac 2.9, the dialog text doesn't resize well if this string is too short
#endif
					 , 101,
                        this,
                        // NOTE: it's very important to give the wxPD_AUTO_HIDE
                        //       style to wxProgressDialog otherwise we may get
                        //       unwanted reentrancies (see wx docs)
                        wxPD_AUTO_HIDE |
                        wxPD_CAN_ABORT |
                        wxPD_APP_MODAL | 
						wxPD_ELAPSED_TIME
#ifndef __WXMAC__
						| wxPD_ESTIMATED_TIME |
                        wxPD_REMAINING_TIME
#endif
                    );

    // inform the thread about which operation it must perform;
    // note that the thread is not running yet so there's no need for a critical section:
    m_mode = mode;

    if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not create the worker thread!"));
        return false;
    }

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the worker thread!"));
        return false;
    }

    // don't block our GUI while we communicate with the attached device

    return true;
}




// UPPMAINWINDOW - event handlers without event argument
// =============================================================================

void UppMainWindow::upp_hex_changed()
{
    // m_hexFile has been automatically modified by the UppHexViewGrid
    // or our m_picType instance was automatically modified by the ConfigViewListbook
    UpdateTitle();
}

void UppMainWindow::upp_new()
{
    if (!ShouldContinueIfUnsaved())
        return;

    Reset();
}

void UppMainWindow::upp_open()
{
    if (!ShouldContinueIfUnsaved())
        return;

    wxFileDialog* openFileDialog =
        new wxFileDialog( this, _("Open hexfile"), m_defaultPath, (""),
                        FILETYPES, wxFD_OPEN, wxDefaultPosition);

    if ( openFileDialog->ShowModal() == wxID_OK )
    {
        // get the folder of the opened file, without the name&extension
        m_defaultPath=wxFileName(openFileDialog->GetPath()).GetPath();

        upp_open_file(openFileDialog->GetPath());
    }
}

void UppMainWindow::upp_examples()
{
    if (!ShouldContinueIfUnsaved())
        return;
	wxString examplepath = ((wxStandardPaths &)wxStandardPaths::Get()).GetDataDir()+wxT("/examples");
    wxFileDialog* openFileDialog =
        new wxFileDialog( this, _("Open hexfile"), examplepath, (""),
                        FILETYPES, wxFD_OPEN, wxDefaultPosition);

    if ( openFileDialog->ShowModal() == wxID_OK )
    {
        // get the folder of the opened file, without the name&extension
        m_defaultPath=wxFileName(openFileDialog->GetPath()).GetPath();

        upp_open_file(openFileDialog->GetPath());
    }
}

bool UppMainWindow::upp_open_file(const wxString& path)
{
    if (!m_hexFile.open(&m_picType,path.mb_str(wxConvUTF8)))
    {
        SetStatusText(_("Unable to open file"),STATUS_FIELD_OTHER);
        wxLogError(_("Unable to open file"));
        return false;
    }
    else
    {
        m_hardware.backupOscCalBandGap(&m_picType);
        m_hexFile.putOscCalBandGap (&m_picType);
        UpdatePicInfo();
        UpdateTitle();
        m_history.AddFileToHistory(path);

        return true;
    }
}

void UppMainWindow::upp_refresh()
{
    if (!m_hexFile.hasFileName())
    {
        SetStatusText(_("No file to refresh"),STATUS_FIELD_OTHER);
        wxLogMessage(_("No file to refresh"));
        return;
    }

    if (!ShouldContinueIfUnsaved())
        return;

    if (!m_hexFile.reload(&m_picType))
    {
        SetStatusText(_("Unable to open file"),STATUS_FIELD_OTHER);
        wxLogError(_("Unable to open file"));
    }
    else
    {
        UpdatePicInfo();
        UpdateTitle();
    }
}

void UppMainWindow::upp_save()
{
    if (m_hexFile.hasFileName())
    {
        if (!m_hexFile.save(&m_picType))
        {
            SetStatusText(_("Unable to save file"),STATUS_FIELD_OTHER);
            wxLogError(_("Unable to save file"));
        }
        else
            UpdateTitle();
    }
    else upp_save_as();
}

void UppMainWindow::upp_save_as()
{
    wxFileDialog* openFileDialog =
        new wxFileDialog( this, _("Save hexfile"), m_defaultPath, (""),
                        FILETYPES, wxFD_SAVE, wxDefaultPosition);

    if ( openFileDialog->ShowModal() == wxID_OK )
    {
        // get the folder of the opened file, without the name&extension
        m_defaultPath=wxFileName(openFileDialog->GetPath()).GetPath();

        if (!m_hexFile.saveAs(&m_picType,openFileDialog->GetPath().mb_str(wxConvUTF8)))
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

void UppMainWindow::upp_program()
{
    if (!m_hardware.connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    // run the operation in a secondary thread
    RunThread(THREAD_PROGRAM);
}

void UppMainWindow::upp_read()
{
    if (!m_hardware.connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    // this command overwrites current code/config/data HEX... so ask to the user
    // before proceeding:
    if (!ShouldContinueIfUnsaved())
        return;

    // run the operation in a secondary thread
    RunThread(THREAD_READ);
}

void UppMainWindow::upp_verify()
{
    if ( !m_hardware.connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    // run the operation in a secondary thread
    RunThread(THREAD_VERIFY);
}

void UppMainWindow::upp_erase()
{
    if ( !m_hardware.connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    // this command overwrites current code/config/data HEX... so ask to the user
    // before proceeding:
    if (!ShouldContinueIfUnsaved())
        return;

    // run the operation in a secondary thread
    RunThread(THREAD_ERASE);
}


void UppMainWindow::upp_restore()
{
    if((m_picType.picFamily!=P12F629)&&(m_picType.picFamily!=P12F508)&&(m_picType.picFamily!=P10F200)&&(m_picType.picFamily!=P10F202))
    {   
        wxLogError(_("Only valid for PIC12F629, PIC12F508, 10F20X and similar devices..."));
        return;
    }
    if ( !m_hardware.connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    // this command overwrites current code/config/data HEX... so ask to the user
    // before proceeding:
    if (!ShouldContinueIfUnsaved())
        return;

//    RunThread(THREAD_ERASE);

//	wxLogMessage(_("

    wxString selectedOscCal = wxGetTextFromUser(_("Please specify a new Osccal Value [3400 - 37FF]"), 
        _("Specify a new Osccal Value"), "3400", this);

    if(selectedOscCal.length()==0)return;
    int iSelectedOscCal;
    sscanf(selectedOscCal.c_str(),"%4X",&iSelectedOscCal);
    if((iSelectedOscCal<0x3400)|(iSelectedOscCal>0x37FF))
    {
        wxLogError(_("Please specify an Oscal Value between 3400 and 37FF"));
        return;
    }
    int selectedBandGap=0;
	if(m_picType.picFamily==P12F629)
	{
		wxArrayString bgChoices;
		bgChoices.Alloc(4);
		for(int i=0;i<4;i++)
		{
			bgChoices.Add("");
			bgChoices[i].Printf("%i",i);
		}
		selectedBandGap = wxGetSingleChoiceIndex(_("Please specify a bandgap value"),	_("Specify a bandgap value"), bgChoices,this);
		if(selectedBandGap == -1) return;
	}
    /**TODO Place these commands into a wxThread
    */

    if (m_hardware.bulkErase(&m_picType,false)<0)wxLogError(_("Error erasing the device"));
    if (m_hardware.restoreOscCalBandGap(&m_picType, iSelectedOscCal, selectedBandGap)<0)wxLogError(_("Error restoring Calibration Registers"));
    Reset();
}


void UppMainWindow::upp_blankcheck()
{
    if (!m_hardware.connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    // run the operation in a secondary thread
    RunThread(THREAD_BLANKCHECK);
}

bool UppMainWindow::upp_autodetect()
{
    if (!m_hardware.connected())
    {
        wxLogError(_("The programmer is not connected"));
        return false;
    }

    // this command changes the pic-type and thus resets the current code/config/data stuff...
    if (!ShouldContinueIfUnsaved())
        return false;

    int devId=m_hardware.autoDetectDevice();
    cout<<"Autodetected PIC ID: 0x"<<hex<<devId<<dec<<endl;

    // if devId is not a valid device ID, select the default PIC
    if((!m_picType.ok())&&(devId==-1))
    {
        m_picType = PicType::FindPIC(UPP_DEFAULT_PIC);
    }
    if (devId != -1)
    {
        m_picType = PicType::FindPIC(devId);
    }

    wxASSERT(m_picType.ok());
    m_hardware.setPicType(&m_picType);

    // sync the choicebox with m_picType
    wxString picName=m_picType.getPicName();
    m_pPICChoice->SetStringSelection(picName);

    if (devId<1)
    {
        SetStatusText(_("No PIC detected!"),STATUS_FIELD_HARDWARE);
        if (m_cfg.ConfigShowPopups)
            wxLogMessage(_("No PIC detected! Selecting the default PIC (%s)..."),
                        picName.c_str());
    }
    else
    {
        wxString msg = wxString::Format(_("Detected PIC model %s with device ID 0x%X"),
                                        picName.c_str(), devId);
        SetStatusText(msg,STATUS_FIELD_HARDWARE);
        if (m_cfg.ConfigShowPopups)
            wxLogMessage(msg);
    }

    Reset();

    return (devId>1);
}



bool UppMainWindow::upp_run_target()
{
    if(!m_hardware.runTarget()) {wxLogError(_("Unable to run target"));return false;}
	return true;
}

bool UppMainWindow::upp_stop_target()
{
    if(!m_hardware.stopTarget()) {wxLogError(_("Unable to stop target"));return false;}
	return true;
}

bool UppMainWindow::upp_connect()
{
    // recreate the hw class
    if (m_hardware.connected()) m_hardware.disconnect();
    m_hardware.connect(this, HW_UPP);

    if (m_hardware.connected())
    {
        upp_autodetect();       // already calls upp_new();

        FirmwareVersion firmwareVersion;
        if (m_hardware.getFirmwareVersion(&firmwareVersion)<0)
        {
            SetStatusText(_("Unable to read firmware version"),STATUS_FIELD_HARDWARE);
            wxLogMessage(_("Unable to read firmware version"));
        }
        else
        {
            wxString tmp = firmwareVersion.versionString;
            tmp.Trim().Append(_(" Connected"));
            
            SetStatusText(tmp, STATUS_FIELD_HARDWARE);
            if (m_cfg.ConfigShowPopups)
                wxLogMessage(tmp);

            if (!firmwareVersion.isBootloader)
                checkFirmwareVersion(firmwareVersion);
        }
    }
    else
    {
        // try to connect to the UPP bootloader since there are no UPP programmers...
        if (m_hardware.connected()) m_hardware.disconnect();
        m_hardware.connect(this, HW_BOOTLOADER);

        if (m_hardware.connected())
        {
            upp_autodetect();       // already calls upp_new();

            FirmwareVersion firmwareVersion;
            if (m_hardware.getFirmwareVersion(&firmwareVersion)<0)
            {
                SetStatusText(_("Unable to read version"),STATUS_FIELD_HARDWARE);
                wxLogMessage(_("Unable to read version"));
            }
            else
            {
                SetStatusText(firmwareVersion.versionString.Trim().Append(_(" Connected")),STATUS_FIELD_HARDWARE);
                if (m_cfg.ConfigShowPopups)
                    wxLogMessage(firmwareVersion.versionString.Trim().Append(_(" Connected")));
            }
        }
        else
        {
            m_picType=PicType::FindPIC(UPP_DEFAULT_PIC);     // select default PIC
            m_hardware.setPicType(&m_picType);
            m_pPICChoice->SetStringSelection(m_picType.getPicName());

            SetStatusText(_("Bootloader or programmer not found"),STATUS_FIELD_HARDWARE);
            if (m_cfg.ConfigShowPopups)
                wxLogMessage(_("Bootloader or programmer not found"));

            upp_new();
        }
    }

    return m_hardware.connected();
}

void UppMainWindow::upp_disconnect()
{
    if (m_hardware.connected())
    {
        m_hardware.disconnect();

        SetStatusText(_("Disconnected"),STATUS_FIELD_HARDWARE);
        if (m_cfg.ConfigShowPopups)
            wxLogMessage(_("Disconnected"));
    }
    else
    {
        SetStatusText(_("Already disconnected"),STATUS_FIELD_HARDWARE);
        wxLogMessage(_("Already disconnected"));
    }
}

void UppMainWindow::upp_preferences()
{
    UppPreferencesDialog dlg(this);

    dlg.SetConfigFields(m_cfg);
    if (dlg.ShowModal() == wxID_OK)
        m_cfg = dlg.GetResult();
}

void UppMainWindow::upp_help()
{
    //wxLaunchDefaultBrowser(("http://usbpicprog.org/"));
	m_HelpController->Display("Introduction");
}

void UppMainWindow::upp_about()
{
    wxAboutDialogInfo aboutInfo;
    wxString ver;

    aboutInfo.SetName(("Usbpicprog"));
    #ifndef UPP_VERSION
    ver = wxString(("(SVN) ")).Append(wxString::FromAscii(SVN_REVISION));
    #else
    ver = wxString::FromAscii(UPP_VERSION);
    #endif
    aboutInfo.SetVersion(ver + " - " + wxPlatformInfo().Get().GetArchName());
    aboutInfo.SetDescription(_("An open source USB pic programmer"));
    //aboutInfo.SetCopyright("(C) 2008");
    aboutInfo.SetWebSite(("http://usbpicprog.org/"));
    aboutInfo.AddDeveloper(("Frans Schreuder"));
    aboutInfo.AddDeveloper(("Jan Paul Posma"));
    aboutInfo.AddDeveloper(("Francesco Montorsi"));
    aboutInfo.AddDeveloper(("Gustav Johansson"));
    aboutInfo.AddDeveloper(("Patrick Nies"));

    wxAboutBox(aboutInfo);
}

void UppMainWindow::upp_io_test()
{
    if ( !m_hardware.connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    IOTestDialog dlg(&m_hardware, this);
    dlg.ShowModal();
}

void UppMainWindow::upp_pic_choice_changed()
{
    // user changed the pic-type and thus we need to either
    // - reset the current code/config/data grids
    // - go back to the previously selected PIC
    if (!ShouldContinueIfUnsaved())
    {
        // revert selection to the previous type
        m_pPICChoice->SetStringSelection(m_picType.getPicName());
        return;
    }
    if (m_hardware.connected())
    {
        if (m_hardware.getCurrentHardware()==HW_BOOTLOADER)
        {
            // revert selection to the previous type
            m_pPICChoice->SetStringSelection(m_picType.getPicName());
            wxLogError(_("Cannot select a different PIC when the bootloader is connected!"));
            return;
        }
    }
    
    // update the pic type
    m_picType = PicType::FindPIC(m_pPICChoice->GetStringSelection());
    if(m_hardware.connected()) m_hardware.setPicType(&m_picType);

    // PIC changed; reset the code/config/data grids
    Reset();
}

void UppMainWindow::upp_pic_choice_changed_bymenu(int id)
{
    // user changed the pic-type and thus we need to either
    // - reset the current code/config/data grids
    // - go back to the previously selected PIC
    if (!ShouldContinueIfUnsaved())
    {
        return;     // do not change PIC selection
    }

    if (m_hardware.connected())
    {
        if (m_hardware.getCurrentHardware()==HW_BOOTLOADER)
        {
            // do not change PIC selection
            wxLogError(_("Cannot select a different PIC when the bootloader is connected!"));
            return;
        }    
    }

    // update the pic type
    m_picType = PicType::FindPIC(m_arrPICName[id]);

    if(m_hardware.connected()) m_hardware.setPicType(&m_picType);
    
    // keep the choice box synchronized
    m_pPICChoice->SetStringSelection(m_arrPICName[id]);

    // PIC changed; reset the code/config/data grids
    Reset();
}

void UppMainWindow::upp_package_variant_changed()
{
    // get the new package
    const ChipPackage& pkg = 
        m_picType.Package[m_pPackageVariants->GetSelection()];

    // set it to the package-view window
    if (m_picType.ok())
        m_pPackageWin->SetChip(wxString(m_picType.GetExtName().c_str()), pkg);
}

void UppMainWindow::upp_size_changed()
{
    static bool firsttime = true;
    
    // update the package view window 
    if (firsttime)
    {
        m_pPackageWin->FitBitmap();
        firsttime = false;
    }
}
