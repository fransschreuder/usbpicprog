#include "io_test.h"
#include "io_test_base.h"
#include "hardware.h"
#include "pictype.h"
#include "formbuilder_includes.h"

IOTest::IOTest( Hardware* hardware, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)  
	: IOTestBase( parent, id, title,pos , size, style)
{
	m_hardware = hardware;
	m_timer = new wxTimer(this,	wxID_TIMER);
	this->Connect( wxID_TIMER, wxEVT_TIMER, wxTimerEventHandler( IOTest::on_timer ) );
	m_timer->Start(1000);
	double VppVoltage = m_hardware->getVppVoltage();
	m_labelVPPVoltage->SetLabel(wxString::Format("%2.2fV",VppVoltage));
	m_radioVPPFLOAT->SetValue(true);
	m_radioVDDFLOAT->SetValue(true);
	m_radioPGD0->SetValue(true);
	m_radioPGC0->SetValue(true);
	upp_timer();
	CenterOnScreen();
	ShowModal();
}


IOTest::~IOTest()
{
	delete m_timer;
	m_hardware->setPinState(SUBCMD_PIN_VPP, PIN_STATE_FLOAT);
	m_hardware->setPinState(SUBCMD_PIN_VDD, PIN_STATE_FLOAT);
	m_hardware->setPinState(SUBCMD_PIN_PGC, PIN_STATE_0V);
	m_hardware->setPinState(SUBCMD_PIN_PGD, PIN_STATE_0V);
}



void IOTest::upp_vpp()
{
	PIN_STATE state=PIN_STATE_0V;
	if(m_radioVPP12->GetValue())state = PIN_STATE_12V;
	if(m_radioVPP5->GetValue())state = PIN_STATE_5V;
	if(m_radioVPPFLOAT->GetValue())state = PIN_STATE_FLOAT;
	if(m_radioVPPRESET->GetValue())state = PIN_STATE_0V;
	m_hardware->setPinState(SUBCMD_PIN_VPP, state);
	upp_timer();
}

void IOTest::upp_vdd()
{
	PIN_STATE state=PIN_STATE_FLOAT;
	if(m_radioVDD5->GetValue())state = PIN_STATE_5V;
	if(m_radioVDDFLOAT->GetValue())state = PIN_STATE_FLOAT;
	m_hardware->setPinState(SUBCMD_PIN_VDD, state);
	upp_timer();
}

void IOTest::upp_pgd()
{
	PIN_STATE state=PIN_STATE_0V;
	if(m_radioPGD5->GetValue())state = PIN_STATE_5V;
	if(m_radioPGD3_3->GetValue())state = PIN_STATE_3_3V;
	if(m_radioPGD0->GetValue())state = PIN_STATE_0V;
	if(m_radioPGDIn->GetValue())state = PIN_STATE_INPUT;
	m_hardware->setPinState(SUBCMD_PIN_PGD, state);
	upp_timer();
}

void IOTest::upp_pgc()
{
	PIN_STATE state=PIN_STATE_0V;
	if(m_radioPGC5->GetValue())state = PIN_STATE_5V;
	if(m_radioPGC3_3->GetValue())state = PIN_STATE_3_3V;
	if(m_radioPGC0->GetValue())state = PIN_STATE_0V;
	m_hardware->setPinState(SUBCMD_PIN_PGC, state);
	upp_timer();
}

void IOTest::upp_timer()
{
	double VppVoltage = m_hardware->getVppVoltage();
	m_labelVPPVoltage->SetLabel(wxString::Format("%2.2fV",VppVoltage));
	PIN_STATE pinState;
	pinState = m_hardware->getPinState(SUBCMD_PIN_PGC);
	switch(pinState)
	{
		case PIN_STATE_0V: m_labelPGCVoltage->SetLabel("0V");break;
		case PIN_STATE_3_3V: m_labelPGCVoltage->SetLabel("3.3V");break;
		case PIN_STATE_5V: m_labelPGCVoltage->SetLabel("5V");break;
		case PIN_STATE_12V: m_labelPGCVoltage->SetLabel("12V");break;
		case PIN_STATE_FLOAT: m_labelPGCVoltage->SetLabel(_("FLOAT"));break;
		default: m_labelPGCVoltage->SetLabel("");break;
	}
	pinState = m_hardware->getPinState(SUBCMD_PIN_PGD);
	switch(pinState)
	{
		case PIN_STATE_0V: m_labelPGDVoltage->SetLabel("0V");break;
		case PIN_STATE_3_3V: m_labelPGDVoltage->SetLabel("3.3V");break;
		case PIN_STATE_5V: m_labelPGDVoltage->SetLabel("5V");break;
		case PIN_STATE_12V: m_labelPGDVoltage->SetLabel("12V");break;
		case PIN_STATE_FLOAT: m_labelPGDVoltage->SetLabel(_("FLOAT"));break;
		default: m_labelPGDVoltage->SetLabel("");break;
	}
	pinState = m_hardware->getPinState(SUBCMD_PIN_VDD);
	switch(pinState)
	{
		case PIN_STATE_0V: m_labelVDDVoltage->SetLabel("0V");break;
		case PIN_STATE_3_3V: m_labelVDDVoltage->SetLabel("3.3V");break;
		case PIN_STATE_5V: m_labelVDDVoltage->SetLabel("5V");break;
		case PIN_STATE_12V: m_labelVDDVoltage->SetLabel("12V");break;
		case PIN_STATE_FLOAT: m_labelVDDVoltage->SetLabel(_("FLOAT"));break;
		default: m_labelVDDVoltage->SetLabel("");break;
	}
	pinState = m_hardware->getPinState(SUBCMD_PIN_VPP);
	switch(pinState)
	{
		case PIN_STATE_0V: m_labelVPPVoltage2->SetLabel("0V");break;
		case PIN_STATE_3_3V: m_labelVPPVoltage2->SetLabel("3.3V");break;
		case PIN_STATE_5V: m_labelVPPVoltage2->SetLabel("5V");break;
		case PIN_STATE_12V: m_labelVPPVoltage2->SetLabel("12V");break;
		case PIN_STATE_FLOAT: m_labelVPPVoltage2->SetLabel(_("FLOAT"));break;
		default: m_labelVPPVoltage2->SetLabel("");break;
	}

	
}


void IOTest::upp_btn_close()
{
	Close();
}
