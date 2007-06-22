;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.7.2 #4854 (Jun 17 2007)
; This file generated Fri Jun 22 08:42:01 2007
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f4550

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _picon_init
	global _prepare_in
	global _picon_in
	global _putchar
	global _prepare_next_packet

;--------------------------------------------------------
; extern variables in this module
;--------------------------------------------------------
	extern __gptrput1
	extern _ep_bdt
	extern _usb_device_state
	extern _usb_active_cfg
	extern _usb_active_alt_setting
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
	extern _fill_in_buffer
	extern _picon_buffer_size
	extern _Picon_InBuffer
	extern _picon_ep
;--------------------------------------------------------
;	Equates to used internal registers
;--------------------------------------------------------
STATUS	equ	0xfd8
WREG	equ	0xfe8
TBLPTRL	equ	0xff6
TBLPTRH	equ	0xff7
TBLPTRU	equ	0xff8
TABLAT	equ	0xff5
FSR0L	equ	0xfe9
FSR0H	equ	0xfea
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
INDF0	equ	0xfef
POSTINC0	equ	0xfee
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

udata_picon_0	udata
_last_packet_was_void	res	1

udata_picon_1	udata
_num_bytes_to_send	res	2

udata_picon_2	udata
_num_bytes	res	1

udata_picon_3	udata
_send_in_progress	res	1

udata_picon_4	udata
_bottom	res	3

udata_picon_5	udata
_top	res	3

udata_picon_6	udata
_tmp_bottom	res	3

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_picon__putchar	code
_putchar:
;	.line	191; picon.c	PUTCHAR(c)
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
	MOVWF	r0x00
;	.line	196; picon.c	ib = INTCONbits.GIE;
	CLRF	r0x01
	BTFSC	_INTCONbits, 7
	INCF	r0x01, F
;	.line	197; picon.c	INTCONbits.GIE = 0;
	BCF	_INTCONbits, 7
;	.line	199; picon.c	if(top >= (Picon_InBuffer + picon_buffer_size))
	MOVLW	LOW(_picon_buffer_size)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_picon_buffer_size)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_picon_buffer_size)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x02
	TBLRD*+	
	MOVFF	TABLAT, r0x03
	MOVLW	LOW(_Picon_InBuffer)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_Picon_InBuffer)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_Picon_InBuffer)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x04
	TBLRD*+	
	MOVFF	TABLAT, r0x05
	TBLRD*+	
	MOVFF	TABLAT, r0x06
	MOVF	r0x04, W
	ADDWF	r0x02, F
	MOVF	r0x05, W
	ADDWFC	r0x03, F
	MOVF	r0x06, W
	CLRF	r0x07
	ADDWFC	r0x07, F
	MOVF	r0x07, W
	BANKSEL	(_top + 2)
	SUBWF	(_top + 2), W, B
	BNZ	_00177_DS_
	MOVF	r0x03, W
	BANKSEL	(_top + 1)
	SUBWF	(_top + 1), W, B
	BNZ	_00177_DS_
	MOVF	r0x02, W
	BANKSEL	_top
	SUBWF	_top, W, B
_00177_DS_:
	BTFSS	STATUS, 0
	GOTO	_00165_DS_
;	.line	201; picon.c	top = Picon_InBuffer;
	MOVLW	LOW(_Picon_InBuffer)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_Picon_InBuffer)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_Picon_InBuffer)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _top
	TBLRD*+	
	MOVFF	TABLAT, (_top + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_top + 2)
_00165_DS_:
	BANKSEL	_top
;	.line	204; picon.c	if((top == bottom) && (num_bytes != 0))
	MOVF	_top, W, B
	BANKSEL	_bottom
	XORWF	_bottom, W, B
	BNZ	_00178_DS_
	BANKSEL	(_top + 1)
	MOVF	(_top + 1), W, B
	BANKSEL	(_bottom + 1)
	XORWF	(_bottom + 1), W, B
	BNZ	_00178_DS_
	BANKSEL	(_top + 2)
	MOVF	(_top + 2), W, B
	BANKSEL	(_bottom + 2)
	XORWF	(_bottom + 2), W, B
	BNZ	_00178_DS_
	GOTO	_00179_DS_
