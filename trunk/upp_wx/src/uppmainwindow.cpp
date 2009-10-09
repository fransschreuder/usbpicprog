/***************************************************************************
*   Copyright (C) 2008-2009 by Frans Schreuder, Francesco Montorsi        *
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


#include <wx/artprov.h>
#include <wx/toolbar.h>
#include <wx/choice.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/debug.h>
#include <wx/dataview.h>
#include <wx/dcmemory.h>

#if wxCHECK_VERSION(2,9,0)
#include <wx/persist/toplevel.h>
#include <wx/evtloop.h>
#endif

#include "uppmainwindow_base.h"
#include "uppmainwindow.h"
#include "hexview.h"
#include "packageview.h"
#include "configview.h"
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
    #include "../icons/win/zoomin.xpm"
    #include "../icons/win/zoomout.xpm"
    #include "../icons/win/zoomfit.xpm"
#endif

static const wxChar *FILETYPES = _T(
    "Hex files|*.hex;*.HEX|All files|*.*"
);

#if wxCHECK_VERSION(2,9,0)
wxDEFINE_EVENT( wxEVT_COMMAND_THREAD_UPDATE, wxThreadEvent );
wxDEFINE_EVENT( wxEVT_COMMAND_THREAD_COMPLETE, wxThreadEvent );
#else
extern const wxEventType wxEVT_COMMAND_THREAD_UPDATE;
extern const wxEventType wxEVT_COMMAND_THREAD_COMPLETE;
DEFINE_EVENT_TYPE(wxEVT_COMMAND_THREAD_UPDATE);
DEFINE_EVENT_TYPE(wxEVT_COMMAND_THREAD_COMPLETE);
#endif



// ----------------------------------------------------------------------------
// UppMainWindow
// ----------------------------------------------------------------------------

UppMainWindow::UppMainWindow(wxWindow* parent, wxWindowID id)
    : UppMainWindowBase( parent, id, wxEmptyString /* will be set later */,
                        wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL ),
    m_history(4)
{
    SetName(wxT("UppMainWindow"));

    // load settings from config file or set a default value
    wxConfigBase* pCfg = wxConfig::Get();
    pCfg->SetPath(wxT("/"));
    if ( !pCfg->Read(wxT("m_defaultPath"), &m_defaultPath) )
        m_defaultPath=wxT("");
    if ( !pCfg->Read(wxT("ConfigProgramCode"), &m_cfg.ConfigProgramCode))
        m_cfg.ConfigProgramCode=true;
    if ( !pCfg->Read(wxT("ConfigProgramConfig"), &m_cfg.ConfigProgramConfig))
        m_cfg.ConfigProgramConfig=true;
    if ( !pCfg->Read(wxT("ConfigProgramData"), &m_cfg.ConfigProgramData))
        m_cfg.ConfigProgramData=true;
    if ( !pCfg->Read(wxT("ConfigVerifyCode"), &m_cfg.ConfigVerifyCode))
        m_cfg.ConfigVerifyCode=true;
    if ( !pCfg->Read(wxT("ConfigVerifyConfig"), &m_cfg.ConfigVerifyConfig))
        m_cfg.ConfigVerifyConfig=true;
    if ( !pCfg->Read(wxT("ConfigVerifyData"), &m_cfg.ConfigVerifyData))
        m_cfg.ConfigVerifyData=true;
    if ( !pCfg->Read(wxT("ConfigEraseBeforeProgramming"), &m_cfg.ConfigEraseBeforeProgramming))
        m_cfg.ConfigEraseBeforeProgramming=true;
    if ( !pCfg->Read(wxT("ConfigVerifyAfterProgramming"), &m_cfg.ConfigVerifyAfterProgramming))
        m_cfg.ConfigShowPopups=false;
    if ( !pCfg->Read(wxT("ConfigShowPopups"), &m_cfg.ConfigShowPopups))
        m_cfg.ConfigShowPopups=false;
    m_history.Load(*pCfg);

    // non-GUI init:
    m_hardware=NULL;      // upp_connect() will allocate it
    m_dlgProgress=NULL;   // will be created when needed
    m_arrPICName=PicType::getSupportedPicNames();

    // GUI init:
    CompleteGUICreation();    // also loads the saved pos&size of this frame

    // find the hardware connected to the PC, if any:
    upp_connect();

#if wxCHECK_VERSION(2,9,0)
    // if upp_connect() didn't find a PIC device, load the last chosen PIC
    if (!m_hardware->connected())
    {
        // NOTE: select a PIC reading the last used one which was saved in wxConfig
        //       or just use the first one (0-th) as pCfg->Read() default value
        long lastPic=0;
        if ((lastPic=pCfg->Read(wxT("SelectedPIC"), (long)0)) >= 0 && 
            lastPic < (int)m_arrPICName.size())
        {
            m_picType = PicType::FindPIC(m_arrPICName[lastPic]);

            // keep the choice box synchronized
            m_pPICChoice->SetStringSelection(m_arrPICName[lastPic]);

            // PIC changed; reset the code/config/data grids
            Reset();
        }
    }
#endif
}

