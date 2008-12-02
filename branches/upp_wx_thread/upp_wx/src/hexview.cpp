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


#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/menu.h>
#include <wx/log.h>

#include "hexview.h"


#define ROWLABELWIDTH       120
#define OTHERWIDTH          52
#define COLWIDTH            28

// array used for fast decimal=>hex conversion in ShowHexFile()
static wxStringCharType g_digits[] =
    { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };


UppHexViewGrid::UppHexViewGrid(wxWindow* parent, wxWindowID id, UppHexViewType type)
    : wxGrid( parent, id, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS )
{
    m_hexFile=NULL;
    m_type=type;

    // NOTE: in wx2.9 we can't allocate a grid of zero rows... at least one
    //       row is always _required_

    CreateGrid( 1, 24 );
    EnableEditing( true );
    EnableGridLines( false );
    EnableDragGridSize( false );
    SetSelectionMode(wxGrid::wxGridSelectRows);
    SetMargins( 0, 0 );

    // Columns
    //EnableDragColMove( false );
    EnableDragColSize( false );
    SetColLabelSize( 30 );
    SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
    for(int i=0;i<GetNumberCols();i++)
        SetColLabelValue(i,wxString::Format(wxT("%02X"),i));
    AutoSizeColumns(true);

    // Rows
    EnableDragRowSize( false );
    SetRowLabelSize( ROWLABELWIDTH );
    SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );

    // since this widget has zero rows, at least initially, we need to explicitely
    // set a reasonable minimum size...
    SetMinClientSize(wxSize(-1,300));

    // Connect Events
    Connect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( UppHexViewGrid::OnCellChanged ), NULL, this );
    Connect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( UppHexViewGrid::OnCellRightClicked ), NULL, this );
    Connect( wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppHexViewGrid::OnCopy ) );
    Connect( wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppHexViewGrid::OnSelectAll ) );
}

UppHexViewGrid::~UppHexViewGrid()
{
    // Disconnect Events
    Disconnect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( UppHexViewGrid::OnCellChanged ), NULL, this );
    Disconnect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( UppHexViewGrid::OnCellRightClicked ), NULL, this );
    Disconnect( wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppHexViewGrid::OnCopy ) );
    Disconnect( wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UppHexViewGrid::OnSelectAll ) );
}

void UppHexViewGrid::ShowHexFile(HexFile* hexFile, PicType* picType)
{
    m_hexFile=hexFile;

    const vector<int>* data = NULL;
    switch (m_type)
    {
        case HEXVIEW_CODE:
            data = &m_hexFile->getCodeMemory();
            break;
        case HEXVIEW_CONFIG:
            data = &m_hexFile->getConfigMemory();
            break;
        case HEXVIEW_DATA:
            data = &m_hexFile->getDataMemory();
            break;
    }

    // clean current contents
    if (GetNumberRows() > 0)
        DeleteRows(0, GetNumberRows(), false);

    int n = GetNumberCols();
    if (n == 0) n = 1;      // avoid divisions by zero

    // append new rows and set grid contents
    AppendRows(((data->size()%n)>0)+data->size()/n, false);

    wxStringCharType value[3];
    value[2] = (wxStringCharType)'\0';

    for(unsigned int i=0;i<data->size();i++)
    {
        int number = (*data)[i];
        wxASSERT(number >= 0 && number <= 255);
#if 1
        // code for fast decimal=>hex conversion
        value[1] = g_digits[number%16];
        number = number/16;
        value[0] = g_digits[number%16];
#else
        // standard code for decimal=>hex conversion (slower)
        snprintf(value, 3, "%02X", number);// = wxString::Format(wxT("%02X"), number);
#endif

        // NOTE: we don't use directly wxGrid::SetTable() since it's slower
        //       than GetTable()->SetValue because it also cares about
        //       marking as invalid the rect of the cell we have modified.
        //       Since we're doing lots of changes, and we need to do them quickly,
        //       we bypass wxGrid and later call Refresh() on the entire window.
        GetTable()->SetValue(i/n, i%n, wxString(value));
    }

    int offset = 0;
    if (m_type == HEXVIEW_CONFIG)
        offset = picType->getCurrentPic().ConfigAddress;

    // set column and row labels now
    for(int i=0;i<GetNumberRows();i++)
        SetRowLabelValue(i,wxString::Format(wxT("%06X"),offset + i*n));

    Refresh();
}

void UppHexViewGrid::Copy()
{
    wxString datastr;
    int cnt=GetSelectionBlockTopLeft().GetCount();
    for(int i=0;i<cnt;i++)
    {
        int topLeftRow=GetSelectionBlockTopLeft().Item(i).GetRow();
        int bottomRightRow=GetSelectionBlockBottomRight().Item(i).GetRow();
        int topLeftCol=GetSelectionBlockTopLeft().Item(i).GetCol();
        int bottomRightCol=GetSelectionBlockBottomRight().Item(i).GetCol();

        for(int r=topLeftRow;r<=bottomRightRow;r++)
        {
            datastr.Append(GetRowLabelValue(r));
            datastr.Append(wxT(": "));
            for(int c=topLeftCol;c<=bottomRightCol;c++)
            {
                datastr.Append(GetCellValue(r,c));
                datastr.Append(wxT(" "));
            }
            datastr.Append(wxT("\n"));
        }
    }

    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData( new wxTextDataObject(datastr) );
        wxTheClipboard->Close();
    }
}

void UppHexViewGrid::OnCopy (wxCommandEvent& event)
{
    Copy();
}

void UppHexViewGrid::OnSelectAll (wxCommandEvent& event)
{
    SelectAll();
}

void UppHexViewGrid::OnCellRightClicked (wxGridEvent& event)
{
    wxMenu oMenu(_("Edit"));
    wxPoint oPos;
    oMenu.Append(wxID_COPY, _("Copy"));
    oMenu.Append(wxID_SELECTALL, _("Select All"));
    oPos=ScreenToClient(wxGetMousePosition());
    PopupMenu(&oMenu, oPos.x, oPos.y);
}

void UppHexViewGrid::OnCellChanged (wxGridEvent& event )
{
    int Position=event.GetCol()+(event.GetRow()*GetNumberCols());

    int Data;
    wxString CellData=GetCellValue(event.GetRow(),event.GetCol());
    sscanf(CellData.mb_str(wxConvUTF8),"%X",&Data);

    if(m_hexFile && (Data>=0) && (Data<=0xFF))
    {
        switch (m_type)
        {
            case HEXVIEW_CODE:
                m_hexFile->putCodeMemory(Position,Data);
                CellData.Printf(wxT("%02X"),m_hexFile->getCodeMemory(Position));
                break;
            case HEXVIEW_CONFIG:
                m_hexFile->putConfigMemory(Position,Data);
                CellData.Printf(wxT("%02X"),m_hexFile->getConfigMemory(Position));
                break;
            case HEXVIEW_DATA:
                m_hexFile->putDataMemory(Position,Data);
                CellData.Printf(wxT("%02X"),m_hexFile->getDataMemory(Position));
                break;
        }

        // inform the parent that something was modified
        GetParent()->ProcessEvent(event);
    }

    SetCellValue(event.GetRow(),event.GetCol(),CellData);
}
