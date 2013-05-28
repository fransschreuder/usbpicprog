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

#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // useful to catch memory leaks when compiling under MSVC 
#endif


// ----------------------------------------------------------------------------
// UppConfigViewBook
// ----------------------------------------------------------------------------

UppConfigViewBook::UppConfigViewBook(wxWindow* parent, wxWindowID id)
    : wxListbook( parent, id, wxDefaultPosition, wxDefaultSize, wxLB_LEFT )
{
}

UppConfigViewBook::~UppConfigViewBook()
{
}

void UppConfigViewBook::SetHexFile(HexFile* hex, const PicType& pic)
{
    m_pic = pic;
    m_hexFile = hex;

    // reset current contents
    DeleteAllPages();
    m_ctrl.clear();

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
#if wxCHECK_VERSION(2,9,2)
			wxStaticText* tx = new wxStaticText(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
			tx->SetLabelMarkup(wxString::Format("<b>%s</b> [%d bits]:", mask.Name, mask.GetBitSize()));
			sz->Add(tx,  0, wxLEFT|wxALIGN_CENTER, 5);
#else
            sz->Add(new wxStaticText(panel, wxID_ANY, 
                                     wxString::Format("<b>%s</b> [%d bits]:", mask.Name, mask.GetBitSize()),
                                     wxDefaultPosition, wxDefaultSize, wxST_MARKUP),
                    0, wxLEFT|wxALIGN_CENTER, 5);
#endif

            // NOTE: we give each wxChoice we build the name of the mask it controls;
            //       in this way from OnChoiceChange() we can easily find out which object
            //       is sending the notification
            wxChoice *choice =
                new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                             mask.GetStringValues(), 0, wxDefaultValidator, mask.Name);

            // set the configuration word
            if (m_pic.bitsPerWord()==16)
            {
                if (word.Offset < hex->getMemory(TYPE_CONFIG).size())
                {
                    ConfigWord = hex->getMemory(TYPE_CONFIG)[word.Offset];
                }
            }
            else        // 14 or 24 bit PIC
            {
                if (word.Offset + 1 < hex->getMemory(TYPE_CONFIG).size())
                {
                    ConfigWord = (hex->getMemory(TYPE_CONFIG)[word.Offset])
                            | (hex->getMemory(TYPE_CONFIG)[word.Offset + 1] << 8);
                }
            }

            choice->SetSelection(0);
            unsigned int ConfigWordMask = mask.GetMask();
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

            pageCtrl.choiceArray.push_back(choice);
        }

        if (word.Masks.size() == 0)
            sz->Add(new wxStaticText(panel, wxID_ANY, ("No available configuration choices for this configuration word.")), 
                    0, wxALL|wxALIGN_CENTER, 5);

        
        wxBoxSizer* boxSz = new wxBoxSizer(wxHORIZONTAL);
        boxSz->AddStretchSpacer(1);

        // add the text control which contains the current hex value
        // for the i-th configuration word

        boxSz->Add(new wxStaticText(panel, wxID_ANY, _("Configuration Word:")),
                   0, wxLEFT|wxRIGHT|wxALIGN_CENTER, 5);

        // NOTE: we use %04X because even for 8 bit devices the configuration words
        //       are typically more than 8 bits wide (they usually are in the 14-16 bits range)
        wxTextCtrl* tc;
		if(m_pic.bitsPerWord()==16)
		{
			tc=new wxTextCtrl(panel, wxID_ANY, wxString::Format(("%02X"), ConfigWord));	
			tc->SetMaxLength(wxString::Format(("%02X"), (unsigned int)word.GetMask()).size());
		}
		else
		{
			tc=new wxTextCtrl(panel, wxID_ANY, wxString::Format(("%04X"), ConfigWord));
			tc->SetMaxLength(wxString::Format(("%04X"), (unsigned int)word.GetMask()).size());
		}
        tc->SetToolTip(
            wxString::Format(_("Current value for the %d-th configuration word as derived from above configuration choices"), i));

        tc->Connect(wxEVT_COMMAND_TEXT_UPDATED, 
                    wxCommandEventHandler(UppConfigViewBook::OnConfigWordDirectChange), 
                    NULL, this);

        boxSz->Add(tc, 0, wxRIGHT|wxALIGN_CENTER|wxEXPAND, 5);

        pageCtrl.textCtrl = tc;
        
        
        
        // add a few more details about the location of current config word in PIC memory

        boxSz->AddStretchSpacer(1);
        boxSz->Add(new wxStaticText(panel, wxID_ANY, wxString::Format(_("Location in memory: 0x%X"), m_pic.ConfigAddress +(int) word.Offset)),
                0, wxLEFT|wxALIGN_CENTER, 5);
                
        boxSz->AddStretchSpacer(1);
        boxSz->Add(new wxStaticText(panel, wxID_ANY, wxString::Format(_("Length: %d bits"), word.GetBitSize())),
                0, wxLEFT|wxALIGN_CENTER, 5);
        boxSz->AddStretchSpacer(1);


        // complete the creation of the panel:
        
        wxBoxSizer* panelSz = new wxBoxSizer(wxVERTICAL);
        panelSz->Add(sz, 0, wxEXPAND);
        panelSz->Add(new wxStaticLine(panel, wxID_ANY), 0, wxEXPAND|wxALL, 10);
        panelSz->Add(boxSz, 0, wxEXPAND|wxALL, 5);
        
        sz->AddGrowableCol(1, 1);
        panel->SetSizerAndFit(panelSz);


        // complete the creation of this page:

        AddPage(panel, word.Name.empty() ? wxString::Format(_("Word %d"), i) : word.Name);
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
    if (m_pic.bitsPerWord()==16)
    {
        if ((SelectedWord+1) <= m_hexFile->getMemory(TYPE_CONFIG).size())
            ConfigWord = m_hexFile->getMemory(TYPE_CONFIG)[SelectedWord];
    }
    else    // 14 bit pic and 24 bit pic
    {
        if ((2*SelectedWord+1) <= m_hexFile->getMemory(TYPE_CONFIG).size())
            ConfigWord = (m_hexFile->getMemory(TYPE_CONFIG)[SelectedWord*2])|
                         (m_hexFile->getMemory(TYPE_CONFIG)[SelectedWord*2+1]<<8);
    }
	// combine the current value with the new value selected by the user
    // through the wxChoice
    ConfigWord &= ~mask->GetMask();
	
    ConfigWord |= mask->Values[choice->GetSelection()].Value;
	
	
	
    // set the new value in the textctrl for the selected configuration word:
    // NOTE: we use %04X because even for 8 bit devices the configuration words
    //       are typically more than 8 bits wide (they usually are in the 14-16 bits range)
    if(m_pic.bitsPerWord()==16)m_ctrl[SelectedWord].textCtrl->ChangeValue(wxString::Format(("%02X"), ConfigWord));
	else m_ctrl[SelectedWord].textCtrl->ChangeValue(wxString::Format(("%04X"), ConfigWord));
    // save the new value back in the HEX file, too:
    if (m_pic.bitsPerWord()==16)
    {
        m_hexFile->putMemory(TYPE_CONFIG, SelectedWord, ConfigWord&0xFF, &m_pic);
    }
    else //for 14 bit and 24 bit, put 16 bits in the config memory per word
    {
        m_hexFile->putMemory(TYPE_CONFIG, SelectedWord*2, ConfigWord&0xFF, &m_pic);
        m_hexFile->putMemory(TYPE_CONFIG, SelectedWord*2+1, (ConfigWord&0xFF00)>>8, &m_pic);
    }

    // notify the main window about this change
    UppMainWindow* main = dynamic_cast<UppMainWindow*>(wxTheApp->GetTopWindow());
    wxASSERT(main);
    main->upp_hex_changed();
}