UppMainWindow::~UppMainWindow()
{
    if (m_hardware)
    {
        delete m_hardware;
        m_hardware = NULL;
    }

    // save settings
    wxConfigBase* pCfg = wxConfig::Get();
    pCfg->SetPath(wxT("/"));
    pCfg->Write(wxT("m_defaultPath"), m_defaultPath);
    pCfg->Write(wxT("ConfigProgramCode"), m_cfg.ConfigProgramCode);
    pCfg->Write(wxT("ConfigProgramConfig"), m_cfg.ConfigProgramConfig);
    pCfg->Write(wxT("ConfigProgramData"), m_cfg.ConfigProgramData);
    pCfg->Write(wxT("ConfigVerifyCode"), m_cfg.ConfigVerifyCode);
    pCfg->Write(wxT("ConfigVerifyConfig"), m_cfg.ConfigVerifyConfig);
    pCfg->Write(wxT("ConfigVerifyData"), m_cfg.ConfigVerifyData);
    pCfg->Write(wxT("ConfigVerifyAfterProgramming"), m_cfg.ConfigVerifyAfterProgramming);
    pCfg->Write(wxT("ConfigEraseBeforeProgramming"), m_cfg.ConfigEraseBeforeProgramming);
    pCfg->Write(wxT("ConfigShowPopups"), m_cfg.ConfigShowPopups);
    pCfg->Write(wxT("SelectedPIC"), m_pPICChoice->GetSelection());
    m_history.Save(*pCfg);
}

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
    pMenuProgram = new wxMenuItem( pMenuActions, wxID_PROGRAM, wxString( _("&Program...") ) + wxT('\t') + wxT("F7"),
                                _("Program the PIC device"), wxITEM_NORMAL );

    wxMenuItem* pMenuRead;
    pMenuRead = new wxMenuItem( pMenuActions, wxID_READ, wxString( _("&Read...") ) + wxT('\t') + wxT("F8"),
                                _("Read the PIC device"), wxITEM_NORMAL );

    wxMenuItem* pMenuVerify;
    pMenuVerify = new wxMenuItem( pMenuActions, wxID_VERIFY, wxString( _("&Verify...") ),
                                _("Verify the PIC device"), wxITEM_NORMAL );

    wxMenuItem* pMenuErase;
    pMenuErase = new wxMenuItem( pMenuActions, wxID_ERASE, wxString( _("&Erase...") ),
                                _("Erase the PIC device"), wxITEM_NORMAL );

    wxMenuItem* pMenuBlankCheck;
    pMenuBlankCheck = new wxMenuItem( pMenuActions, wxID_BLANKCHECK, wxString( _("&Blankcheck...") ),
                                    _("Blankcheck the PIC device"), wxITEM_NORMAL );

    wxMenuItem* pMenuAutoDetect;
    pMenuAutoDetect = new wxMenuItem( pMenuActions, wxID_AUTODETECT, wxString( _("&Autodetect...") ),
                                    _("Detect the type of the PIC device"), wxITEM_NORMAL );

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

    pMenuConnect->SetBitmap(wxArtProvider::GetBitmap(wxT("gtk-connect"), wxART_MENU));
    pMenuDisconnect->SetBitmap(wxArtProvider::GetBitmap(wxT("gtk-disconnect"), wxART_MENU));
