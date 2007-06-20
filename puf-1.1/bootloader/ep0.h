/*-------------------------------------------------------------------------
  ep0.h - USB endpoint 0 callbacks

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

/* $Id: ep0.h,v 1.5 2006/04/17 20:36:15 gaufille Exp $ */

#ifndef EP0_H_
#define EP0_H_

#include "config.h"
#include "usb_std_req.h"

void ep0_init(void);
void ep0_in(void);
void ep0_setup(void);

#endif /*EP0_H_*/
