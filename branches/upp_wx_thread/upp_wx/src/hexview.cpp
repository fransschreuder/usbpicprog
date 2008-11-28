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


#include <wx/wx.h>
#include "hexview.h"


#define ROWLABELWIDTH       120
#define OTHERWIDTH          52
#define COLWIDTH            28


UppHexViewGrid::UppHexViewGrid(wxWindow* parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size,
                               long style)
    : wxGrid( parent, id, pos, size, style )
{
    readHexFile=NULL;

    CreateGrid( 0, 24 );
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
    SetMinSize(wxSize(-1,300));
    Fit();

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

void UppHexViewGrid::ShowHexFile(ReadHexFile* hexFile, vector<int>& data, PicType* picType)
{
    readHexFile=hexFile;

    // clean current contents
    if (GetNumberRows() > 0)
        DeleteRows(0, GetNumberRows(), false);

    // append new rows and set grid contents
    AppendRows(((data.size()%GetNumberCols())>0)+data.size()/GetNumberCols(), false);
    for(unsigned int i=0;i<data.size();i++)
    {
        SetCellValue(i/(GetNumberCols()), i%(GetNumberCols()),
                     wxString::Format(wxT("%02X"), data[i]));
    }

    // set column and row labels now
    for(int i=0;i<GetNumberRows();i++)
        SetRowLabelValue(i,wxString::Format(wxT("%06X"),i*GetNumberCols()));

/*   setLabels(picType->getCurrentPic().ConfigAddress);*/
    Fit();
}
/*
void UppHexViewGrid::setLabels(int configOffset)
{
    for(int i=0;i<configGrid->GetNumberRows();i++)
        configGrid->SetRowLabelValue(i,wxString::Format(wxT("%06X"),configOffset+i*configGrid->GetNumberCols()));
    for(int i=0;i<configGrid->GetNumberCols();i++)
        configGrid->SetColLabelValue(i,wxString::Format(wxT("%02X"),i));
}

/*
void UppHexViewGrid::autoSizeColumns(void)
{
    /*;
    dataGrid->AutoSizeColumns(true);
    configGrid->AutoSizeColumns(true);*
    for(int i=0;i<GetNumberCols();i++)
    {
        SetColSize(i, COLWIDTH);
        configGrid->SetColSize(i, COLWIDTH);
        dataGrid->SetColSize(i,COLWIDTH);
    }
}*/

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
    if((Data>=0)&&(Data<=0xFF))
    {
        // FIXME
        readHexFile->putCodeMemory(Position,Data);
    }

    CellData.Printf(wxT("%02X"),readHexFile->getCodeMemory(Position));
    SetCellValue(event.GetRow(),event.GetCol(),CellData);
}
/*
void UppHexViewGrid::OnConfigChanged (wxGridEvent& event )
{
    int Position=event.GetCol()+(event.GetRow()*configGrid->GetNumberCols());

    int Data;
    wxString CellData=configGrid->GetCellValue(event.GetRow(),event.GetCol());
    sscanf(CellData.mb_str(wxConvUTF8),"%X",&Data);
    if((Data>=0)&&(Data<=0xFF))
    {
        readHexFile->putConfigMemory(Position,Data);
    }

    CellData.Printf(wxT("%02X"),readHexFile->getConfigMemory(Position));
    configGrid->SetCellValue(event.GetRow(),event.GetCol(),CellData);
}

void UppHexViewGrid::OnDataChanged (wxGridEvent& event )
{
    int Position=event.GetCol()+(event.GetRow()*dataGrid->GetNumberCols());

    int Data;
    wxString CellData=dataGrid->GetCellValue(event.GetRow(),event.GetCol());
    sscanf(CellData.mb_str(wxConvUTF8),"%X",&Data);
    if((Data>=0)&&(Data<=0xFF))
    {
        readHexFile->putDataMemory(Position,Data);
    }

    CellData.Printf(wxT("%02X"),readHexFile->getDataMemory(Position));
    dataGrid->SetCellValue(event.GetRow(),event.GetCol(),CellData);
}
*/
