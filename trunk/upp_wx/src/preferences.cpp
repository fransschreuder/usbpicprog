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

#include <wx/statbox.h>
#include <wx/wx.h>

#include "preferences.h"


BEGIN_EVENT_TABLE(UppPreferencesDialog,wxDialog)
    EVT_BUTTON(wxID_OK, UppPreferencesDialog::OnOk)
END_EVENT_TABLE()

UppPreferencesDialog::UppPreferencesDialog(wxWindow *parent, wxWindowID id, const wxString &title, 
                                           const wxPoint &position, const wxSize& size, long style)
    : wxDialog(parent, id, title, position, size, style)
{
    CreateGUIControls();
}

UppPreferencesDialog::~UppPreferencesDialog()
{
}

void UppPreferencesDialog::CreateGUIControls()
{
    WxBoxSizer1 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(WxBoxSizer1);
    this->SetAutoLayout(true);

    wxStaticBox* WxStaticBoxSizerProgram_StaticBoxObj = new wxStaticBox(this, wxID_ANY, wxT("Program"));
    WxStaticBoxSizerProgram = new wxStaticBoxSizer(WxStaticBoxSizerProgram_StaticBoxObj, wxVERTICAL);
    WxBoxSizer1->Add(WxStaticBoxSizerProgram, 0, wxALIGN_CENTER | wxALL | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 5);

    uppConfigProgramCode = new wxCheckBox(this, wxID_ANY, _("Code"), wxPoint(-1,-1), wxSize(300,-1), 0, wxDefaultValidator, wxT("uppConfigProgramCode"));
    WxStaticBoxSizerProgram->Add(uppConfigProgramCode,0,wxALIGN_LEFT | wxALL,5);

    uppConfigProgramConfig = new wxCheckBox(this, wxID_ANY, _("Config"), wxPoint(-1,-1), wxSize(300,-1), 0, wxDefaultValidator, wxT("uppConfigProgramConfig"));
    WxStaticBoxSizerProgram->Add(uppConfigProgramConfig,0,wxALIGN_LEFT | wxALL,5);

    uppConfigProgramData = new wxCheckBox(this, wxID_ANY, _("Data"), wxPoint(-1,-1), wxSize(300,-1), 0, wxDefaultValidator, wxT("uppConfigProgramData"));
    WxStaticBoxSizerProgram->Add(uppConfigProgramData,0,wxALIGN_LEFT | wxALL,5);

    uppConfigEraseBeforeProgramming = new wxCheckBox(this, wxID_ANY, _("Erase before programming"), wxPoint(-1,-1), wxSize(200,-1), 0, wxDefaultValidator, wxT("uppConfigEraseBeforeProgramming"));
    WxStaticBoxSizerProgram->Add(uppConfigEraseBeforeProgramming,0,wxALIGN_LEFT | wxALL,5);

    wxStaticBox* WxStaticBoxSizerVerify_StaticBoxObj = new wxStaticBox(this, wxID_ANY, _("Verify"));
    WxStaticBoxSizerVerify = new wxStaticBoxSizer(WxStaticBoxSizerVerify_StaticBoxObj, wxVERTICAL);
    WxBoxSizer1->Add(WxStaticBoxSizerVerify, 0, wxALIGN_CENTER | wxALL | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 5);

    uppConfigVerifyCode = new wxCheckBox(this, wxID_ANY, _("Code"), wxPoint(-1,-1), wxSize(300,-1), 0, wxDefaultValidator, wxT("uppConfigVerifyCode"));
    WxStaticBoxSizerVerify->Add(uppConfigVerifyCode,0,wxALIGN_LEFT | wxALL,5);

    uppConfigVerifyConfig = new wxCheckBox(this, wxID_ANY, _("Config"), wxPoint(-1,-1), wxSize(300,-1), 0, wxDefaultValidator, wxT("uppConfigVerifyConfig"));
    WxStaticBoxSizerVerify->Add(uppConfigVerifyConfig,0,wxALIGN_LEFT | wxALL,5);

    uppConfigVerifyData = new wxCheckBox(this, wxID_ANY, _("Data"), wxPoint(-1,-1), wxSize(300,-1), 0, wxDefaultValidator, wxT("uppConfigVerifyData"));
    WxStaticBoxSizerVerify->Add(uppConfigVerifyData,0,wxALIGN_LEFT | wxALL,5);

	uppConfigVerifyAfterProgramming = new wxCheckBox(this, wxID_ANY, _("Verify after programming"), wxPoint(-1,-1), wxSize(300,-1), 0, wxDefaultValidator, wxT("uppConfigVerifyAfterProgramming"));
    WxStaticBoxSizerVerify->Add(uppConfigVerifyAfterProgramming,0,wxALIGN_LEFT | wxALL,5);

	
    wxStaticBox* WxStaticBoxSizerGui_StaticBoxObj = new wxStaticBox(this, wxID_ANY, _("Gui"));
    WxStaticBoxSizerGui = new wxStaticBoxSizer(WxStaticBoxSizerGui_StaticBoxObj, wxVERTICAL);
    WxBoxSizer1->Add(WxStaticBoxSizerGui, 0, wxALIGN_CENTER | wxALL | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 5);

    uppConfigShowPopups = new wxCheckBox(this, wxID_ANY, _("Show Popups on connect etc..."), wxPoint(-1,-1), wxSize(300,-1), 0, wxDefaultValidator, wxT("uppConfigShowPopups"));
    WxStaticBoxSizerGui->Add(uppConfigShowPopups,0,wxALIGN_LEFT | wxALL,5);

    wxStdDialogButtonSizer*WxBoxSizer2 = new wxStdDialogButtonSizer();
    WxBoxSizer1->Add(WxBoxSizer2, 0, wxALIGN_CENTER | wxALL, 5);

    WxBoxSizer2->AddButton(new wxButton(this, wxID_OK));
    WxBoxSizer2->AddButton(new wxButton(this, wxID_CANCEL));
    WxBoxSizer2->Realize();

    SetTitle(_("Preferences"));
    SetIcon(wxNullIcon);

    GetSizer()->Layout();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Center();
}

void UppPreferencesDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
	m_cfg.ConfigVerifyAfterProgramming=uppConfigVerifyAfterProgramming->IsChecked();
    m_cfg.ConfigVerifyData=uppConfigVerifyData->IsChecked();
    m_cfg.ConfigVerifyConfig=uppConfigVerifyConfig->IsChecked();
    m_cfg.ConfigVerifyCode=uppConfigVerifyCode->IsChecked();
    m_cfg.ConfigProgramData=uppConfigProgramData->IsChecked();
    m_cfg.ConfigProgramConfig=uppConfigProgramConfig->IsChecked();
    m_cfg.ConfigProgramCode=uppConfigProgramCode->IsChecked();
    m_cfg.ConfigEraseBeforeProgramming=uppConfigEraseBeforeProgramming->IsChecked();
    m_cfg.ConfigShowPopups=uppConfigShowPopups->IsChecked();

    EndModal(wxID_OK);
}

void UppPreferencesDialog::SetConfigFields(UppPreferences cf)
{
    m_cfg=cf;
    uppConfigVerifyAfterProgramming->SetValue(cf.ConfigVerifyAfterProgramming);	
    uppConfigProgramCode->SetValue(cf.ConfigProgramCode);
    uppConfigProgramConfig->SetValue(cf.ConfigProgramConfig);
    uppConfigProgramData->SetValue(cf.ConfigProgramData);
    uppConfigVerifyCode->SetValue(cf.ConfigVerifyCode);
    uppConfigVerifyConfig->SetValue(cf.ConfigVerifyConfig);
    uppConfigVerifyData->SetValue(cf.ConfigVerifyData);
    uppConfigEraseBeforeProgramming->SetValue(cf.ConfigEraseBeforeProgramming);
    uppConfigShowPopups->SetValue(cf.ConfigShowPopups);

}
