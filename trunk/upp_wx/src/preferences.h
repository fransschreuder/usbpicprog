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

#include "preferences_base.h"

/**
    A structure containing the global preferences for the UsbPicProg GUI.
*/
typedef struct {
	bool ConfigVerifyAfterProgramming;
    bool ConfigEraseBeforeProgramming;
    bool ConfigVerifyData;
    bool ConfigVerifyConfig;
    bool ConfigVerifyCode;
    bool ConfigProgramData;
    bool ConfigProgramConfig;
    bool ConfigProgramCode;
    bool ConfigShowPopups;
	bool ConfigLocalize;
	bool ConfigAutoDetect;
	bool ConfigDisableVDD;
	bool ConfigLimitVPP;
	bool ConfigLimitPGDPGC;
} UppPreferences;

/**
    The dialog which allows the user to toggle/untoggle some global options.
*/
class UppPreferencesDialog : public UppPreferencesDialogBase
{
private:
    DECLARE_EVENT_TABLE();

public:
    UppPreferencesDialog(wxWindow *parent);
    virtual ~UppPreferencesDialog();

    const UppPreferences& GetResult() const 
        { return m_cfg; }
    void SetConfigFields(const UppPreferences& cf);

private:
    void OnOk(wxCommandEvent& event);

    UppPreferences m_cfg;
};

#endif  // PREFERENCESDIALOG_H
