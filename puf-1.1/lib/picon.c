/*-------------------------------------------------------------------------
  picon.c - USB PIC console

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

/* $Id: picon.c,v 1.2 2006/12/01 22:23:31 gaufille Exp $ */

#include "common_types.h"
#include "picon.h"
#include "usb.h"
#include "boot_iface.h"
#include <pic18fregs.h>
#include <stdio.h>

static uchar last_packet_was_void; // Has the last packet been sent ?
static uint num_bytes_to_send;     // Number of bytes to be send (linear)
static uchar num_bytes;            // Number of bytes to be send (circular)
static uchar send_in_progress;     // TRUE when the buffer is being sent

/* Circular buffer pointers */
static uchar* bottom;         // Point to the next char to be sent
static uchar* top;            // Point to the first free char in the buffer
static uchar* tmp_bottom;     // Value to be used for bottom after the current packet

/* EP buffer has to be defined on the application side
 * using DEFINE_PICON_BUFFER(size)
 */
extern const uint picon_buffer_size;
extern volatile uchar* const Picon_InBuffer;
extern const uchar picon_ep;

/*
 *    -------------++++++++++++++++++++++++----------------------
 *    ^            ^                       ^                     ^
 *  InBuffer    bottom                    top             InBuffer + PICON_BUFFER_SIZE
 * 
 * so, the buffer is full or void when top == bottom
 * num_bytes can be used to differentiate these 2 cases
 * 
 * send_in_progress is used as a semaphore. putchar will only
 * prepare the send if nothing is in progress. picon_in
 * is responsible for releasing send_in_progress.
 * 
 * Each train of packets must be finished with a incomplete or void packet.
 * last_packet_sent indicates if the previous sent was the last one.
 * 
 */

void picon_init(void)
{
    // In this interface, PICON EP is used as an interrupt EP

    // Init circular pointers, counter and semaphore
    bottom           = Picon_InBuffer;
    top              = Picon_InBuffer;
    num_bytes        = 0;
    send_in_progress = FALSE;
    
    // Configuration of the end point

    // Set DAT1 so that the first call to prepare_in
    // will switch to DAT0, which is correct for the first
    // message
    EP_IN_BD(picon_ep).Stat.uc = BDS_UCPU | BDS_DAT1 | BDS_DTSEN;

    // Init PICON EP as an IN EP
    *((&UEP0) + picon_ep) = EPHSHK_EN | EPINEN_EN | EPCONDIS_EN;
}

void prepare_next_packet(void)
{
    last_packet_was_void = (num_bytes_to_send < PICON_PACKET_SIZE);
        
    // Set the number of bytes to be send in the coming packet
    if(last_packet_was_void)
    {
        num_bytes -= num_bytes_to_send;
    }
    else
    {
        num_bytes -= PICON_PACKET_SIZE;
    }
    
    // Move the start address of the buffer
    EP_IN_BD(picon_ep).ADR = (uchar __data *) bottom;

    // Prepare the EP buffer
    tmp_bottom = bottom;
    fill_in_buffer(picon_ep, &tmp_bottom, PICON_PACKET_SIZE, &num_bytes_to_send);
    
    if(tmp_bottom >= (Picon_InBuffer + picon_buffer_size))
    {
        // cycling the buffer
        tmp_bottom -= picon_buffer_size;
    }

    // Gives control of the buffer to the SIE
    if(EP_IN_BD(picon_ep).Stat.DTS == 0)
    {
        EP_IN_BD(picon_ep).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
    }
    else
    {
        EP_IN_BD(picon_ep).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
    }
}

void prepare_in(void)
{
    uchar* top_send_buffer;
    char ib;
    
    // Critical section
    ib = INTCONbits.GIE;
    INTCONbits.GIE = 0;
    
    if((top <= bottom) && (num_bytes > 0))
    {
        // cycling the buffer
        top_send_buffer = Picon_InBuffer + picon_buffer_size;
    }
    else
    {
        top_send_buffer = top;
    }
    
    // Set the total number of bytes to be sent
    num_bytes_to_send = top_send_buffer - bottom;
    
    prepare_next_packet();
    
    INTCONbits.GIE = ib;
}

void picon_in(void)
{
    char ib;
    
    // Critical section
    ib = INTCONbits.GIE;
    INTCONbits.GIE = 0;

    bottom = tmp_bottom; // Flush the sent chars
    
    if((bottom == Picon_InBuffer) && (num_bytes != 0))
    {
        num_bytes_to_send = num_bytes;
    }
    
    if((num_bytes_to_send == 0) && last_packet_was_void)
    {
        // Arms the SIE if there is still something in the buffer
        if(num_bytes != 0)
        {
            prepare_in();
        }
        else
        {
           
            // There is nothing more to send, so keep
            // the EP buffer under the CPU responsability
            // Release the semaphore
            send_in_progress = FALSE;
        }
        
    
        INTCONbits.GIE = ib;
        return;
    }
        
    prepare_next_packet();
    
    INTCONbits.GIE = ib;
}

PUTCHAR(c)
{
    char ib;
    
    // Critical section
    ib = INTCONbits.GIE;
    INTCONbits.GIE = 0;

    if(top >= (Picon_InBuffer + picon_buffer_size))
    {
        top = Picon_InBuffer;
    }
    
    if((top == bottom) && (num_bytes != 0))
    {
        // Buffer full
    
        INTCONbits.GIE = ib;
        return;
    }

    *top = c;
    top++;
    num_bytes++;

    // Arms the SIE if not already done
    if(!send_in_progress)
    {
        send_in_progress = TRUE;
        prepare_in();
    }
    
    INTCONbits.GIE = ib;
}
