;   list        p=16f871        ; list directive to define processor
    #include    <p16f871.inc>   ; processor specific variable definitions

    __CONFIG _CP_OFF & _WDT_OFF & _BODEN_OFF & _PWRTE_ON & _XT_OSC & _WRT_ENABLE_ON & _LVP_OFF & _CPD_OFF
    ;__CONFIG _CP_OFF & _WDT_OFF & _PWRTE_ON & _EXTRC_CLKOUT

;**********************************************************************
M_SAVE_STATE macro
    mov wf w_saved
    movfw STATUS
    movwf status_saved
    endm

M_RESTORE_STATE macro
    movfw status_saved
    movwf STATUS
    swapf w_saved,f
    swapf w_saved,w
    endm

BANK0 = 0x0
BANK1 = 0x80
BANK2 = 0x100
BANK3 = 0x180

;**********************************************************************
INT_VAR UDATA_SHR
w_saved      RES 1      ; variable used for context saving
status_saved RES 1      ; variable used for context saving

TEMP_VAR UDATA_SHR
count        RES 1
count2       RES 1
ledA         RES 1
ledC         RES 1
ledB         RES 1

;**********************************************************************
STARTUP CODE 0x000          ; processor reset vector
    nop                     ; for debugger
    movlw   high start      ; load upper byte of 'start' label
    movwf   PCLATH          ; initialize PCLATH
    goto    start           ; go to beginning of program

;**********************************************************************
INT_VECTOR CODE 0x004       ; interrupt vector location
    goto interrupt

;**********************************************************************
PROG1 CODE
interrupt
    M_SAVE_STATE
    M_SAVE_STATE ; try using twice a macro

; increment duty cycle
;    btfss  CCP1CON ^ BANK0,4
;    goto   duty_cycle_inc_bit0
;    bcf    CCP1CON ^ BANK0,4
;    btfss  CCP1CON ^ BANK0,5
;    goto   duty_cycle_inc_bit1
;    bcf    CCP1CON ^ BANK0,5
;    incfsz CCPR1L ^ BANK0,f
;    goto   update_duty_cycle_done
;duty_cycle_inc_bit0
;    bsf    CCP1CON ^ BANK0,4
;    goto   update_duty_cycle_done
;duty_cycle_inc_bit1
;    bsf    CCP1CON ^ BANK0,5
;update_duty_cycle_done

; increment count
    decfsz  count,1
    goto    done
    movlw   .10
    movwf   count

; blink and output
    movfw   ledB
    xorlw   B'00001111'
    movwf   ledB
    movwf   PORTB

done
    bcf INTCON,2            ; reset TMR0 interrupt flag
    M_RESTORE_STATE
    retfie                  ; return from interrupt

;**********************************************************************
start
; initialize microchip
    clrf TMR0 ^ BANK0
    clrf INTCON ^ BANK0
;    clrf PORTA ^ BANK0
    clrf PORTB ^ BANK0
;    clrf PORTC ^ BANK0
;    clrf PORTD ^ BANK0
;    clrf PORTE ^ BANK0
    banksel TRISA       ; bank 1
;    clrf TRISA ^ BANK1  ; all outputs
    clrf TRISB ^ BANK1  ; all outputs
;    clrf TRISC ^ BANK1  ; all outputs
;   clrf TRISD ^ BANK1  ; all outputs
;   clrf TRISE ^ BANK1  ; all outputs
    movlw B'11010101'   ; set prescaler to 64
    movwf OPTION_REG ^ BANK1 ; and start TMR0 (internal source clock)
;    movlw B'00000110'   ; set PORTA and PORTE to be all digital
;    movwf ADCON1 ^ BANK1
    banksel INTCON      ; back to bank 0
    bsf INTCON,T0IE     ; enable TMR0 interrupt
    bsf INTCON,GIE      ; enable all interrupts

; initialize PWM
;    CLRF CCP1CON ^ BANK0  ; disable CCP module
;    CLRF TMR2 ^ BANK0     ; clear Timer2 (also set the pre-scaler to 1)
;    MOVLW .1
;    MOVWF CCPR1L ^ BANK0  ; set duty cycle = (CCPR1L+1)/(PR2+1) [25% for (31+1)/(127+1)]
;    banksel PR2           ; select bank 1
;    MOVLW .255
;    MOVWF PR2 ^ BANK1     ; set period = Fosc/4/pre-scaler/(PR2+1)
;    BCF TRISC ^ BANK1,2   ; set CCP1 pin as output
;    banksel PIR1          ; back to bank 0
;    MOVLW B'001100'
;    MOVWF CCP1CON ^ BANK0 ; set PWM mode (the 2 LSBs of the duty cycle are set to '00')
;    BSF T2CON ^ BANK0,TMR2ON ; Timer2 starts to increment

    ; The CCP1 interrupt is disabled,
    ; do polling on the TMR2 Interrupt flag bit
;PWM_Period_Match
;   BTFSS PIR1, TMR2IF
;   GOTO PWM_Period_Match

    ; Update this PWM period and the following PWM Duty cycle
;   BCF PIR1, TMR2IF

; initialize variables
    movlw B00000101'
    movwf ledB
    movlw .10
    movwf count

; loop forever
    goto $

;**********************************************************************
    END