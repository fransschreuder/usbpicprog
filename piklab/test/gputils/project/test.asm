;  list      p=16f877
  #include <p16f871.inc>
  #include <test.inc>

  __CONFIG _CP_OFF & _WDT_OFF & _BODEN_OFF & _PWRTE_ON & _RC_OSC & _WRT_ENABLE_ON & _LVP_ON & _CPD_OFF

;**********************************************************************
BANK0 = 0x0
BANK1 = 0x80
BANK2 = 0x100
BANK3 = 0x180

;**********************************************************************
INT_VAR UDATA_SHR
w_saved      RES 1 ; variable used for context saving
status_saved RES 1 ; variable used for context saving

TEMP_VAR UDATA_SHR 0x20
index       RES 1

;**********************************************************************
STARTUP CODE                    ; processor reset vector
        nop                     ; for debugger
        movlw high start        ; load upper byte of 'start' label
        movwf PCLATH            ; initialize PCLATH
        goto  start             ; go to beginning of program

;**********************************************************************
INT_VECTOR CODE 0x0004          ; interrupt vector location
        movwf w_saved
        movfw  STATUS
        movwf status_saved

        bcf   INTCON,2          ; reset TMR0 interrupt flag
        bsf   PORTB,0

        movfw  status_saved
        movwf STATUS
        swapf w_saved,f
        swapf w_saved,w
        retfie                  ; return from interrupt

;**********************************************************************
MAIN CODE
start
; initialize microchip
        clrf TMR0 ^ BANK0
        clrf INTCON ^ BANK0
        banksel TRISA            ; bank 1
        movlw B'11010100'        ; set prescaler at 32 (32768 Hz / 4 / 256 / 32 = 1 Hz)
        movwf OPTION_REG ^ BANK1 ; and start TMR0 (internal source clock)
        movlw B'00000110'        ; set PORTA and PORTE to be all digital
        movwf ADCON1 ^ BANK1
        banksel INTCON           ; back to bank 0
        bsf INTCON,T0IE          ; enable TMR0 interrupt
        bsf INTCON,GIE           ; enable all interrupts

        nop

        END     ; directive 'end of program'
