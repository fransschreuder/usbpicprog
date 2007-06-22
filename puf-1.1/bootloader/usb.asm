;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.7.2 #4854 (Jun 17 2007)
; This file generated Thu Jun 21 16:16:21 2007
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f4550

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _string_descriptor
	global _configuration_descriptor
	global _device_descriptor
	global _init_usb
	global _reset_usb
	global _usb_sleep
	global _dispatch_usb_event
	global _fill_in_buffer
	global _ep_bdt
	global _ep_init
	global _ep_in
	global _ep_out
	global _ep_setup
	global _usb_device_state
	global _usb_active_cfg
	global _usb_active_alt_setting

;--------------------------------------------------------
; extern variables in this module
;--------------------------------------------------------
	extern __gptrget3
	extern __gptrget2
	extern __gptrput2
	extern __gptrget1
	extern __gptrput1
	extern __gptrput3
	extern _SPPDATAbits
	extern _SPPCFGbits
	extern _SPPEPSbits
	extern _SPPCONbits
	extern _UFRMLbits
	extern _UFRMHbits
	extern _UIRbits
	extern _UIEbits
	extern _UEIRbits
	extern _UEIEbits
	extern _USTATbits
	extern _UCONbits
	extern _UADDRbits
	extern _UCFGbits
	extern _UEP0bits
	extern _UEP1bits
	extern _UEP2bits
	extern _UEP3bits
	extern _UEP4bits
	extern _UEP5bits
	extern _UEP6bits
	extern _UEP7bits
	extern _UEP8bits
	extern _UEP9bits
	extern _UEP10bits
	extern _UEP11bits
	extern _UEP12bits
	extern _UEP13bits
	extern _UEP14bits
	extern _UEP15bits
	extern _PORTAbits
	extern _PORTBbits
	extern _PORTCbits
	extern _PORTDbits
	extern _PORTEbits
	extern _LATAbits
	extern _LATBbits
	extern _LATCbits
	extern _LATDbits
	extern _LATEbits
	extern _TRISAbits
	extern _TRISBbits
	extern _TRISCbits
	extern _TRISDbits
	extern _TRISEbits
	extern _OSCTUNEbits
	extern _PIE1bits
	extern _PIR1bits
	extern _IPR1bits
	extern _PIE2bits
	extern _PIR2bits
	extern _IPR2bits
	extern _EECON1bits
	extern _RCSTAbits
	extern _TXSTAbits
	extern _T3CONbits
	extern _CMCONbits
	extern _CVRCONbits
	extern _ECCP1ASbits
	extern _ECCP1DELbits
	extern _BAUDCONbits
	extern _CCP2CONbits
	extern _CCP1CONbits
	extern _ADCON2bits
	extern _ADCON1bits
	extern _ADCON0bits
	extern _SSPCON2bits
	extern _SSPCON1bits
	extern _SSPSTATbits
	extern _T2CONbits
	extern _T1CONbits
	extern _RCONbits
	extern _WDTCONbits
	extern _HLVDCONbits
	extern _OSCCONbits
	extern _T0CONbits
	extern _STATUSbits
	extern _FSR2Hbits
	extern _BSRbits
	extern _FSR1Hbits
	extern _FSR0Hbits
	extern _INTCON3bits
	extern _INTCON2bits
	extern _INTCONbits
	extern _TBLPTRUbits
	extern _PCLATHbits
	extern _PCLATUbits
	extern _STKPTRbits
	extern _TOSUbits
	extern _stdin
	extern _stdout
	extern _SPPDATA
	extern _SPPCFG
	extern _SPPEPS
	extern _SPPCON
	extern _UFRML
	extern _UFRMH
	extern _UIR
	extern _UIE
	extern _UEIR
	extern _UEIE
	extern _USTAT
	extern _UCON
	extern _UADDR
	extern _UCFG
	extern _UEP0
	extern _UEP1
	extern _UEP2
	extern _UEP3
	extern _UEP4
	extern _UEP5
	extern _UEP6
	extern _UEP7
	extern _UEP8
	extern _UEP9
	extern _UEP10
	extern _UEP11
	extern _UEP12
	extern _UEP13
	extern _UEP14
	extern _UEP15
	extern _PORTA
	extern _PORTB
	extern _PORTC
	extern _PORTD
	extern _PORTE
	extern _LATA
	extern _LATB
	extern _LATC
	extern _LATD
	extern _LATE
	extern _TRISA
	extern _TRISB
	extern _TRISC
	extern _TRISD
	extern _TRISE
	extern _OSCTUNE
	extern _PIE1
	extern _PIR1
	extern _IPR1
	extern _PIE2
	extern _PIR2
	extern _IPR2
	extern _EECON1
	extern _EECON2
	extern _EEDATA
	extern _EEADR
	extern _RCSTA
	extern _TXSTA
	extern _TXREG
	extern _RCREG
	extern _SPBRG
	extern _SPBRGH
	extern _T3CON
	extern _TMR3L
	extern _TMR3H
	extern _CMCON
	extern _CVRCON
	extern _ECCP1AS
	extern _ECCP1DEL
	extern _BAUDCON
	extern _CCP2CON
	extern _CCPR2L
	extern _CCPR2H
	extern _CCP1CON
	extern _CCPR1L
	extern _CCPR1H
	extern _ADCON2
	extern _ADCON1
	extern _ADCON0
	extern _ADRESL
	extern _ADRESH
	extern _SSPCON2
	extern _SSPCON1
	extern _SSPSTAT
	extern _SSPADD
	extern _SSPBUF
	extern _T2CON
	extern _PR2
	extern _TMR2
	extern _T1CON
	extern _TMR1L
	extern _TMR1H
	extern _RCON
	extern _WDTCON
	extern _HLVDCON
	extern _OSCCON
	extern _T0CON
	extern _TMR0L
	extern _TMR0H
	extern _STATUS
	extern _FSR2L
	extern _FSR2H
	extern _PLUSW2
	extern _PREINC2
	extern _POSTDEC2
	extern _POSTINC2
	extern _INDF2
	extern _BSR
	extern _FSR1L
	extern _FSR1H
	extern _PLUSW1
	extern _PREINC1
	extern _POSTDEC1
	extern _POSTINC1
	extern _INDF1
	extern _WREG
	extern _FSR0L
	extern _FSR0H
	extern _PLUSW0
	extern _PREINC0
	extern _POSTDEC0
	extern _POSTINC0
	extern _INDF0
	extern _INTCON3
	extern _INTCON2
	extern _INTCON
	extern _PRODL
	extern _PRODH
	extern _TABLAT
	extern _TBLPTRL
	extern _TBLPTRH
	extern _TBLPTRU
	extern _PCL
	extern _PCLATH
	extern _PCLATU
	extern _STKPTR
	extern _TOSL
	extern _TOSH
	extern _TOSU
	extern __mulint
	extern _section_descriptor
	extern _boot_device_descriptor
	extern _boot_configuration_descriptor
	extern _boot_string_descriptor
	extern _str0
	extern _str1
	extern _str2
	extern _str3
	extern _boot_ep_init
	extern _boot_ep_in
	extern _boot_ep_out
	extern _boot_ep_setup
