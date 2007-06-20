;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.7.2 #4854 (Jun 17 2007)
; This file generated Wed Jun 20 08:40:04 2007
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f2550

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _ep0_init
	global _ep0_in
	global _ep0_setup
	global _SetupBuffer
	global _InBuffer
	global _ep0_usb_std_request

;--------------------------------------------------------
; extern variables in this module
;--------------------------------------------------------
	extern __gptrget1
	extern __gptrget3
	extern __gptrget2
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
	extern _application_data
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
TBLPTRH	equ	0xff7
TBLPTRU	equ	0xff8
TABLAT	equ	0xff5
FSR0L	equ	0xfe9
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


usb_buf	udata
_SetupBuffer	res	8
_InBuffer	res	8

udata_ep0_0	udata
_ep0_state	res	1

udata_ep0_1	udata
_num_bytes_to_be_send	res	2

udata_ep0_2	udata
_sourceData	res	3

udata_ep0_3	udata
_coming_cfg	res	1

udata_ep0_4	udata
_ep0_usb_std_request_unknown_request_1_1	res	1

udata_ep0_5	udata
_ep0_in_i_3_17	res	1

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_ep0__ep0_setup	code
_ep0_setup:
;	.line	238; ep0.c	void ep0_setup(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	MOVFF	r0x05, POSTDEC1
	BANKSEL	_ep0_state
;	.line	242; ep0.c	ep0_state = WAIT_SETUP;
	CLRF	_ep0_state, B
	BANKSEL	_num_bytes_to_be_send
;	.line	243; ep0.c	num_bytes_to_be_send = 0;
	CLRF	_num_bytes_to_be_send, B
	BANKSEL	(_num_bytes_to_be_send + 1)
	CLRF	(_num_bytes_to_be_send + 1), B
;	.line	245; ep0.c	if(ep0_usb_std_request())
	CALL	_ep0_usb_std_request
	MOVWF	r0x00
	MOVF	r0x00, W
	BTFSC	STATUS, 2
	GOTO	_00225_DS_
;	.line	247; ep0.c	UCONbits.PKTDIS = 0;
	BCF	_UCONbits, 4
;	.line	248; ep0.c	if(SetupBuffer.data_transfer_direction == DEVICE_TO_HOST)
	CLRF	r0x00
	BANKSEL	_SetupBuffer
	BTFSC	_SetupBuffer, 7, B
	INCF	r0x00, F
	MOVF	r0x00, W
	XORLW	0x01
	BNZ	_00232_DS_
	GOTO	_00233_DS_
_00232_DS_:
	GOTO	_00222_DS_
_00233_DS_:
;	.line	250; ep0.c	ep0_state = WAIT_IN;
	MOVLW	0x01
	BANKSEL	_ep0_state
	MOVWF	_ep0_state, B
;	.line	252; ep0.c	EP_OUT_BD(0).Cnt = EP0_BUFFER_SIZE;
	MOVLW	0x08
	BANKSEL	(_ep_bdt + 1)
	MOVWF	(_ep_bdt + 1), B
;	.line	253; ep0.c	EP_OUT_BD(0).ADR = (uchar __data *)&SetupBuffer;            
	MOVLW	LOW(_SetupBuffer)
	MOVWF	r0x00
	MOVLW	HIGH(_SetupBuffer)
	MOVWF	r0x01
	MOVF	r0x00, W
	BANKSEL	(_ep_bdt + 2)
	MOVWF	(_ep_bdt + 2), B
	MOVF	r0x01, W
	BANKSEL	(_ep_bdt + 3)
	MOVWF	(_ep_bdt + 3), B
;	.line	254; ep0.c	EP_OUT_BD(0).Stat.uc = BDS_USIE;
	MOVLW	0x80
	BANKSEL	_ep_bdt
	MOVWF	_ep_bdt, B
;	.line	256; ep0.c	EP_IN_BD(0).ADR = (uchar __data *)InBuffer;
	MOVLW	LOW(_InBuffer)
	MOVWF	r0x00
	MOVLW	HIGH(_InBuffer)
	MOVWF	r0x01
	MOVF	r0x00, W
	BANKSEL	(_ep_bdt + 6)
	MOVWF	(_ep_bdt + 6), B
	MOVF	r0x01, W
	BANKSEL	(_ep_bdt + 7)
	MOVWF	(_ep_bdt + 7), B
;	.line	257; ep0.c	if(SetupBuffer.wLength < num_bytes_to_be_send)
	LFSR	0x00, (_SetupBuffer + 6)
	MOVFF	POSTINC0, r0x00
	MOVFF	INDF0, r0x01
	BANKSEL	(_num_bytes_to_be_send + 1)
	MOVF	(_num_bytes_to_be_send + 1), W, B
	SUBWF	r0x01, W
	BNZ	_00234_DS_
	BANKSEL	_num_bytes_to_be_send
	MOVF	_num_bytes_to_be_send, W, B
	SUBWF	r0x00, W
_00234_DS_:
	BTFSC	STATUS, 0
	GOTO	_00220_DS_
;	.line	259; ep0.c	num_bytes_to_be_send = SetupBuffer.wLength;
	LFSR	0x00, (_SetupBuffer + 6)
	MOVFF	POSTINC0, _num_bytes_to_be_send
	MOVFF	INDF0, (_num_bytes_to_be_send + 1)
_00220_DS_:
;	.line	261; ep0.c	fill_in_buffer(0, &sourceData, EP0_BUFFER_SIZE, &num_bytes_to_be_send);
	MOVLW	HIGH(_sourceData)
	MOVWF	r0x01
	MOVLW	LOW(_sourceData)
	MOVWF	r0x00
	MOVLW	0x80
	MOVWF	r0x02
	MOVLW	HIGH(_num_bytes_to_be_send)
	MOVWF	r0x04
	MOVLW	LOW(_num_bytes_to_be_send)
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
	MOVLW	0x08
	MOVWF	POSTDEC1
	MOVF	r0x02, W
	MOVWF	POSTDEC1
	MOVF	r0x01, W
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	CALL	_fill_in_buffer
	MOVLW	0x09
	ADDWF	FSR1L, F
;	.line	262; ep0.c	EP_IN_BD(0).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
	MOVLW	0xc8
	BANKSEL	(_ep_bdt + 4)
	MOVWF	(_ep_bdt + 4), B
	GOTO	_00227_DS_
_00222_DS_:
;	.line	267; ep0.c	ep0_state = WAIT_OUT;
	MOVLW	0x02
	BANKSEL	_ep0_state
	MOVWF	_ep0_state, B
;	.line	269; ep0.c	EP_OUT_BD(0).Cnt = EP0_BUFFER_SIZE;
	MOVLW	0x08
	BANKSEL	(_ep_bdt + 1)
	MOVWF	(_ep_bdt + 1), B
;	.line	270; ep0.c	EP_OUT_BD(0).ADR = (uchar __data *)InBuffer;
	MOVLW	LOW(_InBuffer)
	MOVWF	r0x00
	MOVLW	HIGH(_InBuffer)
	MOVWF	r0x01
	MOVF	r0x00, W
	BANKSEL	(_ep_bdt + 2)
	MOVWF	(_ep_bdt + 2), B
	MOVF	r0x01, W
	BANKSEL	(_ep_bdt + 3)
	MOVWF	(_ep_bdt + 3), B
;	.line	271; ep0.c	EP_OUT_BD(0).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
	MOVLW	0xc8
	BANKSEL	_ep_bdt
	MOVWF	_ep_bdt, B
	BANKSEL	(_ep_bdt + 5)
;	.line	273; ep0.c	EP_IN_BD(0).Cnt = 0;
	CLRF	(_ep_bdt + 5), B
;	.line	274; ep0.c	EP_IN_BD(0).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
	MOVLW	0xc8
	BANKSEL	(_ep_bdt + 4)
	MOVWF	(_ep_bdt + 4), B
	GOTO	_00227_DS_
_00225_DS_:
;	.line	280; ep0.c	UCONbits.PKTDIS = 0;
	BCF	_UCONbits, 4
;	.line	281; ep0.c	EP_OUT_BD(0).Cnt = EP0_BUFFER_SIZE;
	MOVLW	0x08
	BANKSEL	(_ep_bdt + 1)
	MOVWF	(_ep_bdt + 1), B
;	.line	282; ep0.c	EP_OUT_BD(0).ADR = (uchar __data *)&SetupBuffer;
	MOVLW	LOW(_SetupBuffer)
	MOVWF	r0x00
	MOVLW	HIGH(_SetupBuffer)
	MOVWF	r0x01
	MOVF	r0x00, W
	BANKSEL	(_ep_bdt + 2)
	MOVWF	(_ep_bdt + 2), B
	MOVF	r0x01, W
	BANKSEL	(_ep_bdt + 3)
	MOVWF	(_ep_bdt + 3), B
;	.line	283; ep0.c	EP_OUT_BD(0).Stat.uc = BDS_USIE | BDS_BSTALL;
	MOVLW	0x84
	BANKSEL	_ep_bdt
	MOVWF	_ep_bdt, B
;	.line	285; ep0.c	EP_IN_BD(0).Stat.uc  = BDS_USIE | BDS_BSTALL;
	MOVLW	0x84
	BANKSEL	(_ep_bdt + 4)
	MOVWF	(_ep_bdt + 4), B
_00227_DS_:
	MOVFF	PREINC1, r0x05
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_ep0__ep0_in	code
_ep0_in:
;	.line	154; ep0.c	void ep0_in(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	MOVFF	r0x05, POSTDEC1
;	.line	157; ep0.c	if(GET_DEVICE_STATE() == ADDRESS_PENDING_STATE)
	MOVF	_usb_device_state, W
	XORLW	0x04
	BNZ	_00207_DS_
	GOTO	_00208_DS_
_00207_DS_:
	GOTO	_00174_DS_
_00208_DS_:
;	.line	159; ep0.c	UADDR = SetupBuffer.bAddress;
	LFSR	0x00, (_SetupBuffer + 2)
	MOVFF	INDF0, _UADDR
;	.line	160; ep0.c	if(UADDR != 0)
	MOVF	_UADDR, W
	BTFSC	STATUS, 2
	GOTO	_00171_DS_
;	.line	162; ep0.c	SET_DEVICE_STATE(ADDRESS_STATE);
	MOVLW	0x05
	MOVWF	_usb_device_state
	GOTO	_00174_DS_
_00171_DS_:
;	.line	166; ep0.c	SET_DEVICE_STATE(DEFAULT_STATE);
	MOVLW	0x03
	MOVWF	_usb_device_state
_00174_DS_:
	BANKSEL	_ep0_state
;	.line	170; ep0.c	if(ep0_state == WAIT_IN)
	MOVF	_ep0_state, W, B
	XORLW	0x01
	BNZ	_00209_DS_
	GOTO	_00210_DS_
_00209_DS_:
	GOTO	_00179_DS_
_00210_DS_:
;	.line	172; ep0.c	fill_in_buffer(0, &sourceData, EP0_BUFFER_SIZE, &num_bytes_to_be_send);
	MOVLW	HIGH(_sourceData)
	MOVWF	r0x01
	MOVLW	LOW(_sourceData)
	MOVWF	r0x00
	MOVLW	0x80
	MOVWF	r0x02
	MOVLW	HIGH(_num_bytes_to_be_send)
	MOVWF	r0x04
	MOVLW	LOW(_num_bytes_to_be_send)
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
	MOVLW	0x08
	MOVWF	POSTDEC1
	MOVF	r0x02, W
	MOVWF	POSTDEC1
	MOVF	r0x01, W
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	CALL	_fill_in_buffer
	MOVLW	0x09
	ADDWF	FSR1L, F
	BANKSEL	(_ep_bdt + 4)
;	.line	174; ep0.c	if(EP_IN_BD(0).Stat.DTS == 0)
	BTFSC	(_ep_bdt + 4), 6, B
	GOTO	_00176_DS_
;	.line	176; ep0.c	EP_IN_BD(0).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
	MOVLW	0xc8
	BANKSEL	(_ep_bdt + 4)
	MOVWF	(_ep_bdt + 4), B
	GOTO	_00180_DS_
_00176_DS_:
;	.line	180; ep0.c	EP_IN_BD(0).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
	MOVLW	0x88
	BANKSEL	(_ep_bdt + 4)
	MOVWF	(_ep_bdt + 4), B
	GOTO	_00180_DS_
_00179_DS_:
;	.line	185; ep0.c	ep0_init();
	CALL	_ep0_init
_00180_DS_:
;	.line	188; ep0.c	if(GET_DEVICE_STATE() == CONFIGURATION_PENDING_STATE)
	MOVF	_usb_device_state, W
	XORLW	0x06
	BNZ	_00211_DS_
	GOTO	_00212_DS_
_00211_DS_:
	GOTO	_00193_DS_
_00212_DS_:
;	.line	193; ep0.c	UEP1  = 0; UEP2  = 0; UEP3  = 0; UEP4  = 0;
	CLRF	_UEP1
	CLRF	_UEP2
	CLRF	_UEP3
	CLRF	_UEP4
;	.line	194; ep0.c	UEP5  = 0; UEP6  = 0; UEP7  = 0; UEP8  = 0;
	CLRF	_UEP5
	CLRF	_UEP6
	CLRF	_UEP7
	CLRF	_UEP8
;	.line	195; ep0.c	UEP9  = 0; UEP10 = 0; UEP11 = 0; UEP12 = 0;
	CLRF	_UEP9
	CLRF	_UEP10
	CLRF	_UEP11
	CLRF	_UEP12
;	.line	196; ep0.c	UEP13 = 0; UEP14 = 0; UEP15 = 0;
	CLRF	_UEP13
	CLRF	_UEP14
	CLRF	_UEP15
;	.line	199; ep0.c	if(coming_cfg <= FLASH_CONFIGURATION)
	MOVLW	0x03
	BANKSEL	_coming_cfg
	SUBWF	_coming_cfg, W, B
	BTFSC	STATUS, 0
	GOTO	_00182_DS_
;	.line	202; ep0.c	ep_init  = boot_ep_init;
	MOVLW	LOW(_boot_ep_init)
	BANKSEL	_ep_init
	MOVWF	_ep_init, B
	MOVLW	HIGH(_boot_ep_init)
	BANKSEL	(_ep_init + 1)
	MOVWF	(_ep_init + 1), B
	MOVLW	UPPER(_boot_ep_init)
	BANKSEL	(_ep_init + 2)
	MOVWF	(_ep_init + 2), B
;	.line	203; ep0.c	ep_in    = boot_ep_in;
	MOVLW	LOW(_boot_ep_in)
	BANKSEL	_ep_in
	MOVWF	_ep_in, B
	MOVLW	HIGH(_boot_ep_in)
	BANKSEL	(_ep_in + 1)
	MOVWF	(_ep_in + 1), B
	MOVLW	UPPER(_boot_ep_in)
	BANKSEL	(_ep_in + 2)
	MOVWF	(_ep_in + 2), B
;	.line	204; ep0.c	ep_out   = boot_ep_out;
	MOVLW	LOW(_boot_ep_out)
	BANKSEL	_ep_out
	MOVWF	_ep_out, B
	MOVLW	HIGH(_boot_ep_out)
	BANKSEL	(_ep_out + 1)
	MOVWF	(_ep_out + 1), B
	MOVLW	UPPER(_boot_ep_out)
	BANKSEL	(_ep_out + 2)
	MOVWF	(_ep_out + 2), B
;	.line	205; ep0.c	ep_setup = boot_ep_setup;
	MOVLW	LOW(_boot_ep_setup)
	BANKSEL	_ep_setup
	MOVWF	_ep_setup, B
	MOVLW	HIGH(_boot_ep_setup)
	BANKSEL	(_ep_setup + 1)
	MOVWF	(_ep_setup + 1), B
	MOVLW	UPPER(_boot_ep_setup)
	BANKSEL	(_ep_setup + 2)
	MOVWF	(_ep_setup + 2), B
	GOTO	_00183_DS_
_00182_DS_:
;	.line	210; ep0.c	ep_init  = application_data.ep_init;
	MOVLW	LOW(_application_data + 10)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 10)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 10)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _ep_init
	TBLRD*+	
	MOVFF	TABLAT, (_ep_init + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_ep_init + 2)
