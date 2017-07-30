#include <xc.inc>
#include "config.h"


; Bitbang RB4 to transmit serial data at (hardcoded) 57600 bps.
; Requirements:
;
; - Data to transmit must reside in BANK0 to minimize bank switching since 
; timing is critical.
;
; - Also a 4 MHz CPU clock is assumed (1 instruction = 1 us)

    
;----------------------------------------------------------------------
; Global variables in BANK0
;----------------------------------------------------------------------
psect	uart_bitbang_data,global,class=BANK0,space=1,delta=1
_txdata:	
    ds	1
    
;----------------------------------------------------------------------
; Code
;----------------------------------------------------------------------
psect   uart_bitbang_code,global,class=CODE,delta=2 ; PIC10/12/16

_uart_bitbang_write_57600:
    BANKSEL(PORTB)
    movwf _txdata,f ; w reg contains byte to transmit
    bcf RB4 ; start byte, t=1 us
    bsf CARRY ; stop byte, t=2
    goto $+1 ; ; delay until next bit, t=4
    goto $+1 ; t=6
    goto $+1 ; 8
    goto $+1 ; 10
    goto $+1 ; 12
REPT 9    
    rrf _txdata,f ; 13
    btfss CARRY ; 15 (skip), 14 (no skip)
    goto $+4 ; 16
    goto $+1 ; 17
    bsf RB4 ; bit=1, t=1
    goto $+4; 3
    nop ; 17
    bcf RB4 ;bit=0, t=1
    goto $+1 ; 3
    goto $+1 ; 5
    goto $+1 ; 7
    goto $+1 ; 9
    goto $+1 ; 11
    nop ; 12
ENDM
    goto $+1 ; 14
    goto $+1 ; 16
    nop ; 17
    bsf RB4; rest state=high, t=1
    return


global _uart_bitbang_write_57600
    
; To find the C signature create a dummy function with the desired prototype:
; 
; void f(uint8_t) { asm("nop"); }
; main() { f(0); }
;
; MPLAB X -> project settings -> XC8 global options -> XC8 compiler ->
; Optimizations -> Disable Optimizations -> build -> check asm list file.
signat _uart_bitbang_write_57600,4217
