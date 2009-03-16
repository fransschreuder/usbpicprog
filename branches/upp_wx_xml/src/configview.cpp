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


#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/choice.h>

#include "configview.h"


ConfigViewBook::ConfigViewBook(wxWindow* parent, wxWindowID id)
    : wxListbook( parent, id, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT )
{
/*
    // Connect Events
    Connect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( UppHexViewGrid::OnCellChanged ), NULL, this );
    Connect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( UppHexViewGrid::OnCellRightClicked ), NULL, this );
    Connect( wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppHexViewGrid::OnCopy ) );
    Connect( wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppHexViewGrid::OnSelectAll ) );*/
}

ConfigViewBook::~ConfigViewBook()
{/*
    // Disconnect Events
    Disconnect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( UppHexViewGrid::OnCellChanged ), NULL, this );
    Disconnect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( UppHexViewGrid::OnCellRightClicked ), NULL, this );
    Disconnect( wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppHexViewGrid::OnCopy ) );
    Disconnect( wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppHexViewGrid::OnSelectAll ) );*/
}
/*
wxSize ConfigViewBook::DoGetBestSize() const
{
    wxSize sz = wxGrid::DoGetBestSize();

    // we need to do this to avoid the presence of a small annoying horizontal scrollbar:
    sz.x += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X) +
            10;     // small additional border

    // we need this because the minimal height is calculated after construction
    // when we have only a single row in the widget (since ShowHexFile has not
    // been called yet); this sets a reasonable value.
    sz.y = 400;

    return sz;
}*/

void ConfigViewBook::SetPIC(const Pic& pic)
{
    DeleteAllPages();

    for (unsigned int i=0; i<pic.Config.size(); i++)
    {
        const ConfigBlock& block = pic.Config[i];
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

            wxChoice *choice = 
                new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, mask.GetStringValues());
            choice->SetSelection(0);

            // connect to choice selected event?

            sz->Add(choice, 0, wxRIGHT|wxALIGN_CENTER|wxEXPAND, 5);
        }

        sz->AddGrowableCol(1, 1);
        panel->SetSizerAndFit(sz);

        AddPage(panel, block.Name.empty() ? wxString::Format("Word %d", i) : block.Name);
    }
}

