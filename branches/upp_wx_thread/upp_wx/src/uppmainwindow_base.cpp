///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "hexview.h"

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
	
	m_pCodeGrid = new UppHexViewGrid( m_pCodePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_pCodeGrid->CreateGrid( 0, 24 );
	m_pCodeGrid->EnableEditing( true );
	m_pCodeGrid->EnableGridLines( false );
	m_pCodeGrid->EnableDragGridSize( false );
	m_pCodeGrid->SetMargins( 0, 0 );
	
	// Columns
	m_pCodeGrid->EnableDragColMove( false );
	m_pCodeGrid->EnableDragColSize( false );
	m_pCodeGrid->SetColLabelSize( 30 );
	m_pCodeGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_pCodeGrid->EnableDragRowSize( false );
	m_pCodeGrid->SetRowLabelSize( 80 );
	m_pCodeGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_pCodeGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
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
	
	m_pConfigGrid = new wxGrid( m_pConfigPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_pConfigGrid->CreateGrid( 5, 5 );
	m_pConfigGrid->EnableEditing( true );
	m_pConfigGrid->EnableGridLines( true );
	m_pConfigGrid->EnableDragGridSize( false );
	m_pConfigGrid->SetMargins( 0, 0 );
	
	// Columns
	m_pConfigGrid->EnableDragColMove( false );
	m_pConfigGrid->EnableDragColSize( true );
	m_pConfigGrid->SetColLabelSize( 30 );
	m_pConfigGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_pConfigGrid->EnableDragRowSize( true );
	m_pConfigGrid->SetRowLabelSize( 80 );
	m_pConfigGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_pConfigGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
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
	
	m_pDataGrid = new wxGrid( m_pDataPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_pDataGrid->CreateGrid( 5, 5 );
	m_pDataGrid->EnableEditing( true );
	m_pDataGrid->EnableGridLines( true );
	m_pDataGrid->EnableDragGridSize( false );
	m_pDataGrid->SetMargins( 0, 0 );
	
	// Columns
	m_pDataGrid->EnableDragColMove( false );
	m_pDataGrid->EnableDragColSize( true );
	m_pDataGrid->SetColLabelSize( 30 );
	m_pDataGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_pDataGrid->EnableDragRowSize( true );
	m_pDataGrid->SetRowLabelSize( 80 );
	m_pDataGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_pDataGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
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
	uppMenuNew = new wxMenuItem( m_pMenuFile, wxID_NEW, wxString( _("New") ) + wxT('\t') + wxT("CTRL+N"), wxEmptyString, wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuNew );
	
	wxMenuItem* uppMenuOpen;
	uppMenuOpen = new wxMenuItem( m_pMenuFile, wxID_OPEN, wxString( _("Open...") ) + wxT('\t') + wxT("CTRL+O"), wxEmptyString, wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuOpen );
	
	wxMenuItem* uppMenuRefresh;
	uppMenuRefresh = new wxMenuItem( m_pMenuFile, wxID_REFRESH, wxString( _("Refresh") ) + wxT('\t') + wxT("CTRL+R"), wxEmptyString, wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuRefresh );
	
	wxMenuItem* uppMenuSave;
	uppMenuSave = new wxMenuItem( m_pMenuFile, wxID_SAVE, wxString( _("Save") ) + wxT('\t') + wxT("CTRL+S"), wxEmptyString, wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuSave );
	
	wxMenuItem* uppMenuSaveAs;
	uppMenuSaveAs = new wxMenuItem( m_pMenuFile, wxID_SAVE_AS, wxString( _("Save As...") ) , wxEmptyString, wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuSaveAs );
	
	m_pMenuFile->AppendSeparator();
	
	wxMenuItem* uppMenuExit;
	uppMenuExit = new wxMenuItem( m_pMenuFile, wxID_EXIT, wxString( _("Exit") ) + wxT('\t') + wxT("ALT+F4"), wxEmptyString, wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuExit );
	
	m_pMenuBar->Append( m_pMenuFile, _("File") );
	
	m_pMenuEdit = new wxMenu();
	wxMenuItem* uppMenuCopy;
	uppMenuCopy = new wxMenuItem( m_pMenuEdit, wxID_COPY, wxString( _("&Copy") ) , wxEmptyString, wxITEM_NORMAL );
	m_pMenuEdit->Append( uppMenuCopy );
	
	wxMenuItem* uppMenuSelectAll;
	uppMenuSelectAll = new wxMenuItem( m_pMenuEdit, wxID_SELECTALL, wxString( _("Select all") ) , wxEmptyString, wxITEM_NORMAL );
	m_pMenuEdit->Append( uppMenuSelectAll );
	
	m_pMenuBar->Append( m_pMenuEdit, _("Edit") );
	
	uppMenuHelp = new wxMenu();
	wxMenuItem* uppMenuItemHelp;
	uppMenuItemHelp = new wxMenuItem( uppMenuHelp, wxID_HELP, wxString( _("Help") ) + wxT('\t') + wxT("F1"), wxEmptyString, wxITEM_NORMAL );
	uppMenuHelp->Append( uppMenuItemHelp );
	
	uppMenuHelp->AppendSeparator();
	
	wxMenuItem* uppMenuAbout;
	uppMenuAbout = new wxMenuItem( uppMenuHelp, wxID_ABOUT, wxString( _("About...") ) , wxEmptyString, wxITEM_NORMAL );
	uppMenuHelp->Append( uppMenuAbout );
	
	m_pMenuBar->Append( uppMenuHelp, _("Help") );
	
	this->SetMenuBar( m_pMenuBar );
	
	m_pStatusBar = this->CreateStatusBar( 2, wxST_SIZEGRIP, wxID_ANY );
	
	// Connect Events
	this->Connect( uppMenuNew->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_new ) );
	this->Connect( uppMenuOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_open ) );
	this->Connect( uppMenuRefresh->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_refresh ) );
	this->Connect( uppMenuSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_save ) );
	this->Connect( uppMenuSaveAs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_save_as ) );
	this->Connect( uppMenuExit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_exit ) );
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
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_help ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_about ) );
}
