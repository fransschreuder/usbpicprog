;--------------------------------------------------------
; File Created by SDCC : FreeWare ANSI-C Compiler
; Version 2.6.0 #4309 (Nov 10 2006)
; This file generated Fri Jun 22 21:49:06 2007
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f4550

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _stack
	global _stack_end
	global _init_boot
	global _main

;--------------------------------------------------------
; extern variables in this module
;--------------------------------------------------------
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
	extern _PORTEbits
	extern _LATAbits
	extern _LATBbits
	extern _LATCbits
	extern _TRISAbits
	extern _TRISBbits
	extern _TRISCbits
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
	extern _PORTE
	extern _LATA
	extern _LATB
	extern _LATC
	extern _TRISA
	extern _TRISB
	extern _TRISC
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
	extern _init_usb
	extern _usb_sleep
	extern _dispatch_usb_event
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
	extern _application_data
;--------------------------------------------------------
;	Equates to used internal registers
;--------------------------------------------------------
STATUS	equ	0xfd8
PCL	equ	0xff9
PCLATH	equ	0xffa
PCLATU	equ	0xffb
INTCON	equ	0xff2
TOSL	equ	0xffd
TOSH	equ	0xffe
TOSU	equ	0xfff
TBLPTRL	equ	0xff6
TBLPTRH	equ	0xff7
TBLPTRU	equ	0xff8
TABLAT	equ	0xff5
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
POSTDEC1	equ	0xfe5
PREINC1	equ	0xfe4


; Internal registers
.registers	udata_ovr	0x0000
r0x00	res	1
r0x01	res	1
r0x02	res	1

udata_boot_main_0	udata
_init_boot_count_1_1	res	4


ustat_boot_main_00	udata	0X0200
_stack	res	254
_stack_end	res	1

;--------------------------------------------------------
; interrupt vector 
;--------------------------------------------------------

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_boot_main__main	code
_main:
;	.line	87; boot_main.c	init_boot();
	CALL	_init_boot
;	.line	89; boot_main.c	init_usb();
	CALL	_init_usb
_00119_DS_:
;	.line	93; boot_main.c	usb_sleep();
	CALL	_usb_sleep
;	.line	94; boot_main.c	dispatch_usb_event();
	CALL	_dispatch_usb_event
;	.line	95; boot_main.c	LED3=~LED3;
	BSF	_PORTCbits, 2
;	.line	96; boot_main.c	if((application_data.invalid == 0) &&
	MOVLW	LOW(_application_data)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x00
	MOVF	r0x00, W
	BNZ	_00119_DS_
;	.line	97; boot_main.c	(GET_ACTIVE_CONFIGURATION() > FLASH_CONFIGURATION))
	MOVLW	0x03
	SUBWF	_usb_active_cfg, W
	BNC	_00119_DS_
;	.line	100; boot_main.c	LED3=0;
	BCF	_PORTCbits, 2
;	.line	101; boot_main.c	application_data.main();
	MOVLW	LOW(_application_data + 22)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 22)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 22)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x00
	TBLRD*+	
	MOVFF	TABLAT, r0x01
	TBLRD*+	
	MOVFF	TABLAT, r0x02
	MOVFF	INTCON, POSTDEC1
	BCF	INTCON, 7
	PUSH	
	MOVLW	LOW(_00127_DS_)
	MOVWF	TOSL
	MOVLW	HIGH(_00127_DS_)
	MOVWF	TOSH
	MOVLW	UPPER(_00127_DS_)
	MOVWF	TOSU
	MOVF	PREINC1, W
	MOVWF	INTCON
	MOVFF	r0x02, PCLATU
	MOVFF	r0x01, PCLATH
	MOVF	r0x00, W
	MOVWF	PCL
_00127_DS_:
;	.line	103; boot_main.c	INTCON = 0; // Forbid interrupts
	CLRF	_INTCON
	BRA	_00119_DS_
	RETURN	

