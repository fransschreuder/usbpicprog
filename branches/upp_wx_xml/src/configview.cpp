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

#include "configview.h"
#include "uppmainwindow.h"


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
    for (unsigned int i=0; i<m_pic.Config.size(); i++)
    {
        const ConfigBlock& block = m_pic.Config[i];
        if (block.Masks.size() == 0)
            continue;       // skip this block

        wxPanel *panel = new wxPanel(this, wxID_ANY);
        wxFlexGridSizer *sz = new wxFlexGridSizer(2 /* num of columns */, 
                                                  10, 10 /* h and vgap */);

        for (unsigned int j=0; j<block.Masks.size(); j++)
        {
            const ConfigMask& mask = block.Masks[j];

            sz->Add(new wxStaticText(panel, wxID_ANY, mask.Name + ":"),
                    0, wxLEFT|wxALIGN_CENTER, 5);

            // NOTE: we give each wxChoice we build the name of the mask it controls;
            //       in this way from OnChange() we may easily find out which object
            //       is sending the notification
            wxChoice *choice = 
                new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
                             mask.GetStringValues(), 0, wxDefaultValidator, mask.Name);

            // TODO: put the config values from HexFile to this ctrl
            choice->SetSelection(0);
            choice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, 
                            wxCommandEventHandler(UppConfigViewBook::OnChange), 
                            NULL, this);

            sz->Add(choice, 0, wxRIGHT|wxALIGN_CENTER|wxEXPAND, 5);
        }

        sz->AddGrowableCol(1, 1);
        panel->SetSizerAndFit(sz);

        AddPage(panel, block.Name.empty() ? wxString::Format("Word %d", i) : block.Name);
    }
}

void UppConfigViewBook::OnChange(wxCommandEvent& event)
{
    wxChoice *choice = dynamic_cast<wxChoice*>(event.GetEventObject());
    wxASSERT(choice);

    // get the block the user is currently viewing/editing
    const ConfigBlock& block = m_pic.Config[GetSelection()];

    // find the config mask which was changed
    const ConfigMask* mask = NULL;
    for (unsigned int i=0; i<block.Masks.size(); i++)
    {
        if (block.Masks[i].Name == choice->GetName())
        {
            mask = &block.Masks[i];
            break;
        }
    }

    wxASSERT(mask);
    int newConfigValue = mask->Values[choice->GetSelection()].Value;

    // TODO: we need to update the HEX file
	cout<<"newConfigValue: "<<hex<<newConfigValue<<endl;
    // notify the main window about this change
    UppMainWindow* main = dynamic_cast<UppMainWindow*>(wxTheApp->GetTopWindow());
    wxASSERT(main);
    main->upp_hex_changed();
}