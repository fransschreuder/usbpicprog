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

#ifndef HEXVIEW_H
#define HEXVIEW_H

#include <wx/grid.h>

#include "hexfile.h"
#include "pictype.h"

#include <iostream>
using namespace std;

/**
    Used with UppHexViewGrid to specify the code area of the PIC to show.
*/
enum UppHexViewType
{
    HEXVIEW_CODE,
    HEXVIEW_DATA
};

/**
    A specialized grid which shows a portion of memory of a PIC.
*/
class UppHexViewGrid : public wxGrid
{
public:
    UppHexViewGrid(wxWindow* parent, wxWindowID id,
                   UppHexViewType type);
    ~UppHexViewGrid();

    /**
        Copies the selected block of this grid to the clipboard.
    */
    void Copy();

    /**
        Sets the HexFile instance to update upon user changes on the grid.

        This function resets the current contents of the grid using the
        data stored in the HexFile.

        Note that the @a hexFile pointer is stored inside this class
        thus the caller must ensure the validity of the HexFile object 
        for the entire lifetime of this window.
    */
    void SetHexFile(HexFile* hexFile);

private:
    HexFile* m_hexFile;
    UppHexViewType m_type;

protected:
    void OnCopy (wxCommandEvent& event);
    void OnSelectAll (wxCommandEvent& event);
    void OnCellRightClicked (wxGridEvent& event );
    void OnCellChanged( wxGridEvent& event );

    wxSize DoGetBestSize() const;
};

#endif //HEXVIEW_H
