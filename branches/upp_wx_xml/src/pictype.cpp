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

#include <wx/xml/xml.h>
#include <wx/dir.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "pictype.h"


// statics
vector<PicType::PicIndexInfo> PicType::s_arrSupported;
PicType::PicIndexInfo PicType::s_default;
//PackageBitmap ChipPackage::s_arrImage;
//wxBitmap ChipPackage::s_bmpUnknown;


PicType::PicType(string picTypeStr)
{
    for (unsigned int i=0;i<s_arrSupported.size();i++)
    {
        if (picTypeStr.compare("P" + s_arrSupported[i].name)==0)
        {
            // this is a supported PIC!
            if (!LoadPIC(s_arrSupported[i]))
                wxLogError("Could not load data for PIC '%s'.", picTypeStr);

            // we either failed or successfully loaded the PIC data...
            return;
        }
    }
/*
    // if no match was found, try to load the default PIC
    if (!m_currentPic.ok() &&
        !LoadPIC(s_default))
    {
        wxLogError("Could not load data for PIC '%s'.", UPP_DEFAULT_PIC);
        return;
    }*/
}

PicType::PicType(int devId)
{
    for (unsigned int i=0;i<s_arrSupported.size();i++)
    {
        if ((devId & s_arrSupported[i].devIdMask) == s_arrSupported[i].devId)
        {
            // this is a supported PIC!
            if (!LoadPIC(s_arrSupported[i]))
                wxLogError("Could not load data for PIC '%s'.", s_arrSupported[i].name);

            // we either failed or successfully loaded the PIC data...
            return;
        }
    }
/*
    // if no match was found, try to load the default PIC
    if (!m_currentPic.ok() &&
        !LoadPIC(s_default))
    {
        wxLogError("Could not load data for PIC '%s'.", UPP_DEFAULT_PIC);
        return;
    }*/
}

bool PicType::LoadPIC(PicType::PicIndexInfo& indexInfo)
{
    if (indexInfo.pic.ok())
    {
        // this PIC data was already loaded and cached previously;
        // don't reload it again!
        m_currentPic = indexInfo.pic;
    }
    else
    {
        m_currentPic = LoadPiklabXML(indexInfo.name);

        // complete the Piklab data with some UPP-specific data
        // previously loaded from UPP_INDEX_FILE
        m_currentPic.DevIdMask = indexInfo.devIdMask;
        m_currentPic.DevId = indexInfo.devId;
        m_currentPic.picFamily = indexInfo.picFamily;

        // cache the entire structure in our internal static array:
        indexInfo.pic = m_currentPic;
    }

    return m_currentPic.ok();
}

/* static */
const wxArrayString& PicType::getSupportedPicNames()
{
    static wxArrayString picNames;

    if (picNames.size() == 0)
    {
        // cache the array with the names of supported PICs
        picNames.Alloc(s_arrSupported.size());
        for(unsigned int i=0;i<s_arrSupported.size();i++)
            picNames.Add("P" + s_arrSupported[i].name);

        picNames.Sort();
    }

    return picNames;
}

/* static */
bool PicType::Init()
{
    // load the UPP_INDEX_FILE XML file which contains the list of
    // the supported PIC types

    wxXmlDocument idx;
    if (!idx.Load(UPP_INDEX_FILE))
    {
        wxLogError("Cannot load '%s'.", UPP_INDEX_FILE);
        return false;
    }

    if (idx.GetRoot()->GetName() != "upp")
    {
        wxLogError("Invalid '%s' file.", UPP_INDEX_FILE);
        return false;
    }

    wxXmlNode *child = idx.GetRoot()->GetChildren();
    while (child)
    {
        if (child->GetName() == "pic" && 
            child->HasAttribute("name") &&
            child->HasAttribute("upp_family") &&
            child->HasAttribute("upp_devidmask") &&
            child->HasAttribute("upp_devid"))
        {
            PicType::PicIndexInfo info;
            wxString str;
            long num;

            info.name = child->GetAttribute("name");
            info.picFamily = GetFamilyFromString(child->GetAttribute("upp_family"));
            if (info.picFamily == UPP_INVALID_PICFAMILY)
            {
                wxLogError("Invalid PIC family for PIC '%s'.", info.name);
                return false;
            }
            
            child->GetAttribute("upp_devid", &str);
            if (str.ToLong(&num, 0))
                info.devId = num;

            child->GetAttribute("upp_devidmask", &str);
            if (str.ToLong(&num, 0))
                info.devIdMask = num;

            if (info.name == UPP_DEFAULT_PIC_MODEL)
                s_default = info;

            // insert this PicIndexInfo structure into the array of the
            // supported PICs:
            s_arrSupported.push_back(info);
        }

        child = child->GetNext();
    }

    // at the very least we need a valid default PIC type
    if (s_default.name.empty())
    {
        wxLogError("Could not load the data for the PIC '%s'.", UPP_DEFAULT_PIC_MODEL);
        return false;
    }

    return s_arrSupported.size()>0;
}

