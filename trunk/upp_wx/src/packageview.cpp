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

#define MAX_SIZE        5000
#define MIN_SIZE        50

// ----------------------------------------------------------------------------
// UppPackageViewWindow
// ----------------------------------------------------------------------------

UppPackageViewWindow::UppPackageViewWindow(wxWindow* parent, wxWindowID id)
    : wxScrolledCanvas( parent, id, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL|wxFULL_REPAINT_ON_RESIZE, _("packageview") )
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
        wxLogError(_("Can't create the package bitmap!"));
        return;
    }
    wxMemoryDC dc(m_bmp);
    if (!dc.IsOk())
    {
        wxLogError(_("Can't draw the PIC package!"));
        return;
    }

    // clear the bitmap
    dc.SetBackground(*wxWHITE);
    dc.SetBrush(*wxWHITE);
    dc.Clear();
    
    // draw the package on the bitmap
    m_pkg.Draw(dc, sz, m_name);
    dc.SelectObject(wxNullBitmap);
    
    {
        // NOTE: on wxMSW setting the virtual size triggers a size event
        //       and thus our OnSize() would be called with an outdated
        //       m_fitting variable; to avoid this we temporarily block
        //       event generation
        wxEventBlocker nul(this);
        SetVirtualSize(sz);
    }
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
	wxSize dcSize = dc.GetSize();
	wxSize bmpSize = m_bmp.GetSize();
	
    dc.DrawBitmap(m_bmp, (dcSize.GetWidth() - bmpSize.GetWidth())/2, (dcSize.GetHeight() - bmpSize.GetHeight())/2);
}

void UppPackageViewWindow::OnSize(wxSizeEvent& WXUNUSED(event))
{
    if (m_fitting)
        FitBitmap();
    //else: don't bypass the user's custom zoom on the package by calling FitBitmap()
}

void UppPackageViewWindow::OnZoomIn(wxCommandEvent& WXUNUSED(event))
{
    if (max(m_bmp.GetWidth(), m_bmp.GetHeight()) >= MAX_SIZE)
        return;     // zooming into the bitmap consumes memory; without this limit
                    // the app would crash at some point (depending on the user's system)
                    // because of some malloc() call returning NULL...
    UpdateBitmap(m_bmp.GetSize()*1.5);
	m_fitting = m_bmp.GetSize() == GetClientSize();
}

void UppPackageViewWindow::OnZoomOut(wxCommandEvent& WXUNUSED(event))
{
    if (min(m_bmp.GetWidth(), m_bmp.GetHeight()) <= MIN_SIZE)
        return;     // zooming out of the bitmap makes the bitmap smaller and smaller;
                    // when reaching an invalid size (width/height == 0) an assert
                    // would fail in UpdateBitmap(); avoid it
    UpdateBitmap(m_bmp.GetSize()*(1.0/1.5));
	m_fitting = m_bmp.GetSize() == GetClientSize();	
}

void UppPackageViewWindow::OnZoomFit(wxCommandEvent& WXUNUSED(event))
{
    FitBitmap();
}
