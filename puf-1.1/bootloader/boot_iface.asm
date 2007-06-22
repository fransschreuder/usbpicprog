;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.7.2 #4854 (Jun 17 2007)
; This file generated Thu Jun 21 16:16:16 2007
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f4550

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _boot_goto_table

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; ; Starting pCode block
S_boot_iface__boot_goto_table	code	0X00001C
_boot_goto_table:
	extern _dispatch_usb_event
	extern _reset_usb
	extern _usb_sleep
	extern _fill_in_buffer
	extern _ep0_init
	extern _ep0_in
	extern _ep0_setup
	extern _ep1_init
	extern _ep1_out
	extern _ep2_init
	extern _ep2_in
	extern __mullong
	extern __mulint
	extern __gptrget1
	extern __gptrget2
	extern __gptrget3
	extern __gptrget4
	extern __gptrput1
	extern __gptrput2
	extern __gptrput3
	extern _delay10ktcy
	extern _delay1ktcy
	extern _delay10tcy
	;--------------------------------
	DW 0x0010 ; Boot Version in BCD format
	goto _dispatch_usb_event
	goto _reset_usb
	goto _usb_sleep
	goto _fill_in_buffer
	goto _ep0_init
	goto _ep0_in
	goto _ep0_setup
	goto _ep1_init
	goto _ep1_out
	goto _ep2_init
	goto _ep2_in
	goto __mullong
	goto __mulint
	goto __gptrget1
	goto __gptrget2
	goto __gptrget3
	goto __gptrget4
	goto __gptrput1
	goto __gptrput2
	goto __gptrput3
	goto _delay10ktcy
	goto _delay1ktcy
	goto _delay10tcy
	
; I code from now on!


; Statistics:
; code size:	   98 (0x0062) bytes ( 0.07%)
;           	   49 (0x0031) words
; udata size:	    0 (0x0000) bytes ( 0.00%)
; access size:	    0 (0x0000) bytes


	end
