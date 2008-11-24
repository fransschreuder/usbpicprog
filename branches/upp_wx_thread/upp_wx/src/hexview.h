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

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/panel.h>
#include <wx/menu.h>
#include <wx/utils.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include "read_hexfile.h"
#include "pictype.h"
#include <iostream>
using namespace std;


class UppHexview : public wxPanel
{
private:
    void autoSizeColumns (void);
    void setLabels (int configOffset);
    ReadHexFile* readHexFile;

protected:
    wxFlexGridSizer* bSizer2;
    wxBoxSizer* hexViewBoxSizer;
    wxScrolledWindow* hexViewScrolledWindow;
    wxStaticText* labelCode;
    wxGrid* codeGrid;
    wxStaticText* labelConfig;
    wxGrid* configGrid;
    wxStaticText* labelData;
    wxGrid* dataGrid;

    void OnCopy (wxCommandEvent& event);
    void OnSelectAll (wxCommandEvent& event);
    void OnCodeRightClicked (wxGridEvent& event );
    void OnCodeChanged( wxGridEvent& event );
    void OnDataChanged( wxGridEvent& event );
    void OnConfigChanged( wxGridEvent& event );

public:
    UppHexview(wxWindow* parent, wxWindowID id = wxID_ANY, wxString title = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
               long style = wxTAB_TRAVERSAL);
    ~UppHexview();

    void Copy(void);
    void SelectAll(void);
    void putHexFile(ReadHexFile* hexFile, PicType* picType);
};

#endif //__hexview__