void UppConfigViewBook::OnConfigWordDirectChange(wxCommandEvent& event)
{
    unsigned int SelectedWord = GetSelection();
    unsigned long ConfigWordInt = 0; 
    wxString configStr = m_ctrl[SelectedWord].textCtrl->GetValue();
    if (configStr.empty())
        return;     // do not change anything

    if (!configStr.ToULong(&ConfigWordInt, 16))
    {
        // the user has written something invalid (i.e. not convertible to a number)
        // inside the text ctrl; in this case restore the default value for the affected
        // configuration word reading it from the HEX file config memory area

        if (m_pic.bitsPerWord()==16)
        {
            if ((unsigned)(SelectedWord+1) <= m_hexFile->getMemory(TYPE_CONFIG).size())
            {
                ConfigWordInt = m_hexFile->getMemory(TYPE_CONFIG)[SelectedWord];
            }
        }
        else        // 14 bit and 24 bit
        {
            if ((unsigned)(2*SelectedWord+1) <= m_hexFile->getMemory(TYPE_CONFIG).size())
            {
                ConfigWordInt = ((m_hexFile->getMemory(TYPE_CONFIG)[SelectedWord*2])|
                                (m_hexFile->getMemory(TYPE_CONFIG)[SelectedWord*2+1]<<8));
            }
        }

        // NOTE: we use %04X because even for 8 bit devices the configuration words
        //       are typically more than 8 bits wide (they usually are in the 14-16 bits range)
		if(m_pic.bitsPerWord()==16)m_ctrl[SelectedWord].textCtrl->ChangeValue(wxString::Format(("%02X"), (unsigned int)ConfigWordInt));
		else m_ctrl[SelectedWord].textCtrl->ChangeValue(wxString::Format(("%04X"), (unsigned int)ConfigWordInt));
    }
    else
    {
        // the user entered a valid integer; put the value into the hexfile
        if (m_pic.bitsPerWord()==16)
        {
            ConfigWordInt &= 0xFF;
            m_hexFile->putMemory(TYPE_CONFIG, SelectedWord, ConfigWordInt&0xFF, &m_pic);
        }
        else
        {
            if(m_pic.bitsPerWord()==14) ConfigWordInt &= 0x3FFF; // for 14 bit devices, we need a mask
            m_hexFile->putMemory(TYPE_CONFIG, SelectedWord*2, ConfigWordInt&0xFF, &m_pic);
            m_hexFile->putMemory(TYPE_CONFIG, SelectedWord*2+1, (ConfigWordInt&0xFF00)>>8, &m_pic);
        }

        // notify the main window about this change
        UppMainWindow* main = dynamic_cast<UppMainWindow*>(wxTheApp->GetTopWindow());
        wxASSERT(main);
        main->upp_hex_changed();

        // now update the wxChoices for the selected configuration word

        const ConfigWord& word = m_pic.ConfigWords[SelectedWord];
        for (unsigned int i=0; i<word.Masks.size(); i++)
        {
            const ConfigMask& mask = word.Masks[i];
            const vector<wxChoice*>& choices = m_ctrl[SelectedWord].choiceArray;

            // find the wxChoice for the i-th configuration mask:
            wxChoice *choice=NULL;
            for (unsigned int j=0; j<choices.size(); j++)
            {
                if (mask.Name == choices[j]->GetName())
                {
                    // we've found a wxChoice
                    choice = choices[j];
                    break;
                }
            }

            wxASSERT(choice);

            // now update the selection for this wxChoice
            choice->SetSelection(0);
            unsigned int ConfigWordMask = mask.GetMask();
            for (unsigned int k=0; k<mask.Values.size(); k++)
                if ((ConfigWordInt&ConfigWordMask) == mask.Values[k].Value)
                    choice->SetSelection(k);
        }
    }
}