_00178_DS_:
	GOTO	_00167_DS_
_00179_DS_:
	BANKSEL	_num_bytes
	MOVF	_num_bytes, W, B
	BTFSC	STATUS, 2
	GOTO	_00167_DS_
;	.line	208; picon.c	INTCONbits.GIE = ib;
	MOVF	r0x01, W
	MOVWF	r0x02
	MOVF	r0x02, W
	ANDLW	0x01
	RRNCF	WREG, W
	MOVWF	PRODH
	MOVF	_INTCONbits, W
	ANDLW	0x7f
	IORWF	PRODH, W
	MOVWF	_INTCONbits
;	.line	209; picon.c	return;
	GOTO	_00171_DS_
_00167_DS_:
;	.line	212; picon.c	*top = c;
	MOVFF	_top, r0x02
	MOVFF	(_top + 1), r0x03
	MOVFF	(_top + 2), r0x04
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x02, FSR0L
	MOVFF	r0x03, PRODL
	MOVF	r0x04, W
	CALL	__gptrput1
	BANKSEL	_top
;	.line	213; picon.c	top++;
	INCF	_top, F, B
	BTFSS	STATUS, 0
	BRA	_10164_DS_
	BANKSEL	(_top + 1)
	INCF	(_top + 1), F, B
_10164_DS_:
	BTFSS	STATUS, 0
	BRA	_20165_DS_
	BANKSEL	(_top + 2)
	INCF	(_top + 2), F, B
_20165_DS_:
	BANKSEL	_num_bytes
;	.line	214; picon.c	num_bytes++;
	INCF	_num_bytes, F, B
	BANKSEL	_send_in_progress
;	.line	217; picon.c	if(!send_in_progress)
	MOVF	_send_in_progress, W, B
	BTFSS	STATUS, 2
	GOTO	_00170_DS_
;	.line	219; picon.c	send_in_progress = TRUE;
	MOVLW	0x01
	BANKSEL	_send_in_progress
	MOVWF	_send_in_progress, B
;	.line	220; picon.c	prepare_in();
	CALL	_prepare_in
_00170_DS_:
;	.line	223; picon.c	INTCONbits.GIE = ib;
	MOVF	r0x01, W
	ANDLW	0x01
	RRNCF	WREG, W
	MOVWF	PRODH
	MOVF	_INTCONbits, W
	ANDLW	0x7f
	IORWF	PRODH, W
	MOVWF	_INTCONbits
_00171_DS_:
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
S_picon__picon_in	code
_picon_in:
;	.line	150; picon.c	void picon_in(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
;	.line	155; picon.c	ib = INTCONbits.GIE;
	CLRF	r0x00
	BTFSC	_INTCONbits, 7
	INCF	r0x00, F
;	.line	156; picon.c	INTCONbits.GIE = 0;
	BCF	_INTCONbits, 7
;	.line	158; picon.c	bottom = tmp_bottom; // Flush the sent chars
	MOVFF	_tmp_bottom, _bottom
	MOVFF	(_tmp_bottom + 1), (_bottom + 1)
	MOVFF	(_tmp_bottom + 2), (_bottom + 2)
;	.line	160; picon.c	if((bottom == Picon_InBuffer) && (num_bytes != 0))
	MOVLW	LOW(_Picon_InBuffer)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_Picon_InBuffer)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_Picon_InBuffer)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x01
	TBLRD*+	
	MOVFF	TABLAT, r0x02
	TBLRD*+	
	MOVFF	TABLAT, r0x03
	BANKSEL	_bottom
	MOVF	_bottom, W, B
	XORWF	r0x01, W
	BNZ	_00158_DS_
	BANKSEL	(_bottom + 1)
	MOVF	(_bottom + 1), W, B
	XORWF	r0x02, W
	BNZ	_00158_DS_
	BANKSEL	(_bottom + 2)
	MOVF	(_bottom + 2), W, B
	XORWF	r0x03, W
	BNZ	_00158_DS_
	GOTO	_00159_DS_
_00158_DS_:
	GOTO	_00143_DS_
