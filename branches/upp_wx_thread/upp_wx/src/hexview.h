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

#ifndef __hexview__
#define __hexview__

#include <wx/grid.h>

#include "hexfile.h"
#include "pictype.h"

#include <iostream>
using namespace std;


class UppHexViewGrid : public wxGrid
{
public:
    UppHexViewGrid(wxWindow* parent, wxWindowID id = wxID_ANY,
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                   long style = wxWANTS_CHARS);
    ~UppHexViewGrid();

    void Copy();
    void ShowHexFile(HexFile* hexFile, vector<int>& data, PicType* picType);

private:
/*
    void autoSizeColumns (void);
    void setLabels (int configOffset);*/
    HexFile* hexFile;

protected:
    void OnCopy (wxCommandEvent& event);
    void OnSelectAll (wxCommandEvent& event);
    void OnCellRightClicked (wxGridEvent& event );
    void OnCellChanged( wxGridEvent& event );
};

#endif //__hexview__
