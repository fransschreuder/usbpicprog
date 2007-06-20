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

/* $Id: debug.h,v 1.2 2006/11/18 19:52:06 gaufille Exp $ */

#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef _DEBUG

#include <stdio.h>

#define init_debug() {\
    stdout = STREAM_USER; \
}

#define debug(x) printf(x);
#define debug2(x,y) printf(x,y);
#define toggle_A0() { PORTAbits.AN0 = !PORTAbits.AN0; }

#else

#define init_debug()
#define debug(x)
#define debug2(x,y)
#define toggle_A0()

#endif

#endif /*DEBUG_H_*/
