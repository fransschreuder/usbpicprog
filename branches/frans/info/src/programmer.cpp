#include "programmer.h"
#include <QMessageBox>
//
#define VENDOR_ID 0x10
#define TARGET_ID 0x10 
programmer::programmer(  ) 
{
	struct usb_bus *bus;
	struct usb_device *dev;
	//static int		didUsbInit=0;
	//char svend[128],sprod[128];
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
			
			//QMessageBox::warning(NULL,QString("USB Receive"),QString ("message in the terminal"));
			qDebug("Message : %2X %2X, %i bytes", (int)msg[0]&0xFF,(int)msg[1]&0xFF, nBytes);
		}
	}
	
}

int programmer::read_code(char * msg,int address,int size,int lastblock)
{
	char id=0x40;
	char sendstr[10];
	if (_handle !=NULL)
	{
		sendstr[0]=id;
		sendstr[1]=(char)size;
		sendstr[2]=(char)((address>>16)&0xFF);
		sendstr[3]=(char)((address>>8)&0xFF);
		sendstr[4]=(char)(address&0xFF);
		sendstr[5]=(char)lastblock;
		
		
	    
		int nBytes = usb_interrupt_write(_handle,1,sendstr,6,5000);
		if (nBytes < 0 )
		{
			QMessageBox::warning(NULL,QString("USB Error"),QString(usb_strerror()));
			return nBytes;
		}
			
		nBytes = usb_interrupt_read(_handle,1,(char*)msg,64,5000);
		if (nBytes < 0 )
			QMessageBox::warning(NULL,QString("USB Error"),QString(usb_strerror()));
		else
		{
			
			//QMessageBox::warning(NULL,QString("USB Receive"),QString ("message in the terminal"));
			//qDebug("Message : %2X %2X, %i bytes", (int)msg[0]&0xFF,(int)msg[1]&0xFF, nBytes);
		}
		return nBytes;
	}
}

int programmer::read_data(char * msg,int address,int size,int lastblock)
{
	char id=0x60;
	char sendstr[10];
	if (_handle !=NULL)
	{
		sendstr[0]=id;
		sendstr[1]=(char)size;
		sendstr[2]=(char)((address>>8)&0xFF);
		sendstr[3]=(char)(address&0xFF);
		sendstr[4]=(char)lastblock;
		
		
	    
		int nBytes = usb_interrupt_write(_handle,1,sendstr,6,5000);
		if (nBytes < 0 )
		{
			QMessageBox::warning(NULL,QString("USB Error"),QString(usb_strerror()));
			return nBytes;
		}
			
		nBytes = usb_interrupt_read(_handle,1,(char*)msg,64,5000);
		if (nBytes < 0 )
			QMessageBox::warning(NULL,QString("USB Error"),QString(usb_strerror()));
		else
		{
			
			//QMessageBox::warning(NULL,QString("USB Receive"),QString ("message in the terminal"));
			//qDebug("Message : %2X %2X, %i bytes", (int)msg[0]&0xFF,(int)msg[1]&0xFF, nBytes);
		}
		return nBytes;
	}	return -1;
	
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

int programmer::readString(char* msg)
{
	int nBytes = usb_interrupt_read(_handle,1,(char*)msg,64,5000);
		if (nBytes < 0 )
		{
			QMessageBox::warning(NULL,QString("USB Error"),QString(usb_strerror()));
		
		}
		return nBytes;
		
}

const char * programmer::readResponse(void)
{
	
	char msg[64];
	int nBytes = usb_interrupt_read(_handle,1,(char*)msg,64,5000);
		if (nBytes < 0 )
		{
			QMessageBox::warning(NULL,QString("USB Error"),QString(usb_strerror()));
			return NULL;
		}
			
	else
	{
		switch(msg[0])
		{
			case 1:
				return "Received: 1, Ok";
				break;
			case 2:
				return "Received: 2, asked for next block";
				break;
			case 3:
				return "Error, PIC not supported for this action";
				break;
			default:
				return "Received unexpected response";
				break;	
		}
			
	}
}

//
