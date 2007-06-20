/*-------------------------------------------------------------------------
  ep1.c - USB endpoint 1 callbacks

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

/* $Id: ep1.c,v 1.5 2006/04/18 19:44:32 gaufille Exp $ */

#include "ep1.h"
#include "ep2.h"
#include "usb.h"
#include "debug.h"
#include "flash.h"
#include <pic18fregs.h>
#include <delay.h>


#pragma udata usb_buf ep1_OutBuffer
volatile uchar ep1_OutBuffer[EP1_BUFFER_SIZE];

void ep1_init(void)
{
    debug("ep1_init\n");
    EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
    EP_OUT_BD(1).ADR = (uchar __data *)&ep1_OutBuffer;
    EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
    UEP1 = EPHSHK_EN | EPOUTEN_EN | EPCONDIS_EN;       // Init EP1 as an OUT EP
}

void ep1_out(void)
{
    debug("ep1_out\n");
    if(EP_OUT_BD(1).Cnt >= 1)
    {
        switch(ep1_OutBuffer[0])
        {
            case ERASE_FLASH_CMD:
                debug("ERASE_FLASH_CMD\n");
                TBLPTRL = ep1_OutBuffer[1];
                TBLPTRH = ep1_OutBuffer[2];
                TBLPTRU = ep1_OutBuffer[3];
                erase_block();
                break;
            case WRITE_FLASH_CMD:
                debug("WRITE_FLASH_CMD\n");
                TBLPTRL = ep1_OutBuffer[1];
                TBLPTRH = ep1_OutBuffer[2];
                TBLPTRU = ep1_OutBuffer[3];
                write_block((__data uchar *)&(ep1_OutBuffer[4]));
                break;
            case READ_FLASH_CMD:
                debug("READ_FLASH_CMD\n");
                ep2_num_bytes_to_send = EP2_BUFFER_SIZE;
                // TODO: Use a Pointer union to set that
                ep2_source_data = (uchar __code *) (ep1_OutBuffer[1] + ep1_OutBuffer[2] * 256 + ep1_OutBuffer[3] * 65536);
                prepare_ep2_in();
                break;
            case VALID_APPLICATION_CMD:
                debug("VALID_APPLICATION_CMD\n");
                break;
            case RESET_CMD:
                debug("RESET_CMD\n");
                delay10ktcy(10);
                Reset();
                break;
            case SECTION_DESCRIPTOR_CMD:
                debug("SECTION_DESCRIPTOR_CMD\n");
                ep2_num_bytes_to_send = sizeof(section_descriptor);
                ep2_source_data = (uchar __code *) section_descriptor;
                prepare_ep2_in();
                break;
                
            default:
                debug("unknown command\n");
                // Raise an error
                EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
                EP_OUT_BD(1).ADR = (uchar __data *)&ep1_OutBuffer;
                EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_BSTALL;
                break;
        }
        EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
        if(EP_OUT_BD(1).Stat.DTS == 0)
        {
            EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
        }
        else
        {
            EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
        }
    }
    else // Raise an error
    {
        EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
        EP_OUT_BD(1).ADR = (uchar __data *)&ep1_OutBuffer;
        EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_BSTALL;
    }
}
