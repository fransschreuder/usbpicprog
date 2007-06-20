;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.7.2 #4854 (Jun 17 2007)
; This file generated Wed Jun 20 08:33:06 2007
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f2550

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _ep1_init
	global _ep1_out
	global _ep1_OutBuffer

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
	extern _prepare_ep2_in
	extern _erase_block
	extern _write_block
	extern _delay10ktcy
	extern __mulint
	extern __mullong
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
WREG	equ	0xfe8
FSR0L	equ	0xfe9
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
POSTDEC1	equ	0xfe5
PREINC1	equ	0xfe4
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


usb_buf	udata
_ep1_OutBuffer	res	64

udata_ep1_0	udata
iTemp0	res	1

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_ep1__ep1_out	code
_ep1_out:
;	.line	44; ep1.c	void ep1_out(void)
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
;	.line	47; ep1.c	if(EP_OUT_BD(1).Cnt >= 1)
	MOVFF	(_ep_bdt + 9), (iTemp0 + 1)
	MOVLW	0x01
	BANKSEL	(iTemp0 + 1)
	SUBWF	(iTemp0 + 1), W, B
	BTFSS	STATUS, 0
	GOTO	_00124_DS_
;	.line	49; ep1.c	switch(ep1_OutBuffer[0])
	MOVFF	_ep1_OutBuffer, r0x00
	MOVLW	0x06
	SUBWF	r0x00, W
	BTFSC	STATUS, 0
	GOTO	_00118_DS_
	MOVFF	r0x08, POSTDEC1
	MOVFF	r0x09, POSTDEC1
	CLRF	r0x09
	RLCF	r0x00, W
	RLCF	r0x09, F
	RLCF	WREG, W
	RLCF	r0x09, F
	ANDLW	0xfc
	MOVWF	r0x08
	MOVLW	UPPER(_00133_DS_)
	MOVWF	PCLATU
	MOVLW	HIGH(_00133_DS_)
	MOVWF	PCLATH
	MOVLW	LOW(_00133_DS_)
	ADDWF	r0x08, F
	MOVF	r0x09, W
	ADDWFC	PCLATH, F
	BTFSC	STATUS, 0
	INCF	PCLATU, F
	MOVF	r0x08, W
	MOVFF	PREINC1, r0x09
	MOVFF	PREINC1, r0x08
	MOVWF	PCL
_00133_DS_:
	GOTO	_00109_DS_
	GOTO	_00110_DS_
	GOTO	_00111_DS_
	GOTO	_00112_DS_
	GOTO	_00113_DS_
	GOTO	_00117_DS_
_00109_DS_:
;	.line	53; ep1.c	TBLPTRL = ep1_OutBuffer[1];
	MOVFF	(_ep1_OutBuffer + 1), _TBLPTRL
;	.line	54; ep1.c	TBLPTRH = ep1_OutBuffer[2];
	MOVFF	(_ep1_OutBuffer + 2), _TBLPTRH
;	.line	55; ep1.c	TBLPTRU = ep1_OutBuffer[3];
	MOVFF	(_ep1_OutBuffer + 3), _TBLPTRU
;	.line	56; ep1.c	erase_block();
	CALL	_erase_block
;	.line	57; ep1.c	break;
	GOTO	_00119_DS_
_00110_DS_:
;	.line	60; ep1.c	TBLPTRL = ep1_OutBuffer[1];
	MOVFF	(_ep1_OutBuffer + 1), _TBLPTRL
;	.line	61; ep1.c	TBLPTRH = ep1_OutBuffer[2];
	MOVFF	(_ep1_OutBuffer + 2), _TBLPTRH
;	.line	62; ep1.c	TBLPTRU = ep1_OutBuffer[3];
	MOVFF	(_ep1_OutBuffer + 3), _TBLPTRU
;	.line	63; ep1.c	write_block((__data uchar *)&(ep1_OutBuffer[4]));
	MOVLW	HIGH(_ep1_OutBuffer + 4)
	MOVWF	POSTDEC1
	MOVLW	LOW(_ep1_OutBuffer + 4)
	MOVWF	POSTDEC1
	CALL	_write_block
	MOVLW	0x02
	ADDWF	FSR1L, F
;	.line	64; ep1.c	break;
	GOTO	_00119_DS_
