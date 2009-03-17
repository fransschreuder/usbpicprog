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

#ifndef PICTYPE_H
#define PICTYPE_H

#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/bitmap.h>

#include <string>
#include <map>
#include <vector>

using namespace std;
class wxXmlNode;

#define UPP_UNKNOWN_BITMAP_FILE wxT("unknown.png")
#define UPP_INDEX_FILE          wxT("index.xml")
#define UPP_DEFAULT_PIC_MODEL   wxT("18F2550")
#define UPP_DEFAULT_PIC         "P18F2550"

#define UPP_INVALID_PIC         (Pic())

/**
    PicFamily indicates a programming algorith, each one is used by a certain 
    range of PICs  (in some cases a single one!)
*/
typedef enum {
    P18F2XXX,P18FXX2,P16F87XA,P16F62XA,P16F62X,P12F629,P12F6XX,P16F84A,P16F81X,
    P16F7X,P16F7X7,P16F87X,P16F72,P16F87,P16F54,P16F57,P16F785,P16F59,P16F91X,P16F88X,
    P16C6XX,P16C55X,P16C7XX,P16C64X,P14000,P16C50X,P16C432,P17CXX,P17C7XX,
    P18FXX39,P18F6X2X,P18FXX80,P18F8410,P18F6XJXX,P18F45J10,P18F97J60,P18F1X30,P18FXX23,
    P18FXXK20,P24FJXXXGA0XX,P24FJXXXGA1,dsP30F,dsP33F,P24H,P10F200,P10F202,
    UPP_INVALID_PICFAMILY

    // VERY IMPORTANT: when adding/removing enumeration values here, please
    //                 update also the PicType::GetFamilyFromString function!
} PicFamily;

/**
    The possible types of packages.
*/
typedef enum {
    PDIP,
    SOIC,
    SSOP,
    MQFP,
    TQFP,
    PLCC,
    UPP_INVALID_PACKAGETYPE

    // VERY IMPORTANT: when adding/removing enumeration values here, please
    //                 update also the ChipPackage::GetPackageTypeFromString function!
} PackageType;

/**
    Describes the package of a PIC (used only for informative purposes).
*/
class ChipPackage
{
public:
    ChipPackage() : Type(UPP_INVALID_PACKAGETYPE) {}

    /**
        The size of this array indicates the number of pins of this package.
        The name associated with some of the pins maybe the empty string or not.
    */
    wxArrayString PinNames;

    /**
        The type of this package.
    */
    PackageType Type;


    wxString GetName() const
        { return GetStringFromPackageType(Type); }
    unsigned int GetPinCount() const
        { return PinNames.GetCount(); }

    bool IsICSPPin(unsigned int idx) const
    {
        if (PinNames[idx].Contains("VDD") ||
            PinNames[idx].Contains("VSS") ||
            PinNames[idx].Contains("GND") ||
            PinNames[idx].Contains("ICSP") ||
            PinNames[idx].Contains("MCLR") ||
            PinNames[idx].Contains("PGC") ||
            PinNames[idx].Contains("PGD"))
            return true;
        return false;
    }

public:     // static
    /**
        Returns the PackageType for the given package type name or 
        @c UPP_INVALID_PACKAGETYPE on error.
    */
    static PackageType GetPackageTypeFromString(const wxString& str);

    /**
        Returns the name for the given PackageType.
    */
    static wxString GetStringFromPackageType(PackageType type);
};

/**
    Describes a configuration bit value.
*/
class ConfigValue
{
public:
    ConfigValue() : Value(0) {}

    wxString Name;
    unsigned long Value;

    // leave out cname, sdcc_cname
};

/**
    Describes a configuration "mask".
*/
class ConfigMask
{
public:
    ConfigMask() : Value(0) {}

    wxString Name;
    unsigned long Value;
    vector<ConfigValue> Values;

    wxArrayString GetStringValues(bool includeValues = true) const
    {
        wxArrayString ret;
        for (unsigned int i=0; i<Values.size(); i++)
        {
            if (includeValues)
                ret.Add(wxString::Format("%s [0x%X]", Values[i].Name, Values[i].Value));
            else
                ret.Add(Values[i].Name);
        }
        return ret;
    }
};

/**
    Describes a block of configuration bits.
*/
class ConfigBlock
{
public:
    ConfigBlock() : Offset(0), WriteMask(0) {}

    unsigned long Offset;
    unsigned long WriteMask;
    wxString Name;
    vector<ConfigMask> Masks;

    // leave out bvalue
};

