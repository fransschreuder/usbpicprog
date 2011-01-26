;/////////////////////////////////////////////////////////////////////////////////
;// Code Generator: BoostC Compiler - http://www.sourceboost.com
;// Version       : 6.96
;// License Type  : Lite License (Unregistered)
;// Limitations   : PIC12,PIC16 max code size:2048 words, max RAM banks:2, Non commercial use only
;/////////////////////////////////////////////////////////////////////////////////

	include "P12F609.inc"
; Heap block 0, size:50 (0x0000004E - 0x0000007F)
__HEAP_BLOCK0_BANK               EQU	0x00000000
__HEAP_BLOCK0_START_OFFSET       EQU	0x0000004E
__HEAP_BLOCK0_END_OFFSET         EQU	0x0000007F
; Heap block 1, size:0 (0x00000000 - 0x00000000)
__HEAP_BLOCK1_BANK               EQU	0x00000000
__HEAP_BLOCK1_START_OFFSET       EQU	0x00000000
__HEAP_BLOCK1_END_OFFSET         EQU	0x00000000
; Heap block 2, size:0 (0x00000000 - 0x00000000)
__HEAP_BLOCK2_BANK               EQU	0x00000000
__HEAP_BLOCK2_START_OFFSET       EQU	0x00000000
__HEAP_BLOCK2_END_OFFSET         EQU	0x00000000
; Heap block 3, size:0 (0x00000000 - 0x00000000)
__HEAP_BLOCK3_BANK               EQU	0x00000000
__HEAP_BLOCK3_START_OFFSET       EQU	0x00000000
__HEAP_BLOCK3_END_OFFSET         EQU	0x00000000
gbl_indf                         EQU	0x00000000 ; bytes:1
gbl_tmr0                         EQU	0x00000001 ; bytes:1
gbl_pcl                          EQU	0x00000002 ; bytes:1
gbl_status                       EQU	0x00000003 ; bytes:1
gbl_fsr                          EQU	0x00000004 ; bytes:1
gbl_gpio                         EQU	0x00000005 ; bytes:1
gbl_porta                        EQU	0x00000005 ; bytes:1
gbl_pclath                       EQU	0x0000000A ; bytes:1
gbl_intcon                       EQU	0x0000000B ; bytes:1
gbl_pir1                         EQU	0x0000000C ; bytes:1
gbl_tmr1l                        EQU	0x0000000E ; bytes:1
gbl_tmr1h                        EQU	0x0000000F ; bytes:1
gbl_t1con                        EQU	0x00000010 ; bytes:1
gbl_vrcon                        EQU	0x00000019 ; bytes:1
gbl_cmcon0                       EQU	0x0000001A ; bytes:1
gbl_cmcon1                       EQU	0x0000001C ; bytes:1
gbl_option_reg                   EQU	0x00000081 ; bytes:1
gbl_trisio                       EQU	0x00000085 ; bytes:1
gbl_trisa                        EQU	0x00000085 ; bytes:1
gbl_pie1                         EQU	0x0000008C ; bytes:1
gbl_pcon                         EQU	0x0000008E ; bytes:1
gbl_osctune                      EQU	0x00000090 ; bytes:1
gbl_wpu                          EQU	0x00000095 ; bytes:1
gbl_wpua                         EQU	0x00000095 ; bytes:1
gbl_ioc                          EQU	0x00000096 ; bytes:1
gbl_ioca                         EQU	0x00000096 ; bytes:1
gbl_ansel                        EQU	0x0000009F ; bytes:1
gbl_numbers                      EQU	0x00000040 ; bytes:6
gbl_turn                         EQU	0x00000046 ; bytes:4
main_1_a                         EQU	0x0000004A ; bytes:1
main_1_b                         EQU	0x0000004B ; bytes:1
main_1_c                         EQU	0x0000004C ; bytes:1
delay_ms_00000_arg_del           EQU	0x0000004D ; bytes:1
	ORG 0x00000000
	GOTO	_startup
	ORG 0x00000004
delay_ms_00000
; { delay_ms ; function begin
	MOVF delay_ms_00000_arg_del, F
	BTFSS STATUS,Z
	GOTO	label1
	RETURN
label1
	MOVLW 0xF9
label2
	NOP
	NOP
	NOP
	NOP
	ADDLW 0xFF
	BTFSS STATUS,Z
	GOTO	label2
	NOP
	NOP
	NOP
	NOP
	NOP
	DECFSZ delay_ms_00000_arg_del, F
	GOTO	label1
	RETURN
; } delay_ms function end

	ORG 0x00000018
main
; { main ; function begin
	BCF STATUS, RP0
	CLRF main_1_a
	MOVLW 0x28
	BSF STATUS, RP0
	MOVWF gbl_trisio
	MOVLW 0x17
	BCF STATUS, RP0
	MOVWF gbl_gpio
label3
	INCF main_1_a, F
	MOVF main_1_a, W
	SUBLW 0x05
	BTFSS STATUS,C
	CLRF main_1_a
	BTFSC gbl_gpio,5
	GOTO	label3
	CLRF main_1_c
label4
	MOVLW 0x05
	SUBWF main_1_c, W
	BTFSC STATUS,C
	GOTO	label7
	CLRF main_1_b
label5
	MOVLW 0x04
	SUBWF main_1_b, W
	BTFSC STATUS,C
	GOTO	label6
	MOVLW LOW(gbl_turn+D'0')
	MOVWF FSR
	MOVF main_1_b, W
	ADDWF FSR, F
	MOVF INDF, W
	MOVWF gbl_gpio
	MOVLW 0x64
	MOVWF delay_ms_00000_arg_del
	CALL delay_ms_00000
	INCF main_1_b, F
	GOTO	label5
label6
	INCF main_1_c, F
	GOTO	label4
label7
	MOVLW LOW(gbl_numbers+D'0')
	MOVWF FSR
	MOVF main_1_a, W
	ADDWF FSR, F
	MOVF INDF, W
	MOVWF gbl_gpio
	GOTO	label3
; } main function end

	ORG 0x00000045
_startup
	MOVLW 0x10
	BCF STATUS, RP0
	MOVWF gbl_numbers
	MOVLW 0x01
	MOVWF gbl_numbers+D'1'
	MOVLW 0x11
	MOVWF gbl_numbers+D'2'
	MOVLW 0x05
	MOVWF gbl_numbers+D'3'
	MOVLW 0x15
	MOVWF gbl_numbers+D'4'
	MOVLW 0x07
	MOVWF gbl_numbers+D'5'
	MOVLW 0x01
	MOVWF gbl_turn
	MOVLW 0x02
	MOVWF gbl_turn+D'1'
	MOVLW 0x04
	MOVWF gbl_turn+D'2'
	MOVLW 0x10
	MOVWF gbl_turn+D'3'
	BCF PCLATH,3
	BCF PCLATH,4
	GOTO	main
	ORG 0x00002007
	DW 0x3FC4
	END
