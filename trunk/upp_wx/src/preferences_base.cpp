///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan 29 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "preferences_base.h"

///////////////////////////////////////////////////////////////////////////

UppPreferencesDialogBase::UppPreferencesDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, _("Global settings for Usbpicprog:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer7->Add( m_staticText12, 0, wxALL, 10 );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Program options") ), wxVERTICAL );
	
	m_pCheckboxProgramCode = new wxCheckBox( this, wxID_ANY, _("Write code memory"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer5->Add( m_pCheckboxProgramCode, 0, wxALL, 5 );
	
	m_pCheckboxProgramConfig = new wxCheckBox( this, wxID_ANY, _("Write config memory"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer5->Add( m_pCheckboxProgramConfig, 0, wxALL, 5 );
	
	m_pCheckboxProgramData = new wxCheckBox( this, wxID_ANY, _("Write data memory"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer5->Add( m_pCheckboxProgramData, 0, wxALL, 5 );
	
	m_pCheckboxEraseBeforeProgramming = new wxCheckBox( this, wxID_ANY, _("Erase before programming"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer5->Add( m_pCheckboxEraseBeforeProgramming, 0, wxALL, 5 );
	
	m_pCheckboxVerifyAfterProgramming = new wxCheckBox( this, wxID_ANY, _("Verify after programming"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer5->Add( m_pCheckboxVerifyAfterProgramming, 0, wxALL, 5 );
	
	m_checkBoxSelfPowered = new wxCheckBox( this, wxID_ANY, _("Device is self powered (disable VDD)"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer5->Add( m_checkBoxSelfPowered, 0, wxALL, 5 );
	
	m_checkBoxLimitVPP = new wxCheckBox( this, wxID_ANY, _("Limit VPP to 5V"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer5->Add( m_checkBoxLimitVPP, 0, wxALL, 5 );
	
	m_checkBoxLimitPGDPGC = new wxCheckBox( this, wxID_ANY, _("Limit PGD and PGC to 3.3V"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer5->Add( m_checkBoxLimitPGDPGC, 0, wxALL, 5 );
	
	bSizer7->Add( sbSizer5, 0, wxALL|wxEXPAND, 10 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Verify options") ), wxVERTICAL );
	
	m_pCheckboxVerifyCode = new wxCheckBox( this, wxID_ANY, _("Verify code memory"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_pCheckboxVerifyCode, 0, wxALL, 5 );
	
	m_pCheckboxVerifyConfig = new wxCheckBox( this, wxID_ANY, _("Verify config memory"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_pCheckboxVerifyConfig, 0, wxALL, 5 );
	
	m_pCheckboxVerifyData = new wxCheckBox( this, wxID_ANY, _("Verify data memory"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer4->Add( m_pCheckboxVerifyData, 0, wxALL, 5 );
	
	bSizer7->Add( sbSizer4, 0, wxALL|wxEXPAND, 10 );
	
	wxStaticBoxSizer* sbSizer41;
	sbSizer41 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Connect Options") ), wxVERTICAL );
	
	m_pCheckboxAutoDetect = new wxCheckBox( this, wxID_ANY, _("Autodetect on Connect"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer41->Add( m_pCheckboxAutoDetect, 0, wxALL, 5 );
	
	bSizer7->Add( sbSizer41, 1, wxALL|wxEXPAND, 10 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("GUI options") ), wxVERTICAL );
	
	m_pCheckboxShowPopups = new wxCheckBox( this, wxID_ANY, _("Shows popups on connect etc..."), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer3->Add( m_pCheckboxShowPopups, 0, wxALL, 5 );
	
	m_pCheckboxLocalize = new wxCheckBox( this, wxID_ANY, _("Localize Usbpicprog in your System Language"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer3->Add( m_pCheckboxLocalize, 0, wxALL, 5 );
	
	bSizer7->Add( sbSizer3, 0, wxALL|wxEXPAND, 10 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer7->Add( m_sdbSizer1, 1, wxALL|wxEXPAND, 10 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	bSizer7->Fit( this );
}

UppPreferencesDialogBase::~UppPreferencesDialogBase()
{
}
