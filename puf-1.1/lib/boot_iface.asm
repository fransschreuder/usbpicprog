;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.7.2 #4854 (Jun 17 2007)
; This file generated Fri Jun 22 08:41:39 2007
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f4550

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _usb_device_state
	global _usb_active_cfg
	global _usb_active_alt_setting
	global _boot_goto_table


ustat_boot_iface_00	udata	0X005D
_usb_active_alt_setting	res	1
_usb_active_cfg	res	1
_usb_device_state	res	1

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; ; Starting pCode block
S_boot_iface__boot_goto_table	code	0X00001C
_boot_goto_table:
	global _boot_version
	global _dispatch_usb_event
	global _reset_usb
	global _usb_sleep
	global _fill_in_buffer
	global _ep0_init
	global _ep0_in
	global _ep0_setup
	global _ep1_init
	global _ep1_out
	global _ep2_init
	global _ep2_in
	global __mullong
	global __mulint
	global __gptrget1
	global __gptrget2
	global __gptrget3
	global __gptrget4
	global __gptrput1
	global __gptrput2
	global __gptrput3
	global _delay10ktcy
	global _delay1ktcy
	global _delay10tcy
_boot_version:
	res 2
_dispatch_usb_event:
	res 4
_reset_usb:
	res 4
_usb_sleep:
	res 4
_fill_in_buffer:
	res 4
_ep0_init:
	res 4
_ep0_in:
	res 4
_ep0_setup:
	res 4
_ep1_init:
	res 4
_ep1_out:
	res 4
_ep2_init:
	res 4
_ep2_in:
	res 4
__mullong:
	res 4
__mulint:
	res 4
__gptrget1:
	res 4
__gptrget2:
	res 4
__gptrget3:
	res 4
__gptrget4:
	res 4
__gptrput1:
	res 4
__gptrput2:
	res 4
__gptrput3:
	res 4
_delay10ktcy:
	res 4
_delay1ktcy:
	res 4
_delay10tcy:
	res 4
	
; I code from now on!


; Statistics:
; code size:	  146 (0x0092) bytes ( 0.11%)
;           	   73 (0x0049) words
; udata size:	    3 (0x0003) bytes ( 0.17%)
; access size:	    0 (0x0000) bytes


	end
