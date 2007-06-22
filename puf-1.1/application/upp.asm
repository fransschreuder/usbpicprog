;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.7.2 #4854 (Jun 17 2007)
; This file generated Fri Jun 22 15:01:08 2007
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f4550

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _serial_in
	global _serial_out
	global _serial_init
;--------------------------------------------------------
;	Equates to used internal registers
;--------------------------------------------------------
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
POSTDEC1	equ	0xfe5
PREINC1	equ	0xfe4

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_upp__serial_init	code
_serial_init:
;	.line	16; upp.c	}
	RETURN	

; ; Starting pCode block
S_upp__serial_out	code
_serial_out:
;	.line	11; upp.c	}
	RETURN	

; ; Starting pCode block
S_upp__serial_in	code
_serial_in:
;	.line	6; upp.c	}
	RETURN	



; Statistics:
; code size:	    6 (0x0006) bytes ( 0.00%)
;           	    3 (0x0003) words
; udata size:	    0 (0x0000) bytes ( 0.00%)
; access size:	    0 (0x0000) bytes


	end
