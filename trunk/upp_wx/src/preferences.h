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

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H


#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>

#undef PREFERENCESDIALOG_STYLE
#define PREFERENCESDIALOG_STYLE  (wxCAPTION | wxSYSTEM_MENU | wxSTAY_ON_TOP | wxDIALOG_NO_PARENT | wxMINIMIZE_BOX | wxCLOSE_BOX)


typedef struct {
        bool ConfigEraseBeforeProgramming;
        bool ConfigVerifyData;
        bool ConfigVerifyConfig;
        bool ConfigVerifyCode;
        bool ConfigProgramData;
        bool ConfigProgramConfig;
        bool ConfigProgramCode;
	bool ConfigShowPopups;
}ConfigFields;

class PreferencesDialog : public wxDialog
{
private:
    DECLARE_EVENT_TABLE();

public:
    PreferencesDialog(wxWindow *parent, wxWindowID id = 1, const wxString &title = _("Preferences"),
                      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                      long style = PREFERENCESDIALOG_STYLE);
    virtual ~PreferencesDialog();

    ConfigFields GetResult() const {return configFields;}
    void SetConfigFields(ConfigFields cf);

private:

    wxCheckBox *uppConfigVerifyData;
    wxCheckBox *uppConfigVerifyConfig;
    wxCheckBox *uppConfigVerifyCode;
    wxStaticBoxSizer *WxStaticBoxSizerVerify;
    wxCheckBox *uppConfigProgramData;
    wxCheckBox *uppConfigProgramConfig;
    wxCheckBox *uppConfigProgramCode;
    wxCheckBox *uppConfigEraseBeforeProgramming;
    wxStaticBoxSizer *WxStaticBoxSizerProgram;
    wxCheckBox *uppConfigShowPopups;
    wxStaticBoxSizer *WxStaticBoxSizerGui;
    wxBoxSizer *WxBoxSizer1;
    ConfigFields configFields;

private:
    void OnOk(wxCommandEvent& event);
    void CreateGUIControls();
};

#endif  // PREFERENCESDIALOG_H
