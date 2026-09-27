/**************************************************************************
 *   usbpicprog debug link: transport between the host and a debug        *
 *   monitor running on the target. See debug_link.h.                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 **************************************************************************/

#ifdef __XC8
#include <xc.h>
#elif defined(SDCC)
#include <pic18f2550.h>
#else
#include <p18cxxx.h>
#endif
#include "typedefs.h"
#include "usb.h"
#include "interrupt.h"
#include "prog.h"
#include "io_cfg.h"             // I/O pin mapping
#include "prog_lolvl.h"
#include "debug_link.h"

extern unsigned char ConfigDisableVDD;

unsigned char debug_attached = 0;

static unsigned char half_bit;

// About half_bit microseconds at 12 MIPS. Interrupts (charge pump, 1ms
// tick) only make the phases longer, which the target doesn't mind.
static void link_delay( void )
{
	unsigned char n = half_bit;
	do {
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
	} while( --n );
}

static void link_send( unsigned char b )
{
	unsigned char i;

	for( i = 0; i < 8; i++ )
	{
		PGD = (b & 0x80) ? 1 : 0;
		b <<= 1;
		link_delay();
		PGC = 1;
		link_delay();
		PGC = 0;
	}
}

static unsigned char link_receive( void )
{
	unsigned char i, b = 0;

	for( i = 0; i < 8; i++ )
	{
		link_delay();
		PGC = 1;
		link_delay();
		b <<= 1;
		if( PGD_READ )
			b |= 1;
		PGC = 0;
	}
	return b;
}

static void mclr_low( void )
{
	VPP_RUNoff();
	VPP_RSTon();
}

static void mclr_run( void )
{
	VPP_RSToff();
	VPP_RUNon();
}

void debug_detach( unsigned char keep_running )
{
	debug_attached = 0;
	disablePGC_D();		// PGC/PGD and the PGD_LOW pull-down to inputs
	PGD = 0;
	PGC = 0;
	if( keep_running )
		return;
	mclr_low();
	DelayMs( 10 );
	VPP_RSToff();
	VDDoff();
}

unsigned char debug_link( unsigned char *in, unsigned char *out, int nBytes )
{
	unsigned char i, nout, nin;

	out[0] = 1;
	switch( in[1] ) {
	case DBG_LINK_ATTACH:
		if( iscp_active )
			exit_ISCP();
		VPPoff();
		mclr_low();
		TRISPGC_LOW = 1;
		PGC = 0;
		TRISPGC = 0;
		TRISPGD = 1;
		PGD = 0;
		PGD_LOW = 0;
		TRISPGD_LOW = 0;	// 1k pull-down: PGD reads 0 while nothing drives it
		VDDon();
		DelayMs( 50 );
		if( !(in[2] & 1) )
			mclr_run();
		debug_attached = 1;
		return 1;
	case DBG_LINK_DETACH:
		debug_detach( in[2] & 1 );
		return 1;
	case DBG_LINK_STATUS:
		out[1] = debug_attached;
		out[2] = debug_attached && PGD_READ;
		return 3;
	}
	if( !debug_attached )
	{
		out[0] = 3;
		return 1;
	}
	switch( in[1] ) {
	case DBG_LINK_RESET:
		mclr_low();
		DelayMs( 10 );
		if( !(in[2] & 1) )
			mclr_run();
		return 1;
	case DBG_LINK_HALT:
		if( !PGD_READ )
		{
			PGC = 1;
			startTimerMs( in[2] );
			while( timerRunning && !PGD_READ )
				continue;
			PGC = 0;
		}
		out[1] = PGD_READ;
		return 2;
	case DBG_LINK_XFER:
		nout = in[2];
		nin = in[3];
		half_bit = in[4];
		if( nout > USBGEN_EP_SIZE - 5 || nBytes < 5 + nout
		 || nin > USBGEN_EP_SIZE - 1 || half_bit == 0 )
			break;
		// Pulsing PGC would halt a running target: only talk to a halted one
		if( !PGD_READ )
		{
			out[0] = 0;
			return 1;
		}
		PGC = 1;		// sync: the monitor releases PGD
		link_delay();
		PGC = 0;
		link_delay();
		TRISPGD = 0;
		for( i = 0; i < nout; i++ )
			link_send( in[5 + i] );
		PGD = 0;
		TRISPGD = 1;
		link_delay();
		link_delay();
		for( i = 0; i < nin; i++ )
			out[1 + i] = link_receive();
		return 1 + nin;
	}
	out[0] = 3;
	return 1;
}
