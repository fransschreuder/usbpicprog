/*-------------------------------------------------------------------------
  config.h - Bootloader configuration

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

/* $Id: config.h,v 1.1 2006/04/17 20:36:15 gaufille Exp $ */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "common_types.h"

/* BOOTLOADER VERSION */
# define BOOTLOADER_VERSION 0x0010

/* EP0 buffer size */
#define EP0_BUFFER_SIZE 8

/* EP1 buffer size */
#define EP1_BUFFER_SIZE 64

/* EP2 buffer size */
#define EP2_BUFFER_SIZE 64

/* Application data address */
#define APPLICATION_DATA_ADDRESS 0x2000

/* Memory sections for flash operations */
extern const uchar section_descriptor [22];

#endif /*CONFIG_H_*/