_00111_DS_:
;	.line	67; ep1.c	ep2_num_bytes_to_send = EP2_BUFFER_SIZE;
	MOVLW	0x40
	BANKSEL	_ep2_num_bytes_to_send
	MOVWF	_ep2_num_bytes_to_send, B
	BANKSEL	(_ep2_num_bytes_to_send + 1)
	CLRF	(_ep2_num_bytes_to_send + 1), B
;	.line	69; ep1.c	ep2_source_data = (uchar __code *) (ep1_OutBuffer[1] + ep1_OutBuffer[2] * 256 + ep1_OutBuffer[3] * 65536);
	MOVFF	(_ep1_OutBuffer + 1), r0x00
	CLRF	r0x01
	MOVFF	(_ep1_OutBuffer + 2), r0x02
	CLRF	r0x03
	MOVF	r0x03, W
	MOVWF	POSTDEC1
	MOVF	r0x02, W
	MOVWF	POSTDEC1
	MOVLW	0x01
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	CALL	__mulint
	MOVWF	r0x02
	MOVFF	PRODL, r0x03
	MOVLW	0x04
	ADDWF	FSR1L, F
	MOVF	r0x02, W
	ADDWF	r0x00, F
	MOVF	r0x03, W
	ADDWFC	r0x01, F
	MOVFF	(_ep1_OutBuffer + 3), r0x02
	CLRF	r0x03
	CLRF	r0x04
	CLRF	r0x05
	MOVF	r0x05, W
	MOVWF	POSTDEC1
	MOVF	r0x04, W
	MOVWF	POSTDEC1
	MOVF	r0x03, W
	MOVWF	POSTDEC1
	MOVF	r0x02, W
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x01
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	CALL	__mullong
	MOVWF	r0x02
	MOVFF	PRODL, r0x03
	MOVFF	PRODH, r0x04
	MOVFF	FSR0L, r0x05
	MOVLW	0x08
	ADDWF	FSR1L, F
	CLRF	WREG
	BTFSC	r0x01, 7
	MOVLW	0xff
	MOVWF	r0x06
	MOVWF	r0x07
	MOVF	r0x00, W
	ADDWF	r0x02, F
	MOVF	r0x01, W
	ADDWFC	r0x03, F
	MOVF	r0x06, W
	ADDWFC	r0x04, F
	MOVF	r0x07, W
	ADDWFC	r0x05, F
	MOVF	r0x04, W
	BANKSEL	(_ep2_source_data + 2)
	MOVWF	(_ep2_source_data + 2), B
	MOVF	r0x03, W
	BANKSEL	(_ep2_source_data + 1)
	MOVWF	(_ep2_source_data + 1), B
	MOVF	r0x02, W
	BANKSEL	_ep2_source_data
	MOVWF	_ep2_source_data, B
;	.line	70; ep1.c	prepare_ep2_in();
	CALL	_prepare_ep2_in
;	.line	71; ep1.c	break;
	GOTO	_00119_DS_
_00112_DS_:
;	.line	74; ep1.c	break;
	GOTO	_00119_DS_
_00113_DS_:
;	.line	77; ep1.c	delay10ktcy(10);
	MOVLW	0x0a
	CALL	_delay10ktcy
	reset 
;	.line	79; ep1.c	break;
	GOTO	_00119_DS_
_00117_DS_:
;	.line	82; ep1.c	ep2_num_bytes_to_send = sizeof(section_descriptor);
	MOVLW	0x16
	BANKSEL	_ep2_num_bytes_to_send
	MOVWF	_ep2_num_bytes_to_send, B
	BANKSEL	(_ep2_num_bytes_to_send + 1)
	CLRF	(_ep2_num_bytes_to_send + 1), B
;	.line	83; ep1.c	ep2_source_data = (uchar __code *) section_descriptor;
	MOVLW	LOW(_section_descriptor)
	BANKSEL	_ep2_source_data
	MOVWF	_ep2_source_data, B
	MOVLW	HIGH(_section_descriptor)
	BANKSEL	(_ep2_source_data + 1)
	MOVWF	(_ep2_source_data + 1), B
	MOVLW	UPPER(_section_descriptor)
	BANKSEL	(_ep2_source_data + 2)
	MOVWF	(_ep2_source_data + 2), B
