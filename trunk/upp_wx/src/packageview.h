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

#ifndef PACKAGEVIEW_H
#define PACKAGEVIEW_H

#include <wx/scrolwin.h>
#include "pictype.h"

/**
    A simple window showing the package of a PIC.
*/
class UppPackageViewWindow : public wxScrolledCanvas
{
public:
    UppPackageViewWindow(wxWindow* parent, wxWindowID id);

    /**
        Update the chip package to show in this window.
        
        This function fits the package drawing and refreshes this window
        calling FitBitmap().
    */
    void SetChip(const wxString& name, const ChipPackage& pkg)
        { m_name=name; m_pkg=pkg; FitBitmap(); }

    /**
        Updates the m_bmp member so that it fits this window.
        Automatically calls Refresh() after the update.
    */
    void FitBitmap()
        { UpdateBitmap(GetClientSize()); m_fitting=true; }

public:     // event handlers
    void OnZoomIn(wxCommandEvent& event);
    void OnZoomOut(wxCommandEvent& event);
    void OnZoomFit(wxCommandEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);

protected:
    wxSize DoGetBestSize() const;
    
    /**
        Updates the m_bmp member with the package drawing.
        Automatically calls Refresh() after the update.
    */
    void UpdateBitmap(const wxSize& sz);
    
    /** The name of the PIC device currently shown. */
    wxString m_name;
    
    /** The instance of the class holding all info about the device's package. */
    ChipPackage m_pkg;
    
    /** The cached bitmap with the package drawing. */
    wxBitmap m_bmp;
    
    /** Are we currently in the "fit the bitmap in the window" mode? */
    bool m_fitting;
};

#endif //PACKAGEVIEW_H
