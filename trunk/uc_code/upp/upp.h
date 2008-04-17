/*********************************************************************
 *
 *                UsbPicProg v0.1
 *                Frans Schreuder 16-06-2007
 ********************************************************************/

#ifndef UPP_H
#define UPP_H

#define CMD_ERASE 0x10
#define CMD_READ_ID 0x20
#define CMD_WRITE_CODE 0x30
#define CMD_READ_CODE 0x40
#define CMD_WRITE_DATA 0x50
#define CMD_READ_DATA 0x60
#define CMD_WRITE_CONFIG 0x70
#define CMD_SET_PICTYPE 0x80
#define CMD_FIRMWARE_VERSION 0x90

/** P U B L I C  P R O T O T Y P E S *****************************************/
void UserInit(void);
void timer1Init(void);
void ProcessIO(void);
void set_pictype(unsigned char* data);
void setLeds(char n);

#endif //UPP_H