;	.line	211; ep0.c	ep_in    = application_data.ep_in;
	MOVLW	LOW(_application_data + 13)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 13)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 13)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _ep_in
	TBLRD*+	
	MOVFF	TABLAT, (_ep_in + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_ep_in + 2)
;	.line	212; ep0.c	ep_out   = application_data.ep_out;
	MOVLW	LOW(_application_data + 16)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 16)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 16)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _ep_out
	TBLRD*+	
	MOVFF	TABLAT, (_ep_out + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_ep_out + 2)
;	.line	213; ep0.c	ep_setup = application_data.ep_setup;
	MOVLW	LOW(_application_data + 19)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 19)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 19)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _ep_setup
	TBLRD*+	
	MOVFF	TABLAT, (_ep_setup + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_ep_setup + 2)
_00183_DS_:
;	.line	216; ep0.c	SET_ACTIVE_CONFIGURATION(coming_cfg);
	MOVFF	_coming_cfg, _usb_active_cfg
	BANKSEL	_coming_cfg
;	.line	218; ep0.c	if(coming_cfg == 0)
	MOVF	_coming_cfg, W, B
	BTFSS	STATUS, 2
	GOTO	_00185_DS_
;	.line	220; ep0.c	SET_DEVICE_STATE(ADDRESS_STATE);
	MOVLW	0x05
	MOVWF	_usb_device_state
	GOTO	_00193_DS_
