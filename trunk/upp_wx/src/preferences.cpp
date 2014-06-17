/***************************************************************************
*   Copyright (C) 2008 by Frans Schreuder                                 *
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

#include "preferences.h"

#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // useful to catch memory leaks when compiling under MSVC 
#endif


// ----------------------------------------------------------------------------
// UppPreferencesDialog
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(UppPreferencesDialog, wxDialog)
    EVT_BUTTON(wxID_OK, UppPreferencesDialog::OnOk)
END_EVENT_TABLE()

UppPreferencesDialog::UppPreferencesDialog(wxWindow *parent)
    : UppPreferencesDialogBase(parent)
{
    CenterOnScreen();
}

UppPreferencesDialog::~UppPreferencesDialog()
{
}

void UppPreferencesDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
    m_cfg.ConfigVerifyAfterProgramming=m_pCheckboxVerifyAfterProgramming->IsChecked();
    m_cfg.ConfigVerifyData=m_pCheckboxVerifyData->IsChecked();
    m_cfg.ConfigVerifyConfig=m_pCheckboxVerifyConfig->IsChecked();
    m_cfg.ConfigVerifyCode=m_pCheckboxVerifyCode->IsChecked();
    m_cfg.ConfigProgramData=m_pCheckboxProgramData->IsChecked();
    m_cfg.ConfigProgramConfig=m_pCheckboxProgramConfig->IsChecked();
    m_cfg.ConfigProgramCode=m_pCheckboxProgramCode->IsChecked();
    m_cfg.ConfigEraseBeforeProgramming=m_pCheckboxEraseBeforeProgramming->IsChecked();
    m_cfg.ConfigShowPopups=m_pCheckboxShowPopups->IsChecked();
	m_cfg.ConfigLocalize=m_pCheckboxLocalize->IsChecked();
	m_cfg.ConfigAutoDetect=m_pCheckboxAutoDetect->IsChecked();
	m_cfg.ConfigDisableVDD=m_checkBoxSelfPowered->IsChecked();
	m_cfg.ConfigLimitVPP=m_checkBoxLimitVPP->IsChecked();
	m_cfg.ConfigLimitPGDPGC=m_checkBoxLimitPGDPGC->IsChecked();

    EndModal(wxID_OK);
}

void UppPreferencesDialog::SetConfigFields(const UppPreferences& cf)
{
    m_cfg=cf;
    m_pCheckboxVerifyAfterProgramming->SetValue(cf.ConfigVerifyAfterProgramming);	
    m_pCheckboxProgramCode->SetValue(cf.ConfigProgramCode);
    m_pCheckboxProgramConfig->SetValue(cf.ConfigProgramConfig);
    m_pCheckboxProgramData->SetValue(cf.ConfigProgramData);
    m_pCheckboxVerifyCode->SetValue(cf.ConfigVerifyCode);
    m_pCheckboxVerifyConfig->SetValue(cf.ConfigVerifyConfig);
    m_pCheckboxVerifyData->SetValue(cf.ConfigVerifyData);
    m_pCheckboxEraseBeforeProgramming->SetValue(cf.ConfigEraseBeforeProgramming);
    m_pCheckboxShowPopups->SetValue(cf.ConfigShowPopups);
	m_pCheckboxLocalize->SetValue(cf.ConfigLocalize);
	m_pCheckboxAutoDetect->SetValue(cf.ConfigAutoDetect);

    m_checkBoxSelfPowered->SetValue(cf.ConfigDisableVDD);
	m_checkBoxLimitVPP->SetValue(cf.ConfigLimitVPP);
	m_checkBoxLimitPGDPGC->SetValue(cf.ConfigLimitPGDPGC);
}
