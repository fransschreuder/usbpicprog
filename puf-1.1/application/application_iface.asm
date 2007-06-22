;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.7.2 #4854 (Jun 17 2007)
; This file generated Fri Jun 22 15:01:07 2007
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f4550

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _application_data

;--------------------------------------------------------
; extern variables in this module
;--------------------------------------------------------
	extern _device_descriptor
	extern _configuration_descriptor
	extern _string_descriptor
	extern _str0
	extern _str1
	extern _str2
	extern _str3
	extern _ep_init
	extern _ep_in
	extern _ep_out
	extern _ep_setup
	extern _application_main

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; ; Starting pCode block for absolute Ival
S_application_iface__application_data	code	0X002000
_application_data:
	DB	0x00, LOW(_device_descriptor), HIGH(_device_descriptor), UPPER(_device_descriptor), LOW(_configuration_descriptor), HIGH(_configuration_descriptor), UPPER(_configuration_descriptor), LOW(_string_descriptor), HIGH(_string_descriptor), UPPER(_string_descriptor), LOW(_ep_init), HIGH(_ep_init)
	DB	UPPER(_ep_init), LOW(_ep_in), HIGH(_ep_in), UPPER(_ep_in), LOW(_ep_out), HIGH(_ep_out), UPPER(_ep_out), LOW(_ep_setup), HIGH(_ep_setup), UPPER(_ep_setup), LOW(_application_main), HIGH(_application_main)
	DB	UPPER(_application_main)

; I code from now on!


; Statistics:
; code size:	    6 (0x0006) bytes ( 0.00%)
;           	    3 (0x0003) words
; udata size:	    0 (0x0000) bytes ( 0.00%)
; access size:	    0 (0x0000) bytes


	end