/**
    This class represents a PIC device.

    Pic class stores all properties of a PIC, but does NOT store the
    code, config and data memory areas (see HexFile for that).
    
    The info stored in this class are initialized at run-time using one of
    FindPIC() static functions and then are not modified anymore.
*/
class Pic
{
public:
    Pic() : CodeSize(0), ConfigAddress(0), DataAddress(0), DataSize(0), ConfigSize(0), 
            picFamily(UPP_INVALID_PICFAMILY), DevId(0), DevIdMask(0) 
    {
        memset(ConfigMask, 0, sizeof(ConfigMask));
    }

    /// Is this PIC structure correctly initialized?
    bool ok() const
        { return !Name.empty() && picFamily != UPP_INVALID_PICFAMILY; }

    /// Returns the PIC name which starts with "PIC" instead of "P".
    wxString GetExtName() const
        { return wxString("PIC" + Name.substr(1)); }

    // public members to avoid lots of getters/setters:

    string Name;
    unsigned int CodeSize;
    unsigned int ConfigAddress;
    unsigned int DataAddress; // in hex file
    unsigned int DataSize;
    unsigned int ConfigSize;
    PicFamily picFamily;
    unsigned int DevId;
    unsigned int DevIdMask;

    vector<ConfigBlock> Config;

    // TODO: remove this in favour of "Config"
    unsigned int ConfigMask[16];

    // package descriptor:
    vector<ChipPackage> Package;
};

/**
    A PIC type.

    @todo rename this class to PicChooser and rename its ctors
          to FindPIC() and make all its function static.
          Change UppMainWindow to store&use a Pic instance.
          Or otherwise simply merge this class with the Pic class itself.
*/
class PicType
{
public:
    /**
        Constructor to select a pic by string, usage:
        @code Pictype* picType=new PicType("P18F2550"); @endcode
        Use ok() to test if the ctor did successful find a
        valid PIC model.
    */
    PicType(string picType);

    /**
        Constructor to select a pic by devId, usage:
        @code Pictype* picType=new PicType(0x1240); @endcode
        Use ok() to test if the ctor did successful find a
        valid PIC model.
    */
    PicType(int devId);

    /**Returns the PIC that was selected in the ctor.*/
    const Pic& getCurrentPic() const
        { return m_currentPic; }
    Pic& getCurrentPic()
        { return m_currentPic; }
    /**Returns true if the construction of this instance was successful.*/
    bool ok() const 
        { return m_currentPic.ok(); }
    /**Returns the current PIC name as a wxString.*/
    wxString getPicName() const
        { return wxString::FromAscii(m_currentPic.Name.c_str()); }


    // static utilities

    /**
        Returns an array (vector) of strings containing all names of supported PICs.
    */
    static const wxArrayString& getSupportedPicNames();

    /** 
        Loads the @c UPP_INDEX_FILE XML file and initializes the internal static array
        which describes all supported PICs.

        This needs to be called only once at program startup.
    */
    static bool Init();

private:        // data
    /**
        An internal private structure used to store the info extracted by
        the @c UPP_INDEX_FILE file.
    */
    typedef struct
    {
        /// This is the pic name @e without the initial "P"
        wxString name;
        PicFamily picFamily;
        unsigned int devIdMask;
        unsigned int devId;

        /// This is the full cached PIC structure as loaded from the Piklab XML
        /// file. If pic.ok()==false, it means the relative XML file still needs
        /// to be loaded.
        Pic pic;
    } PicIndexInfo;

    /**
        The PIC selected in the ctor
        This is loaded on-the-fly from the Piklab XML files.
    */
    Pic m_currentPic;

    // static data:

    /**
        The names of the supported PICs.
        This is loaded in Init() from @c UPP_INDEX_FILE and is never changed then.
    */
    static vector<PicIndexInfo> s_arrSupported;

    /**
        The PicIndexInfo structure relative to the @c UPP_DEFAULT_PIC.
    */
    static PicIndexInfo s_default;

private:    // utilities
    
    /**
        Loads the given XML file.

        @note Does not log any error.
    */
    bool LoadPIC(PicType::PicIndexInfo& indexInfo);
    
    /**
        Loads the Piklab XML for the given PIC name
        (the name must not start with 'P').

        @note Does not log any error.

        @return UPP_INVALID_PIC on failure, or a valid Pic structure otherwise.
    */
    static Pic LoadPiklabXML(const wxString& picName);
    
    /**
        Returns the range indicated by the given XML node.
    */
    static int GetRange(const wxXmlNode* p);

    /**
        Returns the PicFamily for the given family name or 
        @c UPP_INVALID_PICFAMILY on error.
    */
    static PicFamily GetFamilyFromString(const wxString& str);
};

#endif  //PICTYPE_H