_00159_DS_:
	BANKSEL	_num_bytes
	MOVF	_num_bytes, W, B
	BTFSC	STATUS, 2
	GOTO	_00143_DS_
;	.line	162; picon.c	num_bytes_to_send = num_bytes;
	MOVFF	_num_bytes, _num_bytes_to_send
	BANKSEL	(_num_bytes_to_send + 1)
	CLRF	(_num_bytes_to_send + 1), B
_00143_DS_:
	BANKSEL	_num_bytes_to_send
;	.line	165; picon.c	if((num_bytes_to_send == 0) && last_packet_was_void)
	MOVF	_num_bytes_to_send, W, B
	BANKSEL	(_num_bytes_to_send + 1)
	IORWF	(_num_bytes_to_send + 1), W, B
	BTFSS	STATUS, 2
	GOTO	_00149_DS_
	BANKSEL	_last_packet_was_void
	MOVF	_last_packet_was_void, W, B
	BTFSC	STATUS, 2
	GOTO	_00149_DS_
	BANKSEL	_num_bytes
;	.line	168; picon.c	if(num_bytes != 0)
	MOVF	_num_bytes, W, B
	BTFSC	STATUS, 2
	GOTO	_00146_DS_
;	.line	170; picon.c	prepare_in();
	CALL	_prepare_in
	GOTO	_00147_DS_
_00146_DS_:
	BANKSEL	_send_in_progress
;	.line	178; picon.c	send_in_progress = FALSE;
	CLRF	_send_in_progress, B
_00147_DS_:
;	.line	182; picon.c	INTCONbits.GIE = ib;
	MOVF	r0x00, W
	MOVWF	r0x01
	MOVF	r0x01, W
	ANDLW	0x01
	RRNCF	WREG, W
	MOVWF	PRODH
	MOVF	_INTCONbits, W
	ANDLW	0x7f
	IORWF	PRODH, W
	MOVWF	_INTCONbits
;	.line	183; picon.c	return;
	GOTO	_00151_DS_
_00149_DS_:
;	.line	186; picon.c	prepare_next_packet();
	CALL	_prepare_next_packet
;	.line	188; picon.c	INTCONbits.GIE = ib;
	MOVF	r0x00, W
	ANDLW	0x01
	RRNCF	WREG, W
	MOVWF	PRODH
	MOVF	_INTCONbits, W
	ANDLW	0x7f
	IORWF	PRODH, W
	MOVWF	_INTCONbits
_00151_DS_:
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_picon__prepare_in	code
_prepare_in:
;	.line	123; picon.c	void prepare_in(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	MOVFF	r0x05, POSTDEC1
;	.line	129; picon.c	ib = INTCONbits.GIE;
	CLRF	r0x00
	BTFSC	_INTCONbits, 7
	INCF	r0x00, F
;	.line	130; picon.c	INTCONbits.GIE = 0;
	BCF	_INTCONbits, 7
	BANKSEL	(_top + 2)
;	.line	132; picon.c	if((top <= bottom) && (num_bytes > 0))
	MOVF	(_top + 2), W, B
	BANKSEL	(_bottom + 2)
	SUBWF	(_bottom + 2), W, B
	BNZ	_00137_DS_
	BANKSEL	(_top + 1)
	MOVF	(_top + 1), W, B
	BANKSEL	(_bottom + 1)
	SUBWF	(_bottom + 1), W, B
	BNZ	_00137_DS_
	BANKSEL	_top
	MOVF	_top, W, B
	BANKSEL	_bottom
	SUBWF	_bottom, W, B
_00137_DS_:
	BTFSS	STATUS, 0
	GOTO	_00130_DS_
	BANKSEL	_num_bytes
	MOVF	_num_bytes, W, B
	BTFSC	STATUS, 2
	GOTO	_00130_DS_
;	.line	135; picon.c	top_send_buffer = Picon_InBuffer + picon_buffer_size;
	MOVLW	LOW(_picon_buffer_size)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_picon_buffer_size)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_picon_buffer_size)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x01
	TBLRD*+	
	MOVFF	TABLAT, r0x02
	MOVLW	LOW(_Picon_InBuffer)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_Picon_InBuffer)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_Picon_InBuffer)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x03
	TBLRD*+	
	MOVFF	TABLAT, r0x04
	TBLRD*+	
	MOVFF	TABLAT, r0x05
	MOVF	r0x01, W
	ADDWF	r0x03, W
	MOVWF	r0x03
	MOVF	r0x02, W
	ADDWFC	r0x04, W
	MOVWF	r0x02
	CLRF	WREG
	ADDWFC	r0x05, W
	MOVWF	r0x01
	GOTO	_00131_DS_
