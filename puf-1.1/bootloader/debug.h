/*-------------------------------------------------------------------------
  debug.h - Debug functions and macros

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

/* $Id: debug.h,v 1.3 2006/04/08 17:27:33 gaufille Exp $ */

#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef _DEBUG

#include <stdio.h>
#include <usart.h>

    // BRGH = low, BRG16 = 0
    // 77 -> 9600, 5/6 -> 115200, 12 -> 57600
    // 20 -> 38400
    // BRGH = high, BRG16 = 1
    // 103 -> 115200
    // 51  -> 230400
#define init_debug() {\
    usart_open(USART_ASYNCH_MODE & \
               USART_EIGHT_BIT & \
               USART_BRGH_HIGH & \
               USART_TX_INT_ON & \
               USART_RX_INT_OFF, 103); \
    BAUDCONbits.BRG16 = 1;\
    \
    stdout = STREAM_USART; \
    printf("Module started\n");\
}

#define debug(x) printf(x)
#define debug2(x,y) printf(x,y);
#define toggle_A0() PORTAbits.AN0 = !PORTAbits.AN0;

#else

#define init_debug()
#define debug(x)
#define debug2(x,y)
#define toggle_A0()

#endif

#endif /*DEBUG_H_*/
