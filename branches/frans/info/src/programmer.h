#ifndef PROGRAMMER_H
#define PROGRAMMER_H
#include <usb.h>
#include <QObject>
#include <QDebug>
//
#define VENDOR 0x04D8
#define PRODUCT 0x000E
//
class programmer  :public QObject
{
Q_OBJECT
public:
	programmer();
	void write(const char* msg,int size);
	void getId(char * msg,int size);
	int read_code(char * msg,int address,int size,int lastblock);
	int read_data(char * msg,int address,int size,int lastblock);
	bool connected(void);

private :
	usb_dev_handle	*_handle;
};
#endif