_00130_DS_:
;	.line	139; picon.c	top_send_buffer = top;
	MOVFF	_top, r0x03
	MOVFF	(_top + 1), r0x02
	MOVFF	(_top + 2), r0x01
_00131_DS_:
	BANKSEL	_bottom
;	.line	143; picon.c	num_bytes_to_send = top_send_buffer - bottom;
	MOVF	_bottom, W, B
	SUBWF	r0x03, W
	BANKSEL	_num_bytes_to_send
	MOVWF	_num_bytes_to_send, B
	BANKSEL	(_bottom + 1)
	MOVF	(_bottom + 1), W, B
	SUBWFB	r0x02, W
	BANKSEL	(_num_bytes_to_send + 1)
	MOVWF	(_num_bytes_to_send + 1), B
;	.line	145; picon.c	prepare_next_packet();
	CALL	_prepare_next_packet
;	.line	147; picon.c	INTCONbits.GIE = ib;
	MOVF	r0x00, W
	ANDLW	0x01
	RRNCF	WREG, W
	MOVWF	PRODH
	MOVF	_INTCONbits, W
	ANDLW	0x7f
	IORWF	PRODH, W
	MOVWF	_INTCONbits
	MOVFF	PREINC1, r0x05
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_picon__prepare_next_packet	code
_prepare_next_packet:
;	.line	85; picon.c	void prepare_next_packet(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	MOVFF	r0x05, POSTDEC1
	MOVFF	r0x06, POSTDEC1
;	.line	87; picon.c	last_packet_was_void = (num_bytes_to_send < PICON_PACKET_SIZE);
	MOVLW	0x00
	BANKSEL	(_num_bytes_to_send + 1)
	SUBWF	(_num_bytes_to_send + 1), W, B
	BNZ	_00123_DS_
	MOVLW	0x08
	BANKSEL	_num_bytes_to_send
	SUBWF	_num_bytes_to_send, W, B
_00123_DS_:
	BTG	STATUS, 0
	BANKSEL	_last_packet_was_void
	CLRF	_last_packet_was_void, B
	BANKSEL	_last_packet_was_void
	RLCF	_last_packet_was_void, F, B
	BANKSEL	_last_packet_was_void
;	.line	90; picon.c	if(last_packet_was_void)
	MOVF	_last_packet_was_void, W, B
	BTFSC	STATUS, 2
	GOTO	_00110_DS_
	BANKSEL	_num_bytes_to_send
;	.line	92; picon.c	num_bytes -= num_bytes_to_send;
	MOVF	_num_bytes_to_send, W, B
	MOVWF	r0x00
	MOVF	r0x00, W
	BANKSEL	_num_bytes
	SUBWF	_num_bytes, F, B
	GOTO	_00111_DS_
_00110_DS_:
;	.line	96; picon.c	num_bytes -= PICON_PACKET_SIZE;
	MOVLW	0xf8
	BANKSEL	_num_bytes
	ADDWF	_num_bytes, F, B
_00111_DS_:
;	.line	100; picon.c	EP_IN_BD(picon_ep).ADR = (uchar __data *) bottom;
	MOVLW	LOW(_picon_ep)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_picon_ep)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_picon_ep)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x00
	MOVF	r0x00, W
	ADDWF	r0x00, W
	MOVWF	r0x01
	INCF	r0x01, F