; ; Starting pCode block
S_boot_main__init_boot	code
_init_boot:
;	.line	35; boot_main.c	void init_boot(void)
	MOVFF	r0x00, POSTDEC1
;	.line	39; boot_main.c	ADCON1 = 0x0F;       //all channels of the AD-converter off (digital)
	MOVLW	0x0f
	MOVWF	_ADCON1
;	.line	40; boot_main.c	CMCON  = 0x07;       //Comparator channels off (digital)
	MOVLW	0x07
	MOVWF	_CMCON
;	.line	41; boot_main.c	TRISA  = 0xFF;       //
	SETF	_TRISA
;	.line	42; boot_main.c	PORTA  = 0x01;
	MOVLW	0x01
	MOVWF	_PORTA
; #	MOVLW	0xf8
; #	MOVWF	_TRISC
; #	MOVLW	0xf8
; ;     peep 5 - Removed redundant move
;	.line	43; boot_main.c	TRISC  = 0xF8;
	MOVLW	0xf8
;	.line	44; boot_main.c	PORTC  = 0xF8;
	MOVWF	_TRISC
	MOVWF	_PORTC
;	.line	45; boot_main.c	INTCON2bits.RBPU=0;   //enable PORTB pull-up's
	BCF	_INTCON2bits, 7
	BANKSEL	_init_boot_count_1_1
;	.line	46; boot_main.c	count = 0x80000;
	CLRF	_init_boot_count_1_1, B
; removed redundant BANKSEL
	CLRF	(_init_boot_count_1_1 + 1), B
	MOVLW	0x08
; removed redundant BANKSEL
	MOVWF	(_init_boot_count_1_1 + 2), B
; removed redundant BANKSEL
	CLRF	(_init_boot_count_1_1 + 3), B
_00105_DS_:
	BANKSEL	_init_boot_count_1_1
;	.line	47; boot_main.c	while(count)
	MOVF	_init_boot_count_1_1, W, B
; removed redundant BANKSEL
	IORWF	(_init_boot_count_1_1 + 1), W, B
; removed redundant BANKSEL
	IORWF	(_init_boot_count_1_1 + 2), W, B
; removed redundant BANKSEL
	IORWF	(_init_boot_count_1_1 + 3), W, B
	BZ	_00107_DS_
;	.line	49; boot_main.c	count--;
	MOVLW	0xff
; removed redundant BANKSEL
	ADDWF	_init_boot_count_1_1, F, B
; removed redundant BANKSEL
	ADDWFC	(_init_boot_count_1_1 + 1), F, B
; removed redundant BANKSEL
	ADDWFC	(_init_boot_count_1_1 + 2), F, B
; removed redundant BANKSEL
	ADDWFC	(_init_boot_count_1_1 + 3), F, B
	BRA	_00105_DS_
_00107_DS_:
;	.line	52; boot_main.c	PORTA  = 0x00;
	CLRF	_PORTA
;	.line	55; boot_main.c	if((application_data.invalid == 0) && !PORTBbits.RB6)
	MOVLW	LOW(_application_data)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x00
	MOVF	r0x00, W
	BNZ	_00109_DS_
	BTFSC	_PORTBbits, 6
	BRA	_00109_DS_
;	.line	59; boot_main.c	device_descriptor        = application_data.device_descriptor;
	MOVLW	LOW(_application_data + 1)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 1)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 1)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _device_descriptor
	TBLRD*+	
	MOVFF	TABLAT, (_device_descriptor + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_device_descriptor + 2)
;	.line	60; boot_main.c	configuration_descriptor = application_data.configuration_descriptor;
	MOVLW	LOW(_application_data + 4)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 4)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 4)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _configuration_descriptor
	TBLRD*+	
	MOVFF	TABLAT, (_configuration_descriptor + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_configuration_descriptor + 2)
;	.line	61; boot_main.c	string_descriptor        = application_data.string_descriptor;
	MOVLW	LOW(_application_data + 7)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 7)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 7)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _string_descriptor
	TBLRD*+	
	MOVFF	TABLAT, (_string_descriptor + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_string_descriptor + 2)
	BRA	_00110_DS_
