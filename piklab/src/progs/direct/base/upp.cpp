#include "upp.h"
#include <usb.h> /* libusb header */
#include <stdio.h>
#include <string.h>


#include "common/global/global.h"
#if defined(HAVE_PPDEV)
#  include <linux/ppdev.h>
#  include <linux/parport.h>
#  include <fcntl.h>
#  include <sys/ioctl.h>
#  include <unistd.h> // needed on some system
#  include <errno.h>
#elif defined(HAVE_PPBUS)
#  include <machine/cpufunc.h>
#  include <dev/ppbus/ppi.h>
#  include <dev/ppbus/ppbconf.h>
#  include <fcntl.h>
#  include <errno.h>
#endif
#include "common/common/misc.h"


Port::UPP::UPP(const QString &device, Log::Base &base)
  : Base(base), _device(device), _fd(INVALID_HANDLE)
{}


/* PICkit USB values */ /* From Microchip firmware */
const static int vendorID=0x04d8; // Microchip, Inc
const static int productID=0x000E; // PICDEM FS USB demo app
const static int configuration=1; /*  Configuration 1*/
const static int interface=0;	/* Interface 0 */

/* change by Xiaofan */
/* libusb-win32 requires the correct endpoint address
   including the direction bits. This is the most important
   differece between libusb-win32 and libusb.
*/  
//const static int endpoint=1; /* first endpoint for everything */
const static int endpoint_in=0x1; /* endpoint 0x81 address for IN */
const static int endpoint_out=1; /* endpoint 1 address for OUT */

const static int timeout=20000; /* timeout in ms */

/* PICDEM FS USB max packet size is 64-bytes */
const static int reqLen=64;
typedef unsigned char byte;

void bad(const char *why) {
	fprintf(stderr,"Fatal error> %s\n",why);
	exit(17);
}

/****************** Internal I/O Commands *****************/

/** Send this binary string command. */
void Port::UPP::send_usb(struct usb_dev_handle * d, int len, const char * src)
{
   int r = usb_interrupt_write(d, endpoint_out, (char *)src, len, timeout);
//   if( r != reqLen )
   /*if( r < 0 )
   {
	  perror("usb PICDEM FS USB write"); bad("USB write failed"); 
   }*/
}

/** Read this many bytes from this device */
void Port::UPP::recv_usb(struct usb_dev_handle * d, int len, const char * dest)
{
//   int i;
   int r = usb_interrupt_read(d, endpoint_in, dest, len, timeout);
   if( r != len )
   {
	  perror("usb PICDEM FS USB read"); bad("USB read failed"); 
   }
}


extern int usb_debug;

/* Find the first USB device with this vendor and product.
   Exits on errors, like if the device couldn't be found.
*/
struct usb_dev_handle *usb_picdem_fs_usb_open(void)
{
  struct usb_device * device;
  struct usb_bus * bus;

/*#ifndef WIN32  
  if( geteuid()!=0 )
	 bad("This program must be run as root, or made setuid root");
#endif */ 
#ifdef USB_DEBUG
  usb_debug=4; 
#endif

   /* (libusb setup code stolen from John Fremlin's cool "usb-robot") */

  // added the two debug lines  
  usb_set_debug(255);
  printf("setting USB debug on by adding usb_set_debug(255) \n");
  //End of added codes

  usb_init();
  usb_find_busses();
  usb_find_devices();

/* change by Xiaofan */  
/* libusb-win32: not using global variable like usb_busses*/
/*  for (bus=usb_busses;bus!=NULL;bus=bus->next) */  
  for (bus=usb_get_busses();bus!=NULL;bus=bus->next) 
  {
	 struct usb_device * usb_devices = bus->devices;
	 for( device=usb_devices; device!=NULL; device=device->next )
	 {
		if( device->descriptor.idVendor == vendorID &&
			device->descriptor.idProduct == productID )
		{
		   struct usb_dev_handle *d;
		   printf("Found USB PICDEM FS USB Demo Board as device '%s' on USB bus %s\n",
				   device->filename,
				   device->bus->dirname);
		   d = usb_open(device);
		   if( d )
		   { /* This is our device-- claim it */
//			  byte retData[reqLen];
			  if( usb_set_configuration(d, configuration) ) 
			  {
				 bad("Error setting USB configuration.\n");
			  }
			  if( usb_claim_interface(d, interface) ) 
			  {
				 bad("Claim failed-- the USB PICDEM FS USB is in use by another driver.\n");
			  }
			  printf("Communication established.\n");
			  //picdem_fs_usb_read_version(d);
			  return d;
		   }
		   else 
			  bad("Open failed for USB device");
		}
		/* else some other vendor's device-- keep looking... */
	 }
  }
  bad("Could not find USB PICDEM FS USB Demo Board--\n"
      "you might try lsusb to see if it's actually there.");
  return NULL;
}



/*int main(int argc, char ** argv) 
{
   struct usb_dev_handle * picdem_fs_usb;
   int i;
   byte outbuffer[reqLen],inbuffer[reqLen];
   outbuffer[0]=0xC3;
   picdem_fs_usb = usb_picdem_fs_usb_open();
   //usb_claim_interface(picdem_fs_usb, 0);
   //usb_clear_halt(picdem_fs_usb, endpoint_out);
   //usb_resetep(picdem_fs_usb, endpoint_out);
if (argc==2)
   
   outbuffer[0]=0x80|atoi(argv[1]);
   send_usb(picdem_fs_usb, 1, outbuffer);
   outbuffer[0]=0xC0|atoi(argv[1]);
   send_usb(picdem_fs_usb, 1, outbuffer);

   recv_usb(picdem_fs_usb, 1, inbuffer);
   //for (i=0;i<reqLen;i++)
	   printf("Received byte#%i: 0x%X\n",0,inbuffer[0]);
 

   usb_close(picdem_fs_usb);
   return 0;
}*/
