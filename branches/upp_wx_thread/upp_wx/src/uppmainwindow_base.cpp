///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "uppmainwindow_base.h"

///////////////////////////////////////////////////////////////////////////

UppMainWindowBase::UppMainWindowBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	m_pSizer = new wxBoxSizer( wxVERTICAL );
	
	m_pNotebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_pCodePanel = new wxPanel( m_pNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( m_pCodePanel, wxID_ANY, _("Hexadecimal view of the code area of the PIC:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer2->Add( m_staticText1, 0, wxALL, 5 );
	
	m_pCodeGrid = new UppHexViewGrid(m_pCodePanel, wxID_ANY);
	bSizer2->Add( m_pCodeGrid, 1, wxALL|wxEXPAND, 5 );
	
	m_pCodePanel->SetSizer( bSizer2 );
	m_pCodePanel->Layout();
	bSizer2->Fit( m_pCodePanel );
	m_pNotebook->AddPage( m_pCodePanel, _("Code"), false );
	m_pConfigPanel = new wxPanel( m_pNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( m_pConfigPanel, wxID_ANY, _("Hexadecimal view of the configuration fuses of the PIC:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer3->Add( m_staticText2, 0, wxALL, 5 );
	
	m_pConfigGrid = new UppHexViewGrid(m_pConfigPanel, wxID_ANY);
	bSizer3->Add( m_pConfigGrid, 1, wxALL|wxEXPAND, 5 );
	
	m_pConfigPanel->SetSizer( bSizer3 );
	m_pConfigPanel->Layout();
	bSizer3->Fit( m_pConfigPanel );
	m_pNotebook->AddPage( m_pConfigPanel, _("Configuration flags"), true );
	m_pDataPanel = new wxPanel( m_pNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText3 = new wxStaticText( m_pDataPanel, wxID_ANY, _("Hexadecimal view of the data area of the PIC:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer4->Add( m_staticText3, 0, wxALL, 5 );
	
	m_pDataGrid = new UppHexViewGrid(m_pDataPanel, wxID_ANY);
	bSizer4->Add( m_pDataGrid, 1, wxALL|wxEXPAND, 5 );
	
	m_pDataPanel->SetSizer( bSizer4 );
	m_pDataPanel->Layout();
	bSizer4->Fit( m_pDataPanel );
	m_pNotebook->AddPage( m_pDataPanel, _("Data"), false );
	
	m_pSizer->Add( m_pNotebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( m_pSizer );
	this->Layout();
	m_pMenuBar = new wxMenuBar( 0 );
	m_pMenuFile = new wxMenu();
	wxMenuItem* uppMenuNew;
	uppMenuNew = new wxMenuItem( m_pMenuFile, wxID_NEW, wxString( wxEmptyString ) , _("Resets the code, configuration and data areas of the currently open HEX file"), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuNew );
	
	wxMenuItem* uppMenuOpen;
	uppMenuOpen = new wxMenuItem( m_pMenuFile, wxID_OPEN, wxString( wxEmptyString ) , _("Loads an HEX file for editing or programming..."), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuOpen );
	
	wxMenuItem* uppMenuRefresh;
	uppMenuRefresh = new wxMenuItem( m_pMenuFile, wxID_REFRESH, wxString( _("&Reload") ) + wxT('\t') + wxT("CTRL+R"), _("Reloads the current HEX file from disk"), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuRefresh );
	
	wxMenuItem* uppMenuSave;
	uppMenuSave = new wxMenuItem( m_pMenuFile, wxID_SAVE, wxString( wxEmptyString ) , _("Saves the HEX file with the modified data"), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuSave );
	
	wxMenuItem* uppMenuSaveAs;
	uppMenuSaveAs = new wxMenuItem( m_pMenuFile, wxID_SAVEAS, wxString( wxEmptyString ) , _("Saves current code,config,data stuff as an HEX file"), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuSaveAs );
	
	wxMenuItem* uppMenuExit;
	uppMenuExit = new wxMenuItem( m_pMenuFile, wxID_EXIT, wxString( wxEmptyString ) , wxEmptyString, wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuExit );
	
	m_pMenuBar->Append( m_pMenuFile, _("&File") );
	
	m_pMenuEdit = new wxMenu();
	wxMenuItem* uppMenuCopy;
	uppMenuCopy = new wxMenuItem( m_pMenuEdit, wxID_COPY, wxString( wxEmptyString ) , _("Copies to the clipboard currently selected cells"), wxITEM_NORMAL );
	m_pMenuEdit->Append( uppMenuCopy );
	
	wxMenuItem* uppMenuSelectAll;
	uppMenuSelectAll = new wxMenuItem( m_pMenuEdit, wxID_SELECTALL, wxString( wxEmptyString ) , _("Selects the entire grid currently shown"), wxITEM_NORMAL );
	m_pMenuEdit->Append( uppMenuSelectAll );
	
	wxMenuItem* uppMenuOptions;
	uppMenuOptions = new wxMenuItem( m_pMenuEdit, wxID_PREFERENCES, wxString( wxEmptyString ) , _("Shows the options for this program"), wxITEM_NORMAL );
	m_pMenuEdit->Append( uppMenuOptions );
	
	m_pMenuBar->Append( m_pMenuEdit, _("&Edit") );
	
	m_pMenuHelp = new wxMenu();
	wxMenuItem* uppMenuItemHelp;
	uppMenuItemHelp = new wxMenuItem( m_pMenuHelp, wxID_HELP, wxString( wxEmptyString ) , _("Opens the Usbpicprog's website..."), wxITEM_NORMAL );
	m_pMenuHelp->Append( uppMenuItemHelp );
	
	m_pMenuHelp->AppendSeparator();
	
	wxMenuItem* uppMenuAbout;
	uppMenuAbout = new wxMenuItem( m_pMenuHelp, wxID_ABOUT, wxString( wxEmptyString ) , _("Shows informations about this program"), wxITEM_NORMAL );
	m_pMenuHelp->Append( uppMenuAbout );
	
	m_pMenuBar->Append( m_pMenuHelp, _("&Help") );
	
	this->SetMenuBar( m_pMenuBar );
	
	m_pStatusBar = this->CreateStatusBar( 2, wxST_SIZEGRIP, wxID_ANY );
	
	// Connect Events
	this->Connect( uppMenuNew->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_new ) );
	this->Connect( uppMenuOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_open ) );
	this->Connect( uppMenuRefresh->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_refresh ) );
	this->Connect( uppMenuSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_save ) );
	this->Connect( uppMenuSaveAs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_save_as ) );
	this->Connect( uppMenuExit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_exit ) );
	this->Connect( uppMenuCopy->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_copy ) );
	this->Connect( uppMenuSelectAll->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_selectall ) );
	this->Connect( uppMenuOptions->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_preferences ) );
	this->Connect( uppMenuItemHelp->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_help ) );
	this->Connect( uppMenuAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_about ) );
}

UppMainWindowBase::~UppMainWindowBase()
{
	// Disconnect Events
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_new ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_open ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_refresh ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_save ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_save_as ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_exit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_copy ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_selectall ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_preferences ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_help ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_about ) );
}
