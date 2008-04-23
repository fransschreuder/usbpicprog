///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "uppmainwindow.h"
#include "include_icons.h"

#define wxID_PROGRAM 1000001
#define wxID_READ 1000002
#define wxID_VERIFY 1000003
#define wxID_ERASE 1000004
#define wxID_BLANKCHECK 1000005
#define wxID_AUTODETECT 1000006

///////////////////////////////////////////////////////////////////////////

UppMainWindow::UppMainWindow( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer;
	bSizer = new wxBoxSizer( wxVERTICAL );
	
	uppHexEdit = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP|wxTE_MULTILINE );
	bSizer->Add( uppHexEdit, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	this->SetSizer( bSizer );
	this->Layout();
	uppMenuBar = new wxMenuBar( 0 );
	uppMenuFile = new wxMenu();
	wxMenuItem* uppMenuNew;
	uppMenuNew = new wxMenuItem( uppMenuFile, wxID_NEW, wxString( wxT("New") ) + wxT('\t') + wxT("CTRL+N"), wxEmptyString, wxITEM_NORMAL );
	uppMenuFile->Append( uppMenuNew );
	
	wxMenuItem* uppMenuOpen;
	uppMenuOpen = new wxMenuItem( uppMenuFile, wxID_OPEN, wxString( wxT("Open") ) + wxT('\t') + wxT("CTRL+O"), wxEmptyString, wxITEM_NORMAL );
	uppMenuFile->Append( uppMenuOpen );
	
	wxMenuItem* uppMenuRefresh;
	uppMenuRefresh = new wxMenuItem( uppMenuFile, wxID_REFRESH, wxString( wxT("Refresh") ) + wxT('\t') + wxT("CTRL+R"), wxEmptyString, wxITEM_NORMAL );
	uppMenuFile->Append( uppMenuRefresh );
	
	wxMenuItem* uppMenuSave;
	uppMenuSave = new wxMenuItem( uppMenuFile, wxID_SAVE, wxString( wxT("Save") ) + wxT('\t') + wxT("CTRL+S"), wxEmptyString, wxITEM_NORMAL );
	uppMenuFile->Append( uppMenuSave );
	
	wxMenuItem* uppMenuSaveAs;
	uppMenuSaveAs = new wxMenuItem( uppMenuFile, wxID_SAVE_AS, wxString( wxT("Save As") ) , wxEmptyString, wxITEM_NORMAL );
	uppMenuFile->Append( uppMenuSaveAs );
	
	uppMenuFile->AppendSeparator();
	
	wxMenuItem* uppMenuExit;
	uppMenuExit = new wxMenuItem( uppMenuFile, wxID_EXIT, wxString( wxT("Exit") ) + wxT('\t') + wxT("ALT+F4"), wxEmptyString, wxITEM_NORMAL );
	uppMenuFile->Append( uppMenuExit );
	
	uppMenuBar->Append( uppMenuFile, wxT("File") );
	
	uppMenuActions = new wxMenu();
	wxMenuItem* uppMenuProgram;
	uppMenuProgram = new wxMenuItem( uppMenuActions, wxID_PROGRAM, wxString( wxT("Program") ) + wxT('\t') + wxT("F7"), wxEmptyString, wxITEM_NORMAL );
	uppMenuActions->Append( uppMenuProgram );
	
	wxMenuItem* uppMenuRead;
	uppMenuRead = new wxMenuItem( uppMenuActions, wxID_READ, wxString( wxT("Read") ) + wxT('\t') + wxT("F8"), wxEmptyString, wxITEM_NORMAL );
	uppMenuActions->Append( uppMenuRead );
	
	wxMenuItem* uppMenuVerify;
	uppMenuVerify = new wxMenuItem( uppMenuActions, wxID_VERIFY, wxString( wxT("Verify") ) , wxEmptyString, wxITEM_NORMAL );
	uppMenuActions->Append( uppMenuVerify );
	
	wxMenuItem* uppMenuErase;
	uppMenuErase = new wxMenuItem( uppMenuActions, wxID_ERASE, wxString( wxT("Erase") ) , wxEmptyString, wxITEM_NORMAL );
	uppMenuActions->Append( uppMenuErase );
	
	wxMenuItem* uppMenuBlankCheck;
	uppMenuBlankCheck = new wxMenuItem( uppMenuActions, wxID_BLANKCHECK, wxString( wxT("Blankcheck") ) , wxEmptyString, wxITEM_NORMAL );
	uppMenuActions->Append( uppMenuBlankCheck );
	
	wxMenuItem* uppMenuAutoDetect;
	uppMenuAutoDetect = new wxMenuItem( uppMenuActions, wxID_AUTODETECT, wxString( wxT("Autodetect") ) , wxEmptyString, wxITEM_NORMAL );
	uppMenuActions->Append( uppMenuAutoDetect );
	
	uppMenuBar->Append( uppMenuActions, wxT("Actions") );
	
	uppMenuHelp = new wxMenu();
	wxMenuItem* uppMenuItemHelp;
	uppMenuItemHelp = new wxMenuItem( uppMenuHelp, wxID_HELP, wxString( wxT("Help") ) + wxT('\t') + wxT("F1"), wxEmptyString, wxITEM_NORMAL );
	uppMenuHelp->Append( uppMenuItemHelp );
	
	wxMenuItem* uppMenuAbout;
	uppMenuAbout = new wxMenuItem( uppMenuHelp, wxID_ABOUT, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
	uppMenuHelp->Append( uppMenuAbout );
	
	uppMenuBar->Append( uppMenuHelp, wxT("Help") );
	
	this->SetMenuBar( uppMenuBar );

	m_statusBar1 = this->CreateStatusBar( 2, wxST_SIZEGRIP, wxID_ANY );
	m_toolBar1 = this->CreateToolBar( wxTB_DOCKABLE|wxTB_HORIZONTAL, wxID_ANY ); 
	m_toolBar1->AddTool( wxID_NEW, wxT("new"), wxArtProvider::GetBitmap(wxART_NEW,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, wxT("new"), wxEmptyString );
	m_toolBar1->AddTool( wxID_OPEN, wxT("open"), wxArtProvider::GetBitmap(wxART_FILE_OPEN,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, wxT("open"), wxEmptyString );
	m_toolBar1->AddTool( wxID_REFRESH, wxT("refresh"), wxICON(refresh), wxNullBitmap, wxITEM_NORMAL, wxT("refresh"), wxEmptyString );
	m_toolBar1->AddTool( wxID_SAVE, wxT("save"), wxArtProvider::GetBitmap(wxART_FILE_SAVE,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, wxT("save"), wxEmptyString );
	m_toolBar1->AddTool( wxID_SAVE_AS, wxT("save as"), wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, wxT("save as"), wxEmptyString );
	m_toolBar1->AddSeparator();
	m_toolBar1->AddTool( wxID_PROGRAM, wxT("program"), wxICON( program ), wxNullBitmap, wxITEM_NORMAL, wxT("program"), wxEmptyString );
	m_toolBar1->AddTool( wxID_READ, wxT("read"), wxICON( read ), wxNullBitmap, wxITEM_NORMAL, wxT("read"), wxEmptyString );
	m_toolBar1->AddTool( wxID_VERIFY, wxT("verify"), wxICON( verify ), wxNullBitmap, wxITEM_NORMAL, wxT("verify"), wxEmptyString );
	m_toolBar1->AddTool( wxID_ERASE, wxT("erase"), wxICON( erase ), wxNullBitmap, wxITEM_NORMAL, wxT("erase"), wxEmptyString );
	m_toolBar1->AddTool( wxID_BLANKCHECK, wxT("blankcheck"), wxICON( blankcheck ), wxNullBitmap, wxITEM_NORMAL, wxT("blankcheck"), wxEmptyString );
	m_toolBar1->AddSeparator();
	m_comboBox1 = new wxComboBox( m_toolBar1, wxID_ANY, wxT("P18F2550"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_toolBar1->AddControl( m_comboBox1 );
	m_toolBar1->Realize();
	
	
	// Connect Events
	this->Connect( uppMenuNew->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_new ) );
	this->Connect( uppMenuOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_open ) );
	this->Connect( uppMenuRefresh->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_refresh ) );
	this->Connect( uppMenuSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_save ) );
	this->Connect( uppMenuSaveAs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_save_as ) );
	this->Connect( uppMenuExit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_exit ) );
	this->Connect( uppMenuProgram->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_program ) );
	this->Connect( uppMenuRead->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_read ) );
	this->Connect( uppMenuVerify->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_verify ) );
	this->Connect( uppMenuErase->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_erase ) );
	this->Connect( uppMenuBlankCheck->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_blankcheck ) );
	this->Connect( uppMenuAutoDetect->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_autodetect ) );
	this->Connect( uppMenuItemHelp->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_help ) );
	this->Connect( uppMenuAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_about ) );
	//this->Connect( wxID_NEW, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_new ) );
	//this->Connect( wxID_OPEN, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_open ) );
	//this->Connect( wxID_REFRESH, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_refresh ) );
	//this->Connect( wxID_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_save ) );
	//this->Connect( wxID_SAVE_AS, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_save_as ) );
	//this->Connect( wxID_PROGRAM, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_program ) );
	//this->Connect( wxID_READ, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_read ) );
	//this->Connect( wxID_VERIFY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_verify ) );
	//this->Connect( wxID_ERASE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_erase ) );
	//this->Connect( wxID_BLANKCHECK, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_blankcheck ) );
	m_comboBox1->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( UppMainWindow::on_combo_changed ), NULL, this );
}

UppMainWindow::~UppMainWindow()
{
	// Disconnect Events
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_new ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_open ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_refresh ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_save ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_save_as ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_exit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_program ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_read ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_verify ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_erase ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_blankcheck ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_autodetect ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_help ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_about ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_new ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_save ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_refresh ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_save ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_save_as ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_program ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_read ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_verify ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_erase ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( UppMainWindow::on_blankcheck ) );
	m_comboBox1->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( UppMainWindow::on_combo_changed ), NULL, this );
}
