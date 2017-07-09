#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "config.h"
#include "uart.h"
#include "capsensor.h"
#include "switch.h"

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
 * RA0 (19) I
 * RA1 (18) O RELAY1-S
 * RA2 (17) I T0CKI
 * RA3 ( 4) I MCLR only
 * RA4 ( 3) I
 * RA5 ( 2) I
 * 
 * RB4 (13) I (goes to RF5)
 * RB5 (12) I USART RX
 * RB6 (11) O LED (0=blue, 1=red)
 * RB7 (10) I USART TX (yes, must be I)
 * 
 * RC0 (16) I
 * RC1 (15) X (NC) ---> CAP READ 2 in the 2-gang switch?
 * RC2 (14) O RELAY2-S
 * RC3 ( 7) I C12IN3- (negative feedback of C2), CAP READ 1
 * RC4 ( 6) O C2OUT (uses C2 as a astable multivibrator)
 * RC5 ( 5) X (NC)
 * RC6 ( 8) O RELAY2-R
 * RC7 ( 9) O RELAY1-R
 */


void main(void) {
    uint32_t i;
#if 0
    // Crank up to 8 MHz
    OSCCONbits.IRCF = 0b111; // 8 MHz

    // Crank upper osc
    OSCTUNE = 0b00001111; // this will set Fosc to 8.88 MHz as measured by me
#endif

    // Early (before osc is settled) inits.
    switch_preinit();
    
    // Tris config (1=in, 0=out)
    TRISA   = 0b11111101;
    TRISB   = 0b10111111;
    TRISC   = 0b00101011;
    
    // All pins digital)
    ANSEL   = 0b00000000;
    ANSELH  = 0b00000000;
    
    // Wait until osc is stable
    while (HTS == 0) ;

    switch_init();
    uart_init();
    capsensor_init();

    LED = LED_BLUE;
    
    for (;;) {
        if (OERR) {
            uart_write('O');
            __delay_ms(20);
            CREN = 0;
            CREN = 1;
        }
        if (uart_data_ready()) {
            uint8_t c = uart_read();
            if (c == 'r') LED = LED_RED;
            if (c == 'a') LED = LED_BLUE;
            printf("ST:%d\r\n", LED);
        }
        
        if (capsensor_is_ready_for_another()) {
            if (capsensor_is_button_pressed()) {
                switch_toggle();
            }
            putchar('.');
            //printf("%d\r\n", capsensor_get_rolling_average());
        }
    }
}
