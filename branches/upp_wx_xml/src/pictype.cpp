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

// NOTE: to avoid lots of warnings with MSVC 2008 about deprecated CRT functions
//       it's important to include wx/defs.h before STL headers
#include <wx/defs.h>

#include <wx/xml/xml.h>
#include <wx/dir.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "pictype.h"

// statics
vector<PicType::PicIndexInfo> PicType::s_arrSupported;
PicType::PicIndexInfo PicType::s_default;


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
    if (!idx.Load(wxStandardPaths::Get().GetDataDir() + 
                  wxFileName::GetPathSeparator() + 
                  UPP_INDEX_FILE))
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
			{
                s_default = info;
			}

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
    wxString prefix = wxStandardPaths::Get().GetDataDir() + 
                      wxFileName::GetPathSeparator();
#ifdef __WXMSW__
    prefix += "xml_data";
    prefix += wxFileName::GetPathSeparator();
#endif

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
	wxString t = wxString(p.Name.substr(0,4).c_str());
    if (t.compare("P10F") != 0 &&
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
                p.CodeSize = (GetRange(child)+1)*2; //times 2 because this is in words
            else if (name == "config")
            {
                p.ConfigSize = (GetRange(child)+1)*2; //times 2 becuase this is in words
                
                if (!child->GetAttribute("start", &str) ||
                    !str.ToLong(&num, 0))
                    return UPP_INVALID_PIC;
                p.ConfigAddress = num;
            }
            else if (name == "eeprom")
            {
                p.DataSize = GetRange(child)+1;
                
                if (!child->GetAttribute("hexfile_offset", &str) ||
                    !str.ToLong(&num, 0))
                    return UPP_INVALID_PIC;
                p.DataAddress = num;
            }
        }
        else if (child->GetName() == "voltages")
        {
            for (VoltageType i=(VoltageType)0; i<VOLTAGE_TYPE_MAX; i=VoltageType(i+1))
            {
                wxString attr;
                if (i == MINIMUM) attr = "min";
                else if (i == NOMINAL) attr = "nominal";
                else if (i == MAXIMUM) attr = "max";

                double val;
                if (child->GetAttribute(attr).ToCDouble(&val))
                {
                    if (child->GetAttribute("name") == "vpp")
                        p.ProgVoltages[i] = val;
                    else if (child->GetAttribute("name") == "vdd_prog")
                        p.WorkVoltages[i] = val;
                    else if (child->GetAttribute("name") == "vdd_prog_write")
						p.WorkVoltages[i] = val;
					else
						return UPP_INVALID_PIC;
                }
            }
        }
        else if (child->GetName() == "frequency_range")
        {
            wxXmlNode *freqNode = child->GetChildren();
            while (freqNode)
            {
                if (freqNode->GetName() == "frequency")
                {
                    double val;
                    if (freqNode->GetAttribute("start").ToCDouble(&val))
                        p.MinFreq = std::min(p.MinFreq, (float)val);
                    if (freqNode->GetAttribute("end").ToCDouble(&val))
                        p.MaxFreq = std::max(p.MaxFreq, (float)val);
                }

                freqNode = freqNode->GetNext();
            }
        }
        else if (child->GetName() == "config")
        {
            ConfigBlock block;
            block.Name = child->GetAttribute("name");
            if (!child->GetAttribute("offset").ToULong(&block.Offset, 0))
                return UPP_INVALID_PIC;
            // load the ConfigMask objects belonging to this block
            wxXmlNode *maskNode = child->GetChildren();
            while (maskNode)
            {
                if (maskNode->GetName() == "mask")
                {
                    ConfigMask mask;
                    mask.Name = maskNode->GetAttribute("name");
                    if (!maskNode->GetAttribute("value").ToULong(&mask.Value, 0))
                        return UPP_INVALID_PIC;
                    // load the ConfigValue objects belonging to this mask
                    wxXmlNode *valueNode = maskNode->GetChildren();
                    while (valueNode)
                    {
                        if (valueNode->GetName() == "value")
                        {
                            ConfigValue value;
                            value.Name = valueNode->GetAttribute("name");

                            if (valueNode->GetAttribute("value") != "default")
                            {
                                if (!valueNode->GetAttribute("value").ToULong(&value.Value, 0))
                                    return UPP_INVALID_PIC;

                                mask.Values.push_back(value);
                            }
                            //else: for now ignore the <value value="default" .../> tags
                        }

                        valueNode = valueNode->GetNext();
                    }

                    block.Masks.push_back(mask);
                }

                maskNode = maskNode->GetNext();
            }

            p.Config.push_back(block);
        }
        else if (child->GetName() == "package")
        {
            wxArrayString types = wxSplit(child->GetAttribute("types"), ' ');
            unsigned long npins;
            if (!child->GetAttribute("nb_pins").ToULong(&npins))
			{
				return UPP_INVALID_PIC;
			}

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