/* static */
int PicType::GetRange(const wxXmlNode* p)
{
    wxString temp;
    long start = 1, end = 0;
    if (!p->GetAttribute("start", &temp) ||
        !temp.ToLong(&start, 0))
        return -1;

    if (!p->GetAttribute("end", &temp) ||
        !temp.ToLong(&end, 0))
        return -1;

    return end-start;
}

/* static */
Pic PicType::LoadPiklabXML(const wxString& picName)
{
    Pic p;
    wxXmlDocument doc;
    wxString str;
    long num=0;

    wxString prefix("xml_data");
    prefix += wxFileName::GetPathSeparator();

    if (!doc.Load(prefix + picName + ".xml"))
        return UPP_INVALID_PIC;
    if (doc.GetRoot()->GetName() != "device")
        return UPP_INVALID_PIC;

    // load the name of the PIC: it should match picName
    if (!doc.GetRoot()->GetAttribute("name", &str) ||
        str != picName)
        return UPP_INVALID_PIC;
    p.Name = "P" + str;

    // load the device ID of the PIC
    // NOTE: PIC of the 10F family do not have a device ID (no autodetection is possible)
    wxString t = p.Name.substr(0,4);
    if (p.Name.substr(0,4) != "P10F" &&
        (!doc.GetRoot()->GetAttribute("id", &str) ||
        !str.ToLong(&num, 0)))
        return UPP_INVALID_PIC;
    p.DevId = num;

    wxXmlNode *child = doc.GetRoot()->GetChildren();
    while (child)
    {
        if (child->GetName() == "memory")
        {
            wxString name;
            
            if (!child->GetAttribute("name", &name))
                return UPP_INVALID_PIC;

            if (name == "code")
                p.CodeSize = GetRange(child);
            else if (name == "config")
            {
                p.ConfigSize = GetRange(child);
                
                if (!child->GetAttribute("start", &str) ||
                    !str.ToLong(&num, 0))
                    return UPP_INVALID_PIC;
                p.ConfigAddress = num;
            }
            else if (name == "eeprom")
            {
                p.DataSize = GetRange(child);
                
                if (!child->GetAttribute("hexfile_offset", &str) ||
                    !str.ToLong(&num, 0))
                    return UPP_INVALID_PIC;
                p.DataAddress = num;
            }
        }
        else if (child->GetName() == "config")
        {
            //TODO
        }
        else if (child->GetName() == "package")
        {
            wxArrayString types = wxSplit(child->GetAttribute("types"), ' ');
            unsigned long npins;
            if (!child->GetAttribute("nb_pins").ToULong(&npins))
                return UPP_INVALID_PIC;

            wxArrayString names;
            names.Add(wxEmptyString, npins);

            wxXmlNode *pin = child->GetChildren();
            while (pin)
            {
                wxString name = pin->GetAttribute("name");

                unsigned long idx;
                if (pin->GetAttribute("index").ToULong(&idx) &&
                    idx >= 1 && idx <= npins &&
                    !name.IsEmpty())
                    names[idx-1] = name;

                pin = pin->GetNext();
            }

            for (unsigned int i=0; i<types.GetCount(); i++)
            {
                PackageType pt = ChipPackage::GetPackageTypeFromString(types[i]);
                if (pt == UPP_INVALID_PACKAGETYPE)
                    continue;
                
                // init this valid package
                ChipPackage pkg;
                pkg.Type = pt;
                pkg.PinNames = names;
                p.Package.push_back(pkg);
            }
        }

        child = child->GetNext();
    }

    return p;
}