#endif

    pMenuActions->Append( pMenuProgram );
    pMenuActions->Append( pMenuRead );
    pMenuActions->Append( pMenuVerify );
    pMenuActions->Append( pMenuErase );
    pMenuActions->Append( pMenuBlankCheck );
    pMenuActions->Append( pMenuAutoDetect );
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
        wxString family(m_arrPICName[i].substr(0, 4).c_str());

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
    this->Connect( wxID_BLANKCHECK, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_blankcheck ) );
    this->Connect( wxID_AUTODETECT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_autodetect ) );
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

    m_pPICChoice = new wxChoice(toolbar, wxID_PIC_CHOICE_COMBO, wxDefaultPosition, wxSize(120,-1));
    m_pPICChoice->SetToolTip(_("currently selected PIC type"));
    this->Connect( wxID_PIC_CHOICE_COMBO, wxEVT_COMMAND_CHOICE_SELECTED,
                wxCommandEventHandler( UppMainWindow::on_pic_choice_changed ) );

    toolbar->AddControl( m_pPICChoice );
    toolbar->Realize();

    // set bitmaps on the bitmap buttons
#ifdef __WXGTK__
	#if wxCHECK_VERSION(2,8,10)	
    m_pZoomInButton->SetBitmapLabel(wxArtProvider::GetBitmap("gtk-zoom-in"));
    m_pZoomOutButton->SetBitmapLabel(wxArtProvider::GetBitmap("gtk-zoom-out"));
    m_pZoomFitButton->SetBitmapLabel(wxArtProvider::GetBitmap("gtk-zoom-fit"));
	#else
	m_pZoomInButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxT("gtk-zoom-in")));
    m_pZoomOutButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxT("gtk-zoom-out")));
    m_pZoomFitButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxT("gtk-zoom-fit")));
	#endif
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
    this->SetSizerAndFit(m_pSizer);

#if wxCHECK_VERSION(2,9,0)
    // eventually load position&size of this window from the wxConfig object:
    wxPersistenceManager::Get().RegisterAndRestore(this);
#endif

    // misc event handlers
    this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( UppMainWindow::on_close ) );
    this->Connect( wxEVT_SIZE, wxSizeEventHandler( UppMainWindow::on_size ) );
    this->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( UppMainWindow::on_package_variant_changed ) );
#if wxCHECK_VERSION(2,9,0)
    this->Connect( wxEVT_COMMAND_THREAD_UPDATE, wxThreadEventHandler( UppMainWindow::OnThreadUpdate ) );
    this->Connect( wxEVT_COMMAND_THREAD_COMPLETE, wxThreadEventHandler( UppMainWindow::OnThreadCompleted ) );
#else
    this->Connect( wxEVT_COMMAND_THREAD_UPDATE, wxCommandEventHandler( UppMainWindow::OnThreadUpdate ) );
    this->Connect( wxEVT_COMMAND_THREAD_COMPLETE, wxCommandEventHandler( UppMainWindow::OnThreadCompleted ) );
#endif

    // set default title name
    UpdateTitle();

    // show default stuff
    UpdatePicInfo();

    // this is to be sure that the package-view window gets updated by our upp_size_changed()
    // event handler to reflect the effective window size
    // (this somehow doesn't happen on wxGTK; probably because of deferred top-window resizing)
	#if wxCHECK_VERSION(2,9,0)	
    PostSizeEvent();
	#endif
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
#ifndef SVN_REVISION
#define SVN_REVISION UPP_VERSION
#endif

