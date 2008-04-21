///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "uppmainwindow.h"

///////////////////////////////////////////////////////////////////////////

UppMainWindow::UppMainWindow( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer;
	bSizer = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl3 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP|wxTE_MULTILINE );
	bSizer->Add( m_textCtrl3, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	this->SetSizer( bSizer );
	this->Layout();
	m_menubar1 = new wxMenuBar( 0 );
	uppMenuFile = new wxMenu();
	m_menubar1->Append( uppMenuFile, wxT("File") );
	
	uppMenuActions = new wxMenu();
	m_menubar1->Append( uppMenuActions, wxT("Actions") );
	
	this->SetMenuBar( m_menubar1 );
	
	m_statusBar1 = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	m_toolBar1 = this->CreateToolBar( wxTB_DOCKABLE|wxTB_HORIZONTAL, wxID_ANY ); 
	uppButtonNew = new wxButton( m_toolBar1, wxID_NEW, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_toolBar1->AddControl( uppButtonNew );
	m_comboBox1 = new wxComboBox( m_toolBar1, wxID_ANY, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_toolBar1->AddControl( m_comboBox1 );
	m_toolBar1->Realize();
	
	
	// Connect Events
	uppButtonNew->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UppMainWindow::on_tool1_clicked ), NULL, this );
}

UppMainWindow::~UppMainWindow()
{
	// Disconnect Events
	uppButtonNew->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( UppMainWindow::on_tool1_clicked ), NULL, this );
}
