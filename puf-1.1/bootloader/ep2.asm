;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.7.2 #4854 (Jun 17 2007)
; This file generated Thu Jun 21 10:41:01 2007
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f4550

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _ep2_init
	global _prepare_ep2_in
	global _ep2_in
	global _ep2_InBuffer

;--------------------------------------------------------
; extern variables in this module
;--------------------------------------------------------
	extern _ep_bdt
	extern _usb_device_state
	extern _usb_active_cfg
	extern _usb_active_alt_setting
	extern _device_descriptor
	extern _configuration_descriptor
	extern _string_descriptor
	extern _ep_init
	extern _ep_in
	extern _ep_out
	extern _ep_setup
	extern _ep2_num_bytes_to_send
	extern _ep2_source_data
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
	extern _fill_in_buffer
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
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
POSTDEC1	equ	0xfe5
PREINC1	equ	0xfe4


; Internal registers
.registers	udata_ovr	0x0000
r0x00	res	1
r0x01	res	1
r0x02	res	1
r0x03	res	1
r0x04	res	1
r0x05	res	1


usb_buf	udata
_ep2_InBuffer	res	64

udata_ep2_0	udata
_last_send_was_null	res	1

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_ep2__ep2_in	code
_ep2_in:
;	.line	61; ep2.c	void ep2_in(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	MOVFF	r0x05, POSTDEC1
	BANKSEL	_ep2_num_bytes_to_send
;	.line	64; ep2.c	if((ep2_num_bytes_to_send == 0) && last_send_was_null)
	MOVF	_ep2_num_bytes_to_send, W, B
	BANKSEL	(_ep2_num_bytes_to_send + 1)
	IORWF	(_ep2_num_bytes_to_send + 1), W, B
	BTFSS	STATUS, 2
	GOTO	_00122_DS_
	BANKSEL	_last_send_was_null
	MOVF	_last_send_was_null, W, B
	BTFSC	STATUS, 2
	GOTO	_00122_DS_
;	.line	68; ep2.c	return;
	GOTO	_00127_DS_
_00122_DS_:
;	.line	70; ep2.c	last_send_was_null = (ep2_num_bytes_to_send < EP2_BUFFER_SIZE);
	MOVLW	0x00
	BANKSEL	(_ep2_num_bytes_to_send + 1)
	SUBWF	(_ep2_num_bytes_to_send + 1), W, B
	BNZ	_00133_DS_
	MOVLW	0x40
	BANKSEL	_ep2_num_bytes_to_send
	SUBWF	_ep2_num_bytes_to_send, W, B
_00133_DS_:
	BTG	STATUS, 0
	BANKSEL	_last_send_was_null
	CLRF	_last_send_was_null, B
	BANKSEL	_last_send_was_null
	RLCF	_last_send_was_null, F, B
	BANKSEL	_ep2_num_bytes_to_send
;	.line	71; ep2.c	EP_IN_BD(2).Cnt = ep2_num_bytes_to_send;
	MOVF	_ep2_num_bytes_to_send, W, B
	MOVWF	r0x00
	MOVF	r0x00, W
	BANKSEL	(_ep_bdt + 21)
	MOVWF	(_ep_bdt + 21), B
;	.line	72; ep2.c	fill_in_buffer(2, &ep2_source_data, EP2_BUFFER_SIZE, &ep2_num_bytes_to_send);
	MOVLW	HIGH(_ep2_source_data)
	MOVWF	r0x01
	MOVLW	LOW(_ep2_source_data)
	MOVWF	r0x00
	MOVLW	0x80
	MOVWF	r0x02
	MOVLW	HIGH(_ep2_num_bytes_to_send)
	MOVWF	r0x04
	MOVLW	LOW(_ep2_num_bytes_to_send)
	MOVWF	r0x03
	MOVLW	0x80
	MOVWF	r0x05
	MOVF	r0x05, W
	MOVWF	POSTDEC1
	MOVF	r0x04, W
	MOVWF	POSTDEC1
	MOVF	r0x03, W
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x40
	MOVWF	POSTDEC1
	MOVF	r0x02, W
	MOVWF	POSTDEC1
	MOVF	r0x01, W
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x02
	MOVWF	POSTDEC1
	CALL	_fill_in_buffer
	MOVLW	0x09
	ADDWF	FSR1L, F
	BANKSEL	(_ep_bdt + 20)
;	.line	73; ep2.c	if(EP_IN_BD(2).Stat.DTS == 0)
	BTFSC	(_ep_bdt + 20), 6, B
	GOTO	_00125_DS_
;	.line	75; ep2.c	EP_IN_BD(2).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
	MOVLW	0xc8
	BANKSEL	(_ep_bdt + 20)
	MOVWF	(_ep_bdt + 20), B
	GOTO	_00127_DS_
_00125_DS_:
;	.line	79; ep2.c	EP_IN_BD(2).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
	MOVLW	0x88
	BANKSEL	(_ep_bdt + 20)
	MOVWF	(_ep_bdt + 20), B
_00127_DS_:
	MOVFF	PREINC1, r0x05
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_ep2__prepare_ep2_in	code
_prepare_ep2_in:
;	.line	44; ep2.c	void prepare_ep2_in(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	MOVFF	r0x05, POSTDEC1
;	.line	47; ep2.c	last_send_was_null = (ep2_num_bytes_to_send < EP2_BUFFER_SIZE);
	MOVLW	0x00
	BANKSEL	(_ep2_num_bytes_to_send + 1)
	SUBWF	(_ep2_num_bytes_to_send + 1), W, B
	BNZ	_00116_DS_
	MOVLW	0x40
	BANKSEL	_ep2_num_bytes_to_send
	SUBWF	_ep2_num_bytes_to_send, W, B
_00116_DS_:
	BTG	STATUS, 0
	BANKSEL	_last_send_was_null
	CLRF	_last_send_was_null, B
	BANKSEL	_last_send_was_null
	RLCF	_last_send_was_null, F, B
	BANKSEL	_ep2_num_bytes_to_send
;	.line	48; ep2.c	EP_IN_BD(2).Cnt = ep2_num_bytes_to_send;
	MOVF	_ep2_num_bytes_to_send, W, B
	MOVWF	r0x00
	MOVF	r0x00, W
	BANKSEL	(_ep_bdt + 21)
	MOVWF	(_ep_bdt + 21), B
;	.line	49; ep2.c	EP_IN_BD(2).ADR = (uchar __data *)&ep2_InBuffer;
	MOVLW	LOW(_ep2_InBuffer)
	BANKSEL	(_ep_bdt + 22)
	MOVWF	(_ep_bdt + 22), B
	MOVLW	HIGH(_ep2_InBuffer)
	BANKSEL	(_ep_bdt + 23)
	MOVWF	(_ep_bdt + 23), B
;	.line	50; ep2.c	fill_in_buffer(2, &ep2_source_data, EP2_BUFFER_SIZE, &ep2_num_bytes_to_send);
	MOVLW	HIGH(_ep2_source_data)
	MOVWF	r0x01
	MOVLW	LOW(_ep2_source_data)
	MOVWF	r0x00
	MOVLW	0x80
	MOVWF	r0x02
	MOVLW	HIGH(_ep2_num_bytes_to_send)
	MOVWF	r0x04
	MOVLW	LOW(_ep2_num_bytes_to_send)
	MOVWF	r0x03
	MOVLW	0x80
	MOVWF	r0x05
	MOVF	r0x05, W
	MOVWF	POSTDEC1
	MOVF	r0x04, W
	MOVWF	POSTDEC1
	MOVF	r0x03, W
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x40
	MOVWF	POSTDEC1
	MOVF	r0x02, W
	MOVWF	POSTDEC1
	MOVF	r0x01, W
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x02
	MOVWF	POSTDEC1
	CALL	_fill_in_buffer
	MOVLW	0x09
	ADDWF	FSR1L, F
	BANKSEL	(_ep_bdt + 20)
;	.line	51; ep2.c	if(EP_IN_BD(2).Stat.DTS == 0)
	BTFSC	(_ep_bdt + 20), 6, B
	GOTO	_00110_DS_
;	.line	53; ep2.c	EP_IN_BD(2).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
	MOVLW	0xc8
	BANKSEL	(_ep_bdt + 20)
	MOVWF	(_ep_bdt + 20), B
	GOTO	_00112_DS_
_00110_DS_:
;	.line	57; ep2.c	EP_IN_BD(2).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
	MOVLW	0x88
	BANKSEL	(_ep_bdt + 20)
	MOVWF	(_ep_bdt + 20), B
_00112_DS_:
	MOVFF	PREINC1, r0x05
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_ep2__ep2_init	code
_ep2_init:
;	.line	34; ep2.c	void ep2_init(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	40; ep2.c	EP_IN_BD(2).Stat.uc = BDS_UCPU | BDS_DAT1 | BDS_DTSEN;
	MOVLW	0x48
	BANKSEL	(_ep_bdt + 20)
	MOVWF	(_ep_bdt + 20), B
;	.line	41; ep2.c	UEP2 = EPHSHK_EN | EPINEN_EN | EPCONDIS_EN;       // Init EP2 as an IN EP
	MOVLW	0x1a
	MOVWF	_UEP2
	MOVFF	PREINC1, FSR2L
	RETURN	



; Statistics:
; code size:	  444 (0x01bc) bytes ( 0.34%)
;           	  222 (0x00de) words
; udata size:	   65 (0x0041) bytes ( 3.63%)
; access size:	    6 (0x0006) bytes


	end