_00185_DS_:
;	.line	227; ep0.c	for(i = 15; i > 0; i--)
	MOVLW	0x0f
	BANKSEL	_ep0_in_i_3_17
	MOVWF	_ep0_in_i_3_17, B
_00189_DS_:
	BANKSEL	_ep0_in_i_3_17
	MOVF	_ep0_in_i_3_17, W, B
	BTFSC	STATUS, 2
	GOTO	_00195_DS_
	MOVLW	0x01
	MOVWF	r0x00
	GOTO	_00196_DS_
_00195_DS_:
	CLRF	r0x00
_00196_DS_:
	MOVF	r0x00, W
	BTFSC	STATUS, 2
	GOTO	_00192_DS_
; ;multiply lit val:0x03 by variable _coming_cfg and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BANKSEL	_coming_cfg
;	.line	230; ep0.c	ep_init[coming_cfg][i]();
	MOVF	_coming_cfg, W, B
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
; ;multiply lit val:0x03 by variable _ep0_in_i_3_17 and store in r0x03
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	BANKSEL	_ep0_in_i_3_17
	MOVF	_ep0_in_i_3_17, W, B
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
	MOVLW	LOW(_00214_DS_)
	MOVWF	TOSL
	MOVLW	HIGH(_00214_DS_)
	MOVWF	TOSH
	MOVLW	UPPER(_00214_DS_)
	MOVWF	TOSU
	MOVF	PREINC1, W
	MOVWF	INTCON
	MOVFF	r0x02, PCLATU
	MOVFF	r0x01, PCLATH
	MOVF	r0x00, W
	MOVWF	PCL
