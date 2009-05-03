/***************************************************************************
*   Copyright (C) 2008 by Frans Schreuder, Francesco Montorsi             *
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

#ifndef CONFIGVIEW_H
#define CONFIGVIEW_H

#include <wx/listbook.h>
#include <wx/textctrl.h>

#include "pictype.h"
#include "hexfile.h"


class wxChoice;
class wxTextCtrl;

/// Helper structure used by UppConfigViewBook to keep track of the
/// controls it creates for each of its pages
typedef struct
{
    /// The wxChoices associated with the various ConfigMask objects contained
    /// into the same ConfigWord instance
    vector<wxChoice*> choiceArray;

    /// The text control which holds the HEX value for the configuration word
    wxTextCtrl* textCtrl;
} UppConfigViewPageControls;



/**
    A specialized listbook which shows the configuration flags of a PIC.
*/
class UppConfigViewBook : public wxListbook
{
public:
    UppConfigViewBook(wxWindow* parent, wxWindowID id);
    ~UppConfigViewBook();

    /**
        Sets the HexFile instance to update upon user changes on the config flags.
        This function also needs a Pic instance because the HexFile stores the flags
        in a plain array but this widget needs more info to display the various
        configuration options to the user; the Pic class provides them.

        This function resets the current contents of this control using the
        data stored in the HexFile.

        Note that the @a hex pointer is stored inside this class
        thus the caller must ensure the validity of the HexFile object 
        for the entire lifetime of this window.

    */
    void SetHexFile(HexFile* hex, const PicType& pic);

private:
    /**
        The HexFile contains the current config flags.
    */
    HexFile* m_hexFile;

    /**
        The Pic structure contains info about the config flags for the PIC model
        selected by the user (but does not contain the actual data - m_hexFile contains it).
    */
    PicType m_pic;

    /**
        Pointers to the wxChoice and wxTextCtrl controls which contain the current values 
        of the various configuration words.
    */
    vector<UppConfigViewPageControls> m_ctrl;

protected:
    /**
        Called when the user changes the selection inside one of the wxChoice that are
        contained inside UppConfigViewBook.
    */
    void OnChoiceChange(wxCommandEvent& event);

    /**
        Called when the user changes the contents of one of m_ctrl::textCtrl text controls.
    */
    void OnConfigWordDirectChange(wxCommandEvent& event);
};

#endif //CONFIGVIEW_H
