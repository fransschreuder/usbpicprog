/*-------------------------------------------------------------------------
  usb_descriptors.h - USB device, interface, class, ep, string descriptors

             (c) 2006 Pierre Gaufillet <pierre.gaufillet@magic.fr> 

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
-------------------------------------------------------------------------*/

/* $Id: usb_descriptors.h,v 1.3 2006/04/08 17:29:00 gaufille Exp $ */

#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

#include "common_types.h"
#include "ep0.h"
#include "ep1.h"
#include "ep2.h"

/* Descriptor Types */
#define DEVICE_DESCRIPTOR        0x01
#define CONFIGURATION_DESCRIPTOR 0x02
#define STRING_DESCRIPTOR        0x03
#define INTERFACE_DESCRIPTOR     0x04
#define ENDPOINT_DESCRIPTOR      0x05

/* Configuration Descriptor */

/* Configuration Attributes */
#define DEFAULT               (0x01<<7)         // Default value (bit 7 has to be set)
#define SELF_POWERED          (0x01<<6)         // Self-powered
#define REMOTE_WAKE_UP        (0x01<<5)         // Remote wakeup

/* Endpoint Descriptor */

/* bmAttributes : Endpoint Transfer Type */
#define CONTROL          0x00            // Control Transfer
#define ISOCHRONOUS      0x01            // Isochronous Transfer
#define BULK             0x02            // Bulk Transfer
#define INTERRUPT        0x03            // Interrupt Transfer

/* bEndpointAddress : Endpoint number */
#define EP(n) (n & 0x0f)

/* bEndpointAddress : direction */
#define IN_EP  0x80
#define OUT_EP 0x00


/******************************************************************************
 * USB Device Descriptor (table 9-8)
 *****************************************************************************/
typedef struct
{
    uchar bLength;
    uchar bDescriptorType;
    uint  bcdUSB;
    uchar bDeviceClass;
    uchar bDeviceSubClass;
    uchar bDeviceProtocol;
    uchar bMaxPacketSize0;
    uint  idVendor;
    uint  idProduct;
    uint  bcdDevice;
    uchar iManufacturer;
    uchar iProduct;
    uchar iSerialNumber;
    uchar bNumConfigurations;
} USB_Device_Descriptor;

/******************************************************************************
 * USB Configuration Descriptor (table 9-10)
 *****************************************************************************/
typedef struct 
{
    uchar bLength;
    uchar bDescriptorType;
    uint  wTotalLength;
    uchar bNumInterfaces;
    uchar bConfigurationValue;
    uchar iConfiguration;
    uchar bmAttributes;
    uchar bMaxPower;
} USB_Configuration_Descriptor;

/******************************************************************************
 * USB Interface Descriptor (table 9-12)
 *****************************************************************************/
typedef struct _USB_INTF_DSC
{
    uchar bLength;
    uchar bDescriptorType;
    uchar bInterfaceNumber;
    uchar bAlternateSetting;
    uchar bNumEndpoints;
    uchar bInterfaceClass;
    uchar bInterfaceSubClass;
    uchar bInterfaceProtocol;
    uchar iInterface;
} USB_Interface_Descriptor;

/******************************************************************************
 * USB Endpoint Descriptor (table 9-13)
 *****************************************************************************/
typedef struct
{
    uchar bLength;
    uchar bDescriptorType;
    uchar bEndpointAddress;
    uchar bmAttributes;
    uint  wMaxPacketSize;
    uchar bInterval;
} USB_Endpoint_Descriptor;


/******************************************************************************
 * USB Composite Descriptors (used for GET_DSC/CFG request)
 *****************************************************************************/
typedef struct                        
{   USB_Configuration_Descriptor  cd;                   
    USB_Interface_Descriptor      i0; 
} USB_Default_Composite_Descriptor;

typedef struct                        
{   USB_Configuration_Descriptor  cd;                   
    USB_Interface_Descriptor      i0; 
    USB_Endpoint_Descriptor       ep_dsc[2];                
} USB_Flash_Composite_Descriptor;


/******************************************************************************
 * USB Endpoints callbacks
 *****************************************************************************/

extern const USB_Device_Descriptor boot_device_descriptor;
extern const uchar * const boot_configuration_descriptor[];
extern const uchar * const boot_string_descriptor[];
extern const uchar str0[];
extern const uchar str1[];
extern const uchar str2[];
extern const uchar str3[];

extern void (** const boot_ep_init[])(void);
extern void (** const boot_ep_in[])(void);
extern void (** const boot_ep_out[])(void);
extern void (** const boot_ep_setup[])(void);

#endif /*USB_DESCRIPTORS_H_*/
