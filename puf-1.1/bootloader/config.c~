/*-------------------------------------------------------------------------
  config.c - PIC configuration words

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

/* $Id: config.c,v 1.1 2006/04/17 20:36:15 gaufille Exp $ */

#include <pic18fregs.h>
#include "flash.h"

/* Set the PIC config words */
code char at __CONFIG1L _conf0  = _USBPLL_CLOCK_SRC_FROM_96MHZ_PLL_2_1L & 
                                  _CPUDIV__OSC1_OSC2_SRC___1__96MHZ_PLL_SRC___2__1L &
                                  _PLLDIV_NO_DIVIDE__4MHZ_INPUT__1L;
                                  
code char at __CONFIG1H _conf1  = _IESO_OFF_1H &
                                  _FCMEN_OFF_1H &
                                  _OSC_HS__HS_PLL__USB_HS_1H;

code char at __CONFIG2L _conf2  = _VREGEN_ON_2L &
                                  _BODENV_2_0V_2L &
                                  _BODEN_OFF_2L &
                                  _PUT_ON_2L;

code char at __CONFIG2H _conf3  = _WDTPS_1_1_2H &
                                  _WDT_DISABLED_CONTROLLED_2H;

code char at __CONFIG3H _conf4  = _MCLRE_MCLR_ON_RE3_OFF_3H &
                                  _LPT1OSC_ON_3H &
                                  _PBADEN_PORTB_4_0__CONFIGURED_AS_DIGITAL_I_O_ON_RESET_3H &
                                  _CCP2MUX_RB3_3H;

code char at __CONFIG4L _conf5  = _BACKBUG_OFF_4L &
                                  _ENHCPU_OFF_4L & 
                                  _ENICPORT_OFF_4L & 
                                  _LVP_OFF_4L & 
                                  _STVR_OFF_4L;

code char at __CONFIG5L _conf6  = _CP_0_OFF_5L & 
                                  _CP_1_OFF_5L & 
                                  _CP_2_OFF_5L;// & 
// 4550 specific                  _CP_3_OFF_5L;

code char at __CONFIG5H _conf7  = _CPB_OFF_5H & 
                                  _CPD_OFF_5H;

code char at __CONFIG6L _conf8  = _WRT_0_ON_6L & 
                                  _WRT_1_OFF_6L & 
                                  _WRT_2_OFF_6L;// & 
// 4550 specific                  _WRT_3_OFF_6L;

code char at __CONFIG6H _conf9  = _WRTB_ON_6H & 
                                  _WRTC_ON_6H & 
                                  _WRTD_OFF_6H;

code char at __CONFIG7L _conf10 = _EBTR_0_OFF_7L & 
                                  _EBTR_1_OFF_7L & 
                                  _EBTR_2_OFF_7L;// & 
// 4550 specific                  _EBTR_3_OFF_7L;

code char at __CONFIG7H _conf11 = _EBTRB_OFF_7H;

/* Memory sections for flash operations */
const uchar section_descriptor [22] = {3,
                                     FLASH_SECTION_READ,
                                     0x00, 0x00, 0x00,
                                     0xff, 0x1f, 0x00,
                                     FLASH_SECTION_READ | FLASH_SECTION_WRITE,
                                     0x00, 0x20, 0x00,
                                     0xff, 0x7f, 0x00,
                                     FLASH_SECTION_READ,
                                     0x00, 0x00, 0x30,
                                     0x0d, 0x00, 0x30};

