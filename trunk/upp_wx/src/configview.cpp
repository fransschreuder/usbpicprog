/***************************************************************************
*   Copyright (C) 2008-2009 by Frans Schreuder, Francesco Montorsi        *
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

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/statline.h>

#include "configview.h"
#include "uppmainwindow.h"


// ----------------------------------------------------------------------------
// UppConfigViewBook
// ----------------------------------------------------------------------------

UppConfigViewBook::UppConfigViewBook(wxWindow* parent, wxWindowID id)
    : wxListbook( parent, id, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT )
{
}

UppConfigViewBook::~UppConfigViewBook()
{
}

void UppConfigViewBook::SetHexFile(HexFile* hex, const Pic& pic)
{
    m_pic = pic;
    m_hexFile = hex;

    // reset current contents
    DeleteAllPages();

    // add new GUI selectors for the config flags of this PIC
    for (unsigned int i=0; i<m_pic.ConfigWords.size(); i++)
    {
        UppConfigViewPageControls pageCtrl;

        const ConfigWord& word = m_pic.ConfigWords[i];
        /*if (word.Masks.size() == 0)
            continue;       // skip this word*/

        wxPanel *panel = new wxPanel(this, wxID_ANY);
        wxFlexGridSizer *sz = new wxFlexGridSizer(2 /* num of columns */,
                                                  10, 10 /* h and vgap */);
        unsigned int ConfigWord=0;
        for (unsigned int j=0; j<word.Masks.size(); j++)
        {
            const ConfigMask& mask = word.Masks[j];

            sz->Add(new wxStaticText(panel, wxID_ANY, mask.Name + ":"),
                    0, wxLEFT|wxALIGN_CENTER, 5);

            // NOTE: we give each wxChoice we build the name of the mask it controls;
            //       in this way from OnChoiceChange() we can easily find out which object
            //       is sending the notification
            wxChoice *choice =
                new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                             mask.GetStringValues(), 0, wxDefaultValidator, mask.Name);

            // set the configuration word
            if (m_pic.is16Bit())
            {
                if ((i+1) <= hex->getConfigMemory().size())
                {
                    ConfigWord = hex->getConfigMemory()[i];
                }
            }
            else        // 8 bit PIC
            {
                if ((2*i+1) <= hex->getConfigMemory().size())
                {
                    ConfigWord = (hex->getConfigMemory()[i*2])|
                                 (hex->getConfigMemory()[i*2+1]<<8);
                }
            }

            // build the configuration word mask combining all values allowed
            // for this ConfigMask object
            // TODO: reuse Pic::ConfigMask here!
            unsigned int ConfigWordMask = 0;
            for (unsigned int k=0; k < mask.Values.size(); k++)
                ConfigWordMask |= mask.Values[k].Value;

            choice->SetSelection(0);
            for (unsigned int k=0; k < mask.Values.size(); k++)
            {
                if ((ConfigWord&ConfigWordMask) == mask.Values[k].Value)
                {
                    choice->SetSelection(k);
                    break;  // the current value for this ConfigMask has been found
                }
            }

            choice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, 
                wxCommandEventHandler(UppConfigViewBook::OnChoiceChange), 
                NULL, this);

            sz->Add(choice, 0, wxRIGHT|wxALIGN_CENTER|wxEXPAND, 5);

            pageCtrl.choiceCtrl.push_back(choice);
        }


        sz->AddSpacer(1);
        sz->Add(new wxStaticLine(panel, wxID_ANY), 1, wxEXPAND|wxALL, 10);


        // add the text control which contains the current hex value
        // for the i-th configuration word

        sz->Add(new wxStaticText(panel, wxID_ANY, "Word:"),
                0, wxLEFT|wxALIGN_CENTER, 5);

        wxString configWordHex;
        if (m_pic.is16Bit())
            configWordHex.Printf("%04X", ConfigWord);
        else
            configWordHex.Printf("%02X", ConfigWord);

        wxTextCtrl* tc = new wxTextCtrl(panel, wxID_ANY, configWordHex,
                                         wxDefaultPosition, wxDefaultSize, 0, 
                                         wxDefaultValidator, word.Name+"Configword");
        tc->SetToolTip(
            wxString::Format(_("The current value for the %d-th configuration word as derived from above configuration choices"), i));

        tc->SetMaxLength(m_pic.is16Bit() ? 4 : 2);
        tc->Connect(wxEVT_COMMAND_TEXT_UPDATED, 
                    wxCommandEventHandler(UppConfigViewBook::OnConfigWordDirectChange), 
                    NULL, this);

        sz->Add(tc, 0, wxRIGHT|wxALIGN_CENTER|wxEXPAND, 5);

        pageCtrl.textCtrl = tc;


        // complete the creation of the panel:

        sz->AddGrowableCol(1, 1);
        panel->SetSizerAndFit(sz);


        // complete the creation of this page:

        AddPage(panel, word.Name.empty() ? wxString::Format("Word %d", i) : word.Name);
        m_ctrl.push_back(pageCtrl);
    }
}

