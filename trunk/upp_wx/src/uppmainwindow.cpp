///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "uppmainwindow.h"
#include "include_icons.h"

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
	m_toolBar1->SetToolSeparation( 1 );
	newButton = new wxBitmapButton( m_toolBar1, wxID_ANY, wxICON( new ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_toolBar1->AddControl( newButton );
	openButton = new wxBitmapButton( m_toolBar1, wxID_ANY, wxICON( open ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_toolBar1->AddControl( openButton );
	refreshButton = new wxBitmapButton( m_toolBar1, wxID_ANY, wxICON( refresh ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_toolBar1->AddControl( refreshButton );
	newButton3 = new wxBitmapButton( m_toolBar1, wxID_NEW, wxICON( new ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_toolBar1->AddControl( newButton3 );
	newButton4 = new wxBitmapButton( m_toolBar1, wxID_NEW, wxICON( new ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_toolBar1->AddControl( newButton4 );
	newButton5 = new wxBitmapButton( m_toolBar1, wxID_NEW, wxICON( new ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_toolBar1->AddControl( newButton5 );
	newButton6 = new wxBitmapButton( m_toolBar1, wxID_NEW, wxICON( new ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_toolBar1->AddControl( newButton6 );
	newButton7 = new wxBitmapButton( m_toolBar1, wxID_NEW, wxICON( new ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_toolBar1->AddControl( newButton7 );
	newButton8 = new wxBitmapButton( m_toolBar1, wxID_NEW, wxICON( new ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_toolBar1->AddControl( newButton8 );
	newButton9 = new wxBitmapButton( m_toolBar1, wxID_NEW, wxICON( new ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_toolBar1->AddControl( newButton9 );
	m_comboBox1 = new wxComboBox( m_toolBar1, wxID_ANY, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_toolBar1->AddControl( m_comboBox1 );
	m_toolBar1->Realize();
	
}

UppMainWindow::~UppMainWindow()
{
}