void UppMainWindow::checkFirmwareVersion(FirmwareVersion firmwareVersion)
{
    if (!firmwareVersion.stableRelease)
    {
        if (firmwareVersion.release<DEV_VERSION)
            wxLogMessage(_("Your firmware is too old; please consider upgrading it")); 
        return;
    }
    
    // check major digit
    
    if (firmwareVersion.major>STABLE_VERSION_MAJOR)
    {
        wxLogMessage(_("Firmware probably too new")); 
        return;
    }
    else if (firmwareVersion.major==STABLE_VERSION_MAJOR)
    {
        // check minor digit
        
        if (firmwareVersion.minor>STABLE_VERSION_MINOR)
        {
            wxLogMessage(_("Firmware probably too new")); 
            return;
        }
        else if (firmwareVersion.minor==STABLE_VERSION_MINOR)
        {
            // check release digit
            wxString stableFirmwareVersion;
#if wxCHECK_VERSION(2,8,10)
			stableFirmwareVersion.Printf("%i.%i.%i", STABLE_VERSION_MAJOR, STABLE_VERSION_MINOR, STABLE_VERSION_RELEASE);
#else
			stableFirmwareVersion.Printf(wxT("%i.%i.%i"), STABLE_VERSION_MAJOR, STABLE_VERSION_MINOR, STABLE_VERSION_RELEASE);
#endif			
            if (firmwareVersion.release>STABLE_VERSION_RELEASE)
            {
                wxLogMessage(_("Firmware probably too new")); 
                return;
            }
            else if (firmwareVersion.release==STABLE_VERSION_RELEASE && 
                     (wxT(SVN_REVISION)!=stableFirmwareVersion))
            {
                wxLogMessage(_("You are using the a stable release of the firmware with a development version of the software. Consider upgrading your firmware")); 
                return;
            }
            else
            {
                wxLogMessage(_("Your firmware is too old; please consider upgrading it")); 
                return;
            }
        }
        else
        {
            wxLogMessage(_("Your firmware is too old; please consider upgrading it")); 
            return; 
        }
    }
    else
    {
        wxLogMessage(_("Your firmware is too old; please consider upgrading it"));  
        return; 
    }
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
        str += wxT(" - [") + wxString(_("untitled"));
    else
        str += wxT(" - [") + wxString::FromAscii(m_hexFile.getFileName());

    if (m_hexFile.wasModified())
        str += wxT(" *");

    SetTitle(str + wxT("]"));
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
#if wxCHECK_VERSION(2,9,0)	
    m_pDatasheetLink->SetLabel(wxString::Format(_("%s datasheet"), wxString(m_picType.GetExtName().c_str())));
    m_pDatasheetLink->SetURL(
        wxString::Format("http://www.google.com/search?q=%s%%2Bdatasheet&as_sitesearch=microchip.com", wxString(m_picType.GetExtName().c_str())));
#else
    m_pDatasheetLink->SetLabel(wxString::Format(_("%s datasheet"), wxString(m_picType.GetExtName().c_str()).c_str()));
    m_pDatasheetLink->SetURL(
        wxString::Format(wxT("http://www.google.com/search?q=%s%%2Bdatasheet&as_sitesearch=microchip.com"), wxString(m_picType.GetExtName().c_str()).c_str()));
#endif                       
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
    m_pTypeText->SetLabel(wxString::Format(_("Type: %d-bit microcontroller"), m_picType.is16Bit() ? 16 : 14));

    m_pInfoPanel->Layout();
        // size may have changed: relayout the panel which is the parent of the windows
        // whose size has changed (do it after updating _all_ label texts)

    // update the package variants combobox
    m_pPackageVariants->Clear();
    for (unsigned int i=0; i<pkg.size(); i++)
#if wxCHECK_VERSION(2,9,0)	                         
        m_pPackageVariants->Append(
            wxString::Format(_("%s [%d pins]"), pkg[i].GetName(), pkg[i].GetPinCount()));
#else
                         m_pPackageVariants->Append(
            wxString::Format(_("%s [%d pins]"), pkg[i].GetName().c_str(), pkg[i].GetPinCount()));
#endif                        
    m_pPackageVariants->SetSelection(0);

    // let's update the bitmap and the pin names:
    upp_package_variant_changed();
}

void UppMainWindow::Reset()
{
    m_hexFile.newFile(&m_picType);

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
#if wxCHECK_VERSION(2,9,0)
    if (wxEventLoopBase::GetActive() == NULL)
        return;     // this may happen at startup, when the Hardware class is initialized

    wxThreadEvent* ev = new wxThreadEvent(wxEVT_COMMAND_THREAD_UPDATE);
    ev->SetInt(value);
    wxQueueEvent(this, ev);
#else
    wxCommandEvent ev(wxEVT_COMMAND_THREAD_UPDATE);
    ev.SetInt(value);
    wxPostEvent(this, ev);
#endif
}

