///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE! ---> Haha, I edited it anyway :P
///////////////////////////////////////////////////////////////////////////

#include "uppmainwindow.h"
#include "include_icons.h"
 #include "wx/app.h"

enum{
wxID_PROGRAM=wxID_HIGHEST+1,
wxID_READ,
wxID_VERIFY,
wxID_ERASE,
wxID_BLANKCHECK,
wxID_AUTODETECT,
wxID_CONNECT ,
wxID_CONNECT_BOOT,
wxID_DISCONNECT 
};


///////////////////////////////////////////////////////////////////////////

UppMainWindow::UppMainWindow( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	SetIcon(wxIcon( usbpicprog_xpm ));
	
	
	bSizer = new wxBoxSizer( wxVERTICAL );
	#ifdef USE_UPPHEXVIEW
	uppHexEdit = new UppHexview( this, wxID_ANY, wxT(""),  wxDefaultPosition, wxDefaultSize);
	#else
	uppHexEdit = new wxTextCtrl( this, wxID_ANY, wxT(""),  wxDefaultPosition, wxDefaultSize, wxTE_READONLY| wxTE_MULTILINE | wxNO_BORDER );
	#endif	
	
	uppHexEdit->SetFont(wxFont(12, wxMODERN, wxNORMAL,wxNORMAL));
	
	bSizer->Add( uppHexEdit, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND );
	
	
	this->SetSizer( bSizer );
	this->Layout();
	uppMenuBar = new wxMenuBar( 0 );
	uppMenuFile = new wxMenu();
	wxMenuItem* uppMenuNew;
	uppMenuNew = new wxMenuItem( uppMenuFile, wxID_NEW, wxString( _("&New") ) + wxT('\t') + wxT("CTRL+N"), _("Clear open hex file"), wxITEM_NORMAL );
	uppMenuFile->Append( uppMenuNew );
	
	wxMenuItem* uppMenuOpen;
	uppMenuOpen = new wxMenuItem( uppMenuFile, wxID_OPEN, wxString( _("&Open") ) + wxT('\t') + wxT("CTRL+O"), _("Open a hex file"), wxITEM_NORMAL );
	uppMenuFile->Append( uppMenuOpen );
	
	wxMenuItem* uppMenuRefresh;
	uppMenuRefresh = new wxMenuItem( uppMenuFile, wxID_REFRESH, wxString( _("&Reload") ) + wxT('\t') + wxT("CTRL+R"), _("Reload current hex file"), wxITEM_NORMAL );
	uppMenuFile->Append( uppMenuRefresh );
	
	wxMenuItem* uppMenuSave;
	uppMenuSave = new wxMenuItem( uppMenuFile, wxID_SAVE, wxString( _("&Save") ) + wxT('\t') + wxT("CTRL+S"), _("Save the hex file"), wxITEM_NORMAL );
	uppMenuFile->Append( uppMenuSave );
	
	wxMenuItem* uppMenuSaveAs;
	uppMenuSaveAs = new wxMenuItem( uppMenuFile, wxID_SAVE_AS, wxString( _("Save &As") ) , _("Save the hex file as"), wxITEM_NORMAL );
	uppMenuFile->Append( uppMenuSaveAs );
	
	uppMenuFile->AppendSeparator();
	
	wxMenuItem* uppMenuExit;
	uppMenuExit = new wxMenuItem( uppMenuFile, wxID_EXIT, wxString( _("&Exit") ) + wxT('\t') + wxT("ALT+F4"), _("Exit Usbpicprog"), wxITEM_NORMAL );
	uppMenuFile->Append( uppMenuExit );
	
	uppMenuBar->Append( uppMenuFile, _("&File") );
	
	uppMenuEdit = new wxMenu();
	wxMenuItem* uppMenuCopy;
	uppMenuCopy = new wxMenuItem( uppMenuEdit, wxID_COPY, wxString( _("&Copy") ) + wxT('\t') + wxT("CTRL+C"), _("Copy selection"), wxITEM_NORMAL );
	uppMenuEdit->Append( uppMenuCopy );

	wxMenuItem* uppMenuSelectall;
	uppMenuSelectall = new wxMenuItem( uppMenuEdit, wxID_SELECTALL, wxString( _("Select &All") ) + wxT('\t') + wxT("CTRL+A"), _("Select all"), wxITEM_NORMAL );
	uppMenuEdit->Append( uppMenuSelectall );

    uppMenuBar->Append( uppMenuEdit, _("&Edit") );
	
	uppMenuActions = new wxMenu();
	wxMenuItem* uppMenuProgram;
	#ifdef __WXMAC__
	uppMenuProgram = new wxMenuItem( uppMenuActions, wxID_PROGRAM, wxString( _("&Program") ) + wxT('\t') + wxT("CTRL+7"), _("Program the device"), wxITEM_NORMAL );
	#else	
	uppMenuProgram = new wxMenuItem( uppMenuActions, wxID_PROGRAM, wxString( _("&Program") ) + wxT('\t') + wxT("F7"), _("Program the device"), wxITEM_NORMAL );
	#endif
	uppMenuActions->Append( uppMenuProgram );
	
	wxMenuItem* uppMenuRead;
	#ifdef __WXMAC__
	uppMenuRead = new wxMenuItem( uppMenuActions, wxID_READ, wxString( _("&Read") ) + wxT('\t') + wxT("CTRL+8"), _("Read the device"), wxITEM_NORMAL );
	#else	
	uppMenuRead = new wxMenuItem( uppMenuActions, wxID_READ, wxString( _("&Read") ) + wxT('\t') + wxT("F8"), _("Read the device"), wxITEM_NORMAL );
	#endif
	uppMenuActions->Append( uppMenuRead );
	
	wxMenuItem* uppMenuVerify;
	uppMenuVerify = new wxMenuItem( uppMenuActions, wxID_VERIFY, wxString( _("&Verify") ) , _("Verify the device"), wxITEM_NORMAL );
	uppMenuActions->Append( uppMenuVerify );
	
	wxMenuItem* uppMenuErase;
	uppMenuErase = new wxMenuItem( uppMenuActions, wxID_ERASE, wxString( _("&Erase") ) , _("Erase the device"), wxITEM_NORMAL );
	uppMenuActions->Append( uppMenuErase );
	
	wxMenuItem* uppMenuBlankCheck;
	uppMenuBlankCheck = new wxMenuItem( uppMenuActions, wxID_BLANKCHECK, wxString( _("&Blankcheck") ) , _("Check if the device is blank"), wxITEM_NORMAL );
	uppMenuActions->Append( uppMenuBlankCheck );
	
	wxMenuItem* uppMenuAutoDetect;
	uppMenuAutoDetect = new wxMenuItem( uppMenuActions, wxID_AUTODETECT, wxString( _("&Autodetect") ) , _("Detect the device"), wxITEM_NORMAL );
	uppMenuActions->Append( uppMenuAutoDetect );
	
	wxMenuItem* uppMenuConnect;
	uppMenuConnect = new wxMenuItem( uppMenuActions, wxID_CONNECT, wxString( _("Connect &Usbpicprog") ) + wxT('\t') + wxT("CTRL+U"), _("Connect Usbpicprog"), wxITEM_NORMAL );
	uppMenuActions->Append( uppMenuConnect );
	
	wxMenuItem* uppMenuConnectBoot;
	uppMenuConnectBoot = new wxMenuItem( uppMenuActions, wxID_CONNECT_BOOT, wxString( _("Connect &Bootloader") ) + wxT('\t') + wxT("CTRL+B") , _("Connect Bootloader"), wxITEM_NORMAL );
	uppMenuActions->Append( uppMenuConnectBoot );
	
	wxMenuItem* uppMenuDisConnect;
	uppMenuDisConnect = new wxMenuItem( uppMenuActions, wxID_DISCONNECT, wxString( _("&Disconnect") ) , _("Disconnect Usbpicprog"), wxITEM_NORMAL );
	uppMenuActions->Append( uppMenuDisConnect );
	
	
	uppMenuBar->Append( uppMenuActions, _("&Actions") );
	
	
	uppMenuOptions = new wxMenu();
	wxMenuItem* uppMenuPreferences;
	uppMenuPreferences = new wxMenuItem( uppMenuOptions, wxID_PREFERENCES, wxString( _("&Preferences") ), _("Preferences"), wxITEM_NORMAL );
	uppMenuOptions->Append( uppMenuPreferences );


    uppMenuBar->Append( uppMenuOptions, _("&Options") );
	
	
	uppMenuHelp = new wxMenu();
	wxMenuItem* uppMenuItemHelp;
	uppMenuItemHelp = new wxMenuItem( uppMenuHelp, wxID_HELP, wxString( _("&Help") ) + wxT('\t') + wxT("F1"), _("Open http://usbpicprog.org"), wxITEM_NORMAL );
	uppMenuHelp->Append( uppMenuItemHelp );
	
	wxMenuItem* uppMenuAbout;
	uppMenuAbout = new wxMenuItem( uppMenuHelp, wxID_ABOUT, wxString( _("&About usbpicprog") ) , _("About Usbpicprog"), wxITEM_NORMAL );
	uppMenuHelp->Append( uppMenuAbout );
	
	uppMenuBar->Append( uppMenuHelp, _("&Help") );
	
	this->SetMenuBar( uppMenuBar );

	m_statusBar1 = this->CreateStatusBar( 4, 1, wxID_ANY );

	int statuswidths[10];
	statuswidths[STATUS_FIELD_HARDWARE] = -2;// auto resize left side of the status bar
	statuswidths[STATUS_FIELD_OTHER] = -2; // auto resize message box reserved for other things
	statuswidths[STATUS_FIELD_PROGRESS] = -1; // resize the progressbar also less
	statuswidths[STATUS_FIELD_SIDE] = STATUS_FIELD_SIDE_WIDTH;  // fix for mac os x where in the right bottom corner there is a handle to resize the window
	m_statusBar1->SetStatusWidths(4, statuswidths); 

	wxRect rect;
	m_statusBar1->GetFieldRect(STATUS_FIELD_PROGRESS, rect);

	uppProgressBar = new wxGauge(m_statusBar1, wxID_ANY, 100, rect.GetPosition(), rect.GetSize(), wxGA_HORIZONTAL, wxDefaultValidator, _("Progress"));

	m_toolBar1 = this->CreateToolBar( wxTB_DOCKABLE|wxTB_HORIZONTAL, wxID_ANY ); 
	m_toolBar1->AddTool( wxID_NEW, _("new"), wxArtProvider::GetBitmap(wxART_NEW,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("new"), _("Clear open hex file") );
	m_toolBar1->AddTool( wxID_OPEN, _("open"), wxArtProvider::GetBitmap(wxART_FILE_OPEN,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("open"),  _("Open a hex file") );
	m_toolBar1->AddTool( wxID_REFRESH, _("reload"), wxIcon(refresh_xpm), wxNullBitmap, wxITEM_NORMAL, _("reload"),  _("Reload the hex file") );
	m_toolBar1->AddTool( wxID_SAVE, _("save"), wxArtProvider::GetBitmap(wxART_FILE_SAVE,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("save"), _("Save the hex file") );
	m_toolBar1->AddTool( wxID_SAVE_AS, _("save as"), wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS,wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("save as"), _("Save the hex file as") );
	m_toolBar1->AddSeparator();
	m_toolBar1->AddTool( wxID_PROGRAM, _("program"), wxIcon( program_xpm ), wxNullBitmap, wxITEM_NORMAL, _("program"), _("Program the device") );
	m_toolBar1->AddTool( wxID_READ, _("read"), wxIcon( read_xpm ), wxNullBitmap, wxITEM_NORMAL, _("read"), _("Read the device") );
	m_toolBar1->AddTool( wxID_VERIFY, _("verify"), wxIcon( verify_xpm ), wxNullBitmap, wxITEM_NORMAL, _("verify"), _("Verify the device") );
	m_toolBar1->AddTool( wxID_ERASE, _("erase"), wxIcon( erase_xpm ), wxNullBitmap, wxITEM_NORMAL, _("erase"), _("Erase the device") );
	m_toolBar1->AddTool( wxID_BLANKCHECK, _("blankcheck"), wxIcon( blankcheck_xpm ), wxNullBitmap, wxITEM_NORMAL, _("blankcheck"), _("Blankcheck the device") );
	
	m_toolBar1->AddSeparator();
	m_comboBox1 = new wxComboBox( m_toolBar1, wxID_ANY, wxT("P18F2550"), wxDefaultPosition, wxSize(150,-1), 0, NULL, 0 ); 
	m_toolBar1->AddControl( m_comboBox1 );
	
	m_toolBar1->AddSeparator();
	m_radioButton_upp = new wxRadioButton( m_toolBar1, wxID_ANY, _("Usbpicprog"), wxDefaultPosition, wxDefaultSize, 0);
	m_toolBar1->AddControl(m_radioButton_upp);
	m_radioButton_boot = new wxRadioButton( m_toolBar1, wxID_ANY, _("Bootloader"), wxDefaultPosition, wxDefaultSize, 0);
	m_toolBar1->AddControl(m_radioButton_boot);
	
	m_toolBar1->Realize();

	#ifdef __WXMAC__
	// not compatible with wxWidgets 2.9
	wxApp::s_macAboutMenuItemId = wxID_ABOUT;
	#endif

	
	
	// Connect Events
	this->Connect( uppMenuNew->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_new ) );
	this->Connect( uppMenuOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_open ) );
	this->Connect( uppMenuRefresh->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_refresh ) );
	this->Connect( uppMenuSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_save ) );
	this->Connect( uppMenuSaveAs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_save_as ) );
	this->Connect( uppMenuExit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_exit ) );
	this->Connect( uppMenuCopy->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_copy ) );
	this->Connect( uppMenuSelectall->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_selectall ) );
	this->Connect( uppMenuProgram->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_program ) );
	this->Connect( uppMenuRead->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_read ) );
	this->Connect( uppMenuVerify->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_verify ) );
	this->Connect( uppMenuErase->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_erase ) );
	this->Connect( uppMenuBlankCheck->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_blankcheck ) );
	this->Connect( uppMenuAutoDetect->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_autodetect ) );
	this->Connect( uppMenuConnect->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_connect ) );
	this->Connect( uppMenuConnectBoot->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_connect_boot ) );
	this->Connect( uppMenuDisConnect->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_disconnect ) );
	this->Connect( uppMenuPreferences->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_preferences ) );
	this->Connect( uppMenuItemHelp->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_help ) );
	this->Connect( uppMenuAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_about ) );
	
	m_comboBox1->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( UppMainWindow::on_combo_changed ), NULL, this );
	m_radioButton_upp->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( UppMainWindow::on_connect ), NULL, this );
	m_radioButton_boot->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( UppMainWindow::on_connect_boot ), NULL, this );
	
	this->SetSize(850,600);
}

BEGIN_EVENT_TABLE(UppMainWindow, wxFrame)
	EVT_SIZE(UppMainWindow::OnSize)
END_EVENT_TABLE()

UppMainWindow::~UppMainWindow()
{
	// Disconnect Events
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_new ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_open ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_refresh ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_save ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_save_as ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_exit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_copy ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_selectall ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_program ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_read ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_verify ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_erase ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_blankcheck ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_autodetect ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_connect ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_connect_boot ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_disconnect ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_preferences ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_help ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppMainWindow::on_about ) );
	m_comboBox1->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( UppMainWindow::on_combo_changed ), NULL, this );
	m_radioButton_upp->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( UppMainWindow::on_connect ), NULL, this );
	m_radioButton_boot->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( UppMainWindow::on_connect_boot ), NULL, this );
	
	delete m_toolBar1;
}
