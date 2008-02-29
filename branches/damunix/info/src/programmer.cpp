#include "programmer.h"
#include <QMessageBox>
//
#define VENDOR_ID 0x10
#define TARGET_ID 0x10 
programmer::programmer(  ) 
{
	struct usb_bus *bus;
	struct usb_device *dev;
	static int		didUsbInit=0;
	char svend[128],sprod[128];
	usb_init();
	
	usb_find_busses();
	usb_find_devices();
	
	for (bus=usb_get_busses();bus;bus=bus->next)
	{
		for (dev=bus->devices;dev;dev=dev->next)
		{
			//qDebug(" 0x%.04X:0x%.04X ",dev->descriptor.idVendor,dev->descriptor.idProduct);
			_handle = usb_open(dev);
			if (!_handle)
			{
				qDebug("Warning !! cannot open device");
				continue;
			}
			if ((dev->descriptor.idVendor == VENDOR) && (dev->descriptor.idProduct == PRODUCT) )
				break;
			/*usb_get_string_simple(handle,dev->descriptor.iManufacturer,svend,sizeof(svend));
			usb_get_string_simple(handle,dev->descriptor.iProduct,sprod,sizeof(sprod));
			qDebug("%s : %s",svend,sprod);*/
			usb_close(_handle);
			_handle=NULL;
		}
			if(_handle!=NULL)
				break;
	}
	if (!_handle)
		QMessageBox::warning(NULL,QString("Error"),QString("Error ::: Device not found !!!"));
}


void programmer::getId(char * msg,int size)
{
	char id=0x20;
	if (_handle !=NULL)
	{
		int nBytes = usb_interrupt_write(_handle,1,(char*)&id,1,5000);
		if (nBytes < 0 )
		{
			QMessageBox::warning(NULL,QString("USB Error"),QString(usb_strerror()));
			return ;
		}
			
			
		nBytes = usb_interrupt_read(_handle,1,(char*)msg,size,5000);
		if (nBytes < 0 )
			QMessageBox::warning(NULL,QString("USB Error"),QString(usb_strerror()));
		else
		{
			
			QMessageBox::warning(NULL,QString("USB Receive"),QString ("message in the terminal"));
			qDebug("Message : %d %d", (int)msg[0],(int)msg[1]);
		}
	}
	
}

bool programmer::connected(void) 
{
		if (_handle == NULL)
			return 0;
		else
			return 1;
}


void programmer::write(const char * msg,int size)
{
	if (_handle != NULL)
	{
		//qDebug ("Device Found!!");
		//bzero(bytes,sizeof(bytes));
		//int nBytes = usb_control_msg(handle,USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,1,0,0,bytes,sizeof(bytes),5000);
		int nBytes = usb_interrupt_write(_handle,1,(char*)msg,size,5000);
		if (nBytes < 0 )
			QMessageBox::warning(NULL,QString("USB Error"),QString(usb_strerror()));
	}
	else 
		QMessageBox::warning(NULL,QString("Error"),QString("Error ::: You are not connect!!!"));
}
//
