/*-------------------------------------------------------------------------
  ep2.c - USB endpoint 2 callbacks

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

/* $Id: ep2.c,v 1.5 2006/04/18 19:44:32 gaufille Exp $ */

#include "ep2.h"
#include "usb.h"
#include "debug.h"
#include "flash.h"
#include <pic18fregs.h>

static uchar last_send_was_null;

#pragma udata usb_buf ep2_InBuffer
volatile uchar ep2_InBuffer[EP2_BUFFER_SIZE];

void ep2_init(void)
{
    debug("ep2_init\n");
    // Set DAT1 so that the first call to prepare_ep2_in
    // will switch to DAT0, which is correct for the first
    // message
    EP_IN_BD(2).Stat.uc = BDS_UCPU | BDS_DAT1 | BDS_DTSEN;
    UEP2 = EPHSHK_EN | EPINEN_EN | EPCONDIS_EN;       // Init EP2 as an IN EP
}

void prepare_ep2_in(void)
{
    debug("prepare_ep2_in\n");
    last_send_was_null = (ep2_num_bytes_to_send < EP2_BUFFER_SIZE);
    EP_IN_BD(2).Cnt = ep2_num_bytes_to_send;
    EP_IN_BD(2).ADR = (uchar __data *)&ep2_InBuffer;
    fill_in_buffer(2, &ep2_source_data, EP2_BUFFER_SIZE, &ep2_num_bytes_to_send);
    if(EP_IN_BD(2).Stat.DTS == 0)
    {
        EP_IN_BD(2).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
    }
    else
    {
        EP_IN_BD(2).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
    }
}

void ep2_in(void)
{
    debug2("ep2_in %d\n", (uint) ep2_num_bytes_to_send);
    if((ep2_num_bytes_to_send == 0) && last_send_was_null)
    {
        // There is nothing more to send, so keep
        // the EP2 buffer under the CPU responsability
        return;
    }
    last_send_was_null = (ep2_num_bytes_to_send < EP2_BUFFER_SIZE);
    EP_IN_BD(2).Cnt = ep2_num_bytes_to_send;
    fill_in_buffer(2, &ep2_source_data, EP2_BUFFER_SIZE, &ep2_num_bytes_to_send);
    if(EP_IN_BD(2).Stat.DTS == 0)
    {
        EP_IN_BD(2).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
    }
    else
    {
        EP_IN_BD(2).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
    }
}
