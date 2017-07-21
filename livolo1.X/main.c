#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "config.h"
#include "uart.h"
#include "capsensor.h"
#include "switch.h"
#include "adc.h"

// PIC16F690 Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR pin function is I/O
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/**
 * JP1-CR: C=column R=row where 12v=A7, Vdd=B1, Vss=B2 and so on
 * In brackets [behavior] as observed in the original firmware
 * RF = RF connector on main board
 * J1 = J1 connector on PIC board
 * 
 * RA0 (19) O ??? --- JP1-B7, [4 ms high-Z pulse on start, then 0v]
 * RA1 (18) O RELAY1-S, [4 ms high-Z pulse on start, then 0v]
 * RA2 (17) I T0CKI --- RC4 (6) C2OUT
 * RA3 ( 4) I MCLR only [high-Z]
 * RA4 ( 3) A AN3 --- JP1-A3, ON/OFF STATUS??? [high-Z]
 * RA5 ( 2) I --- JP1-A1 --- R23 (unpopulated) [3v when red, 0v when blue]
 * 
 * RB4 (13) I --- JP1-B3 --- RF5, O if soft-uart TX
 * RB5 (12) I USART RX
 * RB6 (11) O LED1 (0=blue, 1=red)
 * RB7 (10) I (yes, must be I) USART TX --- JP1-A2 --- R24 (unpopulated)
 * 
 * RC0 (16) O BUZZER (if any) [4 ms high-Z pulse on start, then 0v]
 * RC1 (15) I C12IN1- (negative feedback of C2), CAP READ 2 (2-gang only)
 * RC2 (14) O RELAY2-S (2-gang only)
 * RC3 ( 7) I C12IN3- (negative feedback of C2), CAP READ 1
 * RC4 ( 6) O C2OUT (uses C2 as a astable multivibrator)
 * RC5 ( 5) O LED2 (0=blue, 1=red) (2-gang only)
 * RC6 ( 8) O RELAY2-R (2-gang only)
 * RC7 ( 9) O RELAY1-R
 */

void main(void) {
    // Early inits (before osc is settled)
    switch_preinit();
    adc_preinit();
    
    // Tris config (1=in/analog, 0=out)
    TRISA   = 0b11111101;
    TRISB   = 0b10111111;
    TRISC   = 0b00101011;
    
    // All pins digital, AN3 analog
    ANSEL   = 0b00001000;
    ANSELH  = 0b00000000;
    
    // Wait until osc is stable
    while (HTS == 0) ;

    switch_init();
    uart_init();
    capsensor_init();

    for (;;) {
        if (uart_data_ready()) {
            uint8_t c = uart_read();
            printf("\n%c RECV\r\n", c);
            if (c == 'i') switch_on();
            if (c == 'o') switch_off();
            if (c == ' ') switch_toggle();
        }
        
        if (capsensor_is_ready_for_another()) {
            if (capsensor_is_button_pressed()) {
                switch_toggle();
            }
            adc_read_power();
#ifdef DEBUG
            printf("%u,%u,%u,%c,%u,%c\r\n", 
                    capsensor_rolling_avg, 
                    capsensor_rolling_avg_on_trip, 
                    capsensor_freq,
                    capsensor_status,
                    adc_power,
                    switch_status ? 'I' : 'O');
#endif    
        }
    }
}