_00109_DS_:
;	.line	67; boot_main.c	device_descriptor        = &boot_device_descriptor;
	MOVLW	LOW(_boot_device_descriptor)
	BANKSEL	_device_descriptor
	MOVWF	_device_descriptor, B
	MOVLW	HIGH(_boot_device_descriptor)
; removed redundant BANKSEL
	MOVWF	(_device_descriptor + 1), B
	MOVLW	UPPER(_boot_device_descriptor)
; removed redundant BANKSEL
	MOVWF	(_device_descriptor + 2), B
;	.line	68; boot_main.c	configuration_descriptor = boot_configuration_descriptor;
	MOVLW	LOW(_boot_configuration_descriptor)
	BANKSEL	_configuration_descriptor
	MOVWF	_configuration_descriptor, B
	MOVLW	HIGH(_boot_configuration_descriptor)
; removed redundant BANKSEL
	MOVWF	(_configuration_descriptor + 1), B
	MOVLW	UPPER(_boot_configuration_descriptor)
; removed redundant BANKSEL
	MOVWF	(_configuration_descriptor + 2), B
;	.line	69; boot_main.c	string_descriptor        = boot_string_descriptor;
	MOVLW	LOW(_boot_string_descriptor)
	BANKSEL	_string_descriptor
	MOVWF	_string_descriptor, B
	MOVLW	HIGH(_boot_string_descriptor)
; removed redundant BANKSEL
	MOVWF	(_string_descriptor + 1), B
	MOVLW	UPPER(_boot_string_descriptor)
; removed redundant BANKSEL
	MOVWF	(_string_descriptor + 2), B
_00110_DS_:
;	.line	76; boot_main.c	ep_init  = boot_ep_init;
	MOVLW	LOW(_boot_ep_init)
	BANKSEL	_ep_init
	MOVWF	_ep_init, B
	MOVLW	HIGH(_boot_ep_init)
; removed redundant BANKSEL
	MOVWF	(_ep_init + 1), B
	MOVLW	UPPER(_boot_ep_init)
; removed redundant BANKSEL
	MOVWF	(_ep_init + 2), B
;	.line	77; boot_main.c	ep_in    = boot_ep_in;
	MOVLW	LOW(_boot_ep_in)
	BANKSEL	_ep_in
	MOVWF	_ep_in, B
	MOVLW	HIGH(_boot_ep_in)
; removed redundant BANKSEL
	MOVWF	(_ep_in + 1), B
	MOVLW	UPPER(_boot_ep_in)
; removed redundant BANKSEL
	MOVWF	(_ep_in + 2), B
;	.line	78; boot_main.c	ep_out   = boot_ep_out;
	MOVLW	LOW(_boot_ep_out)
	BANKSEL	_ep_out
	MOVWF	_ep_out, B
	MOVLW	HIGH(_boot_ep_out)
; removed redundant BANKSEL
	MOVWF	(_ep_out + 1), B
	MOVLW	UPPER(_boot_ep_out)
; removed redundant BANKSEL
	MOVWF	(_ep_out + 2), B
;	.line	79; boot_main.c	ep_setup = boot_ep_setup;
	MOVLW	LOW(_boot_ep_setup)
	BANKSEL	_ep_setup
	MOVWF	_ep_setup, B
	MOVLW	HIGH(_boot_ep_setup)
; removed redundant BANKSEL
	MOVWF	(_ep_setup + 1), B
	MOVLW	UPPER(_boot_ep_setup)
; removed redundant BANKSEL
	MOVWF	(_ep_setup + 2), B
	MOVFF	PREINC1, r0x00
	RETURN	



; Statistics:
; code size:	  406 (0x0196) bytes ( 0.31%)
;           	  203 (0x00cb) words
; udata size:	  259 (0x0103) bytes (14.45%)
; access size:	    3 (0x0003) bytes


	end
