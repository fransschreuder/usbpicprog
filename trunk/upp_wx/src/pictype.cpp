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
#include <wx/dcclient.h>

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "pictype.h"

#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // useful to catch memory leaks when compiling under MSVC 
#endif


// statics
vector<PicType::PicIndexInfo> PicType::s_arrSupported;
PicType::PicIndexInfo PicType::s_default;


// these symbols are mostly used for debugging; they should be 0 by default since we
// want to load Piklab XML files on demand for max performances
// Typical debug use is to check that all supported devices have valid Piklab XML
// files which can be loaded without warnings/errors
#define LOAD_ALL_PIKLAB_FILES_AT_STARTUP        0
#define LOAD_SUPPORTED_PIKLAB_FILES_AT_STARTUP  0

// ----------------------------------------------------------------------------
// PicType
// ----------------------------------------------------------------------------


PicType::PicType() : CodeSize(0), ConfigAddress(0), DataSize(0), DataAddress(0), ConfigSize(0), 
                picFamily(UPP_INVALID_PICFAMILY), DevId(0), DevIdMask(0), MinFreq(0), MaxFreq(0)
{
    for (unsigned int i=0; i<VOLTAGE_TYPE_MAX; i++)
        WorkVoltages[i] = ProgVoltages[i] = 0.0;
}


bool PicType::LoadPIC(PicType::PicIndexInfo& indexInfo)
{
    if (indexInfo.pic)
    {
        // the data for this PIC was already loaded and cached previously;
        // don't reload it again!
        *this = *indexInfo.pic;
    }
    else
    {
        *this = LoadPiklabXML(indexInfo.name);

        // complete the Piklab data with some UPP-specific data
        // previously loaded from UPP_INDEX_FILE
        DevIdMask = indexInfo.devIdMask;
        DevId = indexInfo.devId;
        picFamily = indexInfo.picFamily;

        // cache the entire structure in our internal static array:
        indexInfo.pic = new PicType(*this);
    }

    return ok();
}

/* static*/
PicType PicType::FindPIC(wxString picTypeStr)
{
    for (unsigned int i=0;i<s_arrSupported.size();i++)
    {
		if (picTypeStr.compare(s_arrSupported[i].name)==0)
        {
            // this is a supported PIC!

            PicType ret;
            if (!ret.LoadPIC(s_arrSupported[i]))
                wxLogError(_("Could not load data for PIC '%s'."), picTypeStr);
            // we either failed or successfully loaded the PIC data...
            return ret;
        }
    }
    
    return UPP_INVALID_PIC;
}

