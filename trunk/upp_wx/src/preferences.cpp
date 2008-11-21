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

#include "preferences.h"
BEGIN_EVENT_TABLE(PreferencesDialog,wxDialog)
	EVT_CLOSE(PreferencesDialog::OnClose)
	EVT_BUTTON(ID_WXBUTTONOK,PreferencesDialog::OnOk)
	EVT_BUTTON(ID_WXBUTTONCANCEL,PreferencesDialog::OnCancel)
END_EVENT_TABLE()

PreferencesDialog::PreferencesDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxDialog(parent, id, title, position, size, style)
{
	CreateGUIControls();
}

PreferencesDialog::~PreferencesDialog()
{
} 

void PreferencesDialog::CreateGUIControls()
{
	WxBoxSizer1 = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(WxBoxSizer1);
	this->SetAutoLayout(true);

	wxStaticBox* WxStaticBoxSizerProgram_StaticBoxObj = new wxStaticBox(this, wxID_ANY, wxT("Program"));
	WxStaticBoxSizerProgram = new wxStaticBoxSizer(WxStaticBoxSizerProgram_StaticBoxObj, wxVERTICAL);
	WxBoxSizer1->Add(WxStaticBoxSizerProgram, 0, wxALIGN_CENTER | wxALL | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 5);

	uppConfigProgramCode = new wxCheckBox(this, wxID_ANY, _("Code"), wxPoint(-1,-1), wxSize(200,-1), 0, wxDefaultValidator, wxT("uppConfigProgramCode"));
	WxStaticBoxSizerProgram->Add(uppConfigProgramCode,0,wxALIGN_LEFT | wxALL,5);

	uppConfigProgramConfig = new wxCheckBox(this, wxID_ANY, _("Config"), wxPoint(-1,-1), wxSize(200,-1), 0, wxDefaultValidator, wxT("uppConfigProgramConfig"));
	WxStaticBoxSizerProgram->Add(uppConfigProgramConfig,0,wxALIGN_LEFT | wxALL,5);

	uppConfigProgramData = new wxCheckBox(this, wxID_ANY, _("Data"), wxPoint(-1,-1), wxSize(200,-1), 0, wxDefaultValidator, wxT("uppConfigProgramData"));
	WxStaticBoxSizerProgram->Add(uppConfigProgramData,0,wxALIGN_LEFT | wxALL,5);

    uppConfigEraseBeforeProgramming = new wxCheckBox(this, wxID_ANY, _("Erase before programming"), wxPoint(-1,-1), wxSize(200,-1), 0, 		wxDefaultValidator, wxT("uppConfigEraseBeforeProgramming"));
	WxStaticBoxSizerProgram->Add(uppConfigEraseBeforeProgramming,0,wxALIGN_LEFT | wxALL,5);

	wxStaticBox* WxStaticBoxSizerVerify_StaticBoxObj = new wxStaticBox(this, wxID_ANY, _("Verify"));
	WxStaticBoxSizerVerify = new wxStaticBoxSizer(WxStaticBoxSizerVerify_StaticBoxObj, wxVERTICAL);
	WxBoxSizer1->Add(WxStaticBoxSizerVerify, 0, wxALIGN_CENTER | wxALL | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 5);

	uppConfigVerifyCode = new wxCheckBox(this, wxID_ANY, _("Code"), wxPoint(-1,-1), wxSize(200,-1), 0, wxDefaultValidator, wxT("uppConfigVerifyCode"));
	WxStaticBoxSizerVerify->Add(uppConfigVerifyCode,0,wxALIGN_LEFT | wxALL,5);

	uppConfigVerifyConfig = new wxCheckBox(this, wxID_ANY, _("Config"), wxPoint(-1,-1), wxSize(200,-1), 0, wxDefaultValidator, wxT("uppConfigVerifyConfig"));
	WxStaticBoxSizerVerify->Add(uppConfigVerifyConfig,0,wxALIGN_LEFT | wxALL,5);

	uppConfigVerifyData = new wxCheckBox(this, wxID_ANY, _("Data"), wxPoint(-1,-1), wxSize(200,-1), 0, wxDefaultValidator, wxT("uppConfigVerifyData"));
	WxStaticBoxSizerVerify->Add(uppConfigVerifyData,0,wxALIGN_LEFT | wxALL,5);
	
	WxBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	WxBoxSizer1->Add(WxBoxSizer2, 0, wxALIGN_CENTER | wxALL, 5);

	WxButtonOk = new wxButton(this, ID_WXBUTTONOK, _("Ok"), wxPoint(-1,-1), wxSize(-1,-1), 0, wxDefaultValidator, wxT("WxButtonOk"));
	WxBoxSizer2->Add(WxButtonOk,0,wxALIGN_CENTER | wxALL,5);

	WxButtonCancel = new wxButton(this, ID_WXBUTTONCANCEL, _("Cancel"), wxPoint(-1,-1), wxSize(-1,-1), 0, wxDefaultValidator, wxT("WxButtonCancel"));
	WxBoxSizer2->Add(WxButtonCancel,0,wxALIGN_CENTER | wxALL,5);

	SetTitle(_("Preferences"));
	SetIcon(wxNullIcon);
	
	GetSizer()->Layout();
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Center();
	
}

void PreferencesDialog::OnClose(wxCloseEvent& event)
{
    configFields.OkClicked=false;
	Hide();
}

void PreferencesDialog::OnOk(wxCommandEvent& event)
{
    configFields.ConfigVerifyData=uppConfigVerifyData->IsChecked();
	configFields.ConfigVerifyConfig=uppConfigVerifyConfig->IsChecked();
    configFields.ConfigVerifyCode=uppConfigVerifyCode->IsChecked();
   	configFields.ConfigProgramData=uppConfigProgramData->IsChecked();
	configFields.ConfigProgramConfig=uppConfigProgramConfig->IsChecked();
	configFields.ConfigProgramCode=uppConfigProgramCode->IsChecked();
	configFields.ConfigEraseBeforeProgramming=uppConfigEraseBeforeProgramming->IsChecked();
    configFields.OkClicked=true;
    Hide();
}

void PreferencesDialog::OnCancel(wxCommandEvent& event)
{
    configFields.OkClicked=false;
    Hide();
}

void PreferencesDialog::SetConfigFields(ConfigFields cf)
{
    configFields=cf;
    uppConfigProgramCode->SetValue(cf.ConfigProgramCode);
    uppConfigProgramConfig->SetValue(cf.ConfigProgramConfig);
    uppConfigProgramData->SetValue(cf.ConfigProgramData);
    uppConfigVerifyCode->SetValue(cf.ConfigVerifyCode);
    uppConfigVerifyConfig->SetValue(cf.ConfigVerifyConfig);
    uppConfigVerifyData->SetValue(cf.ConfigVerifyData);
    uppConfigEraseBeforeProgramming->SetValue(cf.ConfigEraseBeforeProgramming);
}