;--------------------------------------------------------
;	Equates to used internal registers
;--------------------------------------------------------
STATUS	equ	0xfd8
PCL	equ	0xff9
PCLATH	equ	0xffa
PCLATU	equ	0xffb
INTCON	equ	0xff2
WREG	equ	0xfe8
TOSL	equ	0xffd
TOSH	equ	0xffe
TOSU	equ	0xfff
TBLPTRL	equ	0xff6
FSR0L	equ	0xfe9
FSR0H	equ	0xfea
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
INDF0	equ	0xfef
POSTINC0	equ	0xfee
POSTDEC1	equ	0xfe5
PREINC1	equ	0xfe4
PLUSW2	equ	0xfdb
PRODL	equ	0xff3
PRODH	equ	0xff4


; Internal registers
.registers	udata_ovr	0x0000
r0x00	res	1
r0x01	res	1
r0x02	res	1
r0x03	res	1
r0x04	res	1
r0x05	res	1
r0x06	res	1
r0x07	res	1
r0x08	res	1
r0x09	res	1
r0x0a	res	1
r0x0b	res	1

udata_usb_0	udata
_device_descriptor	res	3

udata_usb_1	udata
_configuration_descriptor	res	3

udata_usb_2	udata
_string_descriptor	res	3

udata_usb_3	udata
_reset_usb_i_1_1	res	1

udata_usb_4	udata
_fill_in_buffer_byte_to_send_1_1	res	2

udata_usb_5	udata
_fill_in_buffer_dest_1_1	res	3

udata_usb_6	udata
_ep_init	res	3

udata_usb_7	udata
_ep_setup	res	3

udata_usb_8	udata
_ep_out	res	3

udata_usb_9	udata
_ep_in	res	3


ustat_usb_00	udata	0X005D
_usb_active_alt_setting	res	1
_usb_active_cfg	res	1
_usb_device_state	res	1


