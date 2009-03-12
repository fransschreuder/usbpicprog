///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
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
	
	m_pCodeGrid = new UppHexViewGrid(m_pCodePanel, wxID_ANY, HEXVIEW_CODE);
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
	
	m_pConfigGrid = new UppHexViewGrid(m_pConfigPanel, wxID_ANY, HEXVIEW_CONFIG);
	bSizer3->Add( m_pConfigGrid, 1, wxALL|wxEXPAND, 5 );
	
	m_pConfigPanel->SetSizer( bSizer3 );
	m_pConfigPanel->Layout();
	bSizer3->Fit( m_pConfigPanel );
	m_pNotebook->AddPage( m_pConfigPanel, _("Configuration flags"), false );
	m_pDataPanel = new wxPanel( m_pNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText3 = new wxStaticText( m_pDataPanel, wxID_ANY, _("Hexadecimal view of the data area of the PIC:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer4->Add( m_staticText3, 0, wxALL, 5 );
	
	m_pDataGrid = new UppHexViewGrid(m_pDataPanel, wxID_ANY, HEXVIEW_DATA);
	bSizer4->Add( m_pDataGrid, 1, wxALL|wxEXPAND, 5 );
	
	m_pDataPanel->SetSizer( bSizer4 );
	m_pDataPanel->Layout();
	bSizer4->Fit( m_pDataPanel );
	m_pNotebook->AddPage( m_pDataPanel, _("Data"), true );
	m_pInfoPanel = new wxPanel( m_pNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_pInfoPanel, wxID_ANY, _("Miscellaneous") ), wxVERTICAL );
	
	m_pDatasheetLink = new wxHyperlinkCtrl( m_pInfoPanel, wxID_ANY, _("Datasheet"), wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	sbSizer1->Add( m_pDatasheetLink, 0, wxALL, 5 );
	
	m_pVPPText = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Vpp:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pVPPText->Wrap( -1 );
	sbSizer1->Add( m_pVPPText, 0, wxALL, 5 );
	
	m_pVDDText = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Vdd:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pVDDText->Wrap( -1 );
	sbSizer1->Add( m_pVDDText, 0, wxALL, 5 );
	
	m_pFrequencyText = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Frequency range:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pFrequencyText->Wrap( -1 );
	sbSizer1->Add( m_pFrequencyText, 0, wxALL, 5 );
	
	m_pDeviceIDText = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Device ID:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pDeviceIDText->Wrap( -1 );
	sbSizer1->Add( m_pDeviceIDText, 0, wxALL, 5 );
	
	m_pCodeMemoryText = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Code memory size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pCodeMemoryText->Wrap( -1 );
	sbSizer1->Add( m_pCodeMemoryText, 0, wxALL, 5 );
	
	m_pDataMemoryText = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Data memory size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pDataMemoryText->Wrap( -1 );
	sbSizer1->Add( m_pDataMemoryText, 0, wxALL, 5 );
	
	bSizer5->Add( sbSizer1, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_pInfoPanel, wxID_ANY, _("Package") ), wxVERTICAL );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText7 = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Variants:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	bSizer7->Add( m_staticText7, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxArrayString m_pPackageVariantsChoices;
	m_pPackageVariants = new wxChoice( m_pInfoPanel, wxID_PACKAGE_VARIANT, wxDefaultPosition, wxDefaultSize, m_pPackageVariantsChoices, 0 );
	m_pPackageVariants->SetSelection( 0 );
	bSizer7->Add( m_pPackageVariants, 0, wxALIGN_CENTER|wxALL, 5 );
	
	sbSizer2->Add( bSizer7, 0, wxEXPAND, 5 );
	
	m_pPackageBmp = new wxStaticBitmap( m_pInfoPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_pPackageBmp, 1, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );
	
	m_staticText11 = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Note: red pins are (some of those) used in ICSP."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	sbSizer2->Add( m_staticText11, 0, wxALL, 5 );
	
	bSizer5->Add( sbSizer2, 1, wxALL|wxEXPAND, 5 );
	
	m_pInfoPanel->SetSizer( bSizer5 );
	m_pInfoPanel->Layout();
	bSizer5->Fit( m_pInfoPanel );
	m_pNotebook->AddPage( m_pInfoPanel, _("PIC info"), false );
	
	m_pSizer->Add( m_pNotebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( m_pSizer );
	this->Layout();
	m_pMenuBar = new wxMenuBar( 0 );
	m_pMenuFile = new wxMenu();
	wxMenuItem* uppMenuNew;
	uppMenuNew = new wxMenuItem( m_pMenuFile, wxID_NEW, wxString( _("&New") ) , _("Resets the code, configuration and data areas of the currently open HEX file"), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuNew );
	
	wxMenuItem* uppMenuOpen;
	uppMenuOpen = new wxMenuItem( m_pMenuFile, wxID_OPEN, wxString( _("&Open") ) , _("Loads an HEX file for editing or programming..."), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuOpen );
	
	wxMenuItem* uppMenuRefresh;
	uppMenuRefresh = new wxMenuItem( m_pMenuFile, wxID_REFRESH, wxString( _("&Reload") ) + wxT('\t') + wxT("CTRL+R"), _("Reloads the current HEX file from disk"), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuRefresh );
	
	wxMenuItem* uppMenuSave;
	uppMenuSave = new wxMenuItem( m_pMenuFile, wxID_SAVE, wxString( _("&Save") ) , _("Saves the HEX file with the modified data"), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuSave );
	
	wxMenuItem* uppMenuSaveAs;
	uppMenuSaveAs = new wxMenuItem( m_pMenuFile, wxID_SAVEAS, wxString( _("Save &As") ) , _("Saves current code,config,data as a HEX file"), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuSaveAs );
	
	wxMenuItem* uppMenuExit;
	uppMenuExit = new wxMenuItem( m_pMenuFile, wxID_EXIT, wxString( _("E&xit") ) , _("Exits the program"), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuExit );
	
	m_pMenuBar->Append( m_pMenuFile, _("&File") );
	
	m_pMenuEdit = new wxMenu();
	wxMenuItem* uppMenuCopy;
	uppMenuCopy = new wxMenuItem( m_pMenuEdit, wxID_COPY, wxString( _("&Copy") ) , _("Copies to the clipboard currently selected cells"), wxITEM_NORMAL );
	m_pMenuEdit->Append( uppMenuCopy );
	
	wxMenuItem* uppMenuSelectAll;
	uppMenuSelectAll = new wxMenuItem( m_pMenuEdit, wxID_SELECTALL, wxString( _("Select &All") ) , _("Selects the entire grid currently shown"), wxITEM_NORMAL );
	m_pMenuEdit->Append( uppMenuSelectAll );
	
	m_pMenuEdit->AppendSeparator();
	
	wxMenuItem* uppMenuOptions;
	uppMenuOptions = new wxMenuItem( m_pMenuEdit, wxID_PREFERENCES, wxString( _("&Preferences") ) , _("Shows the options for this program"), wxITEM_NORMAL );
	m_pMenuEdit->Append( uppMenuOptions );
	
	m_pMenuBar->Append( m_pMenuEdit, _("&Edit") );
	
	m_pMenuHelp = new wxMenu();
	wxMenuItem* uppMenuItemHelp;
	uppMenuItemHelp = new wxMenuItem( m_pMenuHelp, wxID_HELP, wxString( _("&Help") ) , _("Opens the Usbpicprog's website..."), wxITEM_NORMAL );
	m_pMenuHelp->Append( uppMenuItemHelp );
	
	m_pMenuHelp->AppendSeparator();
	
	wxMenuItem* uppMenuAbout;
	uppMenuAbout = new wxMenuItem( m_pMenuHelp, wxID_ABOUT, wxString( _("&About") ) , _("Shows informations about this program"), wxITEM_NORMAL );
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
