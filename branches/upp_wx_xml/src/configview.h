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

#ifndef CONFIGVIEW_H
#define CONFIGVIEW_H

#include <wx/listbook.h>

#include "pictype.h"

/**
    A specialized listbook which shows the configuration flags of a PIC.
*/
class ConfigViewBook : public wxListbook
{
public:
    ConfigViewBook(wxWindow* parent, wxWindowID id);
    ~ConfigViewBook();

    void SetPIC(const Pic& pic);

private:

protected:
    void OnCopy (wxCommandEvent& event);

    //wxSize DoGetBestSize() const;
};

#endif //CONFIGVIEW_H
