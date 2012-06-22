///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
// 
// NOTE: despite the above warning this file was edited by hand to fix 
//       layouting problems otherwise occurring because of the GUI
//       additions done later in UppMainWindow::CompleteGUICreation().
//       The hand-edited places are marked with the string "EDITED-BY-HAND".
//       To avoid forgetting about the modifications when this file is regenerated
//       we define a symbol which is then checked by later code:
int g_handModificationsTrickSymbol;
// 
///////////////////////////////////////////////////////////////////////////

#include "uppmainwindow_base.h"

///////////////////////////////////////////////////////////////////////////

UppMainWindowBase::UppMainWindowBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{

// EDITED-BY-HAND:
//  this->SetSizeHints( wxDefaultSize, wxDefaultSize );
// not necessary!
	
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
	m_pNotebook->AddPage( m_pDataPanel, _("Data"), false );
	m_pConfigPanel = new wxPanel( m_pNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( m_pConfigPanel, wxID_ANY, _("Configuration flags for this PIC:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer3->Add( m_staticText2, 0, wxALL, 5 );
	
	m_pConfigListbook = new UppConfigViewBook(m_pConfigPanel, wxID_ANY);
	bSizer3->Add( m_pConfigListbook, 1, wxALL|wxEXPAND, 5 );
	
	m_pConfigPanel->SetSizer( bSizer3 );
	m_pConfigPanel->Layout();
	bSizer3->Fit( m_pConfigPanel );
	m_pNotebook->AddPage( m_pConfigPanel, _("Configuration flags"), false );
	m_pInfoPanel = new wxPanel( m_pNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_pInfoPanel, wxID_ANY, _("Miscellaneous") ), wxVERTICAL );
	
	m_pDatasheetLink = new wxHyperlinkCtrl( m_pInfoPanel, wxID_ANY, _("Datasheet"), wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	sbSizer1->Add( m_pDatasheetLink, 0, wxALL|wxEXPAND, 5 );
	
	m_pVPPText = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Programming voltage (Vpp):\n    MIN\n    NOM\n    MAX\n_dontTranslateThis_"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pVPPText->Wrap( -1 );
	sbSizer1->Add( m_pVPPText, 0, wxALL, 5 );
	
	m_pVDDText = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Working voltages (Vdd):\n    MIN\n    NOM\n    MAX\n_dontTranslateThis_"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pVDDText->Wrap( -1 );
	sbSizer1->Add( m_pVDDText, 0, wxALL, 5 );
	
	m_pFrequencyText = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Frequency range:\n   MIN\n   MAX\n_dontTranslateThis_"), wxDefaultPosition, wxDefaultSize, 0 );
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
	
	m_pTypeText = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTypeText->Wrap( -1 );
	sbSizer1->Add( m_pTypeText, 0, wxALL, 5 );
	
	bSizer5->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* pPackageSizer;
	pPackageSizer = new wxStaticBoxSizer( new wxStaticBox( m_pInfoPanel, wxID_ANY, _("Package") ), wxVERTICAL );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText7 = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Variants:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	bSizer7->Add( m_staticText7, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxArrayString m_pPackageVariantsChoices;
	m_pPackageVariants = new wxChoice( m_pInfoPanel, wxID_PACKAGE_VARIANT, wxDefaultPosition, wxDefaultSize, m_pPackageVariantsChoices, 0 );
	m_pPackageVariants->SetSelection( 0 );
	bSizer7->Add( m_pPackageVariants, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	bSizer7->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_pZoomInButton = new wxBitmapButton( m_pInfoPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_pZoomInButton->SetToolTip( _("Zoom in") );
	
	m_pZoomInButton->SetToolTip( _("Zoom in") );
	
	bSizer7->Add( m_pZoomInButton, 0, wxALL, 5 );
	
	m_pZoomOutButton = new wxBitmapButton( m_pInfoPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_pZoomOutButton->SetToolTip( _("Zoom out") );
	
	m_pZoomOutButton->SetToolTip( _("Zoom out") );
	
	bSizer7->Add( m_pZoomOutButton, 0, wxALL, 5 );
	
	m_pZoomFitButton = new wxBitmapButton( m_pInfoPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_pZoomFitButton->SetToolTip( _("Fit package drawing") );
	
	m_pZoomFitButton->SetToolTip( _("Fit package drawing") );
	
	bSizer7->Add( m_pZoomFitButton, 0, wxALL, 5 );
	
	pPackageSizer->Add( bSizer7, 0, wxEXPAND, 5 );
	
	m_pPackageWin = new UppPackageViewWindow(m_pInfoPanel, wxID_ANY);
	pPackageSizer->Add( m_pPackageWin, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText11 = new wxStaticText( m_pInfoPanel, wxID_ANY, _("Note: red pins are (some of those) used in ICSP."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	pPackageSizer->Add( m_staticText11, 0, wxALL, 5 );
	
	bSizer5->Add( pPackageSizer, 1, wxALL|wxEXPAND, 5 );
	
	m_pInfoPanel->SetSizer( bSizer5 );
	m_pInfoPanel->Layout();
	bSizer5->Fit( m_pInfoPanel );
	m_pNotebook->AddPage( m_pInfoPanel, _("PIC info"), true );
	
	m_pSizer->Add( m_pNotebook, 1, wxEXPAND | wxALL, 5 );
	
// EDITED-BY-HAND:
//	this->SetSizerAndFit( m_pSizer );
//	this->Layout();
// done later in UppMainWindow::CompleteGUICreation()
	m_pMenuBar = new wxMenuBar( 0 );
	m_pMenuFile = new wxMenu();
	wxMenuItem* uppMenuNew;
	uppMenuNew = new wxMenuItem( m_pMenuFile, wxID_NEW, wxString( _("&New") ) , _("Resets the code, configuration and data areas of the currently open HEX file"), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuNew );
	
	wxMenuItem* uppMenuOpen;
	uppMenuOpen = new wxMenuItem( m_pMenuFile, wxID_OPEN, wxString( _("&Open") ) , _("Loads an HEX file for editing or programming..."), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuOpen );
	
	wxMenuItem* uppMenuExamples;
	uppMenuExamples = new wxMenuItem( m_pMenuFile, wxID_EXAMPLES, wxString( _("Examples") ) , _("Open a .hex file from the examples folder"), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuExamples );
	
	wxMenuItem* uppMenuRefresh;
	uppMenuRefresh = new wxMenuItem( m_pMenuFile, wxID_REFRESH, wxString( _("&Reload") ) + wxT('\t') + wxT("CTRL+R"), _("Reloads the current HEX file from disk"), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuRefresh );
	
	wxMenuItem* uppMenuSave;
	uppMenuSave = new wxMenuItem( m_pMenuFile, wxID_SAVE, wxString( _("&Save") ) , _("Saves the HEX file with the modified data"), wxITEM_NORMAL );
	m_pMenuFile->Append( uppMenuSave );
	
	wxMenuItem* uppMenuSaveAs;
	uppMenuSaveAs = new wxMenuItem( m_pMenuFile, wxID_SAVEAS, wxString( _("Save &As") ) + wxT('\t') + wxT("CTRL+SHIFT+S"), _("Saves current code,config,data as a HEX file"), wxITEM_NORMAL );
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
	uppMenuSelectAll = new wxMenuItem( m_pMenuEdit, wxID_SELECTALL, wxString( _("Select &All") ) + wxT('\t') + wxT("CTRL+A"), _("Selects the entire grid currently shown"), wxITEM_NORMAL );
	m_pMenuEdit->Append( uppMenuSelectAll );
	
	m_pMenuBar->Append( m_pMenuEdit, _("&Edit") );
	
	m_pMenuOptions = new wxMenu();
	wxMenuItem* uppMenuOptions1;
	uppMenuOptions1 = new wxMenuItem( m_pMenuOptions, wxID_PREFERENCES, wxString( _("&Preferences") ) , _("Shows the options for this program"), wxITEM_NORMAL );
	m_pMenuOptions->Append( uppMenuOptions1 );
	
	wxMenuItem* uppMenuOptionsIoTest;
	uppMenuOptionsIoTest = new wxMenuItem( m_pMenuOptions, wxID_IO_TEST, wxString( _("&IO Test") ) , _("Shows the options for this program"), wxITEM_NORMAL );
	m_pMenuOptions->Append( uppMenuOptionsIoTest );
	
	m_pMenuBar->Append( m_pMenuOptions, _("&Options") );
	
	m_pMenuHelp = new wxMenu();
	wxMenuItem* uppMenuItemHelp;
	uppMenuItemHelp = new wxMenuItem( m_pMenuHelp, wxID_HELP, wxString( _("&Help") ) , _("Opens the Usbpicprog's website..."), wxITEM_NORMAL );
	m_pMenuHelp->Append( uppMenuItemHelp );
	
	wxMenuItem* m_separator1;
	m_separator1 = m_pMenuHelp->AppendSeparator();
	
	wxMenuItem* uppMenuAbout;
	uppMenuAbout = new wxMenuItem( m_pMenuHelp, wxID_ABOUT, wxString( _("&About") ) , _("Shows informations about this program"), wxITEM_NORMAL );
	m_pMenuHelp->Append( uppMenuAbout );
	
	m_pMenuBar->Append( m_pMenuHelp, _("&Help") );
	
	this->SetMenuBar( m_pMenuBar );
	
// EDITED-BY-HAND: use wxSTB_DEFAULT_STYLE=wxSTB_SIZEGRIP|wxSTB_ELLIPSIZE_END|wxSTB_SHOW_TIPS
//                 instead of wxST_SIZE_GRIP if it's available (i.e. if wx >= 2.9.1)
#ifdef wxSTB_DEFAULT_STYLE
    m_pStatusBar = this->CreateStatusBar( 2, wxSTB_DEFAULT_STYLE, wxID_ANY );
#else
    m_pStatusBar = this->CreateStatusBar( 2, wxST_SIZEGRIP, wxID_ANY );
#endif
	
	// Connect Events
	this->Connect( uppMenuNew->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_new ) );
	this->Connect( uppMenuOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_open ) );
	this->Connect( uppMenuExamples->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_examples ) );
	this->Connect( uppMenuRefresh->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_refresh ) );
	this->Connect( uppMenuSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_save ) );
	this->Connect( uppMenuSaveAs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_save_as ) );
	this->Connect( uppMenuExit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_exit ) );
	this->Connect( uppMenuCopy->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_copy ) );
	this->Connect( uppMenuSelectAll->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_selectall ) );
	this->Connect( uppMenuOptions1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_preferences ) );
	this->Connect( uppMenuOptionsIoTest->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_io_test ) );
	this->Connect( uppMenuItemHelp->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_help ) );
	this->Connect( uppMenuAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_about ) );
}

UppMainWindowBase::~UppMainWindowBase()
{
	// Disconnect Events
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_new ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_open ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_examples ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_refresh ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_save ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_save_as ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_exit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_copy ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_selectall ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_preferences ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_io_test ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_help ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindowBase::on_about ) );
}
