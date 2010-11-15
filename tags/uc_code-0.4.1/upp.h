/**************************************************************************
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
**************************************************************************/

#ifndef UPP_H
#define UPP_H

typedef enum
{
	CMD_ERASE = 0x10,
	CMD_READ_ID = 0x20,
	CMD_WRITE_CODE = 0x30,
	CMD_READ_CODE = 0x40,
	CMD_WRITE_DATA = 0x50,
	CMD_READ_DATA = 0x60,
	CMD_WRITE_CONFIG = 0x70,
	CMD_SET_PICTYPE = 0x80,
	CMD_FIRMWARE_VERSION = 0x90,
	CMD_DEBUG = 0xA0,
	CMD_GET_PIN_STATUS = 0xB0,
	CMD_SET_PIN_STATUS = 0xC0
}CMD_UPP;

typedef enum
{
	SUBCMD_PIN_PGC = 0x01,
	SUBCMD_PIN_PGD = 0x02,
	SUBCMD_PIN_VDD = 0x03,
	SUBCMD_PIN_VPP = 0x04,
	SUBCMD_PIN_VPP_VOLTAGE = 0x05
}SUBCMD_PIN;

typedef enum
{
	PIN_STATE_0V = 0x01,
	PIN_STATE_3_3V = 0x02,
	PIN_STATE_5V = 0x03,
	PIN_STATE_12V = 0x04,
	PIN_STATE_FLOAT = 0x05,
	PIN_STATE_INPUT = 0x06
}PIN_STATE;

#define LEDS_ON 0x04
#define LEDS_RD 0x02
#define LEDS_WR 0x01

void InitAdc(void);
void ReadAdc(unsigned char* data);

void UserInit(void);
void timer1Init(void);
void timer0Init(void);
void ProcessIO(void);
unsigned char set_pictype(unsigned char* data);
void setLeds(char n);

#endif // UPP_H
