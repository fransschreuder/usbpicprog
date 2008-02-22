;    LIST    p=18F452      ;PIC18F452 is the target processor
    INCLUDE "p18f452.inc"  ;Include file with register defines

	;Programming Configuration Information
	__CONFIG    _CONFIG1H, _XT_OSC_1H				;XT HS PLL (10MHz XTAL)
	__CONFIG    _CONFIG2L, _BOR_ON_2L & _PWRT_ON_2L	;Power-Up timer ON, Brown-out at 2.7V

	__CONFIG    _CONFIG2H, _WDT_OFF_2H & _WDTPS_1_2H				;WDT OFF for debug
	__CONFIG    _CONFIG3H, _CCP2MX_OFF_3H							;CCP Module Off
	__CONFIG    _CONFIG4L, _LVP_OFF_4L & _DEBUG_OFF_4L
	
	;UnProtect entire device program space for DEBUG
	__CONFIG    _CONFIG5L, _CP0_OFF_5L & _CP1_OFF_5L & _CP2_OFF_5L
	__CONFIG    _CONFIG5H, _CPB_OFF_5H & _CPD_OFF_5H
	__CONFIG    _CONFIG6L, _WRT0_OFF_6L & _WRT1_OFF_6L & _WRT2_OFF_6L
	__CONFIG    _CONFIG6H, _WRTC_OFF_6H & _WRTB_OFF_6H & _WRTD_OFF_6H

;Assorted miscellaneous general-purpose registers
DlyRegA     equ     0x3D   ;Temp register for delay
DlyRegB     equ     0x3E   ;Temp register for delay
DlyRegC     equ     0x3F   ;Temp register for delay

STARTUP CODE 0x000
	nop					;Required for ICD Debugging
	nop
	goto Main
		
Main
	clrf	BSR,A	 	;Ensure BSR register points to first block
	clrf    TRISB,A
	movlw   0x7
	movwf   PORTB,A

Loop
	bsf		PORTB,0	;Turn the LED on
	call	LongDelay
	bcf		PORTB,0	;Turn the LED off
	call	LongDelay
	goto	Loop

;A long delay
LongDelay
		movlw   H'10'		;Adjust to produce a suitable time delay
		movwf   DlyRegA
ldelayc	movlw   H'10'
		movwf   DlyRegB
ldelayb movlw   H'FF'
		movwf   DlyRegC
ldelaya decfsz  DlyRegC,f	;Inner Loop
		goto    ldelaya
		decfsz  DlyRegB,f   ;Middle Loop
		goto    ldelayb
		decfsz  DlyRegA,f   ;Outer Loop
		goto    ldelayc
		return

       END


