/*********************************************************************
 *
 *             Microchip USB C18 Firmware -  Generic
 *
 *********************************************************************
 * FileName:        usbgen.c
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
#include "system\usb\usb.h"

#ifdef USB_USE_GEN

/** V A R I A B L E S ********************************************************/
#pragma udata
byte usbgen_rx_len;

/** P R I V A T E  P R O T O T Y P E S ***************************************/

/** D E C L A R A T I O N S **************************************************/
#pragma code

/** U S E R  A P I ***********************************************************/

/******************************************************************************
 * Function:        void USBGenInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        USBGenInitEP initializes generic endpoints, buffer
 *                  descriptors, internal state-machine, and variables.
 *                  It should be called after the USB host has sent out a
 *                  SET_CONFIGURATION request.
 *                  See USBStdSetCfgHandler() in usb9.c for examples.
 *
 * Note:            None
 *****************************************************************************/
void USBGenInitEP(void)
{   
    usbgen_rx_len = 0;
    
    USBGEN_UEP = EP_OUT_IN|HSHK_EN;             // Enable 2 data pipes

    /*
     * Do not have to init Cnt of IN pipes here.
     * Reason:  Number of bytes to send to the host
     *          varies from one transaction to
     *          another. Cnt should equal the exact
     *          number of bytes to transmit for
     *          a given IN transaction.
     *          This number of bytes will only
     *          be known right before the data is
     *          sent.
     */
    USBGEN_BD_OUT.Cnt = sizeof(usbgen_out);     // Set buffer size
    USBGEN_BD_OUT.ADR = (byte*)&usbgen_out;     // Set buffer address
    USBGEN_BD_OUT.Stat._byte = _USIE|_DAT0|_DTSEN;// Set status

    USBGEN_BD_IN.ADR = (byte*)&usbgen_in;      // Set buffer address
    USBGEN_BD_IN.Stat._byte = _UCPU|_DAT1;      // Set buffer status

}//end USBGenInitEP

/******************************************************************************
 * Function:        void USBGenWrite(byte *buffer, byte len)
 *
 * PreCondition:    mUSBGenTxIsBusy() must return false.
 *
 *                  Value of 'len' must be equal to or smaller than
 *                  USBGEN_EP_SIZE
 *                  For an interrupt/bulk endpoint, the largest buffer size is
 *                  64 bytes.
 *
 * Input:           buffer  : Pointer to the starting location of data bytes
 *                  len     : Number of bytes to be transferred
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Use this macro to transfer data located in data memory.
 *
 *                  Remember: mUSBGenTxIsBusy() must return false before user
 *                  can call this function.
 *                  Unexpected behavior will occur if this function is called
 *                  when mUSBGenTxIsBusy() != 0
 *
 *                  Typical Usage:
 *                  if(!mUSBGenTxIsBusy())
 *                      USBGenWrite(buffer, 3);
 *
 * Note:            None
 *****************************************************************************/
void USBGenWrite(byte *buffer, byte len)
{
	byte i;
	
    /*
     * Value of len should be equal to or smaller than USBGEN_EP_SIZE.
     * This check forces the value of len to meet the precondition.
     */
	if(len > USBGEN_EP_SIZE)
	    len = USBGEN_EP_SIZE;

   /*
    * Copy data from user's buffer to dual-ram buffer
    */
    for (i = 0; i < len; i++)
    	usbgen_in[i] = buffer[i];

    USBGEN_BD_IN.Cnt = len;
    mUSBBufferReady(USBGEN_BD_IN);

}//end USBGenWrite

/******************************************************************************
 * Function:        byte USBGenRead(byte *buffer, byte len)
 *
 * PreCondition:    Value of input argument 'len' should be smaller than the
 *                  maximum endpoint size responsible for receiving report
 *                  data from USB host for HID class.
 *                  Input argument 'buffer' should point to a buffer area that
 *                  is bigger or equal to the size specified by 'len'.
 *
 * Input:           buffer  : Pointer to where received bytes are to be stored
 *                  len     : The number of bytes expected.
 *
 * Output:          The number of bytes copied to buffer.
 *
 * Side Effects:    Publicly accessible variable usbgen_rx_len is updated
 *                  with the number of bytes copied to buffer.
 *                  Once USBGenRead is called, subsequent retrieval of
 *                  usbgen_rx_len can be done by calling macro
 *                  mUSBGenGetRxLength().
 *
 * Overview:        USBGenRead copies a string of bytes received through
 *                  the OUT endpoint to a user's specified location. 
 *                  It is a non-blocking function. It does not wait
 *                  for data if there is no data available. Instead it returns
 *                  '0' to notify the caller that there is no data available.
 *
 * Note:            If the actual number of bytes received is larger than the
 *                  number of bytes expected (len), only the expected number
 *                  of bytes specified will be copied to buffer.
 *                  If the actual number of bytes received is smaller than the
 *                  number of bytes expected (len), only the actual number
 *                  of bytes received will be copied to buffer.
 *****************************************************************************/
byte USBGenRead(byte *buffer, byte len)
{
    usbgen_rx_len = 0;
    
    if(!mUSBGenRxIsBusy())
    {
        /*
         * Adjust the expected number of bytes to equal
         * the actual number of bytes received.
         */
        if(len > USBGEN_BD_OUT.Cnt)
            len = USBGEN_BD_OUT.Cnt;
        
        /*
         * Copy data from dual-ram buffer to user's buffer
         */
        for(usbgen_rx_len = 0; usbgen_rx_len < len; usbgen_rx_len++)
            buffer[usbgen_rx_len] = usbgen_out[usbgen_rx_len];

        /*
         * Prepare dual-ram buffer for next OUT transaction
         */
        USBGEN_BD_OUT.Cnt = sizeof(usbgen_out);
        mUSBBufferReady(USBGEN_BD_OUT);
    }//end if
    
    return usbgen_rx_len;
    
}//end USBGenRead

#endif //def USB_USE_GEN

/** EOF usbgen.c *************************************************************/
