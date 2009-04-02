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
#include <iostream>

using namespace std;

// forward declarations:
class wxXmlNode;
class wxDC;


// constants:

#define UPP_UNKNOWN_BITMAP_FILE wxT("unknown.png")
#define UPP_INDEX_FILE          wxT("index.xml")
#define UPP_DEFAULT_PIC_MODEL   wxT("18F2550")
#define UPP_DEFAULT_PIC         "P18F2550"

#define UPP_INVALID_PIC         (Pic())


// enums:

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
    The meaning of the voltage values stored inside the arrays of Pic class.
*/
typedef enum {
    MINIMUM = 0,
    NOMINAL,
    MAXIMUM,

    VOLTAGE_TYPE_MAX
} VoltageType;

/**
    Describes the package of a PIC (used only for informative purposes).
    
    In Piklab XML files this class holds the information stored in a @c package node.
*/
class ChipPackage
{
public:
    ChipPackage() : Type(UPP_INVALID_PACKAGETYPE) {}

    /**
        The name of the pins.
    
        The size of this array indicates the number of pins of this package.
        The name associated with some of the pins maybe the empty string or not.
    */
    wxArrayString PinNames;

    /**
        The type of this package.
    */
    PackageType Type;

    /**
        Draws this package on the given @a dc, making it fit inside the given
        size @a sz. In the center of the package the @a chipModel string is drawn.
    */
    void Draw(wxDC& dc, const wxSize& sz, const wxString& chipModel);

    /**
        Returns the name of this package.
    */
    wxString GetName() const
        { return GetStringFromPackageType(Type); }
        
    /**
        Returns the number of pins of this package.
    */
    unsigned int GetPinCount() const
        { return PinNames.GetCount(); }

    /**
        Returns true if the pin with index @a idx is involved in ICSP
        (in circuit serial programming).
    */
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
    
    
protected:  // internal utils
    /**
        Draws a row or column of pins.
        
        More precisely, this function draws a rectangle of appropriate size
        representing the pin (as seen from above the package), the pin number next
        to it and finally the pin name next to it (on the opposite side of the
        pin number).
        
        @param dc
            The DC where the pins are drawn
        @param pt
            The location at which the strip of pins should start
        @param PackageLen
            The len of the package (in pixels) in the direction specified by @a dir
        @param FirstPin
            The index of the first pin to draw
        @param LastPin
            The index of the last pin to draw + 1
        @param invertOrder
            If true then the pins are drawn from the last one back to the first one
        @param dir
            The direction of the strip of pins.
            If wxLEFT or wxRIGHT is given, then a vertical column of pins is drawn
            with the pin names drawn on the left of the pins (first case) or on
            the right of the pins (second case).
            If wxTOP or wxBOTTOM is given, then an horizontal row of pins is drawn
            with the pin names drawn above the pins (first case) or below them
            (second case). 
            The pin names in wxTOP/wxBOTTOM case are printed rotated by 90 degrees.
    */
    void DrawPins(wxDC& dc, const wxPoint& pt, unsigned int PackageLen,
                unsigned int FirstPin, unsigned int LastPin, bool invertOrder,
                wxDirection dir);
};

/**
    Describes a configuration value for a ConfigMask object.
    
    In Piklab XML files this class holds the information stored in a
    <tt>config -> mask -> value</tt> node:
    
    @code
    <config ...>
        <mask ...>
            <value value="0x0000" name="LP" />
        </mask>
    </config>
    @endcode
*/
class ConfigValue
{
public:
    ConfigValue() : Value(0) {}

    /**
        The name of this configuration value; e.g. "XT" to indicate that the oscillator
        of the PIC (if the ConfigMask object which contains us is called "FOSC")
        should use a quartz crystal.
    */
    wxString Name;
    
    /**
        The value that the set of bits grouped by the ConfigMask object which contains us
        should assume when the user selects this configuration value.
        
        E.g. for PIC18F2550 the bits 2-0 in the first config word (named PLLDIV2, PLLDIV1
        and PLLDIV0) should assume configuration value "101" when the user selects
        "Divide by 6 (24 MHz oscillator input)".
    */
    unsigned long Value;