#if wxCHECK_VERSION(2,9,0)
void UppMainWindow::OnThreadUpdate(wxThreadEvent& evt)
#else
void UppMainWindow::OnThreadUpdate(wxCommandEvent& evt)
#endif
{
    // NOTE: this function is executed in the primary thread's context!
    wxASSERT(wxThread::IsMain());

    if (m_dlgProgress)
    {
        wxCriticalSectionLocker lock(m_arrLogCS);

        bool continueOperation =
#if wxCHECK_VERSION(2,9,0)
            m_dlgProgress->Update(evt.GetInt(),
                                _("Please wait until the operations are completed:\n") +
                                wxJoin(m_arrLog, '\n'));

        // update to the new label size
        m_dlgProgress->Fit();

        // make sure the focused window is the progress dialog:
        m_dlgProgress->SetFocus();
#else
            m_dlgProgress->Update(evt.GetInt());
#endif
        if (!continueOperation &&               // user clicked "abort"?
            !m_hardware->operationsAborted())   // is the hardware already aborting?
        {
            m_hardware->abortOperations(true);
            wxLogWarning(_("Operations aborted"));
        }
    }
}

#if wxCHECK_VERSION(2,9,0)
void UppMainWindow::OnThreadCompleted(wxThreadEvent&)
#else
void UppMainWindow::OnThreadCompleted(wxCommandEvent&)
#endif
{
    // NOTE: this function is executed in the primary thread's context!
    wxASSERT(wxThread::IsMain());

    // reset abort flag:
    m_hardware->abortOperations(false);

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

#if !wxCHECK_VERSION(2,9,0)
    // NOTE: since wx2.9.0 thanks to the selective Yield() behaviour,
    //       we are sure that the order of the wxEVT_COMMAND_THREAD_UPDATE
    //       and wxEVT_COMMAND_THREAD_COMPLETE events will be respected.
    //       I.e. the "update" one will always be processed before the "complete" one.
    //       Previous wx2.9 this was not granted since wxProgressDialog::Update
    //       called wxYieldIfNeeded(), thus we use a Sleep(500) to (try to)
    //       ensure that "complete" will be processed after the "update" event.
    //       See ticket #10320 in wxWidgets for more info.
    wxThread::Sleep(500);
#endif

    // signal the main thread we've completed our task; this will result
    // in a call to UppMainWindow::OnThreadCompleted done in the primary
    // thread context:

#if wxCHECK_VERSION(2,9,0)
    wxQueueEvent(this, new wxThreadEvent(wxEVT_COMMAND_THREAD_COMPLETE));
#else
    wxCommandEvent ev(wxEVT_COMMAND_THREAD_COMPLETE);
    wxPostEvent(this, ev);
#endif

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

        switch(m_hardware->bulkErase(&m_picType))
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
        switch(m_hardware->write(TYPE_CODE, &m_hexFile, &m_picType))
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

        switch(m_hardware->write(TYPE_DATA, &m_hexFile, &m_picType))
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

        switch(m_hardware->write(TYPE_CONFIG, &m_hexFile, &m_picType))
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

    if (m_hardware->read(TYPE_CODE, &m_hexFile, &m_picType)<0)
    {
        LogFromThread(wxLOG_Error, _("Error reading code memory"));
        m_hexFile.trimData(&m_picType);
        return false;
    }

    if (GetThread()->TestDestroy())
        return false;   // stop the operation...

    LogFromThread(wxLOG_Message, _("Reading the data area of the PIC..."));
    if (m_hardware->read(TYPE_DATA, &m_hexFile, &m_picType)<0)
    {
        LogFromThread(wxLOG_Error, _("Error reading data memory"));
        m_hexFile.trimData(&m_picType);
        return false;
    }

    if (GetThread()->TestDestroy())
        return false;   // stop the operation...

    LogFromThread(wxLOG_Message, _("Reading the configuration area of the PIC..."));
    if (m_hardware->read(TYPE_CONFIG, &m_hexFile, &m_picType)<0)
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
        m_hardware->verify(&m_hexFile, &m_picType, m_cfg.ConfigVerifyCode,
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

    if (m_hardware->bulkErase(&m_picType)<0)
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
    VerifyResult res = m_hardware->blankCheck(&m_picType);

    switch (res.Result)
    {
    case VERIFY_SUCCESS:
        LogFromThread(wxLOG_Message, _("Device is blank"));
        break;
    case VERIFY_MISMATCH:
        switch (res.DataType)
        {
#if wxCHECK_VERSION(2,8,10)
            case TYPE_CODE: typeText="code";break;
            case TYPE_DATA: typeText="data";break;
            case TYPE_CONFIG: typeText="config";break;
            default: typeText="unknown";break;
#else
			case TYPE_CODE: typeText=wxT("code");break;
            case TYPE_DATA: typeText=wxT("data");break;
            case TYPE_CONFIG: typeText=wxT("config");break;
            default: typeText=wxT("unknown");break;			
#endif			
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
                        _("Initializing..."),
                        101,
                        this,
                        // NOTE: it's very important to give the wxPD_AUTO_HIDE
                        //       style to wxProgressDialog otherwise we may get
                        //       unwanted reentrancies (see wx docs)
                        wxPD_AUTO_HIDE |
                        wxPD_CAN_ABORT |
                        wxPD_APP_MODAL |
                        wxPD_ELAPSED_TIME |
                        wxPD_ESTIMATED_TIME |
                        wxPD_REMAINING_TIME
                    );

    // inform the thread about which operation it must perform;
    // note that the thread is not running yet so there's no need for a critical section:
    m_mode = mode;

#if wxCHECK_VERSION(2,9,0)
    if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
#else
    if (wxThreadHelper::Create() != wxTHREAD_NO_ERROR)
#endif
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
        new wxFileDialog( this, _("Open hexfile"), m_defaultPath, wxT(""),
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
        new wxFileDialog( this, _("Save hexfile"), m_defaultPath, wxT(""),
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
    if (m_hardware == NULL || !m_hardware->connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    // run the operation in a secondary thread
    RunThread(THREAD_PROGRAM);
}

void UppMainWindow::upp_read()
{
    if (m_hardware == NULL || !m_hardware->connected())
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
    if (m_hardware == NULL || !m_hardware->connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    // run the operation in a secondary thread
    RunThread(THREAD_VERIFY);
}

void UppMainWindow::upp_erase()
{
    if (m_hardware == NULL || !m_hardware->connected())
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

void UppMainWindow::upp_blankcheck()
{
    if (m_hardware == NULL || !m_hardware->connected())
    {
        wxLogError(_("The programmer is not connected"));
        return;
    }

    // run the operation in a secondary thread
    RunThread(THREAD_BLANKCHECK);
}

bool UppMainWindow::upp_autodetect()
{
    if (m_hardware == NULL || !m_hardware->connected())
    {
        wxLogError(_("The programmer is not connected"));
        return false;
    }

    // this command changes the pic-type and thus resets the current code/config/data stuff...
    if (!ShouldContinueIfUnsaved())
        return false;

    int devId=m_hardware->autoDetectDevice();
    cout<<"Autodetected PIC ID: 0x"<<hex<<devId<<dec<<endl;

    // if devId is not a valid device ID, select the default PIC
	if((!m_picType.ok())&&(devId==-1))
	{
#if wxCHECK_VERSION(2,8,10)
		m_picType = PicType::FindPIC(UPP_DEFAULT_PIC);
#else		
		m_picType = PicType::FindPIC(wxT(UPP_DEFAULT_PIC));
#endif		
    	wxASSERT(m_picType.ok());
    	m_hardware->setPicType(&m_picType);
	}
	if (devId != -1)
	{
        m_picType = PicType::FindPIC(devId);
    	wxASSERT(m_picType.ok());
    	m_hardware->setPicType(&m_picType);
	}

    // sync the choicebox with m_picType
    wxString picName=m_picType.getPicName();
    m_pPICChoice->SetStringSelection(picName);

    if (devId<1)
    {
        SetStatusText(_("No PIC detected!"),STATUS_FIELD_HARDWARE);
        if (m_cfg.ConfigShowPopups)
            wxLogMessage(_("No PIC detected! Selecting the default PIC (%s)..."),
                        picName.Mid(1).c_str());
    }
    else
    {
        wxString msg = wxString::Format(_("Detected PIC model %s with device ID 0x%X"),
                                        picName.Mid(1).c_str(), devId);
        SetStatusText(msg,STATUS_FIELD_HARDWARE);
        if (m_cfg.ConfigShowPopups)
            wxLogMessage(msg);
    }

    Reset();

    return (devId>1);
}

bool UppMainWindow::upp_connect()
{
    // recreate the hw class
    if (m_hardware != NULL) delete m_hardware;
    m_hardware=new Hardware(this, HW_UPP);

    if (m_hardware->connected())
    {
        upp_autodetect();       // already calls upp_new();

        FirmwareVersion firmwareVersion;
        if (m_hardware->getFirmwareVersion(&firmwareVersion)<0)
        {
            SetStatusText(_("Unable to read firmware version"),STATUS_FIELD_HARDWARE);
            wxLogMessage(_("Unable to read firmware version"));
        }
        else
        {
            SetStatusText(firmwareVersion.versionString.Trim().Append(_(" Connected")),
                          STATUS_FIELD_HARDWARE);
            if (m_cfg.ConfigShowPopups)
                wxLogMessage(firmwareVersion.versionString.Trim().Append(_(" Connected")));

            if (!firmwareVersion.isBootloader)
                checkFirmwareVersion(firmwareVersion);
        }
    }
    else
    {
        // try to connect to the UPP bootloader since there are no UPP programmers...

        delete m_hardware;
        m_hardware=new Hardware(this, HW_BOOTLOADER);

        if (m_hardware->connected())
        {
            upp_autodetect();       // already calls upp_new();

            FirmwareVersion firmwareVersion;
            if (m_hardware->getFirmwareVersion(&firmwareVersion)<0)
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
#if wxCHECK_VERSION(2,8,10)			
            m_picType=PicType::FindPIC(UPP_DEFAULT_PIC);     // select default PIC
#else			
			m_picType=PicType::FindPIC(wxT(UPP_DEFAULT_PIC));     // select default PIC
#endif			
			
            m_hardware->setPicType(&m_picType);
            m_pPICChoice->SetStringSelection(m_picType.getPicName());

            SetStatusText(_("Bootloader or programmer not found"),STATUS_FIELD_HARDWARE);
            if (m_cfg.ConfigShowPopups)
                wxLogMessage(_("Bootloader or programmer not found"));

            upp_new();
        }
    }

    wxASSERT(m_hardware);

    return m_hardware->connected();
}

void UppMainWindow::upp_disconnect()
{
    if (m_hardware != NULL)
    {
        if (m_hardware->connected())
        {
            delete m_hardware;
            m_hardware = NULL;

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
    wxLaunchDefaultBrowser(wxT("http://usbpicprog.org/"));
}

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
    //aboutInfo.SetCopyright("(C) 2008");
    aboutInfo.SetWebSite(wxT("http://usbpicprog.org/"));
    aboutInfo.AddDeveloper(wxT("Frans Schreuder"));
    aboutInfo.AddDeveloper(wxT("Jan Paul Posma"));
    aboutInfo.AddDeveloper(wxT("Francesco Montorsi"));

    wxAboutBox(aboutInfo);
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
    if (m_hardware != NULL)
    {
        if (m_hardware->getCurrentHardware()==HW_BOOTLOADER)
        {
            // revert selection to the previous type
            m_pPICChoice->SetStringSelection(m_picType.getPicName());
            wxLogError(_("Cannot select a different PIC when the bootloader is connected!"));
            return;
        }

        m_hardware->setPicType(&m_picType);
    }
    
    // update the pic type
    m_picType = PicType::FindPIC(m_pPICChoice->GetStringSelection());
    
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

    if (m_hardware != NULL)
    {
        if (m_hardware->getCurrentHardware()==HW_BOOTLOADER)
        {
            // do not change PIC selection
            wxLogError(_("Cannot select a different PIC when the bootloader is connected!"));
            return;
        }

        m_hardware->setPicType(&m_picType);
    }

    // update the pic type
    m_picType = PicType::FindPIC(m_arrPICName[id]);

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
