;--------------------------------------------------------
; File Created by SDCC : FreeWare ANSI-C Compiler
; Version 2.6.0 #4309 (Nov 10 2006)
; This file generated Fri Jun 22 21:24:52 2007
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f4550

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _rt_sequencer

;--------------------------------------------------------
; extern variables in this module
;--------------------------------------------------------
	extern _rt_base_job
	extern _rt_basex2_job
	extern _rt_basex4_job
	extern _rt_basex8_job
	extern _rt_basex16_job
	extern _rt_basex32_job
	extern _rt_basex64_job
	extern _rt_basex128_job
	extern _rt_basex256_job
	extern _rt_basex512_job
	extern _rt_basex1024_job
	extern _rt_basex2048_job
	extern _rt_basex4096_job
	extern _rt_basex8192_job
	extern _rt_basex16384_job
	extern _rt_basex32768_job
	extern _rt_basex65536_job
;--------------------------------------------------------
;	Equates to used internal registers
;--------------------------------------------------------
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
POSTDEC1	equ	0xfe5
PREINC1	equ	0xfe4
PLUSW2	equ	0xfdb


; Internal registers
.registers	udata_ovr	0x0000
r0x00	res	1
r0x01	res	1

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_sequencer__rt_sequencer	code
_rt_sequencer:
;	.line	27; sequencer.c	void rt_sequencer(uint counter) 
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVLW	0x02
	MOVFF	PLUSW2, r0x00
	MOVLW	0x03
	MOVFF	PLUSW2, r0x01
;	.line	29; sequencer.c	rt_base_job();
	CALL	_rt_base_job
;	.line	31; sequencer.c	if(counter & 0x0001)
	BTFSS	r0x00, 0
	BRA	_00106_DS_
;	.line	33; sequencer.c	rt_basex2_job();
	CALL	_rt_basex2_job
;	.line	34; sequencer.c	return;
	BRA	_00137_DS_
_00106_DS_:
;	.line	36; sequencer.c	if(counter & 0x0002)
	BTFSS	r0x00, 1
	BRA	_00108_DS_
;	.line	38; sequencer.c	rt_basex4_job();
	CALL	_rt_basex4_job
;	.line	39; sequencer.c	return;
	BRA	_00137_DS_
_00108_DS_:
;	.line	41; sequencer.c	if(counter & 0x0004)
	BTFSS	r0x00, 2
	BRA	_00110_DS_
;	.line	43; sequencer.c	rt_basex8_job();
	CALL	_rt_basex8_job
;	.line	44; sequencer.c	return;
	BRA	_00137_DS_
_00110_DS_:
;	.line	46; sequencer.c	if(counter & 0x0008)
	BTFSS	r0x00, 3
	BRA	_00112_DS_
;	.line	48; sequencer.c	rt_basex16_job();
	CALL	_rt_basex16_job
;	.line	49; sequencer.c	return;
	BRA	_00137_DS_
_00112_DS_:
;	.line	51; sequencer.c	if(counter & 0x0010)
	BTFSS	r0x00, 4
	BRA	_00114_DS_
;	.line	53; sequencer.c	rt_basex32_job();
	CALL	_rt_basex32_job
;	.line	54; sequencer.c	return;
	BRA	_00137_DS_
_00114_DS_:
;	.line	56; sequencer.c	if(counter & 0x0020)
	BTFSS	r0x00, 5
	BRA	_00116_DS_
;	.line	58; sequencer.c	rt_basex64_job();
	CALL	_rt_basex64_job
;	.line	59; sequencer.c	return;
	BRA	_00137_DS_
_00116_DS_:
;	.line	61; sequencer.c	if(counter & 0x0040)
	BTFSS	r0x00, 6
	BRA	_00118_DS_
;	.line	63; sequencer.c	rt_basex128_job();
	CALL	_rt_basex128_job
;	.line	64; sequencer.c	return;
	BRA	_00137_DS_
_00118_DS_:
;	.line	66; sequencer.c	if(counter & 0x0080)
	BTFSS	r0x00, 7
	BRA	_00120_DS_
;	.line	68; sequencer.c	rt_basex256_job();
	CALL	_rt_basex256_job
;	.line	69; sequencer.c	return;
	BRA	_00137_DS_
_00120_DS_:
;	.line	71; sequencer.c	if(counter & 0x0100)
	BTFSS	r0x01, 0
	BRA	_00122_DS_
;	.line	73; sequencer.c	rt_basex512_job();
	CALL	_rt_basex512_job
;	.line	74; sequencer.c	return;
	BRA	_00137_DS_
_00122_DS_:
;	.line	76; sequencer.c	if(counter & 0x0200)
	BTFSS	r0x01, 1
	BRA	_00124_DS_
;	.line	78; sequencer.c	rt_basex1024_job();
	CALL	_rt_basex1024_job
;	.line	79; sequencer.c	return;
	BRA	_00137_DS_
_00124_DS_:
;	.line	81; sequencer.c	if(counter & 0x0400)
	BTFSS	r0x01, 2
	BRA	_00126_DS_
;	.line	83; sequencer.c	rt_basex2048_job();
	CALL	_rt_basex2048_job
;	.line	84; sequencer.c	return;
	BRA	_00137_DS_
_00126_DS_:
;	.line	86; sequencer.c	if(counter & 0x0800)
	BTFSS	r0x01, 3
	BRA	_00128_DS_
;	.line	88; sequencer.c	rt_basex4096_job();
	CALL	_rt_basex4096_job
;	.line	89; sequencer.c	return;
	BRA	_00137_DS_
_00128_DS_:
;	.line	91; sequencer.c	if(counter & 0x1000)
	BTFSS	r0x01, 4
	BRA	_00130_DS_
;	.line	93; sequencer.c	rt_basex8192_job();
	CALL	_rt_basex8192_job
;	.line	94; sequencer.c	return;
	BRA	_00137_DS_
_00130_DS_:
;	.line	96; sequencer.c	if(counter & 0x2000)
	BTFSS	r0x01, 5
	BRA	_00132_DS_
;	.line	98; sequencer.c	rt_basex16384_job();
	CALL	_rt_basex16384_job
;	.line	99; sequencer.c	return;
	BRA	_00137_DS_
_00132_DS_:
;	.line	101; sequencer.c	if(counter & 0x4000)
	BTFSS	r0x01, 6
	BRA	_00134_DS_
;	.line	103; sequencer.c	rt_basex32768_job();
	CALL	_rt_basex32768_job
;	.line	104; sequencer.c	return;
	BRA	_00137_DS_
_00134_DS_:
;	.line	106; sequencer.c	if(counter & 0x8000)
	BTFSS	r0x01, 7
	BRA	_00137_DS_
;	.line	108; sequencer.c	rt_basex65536_job();
	CALL	_rt_basex65536_job
_00137_DS_:
;	.line	109; sequencer.c	return;
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	



; Statistics:
; code size:	  204 (0x00cc) bytes ( 0.16%)
;           	  102 (0x0066) words
; udata size:	    0 (0x0000) bytes ( 0.00%)
; access size:	    2 (0x0002) bytes


	end