    // leave out cname, sdcc_cname
};

/**
    Describes a configuration "mask".
    
    This is a set of bits inside a configuration word (represented by ConfigWord class)
    dedicated to the same purpose (e.g. the configuration of the PIC oscillator).
    
    In Piklab XML files this class holds the information stored in a
    <tt>config -> mask</tt> node:
    
    @code
    <config ...>
        <mask name="FOSC"  value="0x0003">
            ...
        </mask>
    </config>
    @endcode
*/
class ConfigMask
{
public:
    ConfigMask() : Value(0) {}

    /**
        The name of this set of configuration bits.
    */
    wxString Name;
    
    /**
        The mask to select the bits belonging to this ConfigMask from the entire configuration word ???
    */
    unsigned long Value;
    
    /**
        The various configuration values that this set of bits can assume.
    */
    vector<ConfigValue> Values;

    /**
        Returns the array containing the user-friendly names of the available
        configuration values.
    */
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
    Describes a word of configuration bits.
    
    This is usually 8 bits grouped in some ConfigMask objects.

    In Piklab XML files this class holds the information stored in a
    <tt>config</tt> node:

    @code
    <config offset="0x0" name="CONFIG1L" wmask="0xFF" bvalue="0x00">
        ...
    </config>
    @endcode
*/
class ConfigWord
{
public:
    ConfigWord() : Offset(0), WriteMask(0) {}

    /**
        The offset of this configuration word inside the configuration memory area of the PIC.
    */
    unsigned long Offset;
    
    /**
        The write mask. TODO: currently unused; remove?
    */
    unsigned long WriteMask;
    
    /**
        The name of this configuration word.
    */
    wxString Name;
    
    /**
        The different sets of bits in which this configuration word is divided.
    */
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
            picFamily(UPP_INVALID_PICFAMILY), DevId(0), DevIdMask(0), MinFreq(0), MaxFreq(0)
    {
        memset(ConfigMask, 0, sizeof(ConfigMask));
        for (unsigned int i=0; i<VOLTAGE_TYPE_MAX; i++)
            WorkVoltages[i] = ProgVoltages[i] = 0.0;
    }

    /// Is this PIC structure correctly initialized?
    bool ok() const
        { return !Name.empty() && picFamily != UPP_INVALID_PICFAMILY; }

    /// Returns true if the PIC is a 16bit device (e.g. Pic18)
    bool is16Bit()
        { return (Name.find("P18F")==0); }

    /// Returns the PIC name which starts with "PIC" instead of "P".
    wxString GetExtName() const
        {
            if(Name.empty())return Name;
            return wxString("PIC" + Name.substr(1)); 
        }

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

    /// The array of configuration blocks
    vector<ConfigWord> Config;

    /// Package descriptor
    vector<ChipPackage> Package;

    /// The programming voltages; see VoltageType enum.
    float ProgVoltages[VOLTAGE_TYPE_MAX];

    /// The working voltages; see VoltageType enum.
    float WorkVoltages[VOLTAGE_TYPE_MAX];

    /// The minimum & maximum frequencies for this PIC.
    float MinFreq, MaxFreq;

    // TODO: remove this in favour of "Config"                                              
    unsigned int ConfigMask[16];
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

        Use ok() to test if the ctor did successful find a valid PIC model.
    */
    PicType(string picType);

    /**
        Constructor to select a pic by devId, usage:
        @code Pictype* picType=new PicType(0x1240); @endcode

        Use ok() to test if the ctor did successful find a valid PIC model.
    */
    PicType(int devId);

    /** Returns the PIC that was selected in the ctor. */
    const Pic& getCurrentPic() const
        { return m_currentPic; }
    Pic& getCurrentPic()
        { return m_currentPic; }
        
    /** Returns true if the construction of this instance was successful. */
    bool ok() const 
        { return m_currentPic.ok(); }
        
    /** Returns the current PIC name as a wxString. */
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
