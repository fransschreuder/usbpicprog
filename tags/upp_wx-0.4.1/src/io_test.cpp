/***************************************************************************
*   Copyright (C) 2008-2010 by Frans Schreuder                            *
*   usbpicprog.sourceforge.net                                            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

// NOTE: to avoid lots of warnings with MSVC 2008 about deprecated CRT functions
//       it's important to include wx/defs.h before STL headers
#include <wx/defs.h>

#include "io_test.h"
#include "io_test_base.h"
#include "hardware.h"
#include "pictype.h"
#include "formbuilder_includes.h"

#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // useful to catch memory leaks when compiling under MSVC 
#endif


// ----------------------------------------------------------------------------
// IOTestDialog
// ----------------------------------------------------------------------------

IOTestDialog::IOTestDialog(Hardware* hardware, wxWindow* parent)  
	: IOTestDialogBase(parent)
{
	m_hardware = hardware;

    // init the timer and its handler
	m_timer = new wxTimer(this,	wxID_TIMER);
	this->Connect( wxID_TIMER, wxEVT_TIMER, wxTimerEventHandler( IOTestDialog::on_timer ) );

    // init one of the radio button of each staticboxsizer:
	m_radioVPPFLOAT->SetValue(true);
	m_radioVDDFLOAT->SetValue(true);
	m_radioPGD0->SetValue(true);
	m_radioPGC0->SetValue(true);
	
    upp_update_hw_status();

	CenterOnScreen();

   	m_timer->Start(1000);
}

IOTestDialog::~IOTestDialog()
{
	delete m_timer;

    // reset pin statuses:
	m_hardware->setPinState(SUBCMD_PIN_VPP, PIN_STATE_FLOAT);
	m_hardware->setPinState(SUBCMD_PIN_VDD, PIN_STATE_FLOAT);
	m_hardware->setPinState(SUBCMD_PIN_PGC, PIN_STATE_0V);
	m_hardware->setPinState(SUBCMD_PIN_PGD, PIN_STATE_0V);
}

void IOTestDialog::upp_vpp()
{
	PIN_STATE state=PIN_STATE_0V;
	if(m_radioVPP12->GetValue())state = PIN_STATE_12V;
	if(m_radioVPP5->GetValue())state = PIN_STATE_5V;
	if(m_radioVPPFLOAT->GetValue())state = PIN_STATE_FLOAT;
	if(m_radioVPPRESET->GetValue())state = PIN_STATE_0V;
	m_hardware->setPinState(SUBCMD_PIN_VPP, state);
	upp_update_hw_status();
}

void IOTestDialog::upp_vdd()
{
	PIN_STATE state=PIN_STATE_FLOAT;
	if(m_radioVDD5->GetValue())state = PIN_STATE_5V;
	if(m_radioVDDFLOAT->GetValue())state = PIN_STATE_FLOAT;
	m_hardware->setPinState(SUBCMD_PIN_VDD, state);
	upp_update_hw_status();
}

void IOTestDialog::upp_pgd()
{
	PIN_STATE state=PIN_STATE_0V;
	if(m_radioPGD5->GetValue())state = PIN_STATE_5V;
	if(m_radioPGD3_3->GetValue())state = PIN_STATE_3_3V;
	if(m_radioPGD0->GetValue())state = PIN_STATE_0V;
	if(m_radioPGDIn->GetValue())state = PIN_STATE_INPUT;
	m_hardware->setPinState(SUBCMD_PIN_PGD, state);
	upp_update_hw_status();
}

void IOTestDialog::upp_pgc()
{
	PIN_STATE state=PIN_STATE_0V;
	if(m_radioPGC5->GetValue())state = PIN_STATE_5V;
	if(m_radioPGC3_3->GetValue())state = PIN_STATE_3_3V;
	if(m_radioPGC0->GetValue())state = PIN_STATE_0V;
	m_hardware->setPinState(SUBCMD_PIN_PGC, state);
	upp_update_hw_status();
}

void IOTestDialog::upp_update_hw_status()
{
    if(!m_hardware->connected())return;
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

void IOTestDialog::upp_btn_close()
{
	Close();
}
