///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "io_test_base.h"

///////////////////////////////////////////////////////////////////////////

IOTestBase::IOTestBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableCol( 2 );
	fgSizer1->AddGrowableCol( 3 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	fgSizer1->SetMinSize( wxSize( 721,254 ) ); 
	wxStaticBoxSizer* sbSizerVPP;
	sbSizerVPP = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("VPP") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizerVPP;
	fgSizerVPP = new wxFlexGridSizer( 4, 2, 0, 0 );
	fgSizerVPP->SetFlexibleDirection( wxBOTH );
	fgSizerVPP->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_radioVPP12 = new wxRadioButton( this, wxID_ANY, _("12V"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizerVPP->Add( m_radioVPP12, 0, wxALL, 5 );
	
	m_labelVPP = new wxStaticText( this, wxID_ANY, _("Voltage:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelVPP->Wrap( -1 );
	fgSizerVPP->Add( m_labelVPP, 0, wxALL, 5 );
	
	m_radioVPP5 = new wxRadioButton( this, wxID_ANY, _("5V"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerVPP->Add( m_radioVPP5, 0, wxALL, 5 );
	
	m_labelVPPVoltage = new wxStaticText( this, wxID_ANY, _("0V"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelVPPVoltage->Wrap( -1 );
	fgSizerVPP->Add( m_labelVPPVoltage, 0, wxALL, 5 );
	
	m_radioVPPFLOAT = new wxRadioButton( this, wxID_ANY, _("FLOAT"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerVPP->Add( m_radioVPPFLOAT, 0, wxALL, 5 );
	
	m_labelVPP2 = new wxStaticText( this, wxID_ANY, _("State:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelVPP2->Wrap( -1 );
	fgSizerVPP->Add( m_labelVPP2, 0, wxALL, 5 );
	
	m_radioVPPRESET = new wxRadioButton( this, wxID_ANY, _("RESET"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerVPP->Add( m_radioVPPRESET, 0, wxALL, 5 );
	
	m_labelVPPVoltage2 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_labelVPPVoltage2->Wrap( -1 );
	fgSizerVPP->Add( m_labelVPPVoltage2, 0, wxALL, 5 );
	
	sbSizerVPP->Add( fgSizerVPP, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( sbSizerVPP, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizerVDD;
	sbSizerVDD = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("VDD") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizerVPP1;
	fgSizerVPP1 = new wxFlexGridSizer( 4, 2, 0, 0 );
	fgSizerVPP1->SetFlexibleDirection( wxBOTH );
	fgSizerVPP1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_radioVDD5 = new wxRadioButton( this, wxID_ANY, _("5V"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizerVPP1->Add( m_radioVDD5, 1, wxALL, 5 );
	
	m_labelVDD = new wxStaticText( this, wxID_ANY, _("State:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelVDD->Wrap( -1 );
	fgSizerVPP1->Add( m_labelVDD, 0, wxALL, 5 );
	
	m_radioVDDFLOAT = new wxRadioButton( this, wxID_ANY, _("FLOAT"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerVPP1->Add( m_radioVDDFLOAT, 1, wxALL, 5 );
	
	m_labelVDDVoltage = new wxStaticText( this, wxID_ANY, _("0V"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelVDDVoltage->Wrap( -1 );
	fgSizerVPP1->Add( m_labelVDDVoltage, 0, wxALL, 5 );
	
	sbSizerVDD->Add( fgSizerVPP1, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( sbSizerVDD, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizerPGD;
	sbSizerPGD = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("PGD") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizerPGD;
	fgSizerPGD = new wxFlexGridSizer( 4, 2, 0, 0 );
	fgSizerPGD->SetFlexibleDirection( wxBOTH );
	fgSizerPGD->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_radioPGD5 = new wxRadioButton( this, wxID_ANY, _("5V"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizerPGD->Add( m_radioPGD5, 0, wxALL, 5 );
	
	m_labelPGD = new wxStaticText( this, wxID_ANY, _("State:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelPGD->Wrap( -1 );
	fgSizerPGD->Add( m_labelPGD, 0, wxALL, 5 );
	
	m_radioPGD3_3 = new wxRadioButton( this, wxID_ANY, _("3.3V"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerPGD->Add( m_radioPGD3_3, 0, wxALL, 5 );
	
	m_labelPGDVoltage = new wxStaticText( this, wxID_ANY, _("0V"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelPGDVoltage->Wrap( -1 );
	fgSizerPGD->Add( m_labelPGDVoltage, 0, wxALL, 5 );
	
	m_radioPGD0 = new wxRadioButton( this, wxID_ANY, _("0V"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerPGD->Add( m_radioPGD0, 0, wxALL, 5 );
	
	m_labelPGDSpare1 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_labelPGDSpare1->Wrap( -1 );
	fgSizerPGD->Add( m_labelPGDSpare1, 0, wxALL, 5 );
	
	m_radioPGDIn = new wxRadioButton( this, wxID_ANY, _("INPUT"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerPGD->Add( m_radioPGDIn, 0, wxALL, 5 );
	
	m_labelPGDSpare2 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_labelPGDSpare2->Wrap( -1 );
	fgSizerPGD->Add( m_labelPGDSpare2, 0, wxALL, 5 );
	
	sbSizerPGD->Add( fgSizerPGD, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( sbSizerPGD, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizerPGC;
	sbSizerPGC = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("PGC") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizerPGC;
	fgSizerPGC = new wxFlexGridSizer( 4, 2, 0, 0 );
	fgSizerPGC->SetFlexibleDirection( wxBOTH );
	fgSizerPGC->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_radioPGC5 = new wxRadioButton( this, wxID_ANY, _("5V"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizerPGC->Add( m_radioPGC5, 0, wxALL, 5 );
	
	m_labelPGC = new wxStaticText( this, wxID_ANY, _("State:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelPGC->Wrap( -1 );
	fgSizerPGC->Add( m_labelPGC, 0, wxALL, 5 );
	
	m_radioPGC3_3 = new wxRadioButton( this, wxID_ANY, _("3.3V"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerPGC->Add( m_radioPGC3_3, 0, wxALL, 5 );
	
	m_labelPGCVoltage = new wxStaticText( this, wxID_ANY, _("0V"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelPGCVoltage->Wrap( -1 );
	fgSizerPGC->Add( m_labelPGCVoltage, 0, wxALL, 5 );
	
	m_radioPGC0 = new wxRadioButton( this, wxID_ANY, _("0V"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerPGC->Add( m_radioPGC0, 0, wxALL, 5 );
	
	m_labelPGCSpare1 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_labelPGCSpare1->Wrap( -1 );
	fgSizerPGC->Add( m_labelPGCSpare1, 0, wxALL, 5 );
	
	sbSizerPGC->Add( fgSizerPGC, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( sbSizerPGC, 1, wxEXPAND, 5 );
	
	m_labelVPPSpare21 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_labelVPPSpare21->Wrap( -1 );
	fgSizer1->Add( m_labelVPPSpare21, 0, wxALL, 5 );
	
	m_labelVPPSpare211 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_labelVPPSpare211->Wrap( -1 );
	fgSizer1->Add( m_labelVPPSpare211, 0, wxALL, 5 );
	
	m_labelVPPSpare212 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_labelVPPSpare212->Wrap( -1 );
	fgSizer1->Add( m_labelVPPSpare212, 0, wxALL, 5 );
	
	m_buttonClose = new wxButton( this, wxID_ANY, _("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonClose->SetDefault(); 
	fgSizer1->Add( m_buttonClose, 0, wxALL, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );
	
	// Connect Events
	m_radioVPP12->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_vpp ), NULL, this );
	m_radioVPP5->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_vpp ), NULL, this );
	m_radioVPPFLOAT->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_vpp ), NULL, this );
	m_radioVPPRESET->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_vpp ), NULL, this );
	m_radioVDD5->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_vdd ), NULL, this );
	m_radioVDDFLOAT->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_vdd ), NULL, this );
	m_radioPGD5->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgd ), NULL, this );
	m_radioPGD3_3->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgd ), NULL, this );
	m_radioPGD0->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgd ), NULL, this );
	m_radioPGDIn->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgd ), NULL, this );
	m_radioPGC5->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgc ), NULL, this );
	m_radioPGC3_3->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgc ), NULL, this );
	m_radioPGC0->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgc ), NULL, this );
	m_buttonClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IOTestBase::on_btn_close ), NULL, this );
}

IOTestBase::~IOTestBase()
{
	// Disconnect Events
	m_radioVPP12->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_vpp ), NULL, this );
	m_radioVPP5->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_vpp ), NULL, this );
	m_radioVPPFLOAT->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_vpp ), NULL, this );
	m_radioVPPRESET->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_vpp ), NULL, this );
	m_radioVDD5->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_vdd ), NULL, this );
	m_radioVDDFLOAT->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_vdd ), NULL, this );
	m_radioPGD5->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgd ), NULL, this );
	m_radioPGD3_3->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgd ), NULL, this );
	m_radioPGD0->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgd ), NULL, this );
	m_radioPGDIn->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgd ), NULL, this );
	m_radioPGC5->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgc ), NULL, this );
	m_radioPGC3_3->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgc ), NULL, this );
	m_radioPGC0->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( IOTestBase::on_pgc ), NULL, this );
	m_buttonClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IOTestBase::on_btn_close ), NULL, this );
}
