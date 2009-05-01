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

#include <wx/clipbrd.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/log.h>
#include <wx/bmpbuttn.h>
#include <wx/artprov.h>

#include "packageview.h"


// ----------------------------------------------------------------------------
// UppPackageViewWindow
// ----------------------------------------------------------------------------

UppPackageViewWindow::UppPackageViewWindow(wxWindow* parent, wxWindowID id)
    : wxScrolledCanvas( parent, id, wxDefaultPosition, wxDefaultSize, 
                        wxHSCROLL|wxVSCROLL|wxFULL_REPAINT_ON_RESIZE, "packageview" )
{
    //SetBackgroundStyle(wxBG_STYLE_COLOUR);
    SetBackgroundColour(*wxWHITE);
    SetScrollRate(20,20);

    Connect(wxEVT_PAINT, wxPaintEventHandler(UppPackageViewWindow::OnPaint), NULL, this);
    Connect(wxEVT_SIZE, wxSizeEventHandler(UppPackageViewWindow::OnSize), NULL, this);
    
    m_fitting = false;
}

wxSize UppPackageViewWindow::DoGetBestSize() const
{
    return wxSize(100,150);
}

void UppPackageViewWindow::UpdateBitmap(const wxSize& sz)
{
    // initialize the bitmap
    if (!m_bmp.Create(sz))
    {
        wxLogError("Can't create the package bitmap!");
        return;
    }

    wxMemoryDC dc(m_bmp);
    if (!dc.IsOk())
    {
        wxLogError("Can't draw the PIC package!");
        return;
    }

    // clear the bitmap
    dc.SetBackground(*wxWHITE);
    dc.SetBrush(*wxWHITE);
    dc.Clear();
    
    // draw the package on the bitmap
    m_pkg.Draw(dc, sz, m_name);
    dc.SelectObject(wxNullBitmap);
    
    SetVirtualSize(sz);
    AdjustScrollbars();
    Refresh();
}


// ----------------------------------------------------------------------------
// UppPackageViewWindow - event handlers
// ----------------------------------------------------------------------------

void UppPackageViewWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    DoPrepareDC(dc);

    // should always be valid:
    wxASSERT(m_bmp.IsOk());

    dc.DrawBitmap(m_bmp, 0, 0);
}

void UppPackageViewWindow::OnSize(wxSizeEvent& WXUNUSED(event))
{
    if (m_fitting)
        FitBitmap();
    //else: don't bypass the user's custom zoom on the package by calling FitBitmap()
}

void UppPackageViewWindow::OnZoomIn(wxCommandEvent& WXUNUSED(event))
{
    UpdateBitmap(GetVirtualSize()*1.5);
    
    m_fitting = m_bmp.GetSize() == GetClientSize();
}

void UppPackageViewWindow::OnZoomOut(wxCommandEvent& WXUNUSED(event))
{
    UpdateBitmap(GetVirtualSize()*(1.0/1.5));
    
    m_fitting = m_bmp.GetSize() == GetClientSize();
}

void UppPackageViewWindow::OnZoomFit(wxCommandEvent& WXUNUSED(event))
{
    FitBitmap();
}