ustat_usb_01	udata	0X0400
_ep_bdt	res	128

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_usb__fill_in_buffer	code
_fill_in_buffer:
;	.line	187; usb.c	void fill_in_buffer(uchar EPnum,
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	MOVFF	r0x05, POSTDEC1
	MOVFF	r0x06, POSTDEC1
	MOVFF	r0x07, POSTDEC1
	MOVFF	r0x08, POSTDEC1
	MOVFF	r0x09, POSTDEC1
	MOVFF	r0x0a, POSTDEC1
	MOVFF	r0x0b, POSTDEC1
	MOVLW	0x02
	MOVFF	PLUSW2, r0x00
	MOVLW	0x03
	MOVFF	PLUSW2, r0x01
	MOVLW	0x04
	MOVFF	PLUSW2, r0x02
	MOVLW	0x05
	MOVFF	PLUSW2, r0x03
	MOVLW	0x06
	MOVFF	PLUSW2, r0x04
	MOVLW	0x07
	MOVFF	PLUSW2, r0x05
	MOVLW	0x08
	MOVFF	PLUSW2, r0x06
	MOVLW	0x09
	MOVFF	PLUSW2, r0x07
	MOVLW	0x0a
	MOVFF	PLUSW2, r0x08
;	.line	196; usb.c	if(*nb_byte < buffer_size)
	MOVFF	r0x06, FSR0L
	MOVFF	r0x07, PRODL
	MOVF	r0x08, W
	CALL	__gptrget2
	MOVWF	r0x09
	MOVFF	PRODL, r0x0a
	MOVF	r0x05, W
	SUBWF	r0x0a, W
	BNZ	_00206_DS_
	MOVF	r0x04, W
	SUBWF	r0x09, W
_00206_DS_:
	BTFSC	STATUS, 0
	GOTO	_00196_DS_
;	.line	198; usb.c	byte_to_send = *nb_byte;
	MOVFF	r0x09, _fill_in_buffer_byte_to_send_1_1
	MOVFF	r0x0a, (_fill_in_buffer_byte_to_send_1_1 + 1)
	GOTO	_00197_DS_
_00196_DS_:
;	.line	202; usb.c	byte_to_send = buffer_size;
	MOVFF	r0x04, _fill_in_buffer_byte_to_send_1_1
	MOVFF	r0x05, (_fill_in_buffer_byte_to_send_1_1 + 1)
_00197_DS_:
;	.line	205; usb.c	EP_IN_BD(EPnum).Cnt = byte_to_send;
	MOVF	r0x00, W
	ADDWF	r0x00, W
	MOVWF	r0x04
	INCF	r0x04, W
	MOVWF	r0x00
; ;multiply lit val:0x04 by variable r0x00 and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	MOVF	r0x00, W
	MULLW	0x04
	MOVFF	PRODL, r0x00
	MOVLW	LOW(_ep_bdt)
	ADDWF	r0x00, W
	MOVWF	r0x04
	CLRF	r0x05
	MOVLW	HIGH(_ep_bdt)
	ADDWFC	r0x05, F
	INCF	r0x04, F
	BTFSC	STATUS, 0
	INCF	r0x05, F
	BANKSEL	_fill_in_buffer_byte_to_send_1_1
	MOVF	_fill_in_buffer_byte_to_send_1_1, W, B
	MOVWF	r0x0b
	MOVFF	r0x04, FSR0L
	MOVFF	r0x05, FSR0H
	MOVFF	r0x0b, INDF0
;	.line	208; usb.c	dest = (uchar __data *) EP_IN_BD(EPnum).ADR;
	MOVLW	LOW(_ep_bdt)
	ADDWF	r0x00, F
	MOVLW	HIGH(_ep_bdt)
	CLRF	r0x04
	ADDWFC	r0x04, F
	MOVLW	0x02
	ADDWF	r0x00, F
	BTFSC	STATUS, 0
	INCF	r0x04, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x04, FSR0H
	MOVFF	POSTINC0, r0x00
	MOVFF	INDF0, r0x04
	MOVF	r0x04, W
	BANKSEL	(_fill_in_buffer_dest_1_1 + 1)
	MOVWF	(_fill_in_buffer_dest_1_1 + 1), B
	MOVF	r0x00, W
	BANKSEL	_fill_in_buffer_dest_1_1
	MOVWF	_fill_in_buffer_dest_1_1, B
	MOVLW	0x80
	BANKSEL	(_fill_in_buffer_dest_1_1 + 2)
	MOVWF	(_fill_in_buffer_dest_1_1 + 2), B
	BANKSEL	_fill_in_buffer_byte_to_send_1_1
;	.line	211; usb.c	*nb_byte -= byte_to_send;
	MOVF	_fill_in_buffer_byte_to_send_1_1, W, B
	SUBWF	r0x09, F
	BANKSEL	(_fill_in_buffer_byte_to_send_1_1 + 1)
	MOVF	(_fill_in_buffer_byte_to_send_1_1 + 1), W, B
	SUBWFB	r0x0a, F
	MOVFF	r0x09, POSTDEC1
	MOVFF	r0x0a, PRODH
	MOVFF	r0x06, FSR0L
	MOVFF	r0x07, PRODL
	MOVF	r0x08, W
	CALL	__gptrput2
_00198_DS_:
	BANKSEL	_fill_in_buffer_byte_to_send_1_1
;	.line	214; usb.c	while(byte_to_send)
	MOVF	_fill_in_buffer_byte_to_send_1_1, W, B
	BANKSEL	(_fill_in_buffer_byte_to_send_1_1 + 1)
	IORWF	(_fill_in_buffer_byte_to_send_1_1 + 1), W, B
	BTFSC	STATUS, 2
	GOTO	_00201_DS_
;	.line	216; usb.c	*dest = **source;
	MOVFF	_fill_in_buffer_dest_1_1, r0x00
	MOVFF	(_fill_in_buffer_dest_1_1 + 1), r0x04
	MOVFF	(_fill_in_buffer_dest_1_1 + 2), r0x05
	MOVFF	r0x01, FSR0L
	MOVFF	r0x02, PRODL
	MOVF	r0x03, W
	CALL	__gptrget3
	MOVWF	r0x06
	MOVFF	PRODL, r0x07
	MOVFF	PRODH, r0x08
	MOVFF	r0x06, FSR0L
	MOVFF	r0x07, PRODL
	MOVF	r0x08, W
	CALL	__gptrget1
	MOVWF	r0x09
	MOVFF	r0x09, POSTDEC1
	MOVFF	r0x00, FSR0L
	MOVFF	r0x04, PRODL
	MOVF	r0x05, W
	CALL	__gptrput1
	BANKSEL	_fill_in_buffer_dest_1_1
;	.line	217; usb.c	dest++;
	INCF	_fill_in_buffer_dest_1_1, F, B
	BTFSS	STATUS, 0
	BRA	_10195_DS_
	BANKSEL	(_fill_in_buffer_dest_1_1 + 1)
	INCF	(_fill_in_buffer_dest_1_1 + 1), F, B
_10195_DS_:
	BTFSS	STATUS, 0
	BRA	_20196_DS_
	BANKSEL	(_fill_in_buffer_dest_1_1 + 2)
	INCF	(_fill_in_buffer_dest_1_1 + 2), F, B
_20196_DS_:
;	.line	218; usb.c	(*source)++;
	INCF	r0x06, F
	BTFSC	STATUS, 0
	INCF	r0x07, F
	BTFSC	STATUS, 0
	INCF	r0x08, F
	MOVFF	r0x06, POSTDEC1
	MOVFF	r0x07, PRODH
	MOVFF	r0x08, TBLPTRL
	MOVFF	r0x01, FSR0L
	MOVFF	r0x02, PRODL
	MOVF	r0x03, W
	CALL	__gptrput3
;	.line	219; usb.c	byte_to_send--;
	MOVLW	0xff
	BANKSEL	_fill_in_buffer_byte_to_send_1_1
	ADDWF	_fill_in_buffer_byte_to_send_1_1, F, B
	BTFSC	STATUS, 0
	BRA	_30197_DS_
	BANKSEL	(_fill_in_buffer_byte_to_send_1_1 + 1)
	DECF	(_fill_in_buffer_byte_to_send_1_1 + 1), F, B
_30197_DS_:
	GOTO	_00198_DS_
_00201_DS_:
	MOVFF	PREINC1, r0x0b
	MOVFF	PREINC1, r0x0a
	MOVFF	PREINC1, r0x09
	MOVFF	PREINC1, r0x08
	MOVFF	PREINC1, r0x07
	MOVFF	PREINC1, r0x06
	MOVFF	PREINC1, r0x05
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_usb__dispatch_usb_event	code
_dispatch_usb_event:
;	.line	117; usb.c	void dispatch_usb_event(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
;	.line	120; usb.c	if(UIEbits.ACTVIE && UIRbits.ACTVIF)
	BTFSS	_UIEbits, 2
	GOTO	_00138_DS_
	BTFSS	_UIRbits, 2
	GOTO	_00138_DS_
;	.line	122; usb.c	UCONbits.SUSPND = 0;
	BCF	_UCONbits, 1
;	.line	123; usb.c	UIEbits.ACTVIE = 0;
	BCF	_UIEbits, 2
;	.line	124; usb.c	UIRbits.ACTVIF = 0;
	BCF	_UIRbits, 2
_00138_DS_:
;	.line	128; usb.c	if(UIEbits.URSTIE && UIRbits.URSTIF)
	BTFSS	_UIEbits, 0
	GOTO	_00141_DS_
	BTFSS	_UIRbits, 0
	GOTO	_00141_DS_
;	.line	130; usb.c	reset_usb();
	CALL	_reset_usb
_00141_DS_:
;	.line	134; usb.c	if(UIEbits.IDLEIE && UIRbits.IDLEIF)
	BTFSS	_UIEbits, 4
	GOTO	_00144_DS_
	BTFSS	_UIRbits, 4
	GOTO	_00144_DS_
;	.line	137; usb.c	UIEbits.ACTVIE = 1;
	BSF	_UIEbits, 2
;	.line	138; usb.c	UIRbits.IDLEIF  = 0;
	BCF	_UIRbits, 4
;	.line	139; usb.c	UCONbits.SUSPND = 1;
	BSF	_UCONbits, 1
_00144_DS_:
;	.line	143; usb.c	if(UIEbits.STALLIE && UIRbits.STALLIF)
	BTFSS	_UIEbits, 5
	GOTO	_00149_DS_
	BTFSS	_UIRbits, 5
	GOTO	_00149_DS_
;	.line	148; usb.c	if(UEP0bits.EPSTALL == 1)
	CLRF	r0x00
	BTFSC	_UEP0bits, 0
	INCF	r0x00, F
	MOVF	r0x00, W
	XORLW	0x01
	BNZ	_00180_DS_
	GOTO	_00181_DS_
_00180_DS_:
	GOTO	_00147_DS_
; ;multiply lit val:0x03 by variable _usb_active_cfg and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00181_DS_:
;	.line	150; usb.c	ep_init[GET_ACTIVE_CONFIGURATION()][0]();
	MOVF	_usb_active_cfg, W
	MULLW	0x03
	MOVFF	PRODL, r0x00
	MOVFF	PRODH, r0x01
	BANKSEL	_ep_init
	MOVF	_ep_init, W, B
	ADDWF	r0x00, F
	BANKSEL	(_ep_init + 1)
	MOVF	(_ep_init + 1), W, B
	ADDWFC	r0x01, F
	BANKSEL	(_ep_init + 2)
	MOVF	(_ep_init + 2), W, B
	CLRF	r0x02
	ADDWFC	r0x02, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget3
	MOVWF	r0x00
	MOVFF	PRODL, r0x01
	MOVFF	PRODH, r0x02
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget3
	MOVWF	r0x00
	MOVFF	PRODL, r0x01
	MOVFF	PRODH, r0x02
	MOVFF	INTCON, POSTDEC1
	BCF	INTCON, 7
	PUSH	
	MOVLW	LOW(_00182_DS_)
	MOVWF	TOSL
	MOVLW	HIGH(_00182_DS_)
	MOVWF	TOSH
	MOVLW	UPPER(_00182_DS_)
	MOVWF	TOSU
	MOVF	PREINC1, W
	MOVWF	INTCON
	MOVFF	r0x02, PCLATU
	MOVFF	r0x01, PCLATH
	MOVF	r0x00, W
	MOVWF	PCL
_00182_DS_:
;	.line	151; usb.c	UEP0bits.EPSTALL = 0;
	BCF	_UEP0bits, 0
_00147_DS_:
;	.line	153; usb.c	UIRbits.STALLIF = 0;
	BCF	_UIRbits, 5
_00149_DS_:
;	.line	159; usb.c	if((GET_DEVICE_STATE() >= DEFAULT_STATE) &&
	MOVLW	0x03
	SUBWF	_usb_device_state, W
	BTFSS	STATUS, 0
	GOTO	_00163_DS_
;	.line	160; usb.c	UIEbits.TRNIE && UIRbits.TRNIF)
	BTFSS	_UIEbits, 3
	GOTO	_00163_DS_
	BTFSS	_UIRbits, 3
	GOTO	_00163_DS_
;	.line	163; usb.c	if(USTATbits.DIR == OUT)
	BTFSC	_USTATbits, 2
	GOTO	_00157_DS_
;	.line	165; usb.c	if(EP_OUT_BD(USTATbits.ENDP).Stat.PID == SETUP_TOKEN)
	MOVF	_USTATbits, W
	SWAPF	WREG, W
	RLNCF	WREG, W
	ANDLW	0x0f
	MOVWF	r0x00
	MOVF	r0x00, W
	ADDWF	r0x00, W
	MOVWF	r0x01
; ;multiply lit val:0x04 by variable r0x01 and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	MOVF	r0x01, W
	MULLW	0x04
	MOVFF	PRODL, r0x00
	MOVLW	LOW(_ep_bdt)
	ADDWF	r0x00, F
	MOVLW	HIGH(_ep_bdt)
	CLRF	r0x01
	BTFSC	r0x00, 7
	SETF	r0x01
	ADDWFC	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVF	INDF0, W
	RRNCF	WREG, W
	RRNCF	WREG, W
	ANDLW	0x0f
	MOVWF	r0x00
	MOVF	r0x00, W
	XORLW	0x0d
	BNZ	_00184_DS_
	GOTO	_00185_DS_
_00184_DS_:
	GOTO	_00152_DS_
; ;multiply lit val:0x03 by variable _usb_active_cfg and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00185_DS_:
;	.line	168; usb.c	ep_setup[GET_ACTIVE_CONFIGURATION()][USTATbits.ENDP]();
	MOVF	_usb_active_cfg, W
	MULLW	0x03
	MOVFF	PRODL, r0x00
	MOVFF	PRODH, r0x01
	BANKSEL	_ep_setup
	MOVF	_ep_setup, W, B
	ADDWF	r0x00, F
	BANKSEL	(_ep_setup + 1)
	MOVF	(_ep_setup + 1), W, B
	ADDWFC	r0x01, F
	BANKSEL	(_ep_setup + 2)
	MOVF	(_ep_setup + 2), W, B
	CLRF	r0x02
	ADDWFC	r0x02, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget3
	MOVWF	r0x00
	MOVFF	PRODL, r0x01
	MOVFF	PRODH, r0x02
	MOVF	_USTATbits, W
	SWAPF	WREG, W
	RLNCF	WREG, W
	ANDLW	0x0f
	MOVWF	r0x03
	CLRF	r0x04
	MOVF	r0x04, W
	MOVWF	POSTDEC1
	MOVF	r0x03, W
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x03
	MOVWF	POSTDEC1
	CALL	__mulint
	MOVWF	r0x03
	MOVFF	PRODL, r0x04
	MOVLW	0x04
	ADDWF	FSR1L, F
	MOVF	r0x03, W
	ADDWF	r0x00, F
	MOVF	r0x04, W
	ADDWFC	r0x01, F
	CLRF	WREG
	ADDWFC	r0x02, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget3
	MOVWF	r0x00
	MOVFF	PRODL, r0x01
	MOVFF	PRODH, r0x02
	MOVFF	INTCON, POSTDEC1
	BCF	INTCON, 7
	PUSH	
	MOVLW	LOW(_00186_DS_)
	MOVWF	TOSL
	MOVLW	HIGH(_00186_DS_)
	MOVWF	TOSH
	MOVLW	UPPER(_00186_DS_)
	MOVWF	TOSU
	MOVF	PREINC1, W
	MOVWF	INTCON
	MOVFF	r0x02, PCLATU
	MOVFF	r0x01, PCLATH
	MOVF	r0x00, W
	MOVWF	PCL
_00186_DS_:
	GOTO	_00158_DS_
; ;multiply lit val:0x03 by variable _usb_active_cfg and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00152_DS_:
;	.line	173; usb.c	ep_out[GET_ACTIVE_CONFIGURATION()][USTATbits.ENDP]();
	MOVF	_usb_active_cfg, W
	MULLW	0x03
	MOVFF	PRODL, r0x00
	MOVFF	PRODH, r0x01
	BANKSEL	_ep_out
	MOVF	_ep_out, W, B
	ADDWF	r0x00, F
	BANKSEL	(_ep_out + 1)
	MOVF	(_ep_out + 1), W, B
	ADDWFC	r0x01, F
	BANKSEL	(_ep_out + 2)
	MOVF	(_ep_out + 2), W, B
	CLRF	r0x02
	ADDWFC	r0x02, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget3
	MOVWF	r0x00
	MOVFF	PRODL, r0x01
	MOVFF	PRODH, r0x02
	MOVF	_USTATbits, W
	SWAPF	WREG, W
	RLNCF	WREG, W
	ANDLW	0x0f
	MOVWF	r0x03
	CLRF	r0x04
	MOVF	r0x04, W
	MOVWF	POSTDEC1
	MOVF	r0x03, W
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x03
	MOVWF	POSTDEC1
	CALL	__mulint
	MOVWF	r0x03
	MOVFF	PRODL, r0x04
	MOVLW	0x04
	ADDWF	FSR1L, F
	MOVF	r0x03, W
	ADDWF	r0x00, F
	MOVF	r0x04, W
	ADDWFC	r0x01, F
	CLRF	WREG
	ADDWFC	r0x02, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget3
	MOVWF	r0x00
	MOVFF	PRODL, r0x01
	MOVFF	PRODH, r0x02
	MOVFF	INTCON, POSTDEC1
	BCF	INTCON, 7
	PUSH	
	MOVLW	LOW(_00187_DS_)
	MOVWF	TOSL
	MOVLW	HIGH(_00187_DS_)
	MOVWF	TOSH
	MOVLW	UPPER(_00187_DS_)
	MOVWF	TOSU
	MOVF	PREINC1, W
	MOVWF	INTCON
	MOVFF	r0x02, PCLATU
	MOVFF	r0x01, PCLATH
	MOVF	r0x00, W
	MOVWF	PCL
_00187_DS_:
	GOTO	_00158_DS_
_00157_DS_:
;	.line	176; usb.c	else if(USTATbits.DIR == IN)
	CLRF	r0x00
	BTFSC	_USTATbits, 2
	INCF	r0x00, F
	MOVF	r0x00, W
	XORLW	0x01
	BNZ	_00188_DS_
	GOTO	_00189_DS_
_00188_DS_:
	GOTO	_00158_DS_
; ;multiply lit val:0x03 by variable _usb_active_cfg and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00189_DS_:
;	.line	179; usb.c	ep_in[GET_ACTIVE_CONFIGURATION()][USTATbits.ENDP]();
	MOVF	_usb_active_cfg, W
	MULLW	0x03
	MOVFF	PRODL, r0x00
	MOVFF	PRODH, r0x01
	BANKSEL	_ep_in
	MOVF	_ep_in, W, B
	ADDWF	r0x00, F
	BANKSEL	(_ep_in + 1)
	MOVF	(_ep_in + 1), W, B
	ADDWFC	r0x01, F
	BANKSEL	(_ep_in + 2)
	MOVF	(_ep_in + 2), W, B
	CLRF	r0x02
	ADDWFC	r0x02, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget3
	MOVWF	r0x00
	MOVFF	PRODL, r0x01
	MOVFF	PRODH, r0x02
	MOVF	_USTATbits, W
	SWAPF	WREG, W
	RLNCF	WREG, W
	ANDLW	0x0f
	MOVWF	r0x03
	CLRF	r0x04
	MOVF	r0x04, W
	MOVWF	POSTDEC1
	MOVF	r0x03, W
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x03
	MOVWF	POSTDEC1
	CALL	__mulint
	MOVWF	r0x03
	MOVFF	PRODL, r0x04
	MOVLW	0x04
	ADDWF	FSR1L, F
	MOVF	r0x03, W
	ADDWF	r0x00, F
	MOVF	r0x04, W
	ADDWFC	r0x01, F
	CLRF	WREG
	ADDWFC	r0x02, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget3
	MOVWF	r0x00
	MOVFF	PRODL, r0x01
	MOVFF	PRODH, r0x02
	MOVFF	INTCON, POSTDEC1
	BCF	INTCON, 7
	PUSH	
	MOVLW	LOW(_00190_DS_)
	MOVWF	TOSL
	MOVLW	HIGH(_00190_DS_)
	MOVWF	TOSH
	MOVLW	UPPER(_00190_DS_)
	MOVWF	TOSU
	MOVF	PREINC1, W
	MOVWF	INTCON
	MOVFF	r0x02, PCLATU
	MOVFF	r0x01, PCLATH
	MOVF	r0x00, W
	MOVWF	PCL
_00190_DS_:
_00158_DS_:
;	.line	183; usb.c	UIRbits.TRNIF = 0;
	BCF	_UIRbits, 3
_00163_DS_:
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_usb__usb_sleep	code
_usb_sleep:
;	.line	100; usb.c	void usb_sleep(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	104; usb.c	if(UCONbits.SUSPND)
	BTFSS	_UCONbits, 1
	GOTO	_00132_DS_
;	.line	111; usb.c	PIE2bits.USBIE = 1; 
	BSF	_PIE2bits, 5
	sleep 
;	.line	113; usb.c	PIR2bits.USBIF = 0;
	BCF	_PIR2bits, 5
_00132_DS_:
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_usb__reset_usb	code
_reset_usb:
;	.line	65; usb.c	void reset_usb(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
;	.line	70; usb.c	UIR = 0;
	CLRF	_UIR
;	.line	71; usb.c	UIE = 0x7b;
	MOVLW	0x7b
	MOVWF	_UIE
;	.line	72; usb.c	UEIE = 0;
	CLRF	_UEIE
;	.line	73; usb.c	UADDR = 0x00;
	CLRF	_UADDR
_00110_DS_:
;	.line	75; usb.c	while(UIRbits.TRNIF)
	BTFSS	_UIRbits, 3
	GOTO	_00112_DS_
;	.line	77; usb.c	UIRbits.TRNIF = 0;
	BCF	_UIRbits, 3
	GOTO	_00110_DS_
_00112_DS_:
;	.line	81; usb.c	SET_DEVICE_STATE(DEFAULT_STATE);
	MOVLW	0x03
	MOVWF	_usb_device_state
;	.line	84; usb.c	SET_ACTIVE_CONFIGURATION(0);
	CLRF	_usb_active_cfg
;	.line	85; usb.c	SET_ACTIVE_ALTERNATE_SETTING(0);
	CLRF	_usb_active_alt_setting
;	.line	87; usb.c	UEP0  = 0; UEP1  = 0; UEP2  = 0; UEP3  = 0;
	CLRF	_UEP0
	CLRF	_UEP1
	CLRF	_UEP2
	CLRF	_UEP3
;	.line	88; usb.c	UEP4  = 0; UEP5  = 0; UEP6  = 0; UEP7  = 0;
	CLRF	_UEP4
	CLRF	_UEP5
	CLRF	_UEP6
	CLRF	_UEP7
;	.line	89; usb.c	UEP8  = 0; UEP9  = 0; UEP10 = 0; UEP11 = 0;
	CLRF	_UEP8
	CLRF	_UEP9
	CLRF	_UEP10
	CLRF	_UEP11
;	.line	90; usb.c	UEP12 = 0; UEP13 = 0; UEP14 = 0; UEP15 = 0;
	CLRF	_UEP12
	CLRF	_UEP13
	CLRF	_UEP14
	CLRF	_UEP15
;	.line	92; usb.c	UCONbits.PKTDIS = 0;
	BCF	_UCONbits, 4
;	.line	94; usb.c	for(i = 0; i < 16; i++)
	MOVLW	0x10
	BANKSEL	_reset_usb_i_1_1
	MOVWF	_reset_usb_i_1_1, B
_00115_DS_:
;	.line	96; usb.c	ep_init[0][i]();
	MOVFF	_ep_init, r0x00
	MOVFF	(_ep_init + 1), r0x01
	MOVFF	(_ep_init + 2), r0x02
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget3
	MOVWF	r0x00
	MOVFF	PRODL, r0x01
	MOVFF	PRODH, r0x02
	BANKSEL	_reset_usb_i_1_1
	DECF	_reset_usb_i_1_1, W, B
	MOVWF	r0x03
; ;multiply lit val:0x03 by variable r0x03 and store in r0x03
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	MOVF	r0x03, W
	MULLW	0x03
	MOVFF	PRODL, r0x03
	MOVFF	PRODH, r0x04
	MOVF	r0x03, W
	ADDWF	r0x00, F
	MOVF	r0x04, W
	ADDWFC	r0x01, F
	CLRF	WREG
	ADDWFC	r0x02, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget3
	MOVWF	r0x00
	MOVFF	PRODL, r0x01
	MOVFF	PRODH, r0x02
	MOVFF	INTCON, POSTDEC1
	BCF	INTCON, 7
	PUSH	
	MOVLW	LOW(_00122_DS_)
	MOVWF	TOSL
	MOVLW	HIGH(_00122_DS_)
	MOVWF	TOSH
	MOVLW	UPPER(_00122_DS_)
	MOVWF	TOSU
	MOVF	PREINC1, W
	MOVWF	INTCON
	MOVFF	r0x02, PCLATU
	MOVFF	r0x01, PCLATH
	MOVF	r0x00, W
	MOVWF	PCL
_00122_DS_:
	BANKSEL	_reset_usb_i_1_1
	DECF	_reset_usb_i_1_1, F, B
	BANKSEL	_reset_usb_i_1_1
;	.line	94; usb.c	for(i = 0; i < 16; i++)
	MOVF	_reset_usb_i_1_1, W, B
	BTFSS	STATUS, 2
	GOTO	_00115_DS_
	MOVLW	0x10
	BANKSEL	_reset_usb_i_1_1
	MOVWF	_reset_usb_i_1_1, B
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_usb__init_usb	code
_init_usb:
;	.line	48; usb.c	void init_usb(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	50; usb.c	UIE  = 0;
	CLRF	_UIE
;	.line	51; usb.c	UCFG = 0x14;
	MOVLW	0x14
	MOVWF	_UCFG
;	.line	52; usb.c	UCON = 0x08;
	MOVLW	0x08
	MOVWF	_UCON
_00105_DS_:
;	.line	53; usb.c	while(UCONbits.SE0);
	BTFSC	_UCONbits, 5
	GOTO	_00105_DS_
;	.line	54; usb.c	UIR  = 0;
	CLRF	_UIR
;	.line	55; usb.c	UIE  = 0x11;
	MOVLW	0x11
	MOVWF	_UIE
;	.line	58; usb.c	SET_DEVICE_STATE(POWERED_STATE);
	MOVLW	0x02
	MOVWF	_usb_device_state
;	.line	61; usb.c	SET_ACTIVE_CONFIGURATION(0);
	CLRF	_usb_active_cfg
;	.line	62; usb.c	SET_ACTIVE_ALTERNATE_SETTING(0);
	CLRF	_usb_active_alt_setting
	MOVFF	PREINC1, FSR2L
	RETURN	



; Statistics:
; code size:	 1754 (0x06da) bytes ( 1.34%)
;           	  877 (0x036d) words
; udata size:	  158 (0x009e) bytes ( 8.82%)
; access size:	   12 (0x000c) bytes


	end
