/*-------------------------------------------------------------------------
  timer2.h - timer 2 functions

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

/* $Id: timer2.h,v 1.1 2006/11/18 19:52:06 gaufille Exp $ */

#ifndef TIMER2_H_
#define TIMER2_H_

#define TIMER2_PRESCALER_1  0
#define TIMER2_PRESCALER_4  1
#define TIMER2_PRESCALER_16 2

#define TIMER2_POSTSCALER(x) (x-1) 

#define TIMER2_ON  1
#define TIMER2_OFF 0

#define timer2_set_period(p) { PR2 = p; }
#define timer2_set_control(prescaler, postscaler, state) { T2CON = prescaler | (state << 2) | (postscaler << 3); }
#define timer2_on() { T2CONbits.TMR2ON = 1;}
#define timer2_off() { T2CONbits.TMR2ON = 0;}
 


#endif /*TIMER2_H_*/
