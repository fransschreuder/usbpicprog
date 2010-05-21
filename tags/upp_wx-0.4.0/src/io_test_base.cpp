///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "io_test_base.h"

///////////////////////////////////////////////////////////////////////////

IOTestDialogBase::IOTestDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText17 = new wxStaticText( this, wxID_ANY, _("In this dialog you can test the UsbPicProg hardware by sending requests to set a specific voltage on a specific output pin.\nThe real voltage of the VPP pin and the other pins' statuses are updated once per second and are reported in bold below."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	bSizer1->Add( m_staticText17, 0, wxALL, 10 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizerVPP;
	sbSizerVPP = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("VPP pin") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizerVPP;
	fgSizerVPP = new wxFlexGridSizer( 4, 2, 0, 0 );
	fgSizerVPP->AddGrowableCol( 1 );
	fgSizerVPP->SetFlexibleDirection( wxHORIZONTAL );
	fgSizerVPP->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_radioVPP12 = new wxRadioButton( this, wxID_ANY, _("12V"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizerVPP->Add( m_radioVPP12, 0, wxALL, 5 );
	
	m_labelVPP = new wxStaticText( this, wxID_ANY, _("Current HW voltage:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelVPP->Wrap( -1 );
	fgSizerVPP->Add( m_labelVPP, 0, wxALL, 5 );
	
	m_radioVPP5 = new wxRadioButton( this, wxID_ANY, _("5V"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerVPP->Add( m_radioVPP5, 0, wxALL, 5 );
	
	m_labelVPPVoltage = new wxStaticText( this, wxID_ANY, _("0V"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE|wxST_NO_AUTORESIZE );
	m_labelVPPVoltage->Wrap( -1 );
	m_labelVPPVoltage->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizerVPP->Add( m_labelVPPVoltage, 1, wxALL|wxEXPAND, 5 );
	
	m_radioVPPFLOAT = new wxRadioButton( this, wxID_ANY, _("FLOAT"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerVPP->Add( m_radioVPPFLOAT, 0, wxALL, 5 );
	
	m_labelVPP2 = new wxStaticText( this, wxID_ANY, _("Current HW state:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelVPP2->Wrap( -1 );
	fgSizerVPP->Add( m_labelVPP2, 0, wxALL, 5 );
	
	m_radioVPPRESET = new wxRadioButton( this, wxID_ANY, _("RESET"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerVPP->Add( m_radioVPPRESET, 0, wxALL, 5 );
	
	m_labelVPPVoltage2 = new wxStaticText( this, wxID_ANY, _("0V"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE|wxST_NO_AUTORESIZE );
	m_labelVPPVoltage2->Wrap( -1 );
	m_labelVPPVoltage2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizerVPP->Add( m_labelVPPVoltage2, 1, wxALL|wxEXPAND, 5 );
	
	sbSizerVPP->Add( fgSizerVPP, 1, wxEXPAND, 5 );
	
	bSizer2->Add( sbSizerVPP, 1, wxEXPAND|wxLEFT, 15 );
	
	wxStaticBoxSizer* sbSizerVDD;
	sbSizerVDD = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("VDD pin") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizerVPP1;
	fgSizerVPP1 = new wxFlexGridSizer( 4, 2, 0, 0 );
	fgSizerVPP1->AddGrowableCol( 1 );
	fgSizerVPP1->SetFlexibleDirection( wxHORIZONTAL );
	fgSizerVPP1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_radioVDD5 = new wxRadioButton( this, wxID_ANY, _("5V"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizerVPP1->Add( m_radioVDD5, 1, wxALL, 5 );
	
	m_labelVDD = new wxStaticText( this, wxID_ANY, _("Current HW state:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelVDD->Wrap( -1 );
	fgSizerVPP1->Add( m_labelVDD, 0, wxALL, 5 );
	
	m_radioVDDFLOAT = new wxRadioButton( this, wxID_ANY, _("FLOAT"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerVPP1->Add( m_radioVDDFLOAT, 1, wxALL, 5 );
	
	m_labelVDDVoltage = new wxStaticText( this, wxID_ANY, _("0V"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE|wxST_NO_AUTORESIZE );
	m_labelVDDVoltage->Wrap( -1 );
	m_labelVDDVoltage->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizerVPP1->Add( m_labelVDDVoltage, 1, wxALL|wxEXPAND, 5 );
	
	sbSizerVDD->Add( fgSizerVPP1, 1, wxEXPAND, 5 );
	
	bSizer2->Add( sbSizerVDD, 1, wxEXPAND|wxLEFT, 5 );
	
	wxStaticBoxSizer* sbSizerPGD;
	sbSizerPGD = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("PGD pin") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizerPGD;
	fgSizerPGD = new wxFlexGridSizer( 4, 2, 0, 0 );
	fgSizerPGD->AddGrowableCol( 1 );
	fgSizerPGD->SetFlexibleDirection( wxHORIZONTAL );
	fgSizerPGD->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_radioPGD5 = new wxRadioButton( this, wxID_ANY, _("5V"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizerPGD->Add( m_radioPGD5, 0, wxALL, 5 );
	
	m_labelPGD = new wxStaticText( this, wxID_ANY, _("Current HW state:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelPGD->Wrap( -1 );
	fgSizerPGD->Add( m_labelPGD, 0, wxALL, 5 );
	
	m_radioPGD3_3 = new wxRadioButton( this, wxID_ANY, _("3.3V"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerPGD->Add( m_radioPGD3_3, 0, wxALL, 5 );
	
	m_labelPGDVoltage = new wxStaticText( this, wxID_ANY, _("0V"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE|wxST_NO_AUTORESIZE );
	m_labelPGDVoltage->Wrap( -1 );
	m_labelPGDVoltage->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizerPGD->Add( m_labelPGDVoltage, 1, wxALL|wxEXPAND, 5 );
	
	m_radioPGD0 = new wxRadioButton( this, wxID_ANY, _("0V"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerPGD->Add( m_radioPGD0, 0, wxALL, 5 );
	
	
	fgSizerPGD->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_radioPGDIn = new wxRadioButton( this, wxID_ANY, _("INPUT"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerPGD->Add( m_radioPGDIn, 0, wxALL, 5 );
	
	
	fgSizerPGD->Add( 0, 0, 1, wxEXPAND, 5 );
	
	sbSizerPGD->Add( fgSizerPGD, 1, wxEXPAND, 5 );
	
	bSizer2->Add( sbSizerPGD, 1, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	wxStaticBoxSizer* sbSizerPGC;
	sbSizerPGC = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("PGC pin") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizerPGC;
	fgSizerPGC = new wxFlexGridSizer( 4, 2, 0, 0 );
	fgSizerPGC->AddGrowableCol( 1 );
	fgSizerPGC->SetFlexibleDirection( wxHORIZONTAL );
	fgSizerPGC->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_radioPGC5 = new wxRadioButton( this, wxID_ANY, _("5V"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizerPGC->Add( m_radioPGC5, 0, wxALL, 5 );
	
	m_labelPGC = new wxStaticText( this, wxID_ANY, _("Current HW state:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelPGC->Wrap( -1 );
	fgSizerPGC->Add( m_labelPGC, 0, wxALL, 5 );
	
	m_radioPGC3_3 = new wxRadioButton( this, wxID_ANY, _("3.3V"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerPGC->Add( m_radioPGC3_3, 0, wxALL, 5 );
	
	m_labelPGCVoltage = new wxStaticText( this, wxID_ANY, _("0V"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE|wxST_NO_AUTORESIZE );
	m_labelPGCVoltage->Wrap( -1 );
	m_labelPGCVoltage->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizerPGC->Add( m_labelPGCVoltage, 1, wxALL|wxEXPAND, 5 );
	
	m_radioPGC0 = new wxRadioButton( this, wxID_ANY, _("0V"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerPGC->Add( m_radioPGC0, 0, wxALL, 5 );
	
	
	fgSizerPGC->Add( 0, 0, 1, wxEXPAND, 5 );
	
	sbSizerPGC->Add( fgSizerPGC, 1, wxEXPAND, 5 );
	
	bSizer2->Add( sbSizerPGC, 1, wxEXPAND|wxRIGHT, 15 );
	
	bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1->Realize();
	bSizer1->Add( m_sdbSizer1, 0, wxALL|wxEXPAND, 10 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_radioVPP12->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_vpp ), NULL, this );
	m_radioVPP5->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_vpp ), NULL, this );
	m_radioVPPFLOAT->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_vpp ), NULL, this );
	m_radioVPPRESET->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_vpp ), NULL, this );
	m_radioVDD5->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_vdd ), NULL, this );
	m_radioVDDFLOAT->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_vdd ), NULL, this );
	m_radioPGD5->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgd ), NULL, this );
	m_radioPGD3_3->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgd ), NULL, this );
	m_radioPGD0->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgd ), NULL, this );
	m_radioPGDIn->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgd ), NULL, this );
	m_radioPGC5->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgc ), NULL, this );
	m_radioPGC3_3->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgc ), NULL, this );
	m_radioPGC0->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgc ), NULL, this );
}

IOTestDialogBase::~IOTestDialogBase()
{
	// Disconnect Events
	m_radioVPP12->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_vpp ), NULL, this );
	m_radioVPP5->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_vpp ), NULL, this );
	m_radioVPPFLOAT->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_vpp ), NULL, this );
	m_radioVPPRESET->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_vpp ), NULL, this );
	m_radioVDD5->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_vdd ), NULL, this );
	m_radioVDDFLOAT->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_vdd ), NULL, this );
	m_radioPGD5->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgd ), NULL, this );
	m_radioPGD3_3->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgd ), NULL, this );
	m_radioPGD0->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgd ), NULL, this );
	m_radioPGDIn->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgd ), NULL, this );
	m_radioPGC5->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgc ), NULL, this );
	m_radioPGC3_3->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgc ), NULL, this );
	m_radioPGC0->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestDialogBase::on_pgc ), NULL, this );
}