/* static*/
PicType PicType::FindPIC(unsigned int devId)
{
    for (unsigned int i=0;i<s_arrSupported.size();i++)
    {
        if ((devId & s_arrSupported[i].devIdMask) == s_arrSupported[i].devId)
        {
            // this is a supported PIC!
            
            PicType ret;
            if (!ret.LoadPIC(s_arrSupported[i]))
                wxLogError(_("Could not load data for PIC '%s'."), s_arrSupported[i].name);
            // we either failed or successfully loaded the PIC data...
            return ret;
        }
    }
    
	return UPP_INVALID_PIC;
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
            picNames.Add(s_arrSupported[i].name);

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
    if (!idx.Load(UPP_INDEX_PATH + UPP_INDEX_FILE))
    {
        wxLogError(_("Cannot load '%s'."), UPP_INDEX_FILE);
        return false;
    }

    if (idx.GetRoot()->GetName() != ("upp"))
    {
        wxLogError(_("Invalid '%s' file."), UPP_INDEX_FILE);
        return false;
    }

    wxXmlNode *child = idx.GetRoot()->GetChildren();
    while (child)
    {
        if (child->GetName() == ("pic") && 
            child->HasAttribute(("name")) &&
            child->HasAttribute(("upp_family")) &&
            child->HasAttribute(("upp_devidmask")) &&
            child->HasAttribute(("upp_devid")))
        {
            PicType::PicIndexInfo info;
            wxString str;
            long num;

            info.pic = NULL;
            info.name = child->GetAttribute(("name"));
            info.picFamily = GetFamilyFromString(child->GetAttribute(("upp_family")));
            if (info.picFamily == UPP_INVALID_PICFAMILY)
            {
                wxLogError(_("Invalid PIC family for PIC '%s'."), info.name);
                return false;
            }

            child->GetAttribute(("upp_devid"), &str);
            if (str.ToLong(&num, 0))
                info.devId = num;

            child->GetAttribute(("upp_devidmask"), &str);
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
        wxLogError(_("Could not load data for PIC '%s'."), UPP_DEFAULT_PIC_MODEL);
        return false;
    }
    
#if LOAD_SUPPORTED_PIKLAB_FILES_AT_STARTUP
    for (unsigned int i=0;i<s_arrSupported.size();i++)
    {
        // constructing this instance triggers loading of the XML file:
        PicType temp(s_arrSupported[i].name);
    }
#endif
#if LOAD_ALL_PIKLAB_FILES_AT_STARTUP
    wxString prefix = UPP_INDEX_PATH;
#ifdef __WXMSW__
    prefix += "xml_data";
    prefix += wxFileName::GetPathSeparator();
#endif

    wxDir dir;
    if ( !dir.Open(prefix) )
    {
        wxLogError(_("Could not access the 'xml_data' folder."));
        return false;
    }

    wxString filename;
    for ( bool cont = dir.GetFirst(&filename, "*.xml", wxDIR_FILES);
          cont;
          cont = dir.GetNext(&filename) )
    {
        if (!LoadPiklabXMLFile(prefix + filename).ok())
            wxLogError("Could not load the PIC data from '%s'.", filename);
    }
#endif

    return s_arrSupported.size()>0;
}

/* static*/
void PicType::CleanUp()
{
    for (unsigned int i=0;i<s_arrSupported.size();i++)
    {
        if (s_arrSupported[i].pic)
            delete s_arrSupported[i].pic;
    }
}

/* static */
int PicType::GetRange(const wxXmlNode* p)
{
    wxString temp;
    long start = 1, end = 0;
    if (!p->GetAttribute(("start"), &temp) ||
        !temp.ToLong(&start, 0))
        return -1;

    if (!p->GetAttribute(("end"), &temp) ||
        !temp.ToLong(&end, 0))
        return -1;

    return end-start;
}

/* static */
PicType PicType::LoadPiklabXML(const wxString& picName)
{
    wxString prefix = UPP_INDEX_PATH;
#ifdef __WXMSW__
    prefix += ("xml_data");
    prefix += wxFileName::GetPathSeparator();
#endif

    PicType ret = LoadPiklabXMLFile(prefix + picName + (".xml"));
	if (wxString(ret.Name.c_str()) != picName)  // PIC name should match picName
        {cout<<"error: PIC name should match picName: "<<ret.Name.mb_str(wxConvUTF8)<<", "<<picName.mb_str(wxConvUTF8)<<endl;return UPP_INVALID_PIC;}
    
    return ret;
}

/* static */
PicType PicType::LoadPiklabXMLFile(const wxString& fileName)
{
    PicType p;
    wxXmlDocument doc;
    wxString str;
    long num=0;
    
    if (!doc.Load(fileName))
        {cout<<"could not loade fileName"<<endl;return UPP_INVALID_PIC;}
    if (doc.GetRoot()->GetName() != ("device"))
        {cout<<"name differs from Device"<<endl;return UPP_INVALID_PIC;}

    // load the name of the PIC:
    if (!doc.GetRoot()->GetAttribute(("name"), &str))
        {cout<<"name attribute not found"<<endl;return UPP_INVALID_PIC;}
    p.Name = str;

    // load the device ID of the PIC
    // NOTE: PIC of the 10F family do not have a device ID (no autodetection is possible)
    wxString t = wxString(p.Name.substr(0,3).c_str());
	wxString t2 = wxString(p.Name.substr(0,4).c_str());
    if ((t.compare(("10F")) != 0)&&(t2.compare(("16F5")) != 0)&&(t2.compare(("12F5")) != 0) &&
        (!doc.GetRoot()->GetAttribute(("id"), &str) ||
        !str.ToLong(&num, 0)))
        {cout<<"device ID not found"<<endl;return UPP_INVALID_PIC;}

    p.DevId = num;
    wxXmlNode *child = doc.GetRoot()->GetChildren();
    while (child)
    {
        if (child->GetName() == ("memory"))
        {
            wxString name;

            if (!child->GetAttribute(("name"), &name))
                {cout<<"name attribute in memory not found"<<endl;return UPP_INVALID_PIC;}
            if (name == ("code"))
			{
				switch(p.bitsPerWord())
				{
					case 8:
					case 16:
						p.CodeSize = (GetRange(child)+1);
						break;
					case 14:
						p.CodeSize = (GetRange(child)+1)*2;   // times 2 because this is in word units
						break;
					case 24:
						p.CodeSize = ((GetRange(child)+1)*3)/2;   // times 1.5 because Microchip had the crazy idea that for every 3 bytes, the address increases with 2
						break;
					default:
						p.CodeSize = (GetRange(child)+1);
						
				}
			}
            else if (name == ("config"))
            {
				switch(p.bitsPerWord())
				{
					case 8:
						p.ConfigSize = 0;
					case 14:
					case 16:
						p.ConfigSize = (GetRange(child)+1)*2; // times 2 because this is in word units		
						break;
	                case 24:
						p.ConfigSize = (GetRange(child)+1); 
						break;
					default:
						p.ConfigSize = 0;
				}
                if (!child->GetAttribute(("start"), &str) ||
                    !str.ToLong(&num, 0))
                    {cout<<"start attribute in config memory not found"<<endl;return UPP_INVALID_PIC;}
                p.ConfigAddress = num;
            }
            else if (name == ("eeprom"))
            {
                p.DataSize = GetRange(child)+1;

                if (!child->GetAttribute(("hexfile_offset"), &str) ||
                    !str.ToLong(&num, 0))
					{if(!child->GetAttribute(("start"), &str) ||
                    !str.ToLong(&num, 0))						
						{cout<<"start attribute in hexfile_offset not found"<<endl;return UPP_INVALID_PIC;}}
                p.DataAddress = num;
            }
        }
        else if (child->GetName() == ("voltages"))
        {
            for (VoltageType i=(VoltageType)0; i<VOLTAGE_TYPE_MAX; i=VoltageType(i+1))
            {
                wxString attr;
                if (i == MINIMUM) attr = ("min");
                else if (i == NOMINAL) attr = ("nominal");
                else if (i == MAXIMUM) attr = ("max");

                double val;
                if (child->GetAttribute(attr).ToCDouble(&val))
                {
                    if (child->GetAttribute(("name"))== ("vpp"))
                        p.ProgVoltages[i] = val;
                    else if (child->GetAttribute(("name")) == ("vdd_prog"))
                        p.WorkVoltages[i] = val;
                    else if (child->GetAttribute(("name")) == ("vdd_prog_write"))
						p.WorkVoltages[i] = val;
                    else
                        {cout<<"name attribute for voltage not vpp, vdd_prog or vdd_prog_write"<<endl;return UPP_INVALID_PIC;}
                }
            }
        }
        else if (child->GetName() == ("frequency_range"))
        {
            wxXmlNode *freqNode = child->GetChildren();
            while (freqNode)
            {
                if (freqNode->GetName() == ("frequency"))
                {
                    double val;
                    if (freqNode->GetAttribute(("start")).ToCDouble(&val))
                        p.MinFreq = std::min(p.MinFreq, (float)val);
                    if (freqNode->GetAttribute(("end")).ToCDouble(&val))
                        p.MaxFreq = std::max(p.MaxFreq, (float)val);
                }

                freqNode = freqNode->GetNext();
            }
        }
        else if (child->GetName() == ("config"))
        {
            ConfigWord block;
            block.Name = child->GetAttribute(("name"));
            if (!child->GetAttribute(("offset")).ToULong(&block.Offset, 0))
                {cout<<"error getting attribute config -> name -> offset"<<endl;return UPP_INVALID_PIC;}
            // load the ConfigMask objects belonging to this word
            wxXmlNode *maskNode = child->GetChildren();
            while (maskNode)
            {
                if (maskNode->GetName() == ("mask"))
                {
                    ConfigMask mask;
                    mask.Name = maskNode->GetAttribute("name");
                    // load the ConfigValue objects belonging to this mask
                    wxXmlNode *valueNode = maskNode->GetChildren();
                    while (valueNode)
                    {
                        if (valueNode->GetName() == ("value"))
                        {
                            ConfigValue value;
                            value.Name = valueNode->GetAttribute(("name"));
							if (valueNode->GetAttribute(("value")) != ("default"))
                            {
                                if (!valueNode->GetAttribute(("value")).ToULong(&value.Value, 0))								
                                    {cout<<"couldn't find attribute value in mask"<<endl;return UPP_INVALID_PIC;}

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

            p.ConfigWords.push_back(block);
        }
        else if (child->GetName() == ("package"))
        {
            wxArrayString types = wxSplit(child->GetAttribute(("types")), ' ');
            unsigned long npins;
            if (!child->GetAttribute(("nb_pins")).ToULong(&npins))
                {cout<<"nb_pins not found in package"<<endl;return UPP_INVALID_PIC;}

            wxArrayString names;
            names.Add(wxEmptyString, npins);

            wxXmlNode *pin = child->GetChildren();
            while (pin)
            {
                wxString name;
				pin->GetAttribute(("name"),&name);

                unsigned long idx;

                if (pin->GetAttribute(("index")).ToULong(&idx) &&
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

                // add it to the PicType instance:
                p.Package.push_back(pkg);
            }
        }
		else if (child->GetName() == ("checksums"))
        {
            wxXmlNode *checksumsNode = child->GetChildren();
            while (checksumsNode)
            {

                unsigned long val;
				if (checksumsNode->GetName() == ("checksum"))
                {
		            if (checksumsNode->GetAttribute(("protected"))==("Off"))
					{
						if (checksumsNode->GetAttribute(("bchecksum")).AfterFirst('x').ToULong(&val,16))
							p.CheckSums.bChecksumCpOff=(unsigned int) val;
						if (checksumsNode->GetAttribute(("cchecksum")).AfterFirst('x').ToULong(&val,16))
							p.CheckSums.cChecksumCpOff=(unsigned int) val;
					}
					if (checksumsNode->GetAttribute(("protected"))==("All"))
					{
						if (checksumsNode->GetAttribute(("bchecksum")).AfterFirst('x').ToULong(&val,16))
							p.CheckSums.bChecksumCpAll=(unsigned int) val;
						if (checksumsNode->GetAttribute(("cchecksum")).AfterFirst('x').ToULong(&val,16))
							p.CheckSums.cChecksumCpAll=(unsigned int) val;
					}
				}
                checksumsNode = checksumsNode->GetNext();
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
	FAMILY(P12F61X);    
	FAMILY(P16F716);
	FAMILY(P18FXX31);
	FAMILY(P18FX220);
	FAMILY(I2C_EE_1);
	FAMILY(I2C_EE_2);
	FAMILY(P12F508);
	FAMILY(P24FXXKAXXX);
    return UPP_INVALID_PICFAMILY;
}


// ----------------------------------------------------------------------------
// ChipPackage
// ----------------------------------------------------------------------------

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
	PACKAGE(QFN);
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
	PACKAGE(QFN);
    PACKAGE(PLCC);

    return wxEmptyString;
}

#ifdef SWAP
#undef SWAP(x,y,type)
#endif

#define SWAP(x,y,type)     { type tmp=x; x=y; y=tmp; }

void ChipPackage::DrawPins(wxDC& dc, const wxPoint& pt, unsigned int PackageLen,
                           unsigned int FirstPin, unsigned int LastPin,
                           int flags,
                           wxDirection dir)
{
    wxASSERT_MSG(LastPin>FirstPin, "invalid pin indexes");

    // NOTE: inside this function context
    //    pin number == the index of the pin (e.g. "1", "2", etc)
    //    pin name == pin label == the name of the pin signal (e.g. "VCC", "GND" etc)
    //    pin == pin rectangle


    // some drawing constants:

    const unsigned int PinCount = LastPin - FirstPin;

    // pin height is calculated imposing that
    // 1) PinCount*PinH + (PinCount+1)*PinSpacing = PackageLen
    // 2) PinSpacing = PinH/2
    // solving for PinH yields:
    unsigned int PinH = (unsigned int)floor(2*PackageLen/(3.0*PinCount+1));
    unsigned int PinW = (unsigned int)1.5*PinH;
    const unsigned int PinSpacing = (const unsigned int)floor(PinH/2.0);

    // the error which force us to have a "pin y offset" is caused by rounding
    // in the calculation of PinH:
    const int PinOffset = (PackageLen - (PinCount*PinH + (PinCount+1)*PinSpacing))/2;
    wxASSERT(PinOffset>=0);

    if (int(PinH*0.8) == 0)
        return;     // this happens for very small package sizes;
                    // the check avoids an assertion failure from wxFont ctor
    
    // select a font suitable for the PinH we have computed above:
	int fSize = int(PinH*0.8);
	if(fSize>12)fSize=12;
    wxFont fnt(wxSize(0,fSize), wxFONTFAMILY_DEFAULT,
               wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    dc.SetFont(fnt);
    const unsigned int PinNumberW = 2*dc.GetCharWidth();
    const unsigned int PinNumberH = dc.GetCharHeight();

    bool invertOrder = (flags & DRAWPIN_INVERTED_ORDER) != 0;
    unsigned int start = invertOrder ? LastPin-1 : FirstPin;
    unsigned int end = invertOrder ? FirstPin-1 : LastPin;
    int inc = invertOrder ? -1 : +1;

    if (dir == wxLEFT || dir == wxRIGHT)
    {
        unsigned int PinX = (dir == wxLEFT) ? pt.x-PinW+1 : pt.x-1;
        unsigned int PinY = pt.y + PinOffset + PinSpacing;

        unsigned int PinNumberX = (dir == wxLEFT) ? 
                pt.x+PinSpacing : pt.x-PinNumberW-PinSpacing;

        if (flags & DRAWPIN_NUMBERS_INSIDE_PINS)
            PinNumberX = PinX + (PinW-PinNumberW)/2;

        // draw the pins organizing them in a vertical column
        for (unsigned int i=start; i != end; i+=inc)
        {
            unsigned int PinNumberY = PinY + (PinH-PinNumberH)/2;

            unsigned int pinLabelX = (dir == wxLEFT) ? 
                PinX-PinSpacing-dc.GetTextExtent(PinNames[i]).GetWidth() : 
                PinX+PinW+PinSpacing;
            unsigned int pinLabelY = PinNumberY;

            // draw the pin rect
            dc.DrawRectangle(PinX, PinY, PinW, PinH);

            // print the pin number
            dc.SetTextForeground(IsICSPPin(i) ? *wxRED : *wxBLACK);
            dc.DrawText(wxString::Format(("%d"), i+1), PinNumberX, PinNumberY);

            // print the pin name
            dc.DrawText(PinNames[i], pinLabelX, pinLabelY);

            PinY += PinH+PinSpacing;
        }
    }
    else if (dir == wxTOP || dir == wxBOTTOM)
    {
        SWAP(PinH, PinW, unsigned int);
            // in this case the PackageLen is the number of horizontal pixels available
            // for drawing the pin strip; thus we want to compute PinW and set PinH

        // VERY IMPORTANT: the code below is the dual of the code for wxLEFT|wxRIGHT case!
        //                 If you change something here, you may want to change it also
        //                 above, with the appropriate differences
        // NOTE: remember however that the text is drawn rotated by 90 degrees

        unsigned int PinX = pt.x + PinOffset + PinSpacing;
        unsigned int PinY = (dir == wxTOP) ? pt.y-PinH+1 : pt.y-1;

        unsigned int PinNumberY = (dir == wxTOP) ? 
            pt.y+PinSpacing : pt.y-PinNumberH-PinSpacing;

        if (flags & DRAWPIN_NUMBERS_INSIDE_PINS)
            PinNumberY = PinY + (PinH-PinNumberW)/2;

        // draw the pins organizing them in a horizontal column
        for (unsigned int i=start; i != end; i+=inc)
        {
            unsigned int PinNumberX = PinX + PinNumberH + (PinW-PinNumberH)/2;

            unsigned int pinLabelX = PinNumberX;
            unsigned int pinLabelY = (dir == wxTOP) ? 
                PinY-PinSpacing-dc.GetTextExtent(PinNames[i]).GetWidth() : 
                PinY+PinH+PinSpacing;

            // draw the pin rect
            dc.DrawRectangle(PinX, PinY, PinW, PinH);

            // print the pin number
            dc.SetTextForeground(IsICSPPin(i) ? *wxRED : *wxBLACK);
            dc.DrawRotatedText(wxString::Format(("%d"), i+1), PinNumberX, PinNumberY, -90);

            // print the pin name
            dc.DrawRotatedText(PinNames[i], pinLabelX, pinLabelY, -90);

            PinX += PinW+PinSpacing;
        }
    }
    else
        wxLogWarning(_("Invalid direction"));
}

void ChipPackage::Draw(wxDC& dc, const wxSize& sz, const wxString& chipModel)
{
    // set some GUI objects common to all packages-drawing code
    dc.SetPen(*wxBLACK_PEN);

    switch (Type)
    {
    case PDIP:
    case SOIC:
    case SSOP:
        {
            // some drawing constants:

            // in these package types, pins are organized in two columns:
            const unsigned int PinPerSide = GetPinCount()/2;
            if ((GetPinCount()%2) != 0)
            {
                wxLogWarning(_("Invalid odd pin count: %d"), GetPinCount());
                return;
            }

            // choose reasonable package width&height to
            // - make best use of the available space
            // - avoid drawing package excessively big
            const unsigned int BoxW = sz.GetWidth()/3;
            const unsigned int BoxH = (unsigned int)((double)sz.GetHeight()*0.8);
            const unsigned int BoxX = (sz.GetWidth()-BoxW)/2;
            const unsigned int BoxY = (sz.GetHeight()-BoxH)/2;
            const unsigned int R = BoxW/6;
            
            if (int(BoxW/8) == 0)
                return;     // this happens for very small package sizes;
                            // the check avoids an assertion failure from wxFont ctor later

            // draw the PIC package box
            dc.DrawRectangle(BoxX, BoxY, BoxW, BoxH);
            dc.DrawArc(sz.GetWidth()/2-R, BoxY + 1,
                       sz.GetWidth()/2+R, BoxY + 1,
                       sz.GetWidth()/2, BoxY + 1);


            dc.SetFont(wxFont(wxSize(0, BoxW/8), wxFONTFAMILY_DEFAULT,
                              wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
            const wxSize& nameSz = dc.GetTextExtent(chipModel);
            dc.DrawRotatedText(chipModel,
                               (sz.GetWidth() + nameSz.GetHeight())/2,
                               (sz.GetHeight() - nameSz.GetWidth())/2,
                               -90);

            // draw the pins
            DrawPins(dc, wxPoint(BoxX, BoxY), BoxH, 0, PinPerSide, 
                     0, wxLEFT);
            DrawPins(dc, wxPoint(BoxX+BoxW, BoxY), BoxH, PinPerSide, GetPinCount(), 
                     DRAWPIN_INVERTED_ORDER, wxRIGHT);
        }
        break;

    case MQFP:
    case TQFP:
    case PLCC:
	case QFN:
        {
            // some drawing constants:

            // in these package types, pins are organized in two columns:
            const unsigned int PinPerSide = GetPinCount()/4;
            if ((GetPinCount()%4) != 0)
            {
                wxLogWarning(_("Invalid pin count: %d"), GetPinCount());
                return;
            }

            // choose reasonable package width&height to
            // - make best use of the available space
            // - avoid drawing package excessively big
            const unsigned int BoxL = max(sz.GetWidth()/2,80);
            const unsigned int BoxX = (sz.GetWidth()-BoxL)/2;
            const unsigned int BoxY = (sz.GetHeight()-BoxL)/2;
            const unsigned int R = BoxL/10;
            
            if (int(BoxL/10) == 0)
                return;     // this happens for very small package sizes;
                            // the check avoids an assertion failure from wxFont ctor later

            // draw the PIC package box
            dc.DrawRectangle(BoxX, BoxY, BoxL, BoxL);
            dc.DrawCircle(BoxX+int(R*1.3), BoxY+int(R*1.3), R/2);

            // draw the name of the PIC model in the centre of the box
            dc.SetFont(wxFont(wxSize(0, BoxL/10), wxFONTFAMILY_DEFAULT,
                              wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
			const wxSize& nameSz = dc.GetTextExtent(chipModel);
            dc.DrawText(chipModel,
                        (sz.GetWidth() - nameSz.GetWidth())/2,
                        (sz.GetHeight() - nameSz.GetHeight())/2);

            // draw the pins
            DrawPins(dc, wxPoint(BoxX, BoxY), BoxL, 0, PinPerSide, 
                     DRAWPIN_NUMBERS_INSIDE_PINS, wxLEFT);
            DrawPins(dc, wxPoint(BoxX, BoxY+BoxL), BoxL, PinPerSide, PinPerSide*2, 
                     DRAWPIN_NUMBERS_INSIDE_PINS, wxBOTTOM);
            DrawPins(dc, wxPoint(BoxX+BoxL, BoxY), BoxL, PinPerSide*2, PinPerSide*3, 
                     DRAWPIN_INVERTED_ORDER|DRAWPIN_NUMBERS_INSIDE_PINS, wxRIGHT);
            DrawPins(dc, wxPoint(BoxX, BoxY), BoxL, PinPerSide*3, PinPerSide*4, 
                     DRAWPIN_INVERTED_ORDER|DRAWPIN_NUMBERS_INSIDE_PINS, wxTOP);
        }
        break;

    default:
        break;
    }
}