; ;multiply lit val:0x04 by variable r0x01 and store in r0x01
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	MOVF	r0x01, W
	MULLW	0x04
	MOVFF	PRODL, r0x01
	MOVLW	LOW(_ep_bdt)
	ADDWF	r0x01, F
	MOVLW	HIGH(_ep_bdt)
	CLRF	r0x02
	ADDWFC	r0x02, F
	MOVLW	0x02
	ADDWF	r0x01, F
	BTFSC	STATUS, 0
	INCF	r0x02, F
	BANKSEL	_bottom
	MOVF	_bottom, W, B
	MOVWF	r0x03
	BANKSEL	(_bottom + 1)
	MOVF	(_bottom + 1), W, B
	MOVWF	r0x04
	MOVFF	r0x01, FSR0L
	MOVFF	r0x02, FSR0H
	MOVFF	r0x03, POSTINC0
	MOVFF	r0x04, INDF0
;	.line	103; picon.c	tmp_bottom = bottom;
	MOVFF	_bottom, _tmp_bottom
	MOVFF	(_bottom + 1), (_tmp_bottom + 1)
	MOVFF	(_bottom + 2), (_tmp_bottom + 2)
;	.line	104; picon.c	fill_in_buffer(picon_ep, &tmp_bottom, PICON_PACKET_SIZE, &num_bytes_to_send);
	MOVLW	HIGH(_tmp_bottom)
	MOVWF	r0x02
	MOVLW	LOW(_tmp_bottom)
	MOVWF	r0x01
	MOVLW	0x80
	MOVWF	r0x03
	MOVLW	HIGH(_num_bytes_to_send)
	MOVWF	r0x05
	MOVLW	LOW(_num_bytes_to_send)
	MOVWF	r0x04
	MOVLW	0x80
	MOVWF	r0x06
	MOVF	r0x06, W
	MOVWF	POSTDEC1
	MOVF	r0x05, W
	MOVWF	POSTDEC1
	MOVF	r0x04, W
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x08
	MOVWF	POSTDEC1
	MOVF	r0x03, W
	MOVWF	POSTDEC1
	MOVF	r0x02, W
	MOVWF	POSTDEC1
	MOVF	r0x01, W
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	CALL	_fill_in_buffer
	MOVLW	0x09
	ADDWF	FSR1L, F
;	.line	106; picon.c	if(tmp_bottom >= (Picon_InBuffer + picon_buffer_size))
	MOVLW	LOW(_picon_buffer_size)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_picon_buffer_size)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_picon_buffer_size)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x00
	TBLRD*+	
	MOVFF	TABLAT, r0x01
	MOVLW	LOW(_Picon_InBuffer)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_Picon_InBuffer)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_Picon_InBuffer)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x02
	TBLRD*+	
	MOVFF	TABLAT, r0x03
	TBLRD*+	
	MOVFF	TABLAT, r0x04
	MOVF	r0x00, W
	ADDWF	r0x02, F
	MOVF	r0x01, W
	ADDWFC	r0x03, F
	CLRF	WREG
	ADDWFC	r0x04, F
	MOVF	r0x04, W
	BANKSEL	(_tmp_bottom + 2)
	SUBWF	(_tmp_bottom + 2), W, B
	BNZ	_00124_DS_
	MOVF	r0x03, W
	BANKSEL	(_tmp_bottom + 1)
	SUBWF	(_tmp_bottom + 1), W, B
	BNZ	_00124_DS_
	MOVF	r0x02, W
	BANKSEL	_tmp_bottom
	SUBWF	_tmp_bottom, W, B
_00124_DS_:
	BTFSS	STATUS, 0
	GOTO	_00113_DS_
;	.line	109; picon.c	tmp_bottom -= picon_buffer_size;
	MOVF	r0x00, W
	BANKSEL	_tmp_bottom
	SUBWF	_tmp_bottom, F, B
	MOVF	r0x01, W
	BANKSEL	(_tmp_bottom + 1)
	SUBWFB	(_tmp_bottom + 1), F, B
	CLRF	WREG
	BANKSEL	(_tmp_bottom + 2)
	SUBWFB	(_tmp_bottom + 2), F, B
_00113_DS_:
;	.line	113; picon.c	if(EP_IN_BD(picon_ep).Stat.DTS == 0)
	MOVLW	LOW(_picon_ep)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_picon_ep)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_picon_ep)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x00
	MOVF	r0x00, W
	ADDWF	r0x00, W
	MOVWF	r0x01
	INCF	r0x01, W
	MOVWF	r0x00
