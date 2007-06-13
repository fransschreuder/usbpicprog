/*********************************************************************
 *
 *                Thermo Sensor with SPI(TM) Interface
 *
 *********************************************************************
 * FileName:        temperature.c
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 2.30.01+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rawin Rojvanit       11/19/04    Original.
 ********************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system\typedefs.h"
#include <spi.h>
#include "io_cfg.h"
#include "user\temperature.h"

/** V A R I A B L E S ********************************************************/
#pragma udata
WORD temperature;           // Raw data format
char tempString[10];        // Buffer for storing data in ASCII format

/** P R I V A T E  P R O T O T Y P E S ***************************************/

/** D E C L A R A T I O N S **************************************************/
#pragma code
/******************************************************************************
 * Function:        void InitTempSensor(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initializes SPI interface & chip select line
 *
 * Note:            None
 *****************************************************************************/
void InitTempSensor(void)
{
    cs_temp_sensor = 1;
    tris_cs_temp_sensor = OUTPUT_PIN;
    OpenSPI(SPI_FOSC_64, MODE_11, SMPMID);
    
    // Initialize readable values - default to room temperature
    temperature._word = 0x0C87;       // 25 degree celsius
    UpdateCelsiusASCII();
    
}//end InitTempSensor

/******************************************************************************
 * Function:        void AcquireTemperature(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 *****************************************************************************/
BOOL AcquireTemperature(void)
{
    cs_temp_sensor = 0;
    MSB(temperature) = ReadSPI();
    LSB(temperature) = ReadSPI();
    cs_temp_sensor = 1;
    
    if(temperature.LowB.b2 == 0)
        return FAIL;

    return OK;
}//end AcquireTemperature

/******************************************************************************
 * Function:        void UpdateCelsiusASCII(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine converts data output to ASCII string
 *
 * Note:            None
 *****************************************************************************/
void UpdateCelsiusASCII(void)
{
    WORD temp;
    byte i;
        
    temp._word = temperature._word >> 3;
    
    if(temp.HighB.b4 == 0)
    {
        temp.HighB._byte &= 0b00011111;
        tempString[0] = '+';
    }
    else
    {
        temp.HighB._byte |= 0b11100000;
        tempString[0] = '-';
        temp._word = temp._word ^ 0xFFFF;   // Negate
        temp._word++;
    }//end if

    temp._word = (temp._word*10U) >> 4;     // Turn into celsius xx.x
    
    /* Populate string */
    for(i=4;i>0;i--)
    {
        tempString[i] = (((char)(temp._word % 10)) & 0x0F) | 0x30;
        temp._word /= 10;
    }//end for
    
    /* Turn leading zeros into spaces */
    if(tempString[1] == '0')
    {
        tempString[1] = ' ';
        if(tempString[2] == '0')
            tempString[2] = ' ';
    }//end if
    
    /* Adjust decimal digit */
    tempString[5] = tempString[4];
    tempString[4]='.';
    tempString[6]=0xF8;     // Degree symbol
    tempString[7]='C';
    tempString[8]='\r';
    tempString[9]=0x00;     // Null-Terminated
    
}//end UpdateCelsiusASCII

/** EOF temperature.c ********************************************************/
