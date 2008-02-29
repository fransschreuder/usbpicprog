/*********************************************************************
 *
 *                UsbPicProg v0.1
 *                Frans Schreuder 16-06-2007
 ********************************************************************/

#ifndef UPP_H
#define UPP_H

/** P U B L I C  P R O T O T Y P E S *****************************************/
void UserInit(void);
void timer1Init(void);
void ProcessIO(void);
void setLeds(char n);
void Exercise_Example(void);

/*
union {
	struct
	{
		byte cmd;
		byte size;
		byte data[128];
	};
	byte dataUsb[130] ;
} USBdata;
*/
#endif //UPP_H
