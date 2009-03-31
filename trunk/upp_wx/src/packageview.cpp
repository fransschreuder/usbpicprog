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

#include "packageview.h"


UppPackageViewWindow::UppPackageViewWindow(wxWindow* parent, wxWindowID id)
    : wxScrolledWindow( parent, id, wxDefaultPosition, wxDefaultSize, 
                        wxVSCROLL|wxFULL_REPAINT_ON_RESIZE, "packageview" )
{
    SetBackgroundStyle(wxBG_STYLE_COLOUR);
    SetBackgroundColour(*wxWHITE);
    
    Connect(wxEVT_PAINT, wxPaintEventHandler(UppPackageViewWindow::OnPaint), NULL, this);
}

wxSize UppPackageViewWindow::DoGetBestSize() const
{
    return wxSize(100,150);
}

void UppPackageViewWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    
    // center the bitmap in the window
    wxSize sz = GetClientSize();

    if (m_bmp.IsOk())
        dc.DrawBitmap(m_bmp, (sz.GetWidth()-m_bmp.GetWidth())/2, (sz.GetHeight()-m_bmp.GetHeight())/2);
}

void UppPackageViewWindow::Refresh()
{
    wxSize sz = GetClientSize();

    // initialize the bitmap
    if (!m_bmp.Create(sz.GetWidth(), sz.GetHeight()))
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

    // set some GUI objects common to all packages-drawing code
    dc.SetFont(*wxSMALL_FONT);
    dc.SetPen(*wxBLACK_PEN);

    switch (m_pkg.Type)
    {
    case PDIP:
    case SOIC:
    case SSOP:
        {
            // some drawing constants:

            const unsigned int PinPerSide = m_pkg.GetPinCount()/2;

            // choose reasonable package width&height to
            // - make best use of the available space
            // - avoid drawing package excessively big
            const unsigned int BoxW = min(sz.GetWidth()/3,80);
            const unsigned int BoxH = min(BoxW*PinPerSide/3,(unsigned int)(sz.GetHeight()*0.8));
            const unsigned int BoxX = (sz.GetWidth()-BoxW)/2;
            const unsigned int BoxY = (sz.GetHeight()-BoxH)/2;
            const unsigned int R = BoxW/6;

            // pin height is calculated imposing that
            // 1) PinPerSide*PinH + (PinPerSide+1)*PinSpacing = BoxH
            // 2) PinSpacing = PinH/2
            // solving for PinH yields:
            const unsigned int PinH = 2*BoxH/(3.0*PinPerSide+1);
            const unsigned int PinSpacing = PinH/2;
            const unsigned int PinW = PinH;

            // the error is caused by rounding:
            const unsigned int PinYOffset =
                (BoxH - (PinPerSide*PinH + (PinPerSide+1)*PinSpacing))/2;

            // select a font suitable for 
            wxFont fnt(wxSize(0,int(PinH*0.8)), wxFONTFAMILY_DEFAULT,
                       wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
            dc.SetFont(fnt);
            const unsigned int PinLabelW = 2*dc.GetCharWidth();
            const unsigned int PinLabelH = dc.GetCharHeight();

            // draw the PIC package box
            dc.DrawRectangle(BoxX, BoxY, BoxW, BoxH);
            dc.DrawArc(sz.GetWidth()/2-R, BoxY + 1,
                       sz.GetWidth()/2+R, BoxY + 1,
                       sz.GetWidth()/2, BoxY + 1);

            // draw the name of the PIC model in the centre of the box
            wxSize nameSz = dc.GetTextExtent(m_name);
            dc.DrawRotatedText(m_name,
                               (sz.GetWidth() + nameSz.GetHeight())/2,
                               (sz.GetHeight() - nameSz.GetWidth())/2,
                               -90);                               

            // draw the pins
            for (unsigned int i=0; i<PinPerSide; i++)
            {
                unsigned int pinY = BoxY + PinYOffset + (i+1)*PinSpacing + i*PinH;
                unsigned int pinLabelY = pinY + (PinH-PinLabelH)/2;

                // pins on the left side
                dc.SetTextForeground(m_pkg.IsICSPPin(i) ? *wxRED : *wxBLACK);
                dc.DrawRectangle(BoxX-PinW, pinY,
                                 PinW+1, PinH);
                dc.DrawText(wxString::Format("%d", i+1), BoxX+PinSpacing, pinLabelY);
                dc.DrawText(m_pkg.PinNames[i], 
                            BoxX-PinW-PinSpacing-dc.GetTextExtent(m_pkg.PinNames[i]).GetWidth(),
                            pinLabelY);

                // pins on the right side
                unsigned int pinIdx = m_pkg.GetPinCount()-i-1;
                dc.SetTextForeground(m_pkg.IsICSPPin(pinIdx) ? *wxRED : *wxBLACK);
                dc.DrawRectangle(BoxX+BoxW-1, pinY,
                                 PinW, PinH);
                dc.DrawText(wxString::Format("%d", pinIdx+1), 
                            BoxX+BoxW-PinLabelW-PinSpacing, pinLabelY);
                dc.DrawText(m_pkg.PinNames[pinIdx], 
                            BoxX+BoxW+PinW+PinSpacing,
                            pinLabelY);
            }
        }
        break;

    case MQFP:
    case TQFP:
    case PLCC:
        // TODO
        break;

    default:
        break;
    }
    
    dc.SelectObject(wxNullBitmap);
    
    wxScrolledWindow::Refresh();
}