void UppConfigViewBook::OnChoiceChange(wxCommandEvent& event)
{
    wxChoice *choice = dynamic_cast<wxChoice*>(event.GetEventObject());
    wxASSERT(choice);

    unsigned int SelectedWord = GetSelection();

    // get the cfg word the user is currently viewing/editing
    const ConfigWord& word = m_pic.ConfigWords[SelectedWord];

    // find the config mask (associated with the choice control which emitted
    // the event) which needs to be updated
    const ConfigMask* mask = NULL;
    for (unsigned int i=0; i<word.Masks.size(); i++)
    {
        if (word.Masks[i].Name == choice->GetName())
        {
            mask = &word.Masks[i];
            break;
        }
    }

    wxASSERT(mask);

    // get the current value of the configuration word from the HEX file
    int ConfigWord = 0; 
    if (m_pic.is16Bit())
    {
        if ((SelectedWord+1) <= m_hexFile->getConfigMemory().size())
            ConfigWord = m_hexFile->getConfigMemory()[SelectedWord];
    }
    else    // 8 bit PIC
    {
        if ((2*SelectedWord+1) <= m_hexFile->getConfigMemory().size())
            ConfigWord = (m_hexFile->getConfigMemory()[SelectedWord*2])|
                         (m_hexFile->getConfigMemory()[SelectedWord*2+1]<<8);
    }

    // combine the current value with the new value selected by the user
    // through the wxChoice
    // TODO: reuse Pic::ConfigMask
    for (unsigned int i=0; i<mask->Values.size();i++)
        ConfigWord &= ~mask->Values[i].Value;
    ConfigWord |= mask->Values[choice->GetSelection()].Value;

    wxString configWordHex;
    if (m_pic.is16Bit())
        configWordHex.Printf("%04X", ConfigWord);
    else
        configWordHex.Printf("%02X", ConfigWord);

    // save the new value back in the HEX file and in the textctrl for the
    // selected configuration word:
    m_ctrl[SelectedWord].textCtrl->ChangeValue(configWordHex);
    if (m_pic.is16Bit())
    {
        m_hexFile->putConfigMemory(SelectedWord, ConfigWord&0xFF);
    }
    else
    {
        m_hexFile->putConfigMemory(SelectedWord*2, ConfigWord&0xFF);
        m_hexFile->putConfigMemory(SelectedWord*2+1, (ConfigWord&0xFF00)>>8);
    }

    // notify the main window about this change
    UppMainWindow* main = dynamic_cast<UppMainWindow*>(wxTheApp->GetTopWindow());
    wxASSERT(main);
    main->upp_hex_changed();
}

void UppConfigViewBook::OnConfigWordDirectChange(wxCommandEvent& event)
{
    unsigned long ConfigWordInt = 0; 

    wxString configStr = m_ctrl[GetSelection()].textCtrl->GetValue();
    if (configStr.empty())
        return;     // do not change anything

    if (!configStr.ToULong(&ConfigWordInt, 16))
    {
        // the user has written something invalid (i.e. not convertible to a number)
        // inside the text ctrl; in this case restore the default value for the affected
        // configuration word reading it from the HEX file config memory area

        if (m_pic.is16Bit())
        {
            if ((unsigned)(GetSelection()+1) <= m_hexFile->getConfigMemory().size())
            {
                ConfigWordInt = m_hexFile->getConfigMemory()[GetSelection()];
            }
        }
        else        // 8 bit PIC
        {
            if ((unsigned)(2*GetSelection()+1) <= m_hexFile->getConfigMemory().size())
            {
                ConfigWordInt = ((m_hexFile->getConfigMemory()[GetSelection()*2])|
                                (m_hexFile->getConfigMemory()[GetSelection()*2+1]<<8));
            }
        }

        wxString configWordHex;
        if (m_pic.is16Bit())
            configWordHex.Printf("%02X",ConfigWordInt);
        else
            configWordHex.Printf("%04X",ConfigWordInt);

        m_ctrl[GetSelection()].textCtrl->ChangeValue(configWordHex);
    }
    else
    {
        // the user entered a valid integer; put the value into the hexfile
        if (m_pic.is16Bit())
        {
            ConfigWordInt &= 0xFF;
            m_hexFile->putConfigMemory(GetSelection(), ConfigWordInt&0xFF);
        }
        else
        {
            ConfigWordInt &= 0x3FFF;
            m_hexFile->putConfigMemory(GetSelection()*2, ConfigWordInt&0xFF);
            m_hexFile->putConfigMemory(GetSelection()*2+1, (ConfigWordInt&0xFF00)>>8);
        }

        // notify the main window about this change
        UppMainWindow* main = dynamic_cast<UppMainWindow*>(wxTheApp->GetTopWindow());
        wxASSERT(main);
        main->upp_hex_changed();

        // now update the wxChoices for the selected configuration word

        const ConfigWord& word = m_pic.ConfigWords[GetSelection()];
        for (unsigned int i=0;i<word.Masks.size();i++)
        {
            const ConfigMask& mask = word.Masks[i];

            // walk among the childrens of the selected page
            wxChoice *choice;
            for (unsigned int j=0;j<GetCurrentPage()->GetChildren().size();j++)
            {
                if (mask.Name == GetCurrentPage()->GetChildren().Item(j)->GetData()->GetName())
                {
                    // we've found a wxChoice
                    choice = dynamic_cast<wxChoice*> (GetCurrentPage()->GetChildren().Item(j)->GetData());
                    break;
                }
            }

            wxASSERT(choice);

            // build the mask for this configuration word
            // TODO: reuse Pic::ConfigMask
            int ConfigWordMask=0;
            for (unsigned int k=0;k<mask.Values.size();k++)
                ConfigWordMask|=mask.Values[k].Value;

            // now update the selection for this wxChoice
            choice->SetSelection(0);
            for (unsigned int k=0; k<mask.Values.size(); k++)
                if ((ConfigWordInt&ConfigWordMask) == mask.Values[k].Value)
                    choice->SetSelection(k);
        }
    }
}