_00214_DS_:
	BANKSEL	_ep0_in_i_3_17
;	.line	227; ep0.c	for(i = 15; i > 0; i--)
	DECF	_ep0_in_i_3_17, F, B
	GOTO	_00189_DS_
_00192_DS_:
;	.line	233; ep0.c	SET_DEVICE_STATE(CONFIGURED_STATE);
	MOVLW	0x07
	MOVWF	_usb_device_state
_00193_DS_:
	MOVFF	PREINC1, r0x05
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_ep0__ep0_init	code
_ep0_init:
;	.line	144; ep0.c	void ep0_init(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	BANKSEL	_ep0_state
;	.line	146; ep0.c	ep0_state = WAIT_SETUP;
	CLRF	_ep0_state, B
;	.line	147; ep0.c	EP_OUT_BD(0).Cnt = EP0_BUFFER_SIZE;
	MOVLW	0x08
	BANKSEL	(_ep_bdt + 1)
	MOVWF	(_ep_bdt + 1), B
;	.line	148; ep0.c	EP_OUT_BD(0).ADR = (uchar __data *)&SetupBuffer;
	MOVLW	LOW(_SetupBuffer)
	MOVWF	r0x00
	MOVLW	HIGH(_SetupBuffer)
	MOVWF	r0x01
	MOVF	r0x00, W
	BANKSEL	(_ep_bdt + 2)
	MOVWF	(_ep_bdt + 2), B
	MOVF	r0x01, W
	BANKSEL	(_ep_bdt + 3)
	MOVWF	(_ep_bdt + 3), B
;	.line	149; ep0.c	EP_OUT_BD(0).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
	MOVLW	0x88
	BANKSEL	_ep_bdt
	MOVWF	_ep_bdt, B
	BANKSEL	(_ep_bdt + 4)
;	.line	150; ep0.c	EP_IN_BD(0).Stat.uc = BDS_UCPU;
	CLRF	(_ep_bdt + 4), B
;	.line	151; ep0.c	UEP0 = EPINEN_EN | EPOUTEN_EN | EPHSHK_EN;
	MOVLW	0x16
	MOVWF	_UEP0
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_ep0__ep0_usb_std_request	code
_ep0_usb_std_request:
;	.line	47; ep0.c	uchar ep0_usb_std_request(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	BANKSEL	_ep0_usb_std_request_unknown_request_1_1
;	.line	52; ep0.c	unknown_request = FALSE;
	CLRF	_ep0_usb_std_request_unknown_request_1_1, B
	BANKSEL	_SetupBuffer
;	.line	54; ep0.c	if(SetupBuffer.request_type != STANDARD) 
	MOVF	_SetupBuffer, W, B
	SWAPF	WREG, W
	RRNCF	WREG, W
	ANDLW	0x03
	MOVWF	r0x00
	MOVF	r0x00, W
	BTFSC	STATUS, 2
	GOTO	_00106_DS_
;	.line	56; ep0.c	return FALSE;
	CLRF	WREG
	GOTO	_00126_DS_
_00106_DS_:
;	.line	59; ep0.c	switch(SetupBuffer.bRequest)
	LFSR	0x00, (_SetupBuffer + 1)
	MOVFF	INDF0, r0x00
	MOVF	r0x00, W
	BNZ	_00142_DS_
	GOTO	_00125_DS_
_00142_DS_:
	MOVF	r0x00, W
	XORLW	0x01
	BNZ	_00144_DS_
	GOTO	_00125_DS_
_00144_DS_:
	MOVF	r0x00, W
	XORLW	0x03
	BNZ	_00146_DS_
	GOTO	_00125_DS_
_00146_DS_:
	MOVF	r0x00, W
	XORLW	0x05
	BNZ	_00148_DS_
	GOTO	_00117_DS_
_00148_DS_:
	MOVF	r0x00, W
	XORLW	0x06
	BNZ	_00150_DS_
	GOTO	_00109_DS_
_00150_DS_:
	MOVF	r0x00, W
	XORLW	0x08
	BNZ	_00152_DS_
	GOTO	_00108_DS_
_00152_DS_:
	MOVF	r0x00, W
	XORLW	0x09
	BNZ	_00154_DS_
	GOTO	_00118_DS_
_00154_DS_:
	MOVF	r0x00, W
	XORLW	0x0a
	BNZ	_00156_DS_
	GOTO	_00125_DS_
_00156_DS_:
	MOVF	r0x00, W
	XORLW	0x0b
	BNZ	_00158_DS_
	GOTO	_00125_DS_
_00158_DS_:
	GOTO	_00124_DS_
_00108_DS_:
;	.line	67; ep0.c	sourceData = &GET_ACTIVE_CONFIGURATION();
	MOVLW	HIGH(_usb_active_cfg)
	BANKSEL	(_sourceData + 1)
	MOVWF	(_sourceData + 1), B
	MOVLW	LOW(_usb_active_cfg)
	BANKSEL	_sourceData
	MOVWF	_sourceData, B
	MOVLW	0x80
	BANKSEL	(_sourceData + 2)
	MOVWF	(_sourceData + 2), B
;	.line	68; ep0.c	num_bytes_to_be_send = 1;
	MOVLW	0x01
	BANKSEL	_num_bytes_to_be_send
	MOVWF	_num_bytes_to_be_send, B
	BANKSEL	(_num_bytes_to_be_send + 1)
	CLRF	(_num_bytes_to_be_send + 1), B
;	.line	69; ep0.c	break;
	GOTO	_00125_DS_
_00109_DS_:
;	.line	72; ep0.c	switch(SetupBuffer.bDescType)
	LFSR	0x00, (_SetupBuffer + 3)
	MOVFF	INDF0, r0x00
	MOVLW	0x01
	SUBWF	r0x00, W
	BTFSS	STATUS, 0
	GOTO	_00113_DS_
	MOVLW	0x04
	SUBWF	r0x00, W
	BTFSC	STATUS, 0
	GOTO	_00113_DS_
	DECF	r0x00, F
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	CLRF	r0x04
	RLCF	r0x00, W
	RLCF	r0x04, F
	RLCF	WREG, W
	RLCF	r0x04, F
	ANDLW	0xfc
	MOVWF	r0x03
	MOVLW	UPPER(_00161_DS_)
	MOVWF	PCLATU
	MOVLW	HIGH(_00161_DS_)
	MOVWF	PCLATH
	MOVLW	LOW(_00161_DS_)
	ADDWF	r0x03, F
	MOVF	r0x04, W
	ADDWFC	PCLATH, F
	BTFSC	STATUS, 0
	INCF	PCLATU, F
	MOVF	r0x03, W
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVWF	PCL
_00161_DS_:
	GOTO	_00110_DS_
	GOTO	_00111_DS_
	GOTO	_00112_DS_
_00110_DS_:
;	.line	77; ep0.c	sourceData = (uchar *) device_descriptor;
	MOVFF	_device_descriptor, _sourceData
	MOVFF	(_device_descriptor + 1), (_sourceData + 1)
	MOVFF	(_device_descriptor + 2), (_sourceData + 2)
;	.line	78; ep0.c	num_bytes_to_be_send = device_descriptor->bLength;
	MOVFF	_device_descriptor, r0x00
	MOVFF	(_device_descriptor + 1), r0x01
	MOVFF	(_device_descriptor + 2), r0x02
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget1
	MOVWF	r0x00
	MOVFF	r0x00, _num_bytes_to_be_send
	BANKSEL	(_num_bytes_to_be_send + 1)
	CLRF	(_num_bytes_to_be_send + 1), B
;	.line	79; ep0.c	break;
	GOTO	_00125_DS_
_00111_DS_:
;	.line	82; ep0.c	sourceData = configuration_descriptor[SetupBuffer.bDescIndex];
	LFSR	0x00, (_SetupBuffer + 2)
	MOVFF	INDF0, r0x00
; ;multiply lit val:0x03 by variable r0x00 and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	MOVF	r0x00, W
	MULLW	0x03
	MOVFF	PRODL, r0x00
	MOVFF	PRODH, r0x01
	BANKSEL	_configuration_descriptor
	MOVF	_configuration_descriptor, W, B
	ADDWF	r0x00, F
	BANKSEL	(_configuration_descriptor + 1)
	MOVF	(_configuration_descriptor + 1), W, B
	ADDWFC	r0x01, F
	BANKSEL	(_configuration_descriptor + 2)
	MOVF	(_configuration_descriptor + 2), W, B
	CLRF	r0x02
	ADDWFC	r0x02, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget3
	BANKSEL	_sourceData
	MOVWF	_sourceData, B
	MOVFF	PRODL, (_sourceData + 1)
	MOVFF	PRODH, (_sourceData + 2)
;	.line	83; ep0.c	num_bytes_to_be_send = ((USB_Configuration_Descriptor*)sourceData)->wTotalLength;
	MOVFF	_sourceData, r0x00
	MOVFF	(_sourceData + 1), r0x01
	MOVFF	(_sourceData + 2), r0x02
	MOVLW	0x02
	ADDWF	r0x00, F
	MOVLW	0x00
	ADDWFC	r0x01, F
	MOVLW	0x00
	ADDWFC	r0x02, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget2
	BANKSEL	_num_bytes_to_be_send
	MOVWF	_num_bytes_to_be_send, B
	MOVFF	PRODL, (_num_bytes_to_be_send + 1)
;	.line	84; ep0.c	break;
	GOTO	_00125_DS_
_00112_DS_:
;	.line	87; ep0.c	sourceData = string_descriptor[SetupBuffer.bDescIndex];
	LFSR	0x00, (_SetupBuffer + 2)
	MOVFF	INDF0, r0x00
; ;multiply lit val:0x03 by variable r0x00 and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
	MOVF	r0x00, W
	MULLW	0x03
	MOVFF	PRODL, r0x00
	MOVFF	PRODH, r0x01
	BANKSEL	_string_descriptor
	MOVF	_string_descriptor, W, B
	ADDWF	r0x00, F
	BANKSEL	(_string_descriptor + 1)
	MOVF	(_string_descriptor + 1), W, B
	ADDWFC	r0x01, F
	BANKSEL	(_string_descriptor + 2)
	MOVF	(_string_descriptor + 2), W, B
	CLRF	r0x02
	ADDWFC	r0x02, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget3
	BANKSEL	_sourceData
	MOVWF	_sourceData, B
	MOVFF	PRODL, (_sourceData + 1)
	MOVFF	PRODH, (_sourceData + 2)
;	.line	88; ep0.c	num_bytes_to_be_send = sourceData[0];
	MOVFF	_sourceData, r0x00
	MOVFF	(_sourceData + 1), r0x01
	MOVFF	(_sourceData + 2), r0x02
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget1
	MOVWF	r0x00
	MOVFF	r0x00, _num_bytes_to_be_send
	BANKSEL	(_num_bytes_to_be_send + 1)
	CLRF	(_num_bytes_to_be_send + 1), B
;	.line	89; ep0.c	break;
	GOTO	_00125_DS_
_00113_DS_:
;	.line	93; ep0.c	unknown_request = TRUE;
	MOVLW	0x01
	BANKSEL	_ep0_usb_std_request_unknown_request_1_1
	MOVWF	_ep0_usb_std_request_unknown_request_1_1, B
;	.line	96; ep0.c	break;
	GOTO	_00125_DS_
_00117_DS_:
;	.line	107; ep0.c	SET_DEVICE_STATE(ADDRESS_PENDING_STATE);
	MOVLW	0x04
	MOVWF	_usb_device_state
;	.line	108; ep0.c	break;
	GOTO	_00125_DS_
_00118_DS_:
;	.line	112; ep0.c	if(device_descriptor->bNumConfigurations >= SetupBuffer.bConfigurationValue)
	MOVFF	_device_descriptor, r0x00
	MOVFF	(_device_descriptor + 1), r0x01
	MOVFF	(_device_descriptor + 2), r0x02
	MOVLW	0x11
	ADDWF	r0x00, F
	MOVLW	0x00
	ADDWFC	r0x01, F
	MOVLW	0x00
	ADDWFC	r0x02, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, PRODL
	MOVF	r0x02, W
	CALL	__gptrget1
	MOVWF	r0x00
	LFSR	0x00, (_SetupBuffer + 2)
	MOVFF	INDF0, r0x01
	MOVF	r0x01, W
	SUBWF	r0x00, W
	BTFSS	STATUS, 0
	GOTO	_00120_DS_
;	.line	114; ep0.c	coming_cfg = SetupBuffer.bConfigurationValue;
	LFSR	0x00, (_SetupBuffer + 2)
	MOVFF	INDF0, _coming_cfg
;	.line	115; ep0.c	SET_DEVICE_STATE(CONFIGURATION_PENDING_STATE);
	MOVLW	0x06
	MOVWF	_usb_device_state
	GOTO	_00125_DS_
_00120_DS_:
;	.line	121; ep0.c	unknown_request = TRUE;
	MOVLW	0x01
	BANKSEL	_ep0_usb_std_request_unknown_request_1_1
	MOVWF	_ep0_usb_std_request_unknown_request_1_1, B
;	.line	124; ep0.c	break;
	GOTO	_00125_DS_
_00124_DS_:
;	.line	137; ep0.c	unknown_request = TRUE;
	MOVLW	0x01
	BANKSEL	_ep0_usb_std_request_unknown_request_1_1
	MOVWF	_ep0_usb_std_request_unknown_request_1_1, B
_00125_DS_:
	BANKSEL	_ep0_usb_std_request_unknown_request_1_1
;	.line	140; ep0.c	return !unknown_request;
	MOVF	_ep0_usb_std_request_unknown_request_1_1, W, B
	BSF	STATUS, 0
	TSTFSZ	WREG
	BCF	STATUS, 0
	CLRF	r0x00
	RLCF	r0x00, F
	MOVF	r0x00, W
_00126_DS_:
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	



; Statistics:
; code size:	 1836 (0x072c) bytes ( 1.40%)
;           	  918 (0x0396) words
; udata size:	   25 (0x0019) bytes ( 1.40%)
; access size:	    6 (0x0006) bytes


	end
