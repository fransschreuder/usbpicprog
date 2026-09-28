/**************************************************************************
 *   usbpicprog debug link                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 **************************************************************************/

#ifndef DEBUG_LINK_H
#define DEBUG_LINK_H

/*
 * CMD_DEBUG_LINK (PROT_UPP4) runs a target with a debug monitor linked in
 * and moves bytes between the host and that monitor over PGC/PGD. The
 * programmer only provides the transport; the monitor's command set is
 * defined by the host and the monitor library.
 *
 * Packet: CMD_DEBUG_LINK, subcommand, arguments. Every reply starts with a
 * status byte: 1 ok, 0 target not halted (XFER), 3 bad command/arguments.
 *
 *   ATTACH flags      power the target (unless VDD is disabled), hold MCLR
 *                     low, then release it to VDD so the target runs.
 *                     flags bit 0: keep MCLR low (target held in reset).
 *                     -> 1
 *   DETACH flags      flags bit 0: leave the target powered and running,
 *                     only release PGC/PGD; otherwise reset and power off.
 *                     -> 1
 *   RESET flags       pulse MCLR low for 10ms; flags bit 0: keep it low.
 *                     -> 1
 *   STATUS            -> 1, attached, halted
 *   HALT timeout_ms   raise PGC until the monitor answers on PGD.
 *                     -> 1, halted
 *   XFER nout nin half out[nout]
 *                     one monitor transaction: a PGC sync pulse, nout
 *                     bytes to the target, then nin bytes from it. half
 *                     is the half bit time in about 1us units.
 *                     -> 1, in[nin]  (or 0 when the target is not halted)
 *
 * Wire protocol (the target is the one that has to keep up; it polls PGC):
 *  - While the target runs, PGD is an input on both sides and the
 *    programmer's 1k PGD_LOW resistor pulls it low.
 *  - The monitor drives PGD high while it is halted and idle. A rising PGC
 *    edge halts a running target (interrupt on change); the target
 *    acknowledges by driving PGD high.
 *  - A transaction starts with a PGC pulse, on which the monitor releases
 *    PGD. Bytes are sent MSb first: host to target, the programmer sets
 *    PGD, then pulses PGC and the target samples PGD while PGC is high;
 *    target to host, the target sets PGD after the rising PGC edge and the
 *    programmer samples it before the falling edge.
 */

#define DBG_LINK_ATTACH	0x01
#define DBG_LINK_DETACH	0x02
#define DBG_LINK_RESET	0x03
#define DBG_LINK_STATUS	0x04
#define DBG_LINK_HALT	0x05
#define DBG_LINK_XFER	0x06

extern unsigned char debug_attached;

unsigned char debug_link( unsigned char *in, unsigned char *out, int nBytes );
void debug_detach( unsigned char keep_running );

#endif // DEBUG_LINK_H