;	.line	84; ep1.c	prepare_ep2_in();
	CALL	_prepare_ep2_in
;	.line	85; ep1.c	break;
	GOTO	_00119_DS_
_00118_DS_:
;	.line	90; ep1.c	EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
	MOVLW	0x40
	BANKSEL	(_ep_bdt + 9)
	MOVWF	(_ep_bdt + 9), B
;	.line	91; ep1.c	EP_OUT_BD(1).ADR = (uchar __data *)&ep1_OutBuffer;
	MOVLW	LOW(_ep1_OutBuffer)
	BANKSEL	(_ep_bdt + 10)
	MOVWF	(_ep_bdt + 10), B
	MOVLW	HIGH(_ep1_OutBuffer)
	BANKSEL	(_ep_bdt + 11)
	MOVWF	(_ep_bdt + 11), B
;	.line	92; ep1.c	EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_BSTALL;
	MOVLW	0x84
	BANKSEL	(_ep_bdt + 8)
	MOVWF	(_ep_bdt + 8), B
_00119_DS_:
;	.line	95; ep1.c	EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
	MOVLW	0x40
	BANKSEL	(_ep_bdt + 9)
	MOVWF	(_ep_bdt + 9), B
	BANKSEL	(_ep_bdt + 8)
;	.line	96; ep1.c	if(EP_OUT_BD(1).Stat.DTS == 0)
	BTFSC	(_ep_bdt + 8), 6, B
	GOTO	_00121_DS_
;	.line	98; ep1.c	EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
	MOVLW	0xc8
	BANKSEL	(_ep_bdt + 8)
	MOVWF	(_ep_bdt + 8), B
	GOTO	_00126_DS_
_00121_DS_:
;	.line	102; ep1.c	EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
	MOVLW	0x88
	BANKSEL	(_ep_bdt + 8)
	MOVWF	(_ep_bdt + 8), B
	GOTO	_00126_DS_
_00124_DS_:
;	.line	107; ep1.c	EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
	MOVLW	0x40
	BANKSEL	(_ep_bdt + 9)
	MOVWF	(_ep_bdt + 9), B
;	.line	108; ep1.c	EP_OUT_BD(1).ADR = (uchar __data *)&ep1_OutBuffer;
	MOVLW	LOW(_ep1_OutBuffer)
	BANKSEL	(_ep_bdt + 10)
	MOVWF	(_ep_bdt + 10), B
	MOVLW	HIGH(_ep1_OutBuffer)
	BANKSEL	(_ep_bdt + 11)
	MOVWF	(_ep_bdt + 11), B
;	.line	109; ep1.c	EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_BSTALL;
	MOVLW	0x84
	BANKSEL	(_ep_bdt + 8)
	MOVWF	(_ep_bdt + 8), B
_00126_DS_:
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
S_ep1__ep1_init	code
_ep1_init:
;	.line	35; ep1.c	void ep1_init(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	38; ep1.c	EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
	MOVLW	0x40
	BANKSEL	(_ep_bdt + 9)
	MOVWF	(_ep_bdt + 9), B
;	.line	39; ep1.c	EP_OUT_BD(1).ADR = (uchar __data *)&ep1_OutBuffer;
	MOVLW	LOW(_ep1_OutBuffer)
	BANKSEL	(_ep_bdt + 10)
	MOVWF	(_ep_bdt + 10), B
	MOVLW	HIGH(_ep1_OutBuffer)
	BANKSEL	(_ep_bdt + 11)
	MOVWF	(_ep_bdt + 11), B
;	.line	40; ep1.c	EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
	MOVLW	0x88
	BANKSEL	(_ep_bdt + 8)
	MOVWF	(_ep_bdt + 8), B
;	.line	41; ep1.c	UEP1 = EPHSHK_EN | EPOUTEN_EN | EPCONDIS_EN;       // Init EP1 as an OUT EP
	MOVLW	0x1c
	MOVWF	_UEP1
	MOVFF	PREINC1, FSR2L
	RETURN	



; Statistics:
; code size:	  590 (0x024e) bytes ( 0.45%)
;           	  295 (0x0127) words
; udata size:	   65 (0x0041) bytes ( 3.63%)
; access size:	   10 (0x000a) bytes


	end