; ;multiply lit val:0x04 by variable r0x00 and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	MOVF	r0x00, W
	MULLW	0x04
	MOVFF	PRODL, r0x00
	MOVLW	LOW(_ep_bdt)
	ADDWF	r0x00, W
	MOVWF	r0x01
	CLRF	r0x02
	MOVLW	HIGH(_ep_bdt)
	ADDWFC	r0x02, F
	CLRF	WREG
	MOVFF	r0x01, FSR0L
	MOVFF	r0x02, FSR0H
	BTFSC	INDF0, 6
	INCF	WREG, F
	MOVWF	r0x01
	MOVF	r0x01, W
	BTFSS	STATUS, 2
	GOTO	_00115_DS_
;	.line	115; picon.c	EP_IN_BD(picon_ep).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
	MOVLW	LOW(_ep_bdt)
	ADDWF	r0x00, W
	MOVWF	r0x01
	CLRF	r0x02
	MOVLW	HIGH(_ep_bdt)
	ADDWFC	r0x02, F
	MOVFF	r0x01, FSR0L
	MOVFF	r0x02, FSR0H
	MOVLW	0xc8
	MOVWF	INDF0
	GOTO	_00117_DS_
_00115_DS_:
;	.line	119; picon.c	EP_IN_BD(picon_ep).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
	MOVLW	LOW(_ep_bdt)
	ADDWF	r0x00, F
	MOVLW	HIGH(_ep_bdt)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVLW	0x88
	MOVWF	INDF0
_00117_DS_:
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
S_picon__picon_init	code
_picon_init:
;	.line	64; picon.c	void picon_init(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
;	.line	69; picon.c	bottom           = Picon_InBuffer;
	MOVLW	LOW(_Picon_InBuffer)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_Picon_InBuffer)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_Picon_InBuffer)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _bottom
	TBLRD*+	
	MOVFF	TABLAT, (_bottom + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_bottom + 2)
;	.line	70; picon.c	top              = Picon_InBuffer;
	MOVLW	LOW(_Picon_InBuffer)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_Picon_InBuffer)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_Picon_InBuffer)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _top
	TBLRD*+	
	MOVFF	TABLAT, (_top + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_top + 2)
	BANKSEL	_num_bytes
;	.line	71; picon.c	num_bytes        = 0;
	CLRF	_num_bytes, B
	BANKSEL	_send_in_progress
;	.line	72; picon.c	send_in_progress = FALSE;
	CLRF	_send_in_progress, B
;	.line	79; picon.c	EP_IN_BD(picon_ep).Stat.uc = BDS_UCPU | BDS_DAT1 | BDS_DTSEN;
	MOVLW	LOW(_picon_ep)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_picon_ep)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_picon_ep)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x00
	MOVF	r0x00, W
	ADDWF	r0x00, W
	MOVWF	r0x01
	INCF	r0x01, F
; ;multiply lit val:0x04 by variable r0x01 and store in r0x01
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	MOVF	r0x01, W
	MULLW	0x04
	MOVFF	PRODL, r0x01
	MOVLW	LOW(_ep_bdt)
	ADDWF	r0x01, F
	MOVLW	HIGH(_ep_bdt)
	CLRF	r0x02
	ADDWFC	r0x02, F
	MOVFF	r0x01, FSR0L
	MOVFF	r0x02, FSR0H
	MOVLW	0x48
	MOVWF	INDF0
;	.line	82; picon.c	*((&UEP0) + picon_ep) = EPHSHK_EN | EPINEN_EN | EPCONDIS_EN;
	MOVLW	LOW(_UEP0)
	ADDWF	r0x00, F
	MOVLW	HIGH(_UEP0)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVLW	0x1a
	MOVWF	INDF0
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	



; Statistics:
; code size:	 1558 (0x0616) bytes ( 1.19%)
;           	  779 (0x030b) words
; udata size:	   14 (0x000e) bytes ( 0.78%)
; access size:	    8 (0x0008) bytes


	end