/* static */
PicFamily PicType::GetFamilyFromString(const wxString& str)
{
#define FAMILY(x)           \
    if (str == #x)          \
        return x;

    FAMILY(P18F2XXX);
    FAMILY(P18FXX2);
    FAMILY(P16F87XA);
    FAMILY(P16F62XA);
    FAMILY(P16F62X);
    FAMILY(P12F629);
    FAMILY(P12F6XX);
    FAMILY(P16F84A);
    FAMILY(P16F81X);
    FAMILY(P16F7X);
    FAMILY(P16F7X7);
    FAMILY(P16F87X);
    FAMILY(P16F72);
    FAMILY(P16F87);
    FAMILY(P16F54);
    FAMILY(P16F57);
    FAMILY(P16F785);
    FAMILY(P16F59);
    FAMILY(P16F91X);
    FAMILY(P16F88X);
    FAMILY(P16C6XX);
    FAMILY(P16C55X);
    FAMILY(P16C7XX);
    FAMILY(P16C64X);
    FAMILY(P14000);
    FAMILY(P16C50X);
    FAMILY(P17CXX);
    FAMILY(P17C7XX);
    FAMILY(P18FXX39);
    FAMILY(P18F6X2X);
    FAMILY(P18FXX80);
    FAMILY(P18F8410);
    FAMILY(P18F6XJXX);    
    FAMILY(P18F45J10);    
    FAMILY(P18F97J60);    
    FAMILY(P18F1X30);    
    FAMILY(P18FXX23);    
    FAMILY(P18FXXK20);    
    FAMILY(P24FJXXXGA0XX);    
    FAMILY(P24FJXXXGA1);    
    FAMILY(dsP30F);    
    FAMILY(dsP33F);    
    FAMILY(P24H);    
    FAMILY(P10F200);    
    FAMILY(P10F202);    

    return UPP_INVALID_PICFAMILY;
}

/* static 
wxString PicType::GetStringFromFamily(PicFamily type)
{
#undef FAMILY
#define FAMILY(x)           \
    if (type == x)          \
        return wxString(#x);
    
    FAMILY(P18F2XXX);
    FAMILY(P18FXX2);
    FAMILY(P16F87XA);
    FAMILY(P16F62XA);
    FAMILY(P16F62X);
    FAMILY(P12F629);
    FAMILY(P12F6XX);
    FAMILY(P16F84A);
    FAMILY(P16F81X);
    FAMILY(P16F7X);
    FAMILY(P16F7X7);
    FAMILY(P16F87X);
    FAMILY(P16F72);
    FAMILY(P16F87);
    FAMILY(P16F54);
    FAMILY(P16F57);
    FAMILY(P16F785);
    FAMILY(P16F59);
    FAMILY(P16F91X);
    FAMILY(P16F88X);
    FAMILY(P16C6XX);
    FAMILY(P16C55X);
    FAMILY(P16C7XX);
    FAMILY(P16C64X);
    FAMILY(P14000);
    FAMILY(P16C50X);
    FAMILY(P17CXX);
    FAMILY(P17C7XX);
    FAMILY(P18FXX39);
    FAMILY(P18F6X2X);
    FAMILY(P18FXX80);
    FAMILY(P18F8410);
    FAMILY(P18F6XJXX);    
    FAMILY(P18F45J10);    
    FAMILY(P18F97J60);    
    FAMILY(P18F1X30);    
    FAMILY(P18FXX23);    
    FAMILY(P18FXXK20);    
    FAMILY(P24FJXXXGA0XX);    
    FAMILY(P24FJXXXGA1);    
    FAMILY(dsP30F);    
    FAMILY(dsP33F);    
    FAMILY(P24H);    
    FAMILY(P10F200);    
    FAMILY(P10F202);    

    return wxEmptyString;
}*/

/* static */
PackageType ChipPackage::GetPackageTypeFromString(const wxString& str)
{
#define PACKAGE(x)                       \
    if (str.CmpNoCase(#x) == 0)          \
        return x;

    PACKAGE(PDIP);
    PACKAGE(SOIC);
    PACKAGE(SSOP);
    PACKAGE(MQFP);
    PACKAGE(TQFP);
    PACKAGE(PLCC);

    return UPP_INVALID_PACKAGETYPE;
}

/* static */
wxString ChipPackage::GetStringFromPackageType(PackageType type)
{
#undef PACKAGE
#define PACKAGE(x)          \
    if (type == x)          \
        return wxString(#x);

    PACKAGE(PDIP);
    PACKAGE(SOIC);
    PACKAGE(SSOP);
    PACKAGE(MQFP);
    PACKAGE(TQFP);
    PACKAGE(PLCC);

    return wxEmptyString;
}

/* static 
bool ChipPackage::Init()
{
    wxDir dir;
    if ( !dir.Open("packages") )
    {
        wxLogError(_("Could not access the 'packages' folder."));
        return false;
    }

    wxString prefix("packages");
    prefix += wxFileName::GetPathSeparator();

    if (!s_bmpUnknown.LoadFile(prefix + UPP_UNKNOWN_BITMAP_FILE, wxBITMAP_TYPE_PNG))
    {
        wxLogError(_("Couldn't load the image '%s'."), prefix + UPP_UNKNOWN_BITMAP_FILE);
        return false;
    }

    wxString filename;
    for ( bool cont = dir.GetFirst(&filename, "*.png", wxDIR_FILES);
          cont;
          cont = dir.GetNext(&filename) )
    {
        wxString str = wxFileName(filename).GetName();

        if (str.Freq('_') != 1)
            continue;       // skip this

        PackageType pt = GetPackageTypeFromString(str.BeforeFirst('_'));
        if (pt == UPP_INVALID_PACKAGETYPE)
            continue;       // skip this

        unsigned long npins;
        if (!str.AfterFirst('_').ToULong(&npins))
            continue;       // skip this

        wxBitmap bmp;
        if (!bmp.LoadFile(prefix + filename, wxBITMAP_TYPE_PNG))
        {
            wxLogError(_("Couldn't load the package file '%s'."), prefix + filename);
            continue;
        }
        
        if (bmp.IsOk())
            s_arrImage[pt][npins] = bmp;
    }

    return s_arrImage.size()>0;
}*/

