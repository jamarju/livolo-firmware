#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "config.h"
#include "uart.h"
#include "capsensor.h"
#include "switch.h"
#include "heartbeat.h"
#include "util.h"
#include "extrigger.h"


void main(void) {
    // Early inits (before osc is settled)
    switch_preinit();
    heartbeat_preinit();

    /* Tris config (1=in/analog, 0=out)
     * 
     * JP1-CR: C=column R=row where 12v=A7, Vdd=B1, Vss=B2 and so on
     * In brackets [behavior] as observed in the original firmware
     * RF = RF connector on main board
     * J1 = J1 connector on PIC board
     */
    
    TRISA   = 0b11011100;
    // RA0 (19) -------0 <--> JP1-B7, ??? [4 ms high-Z pulse on start, then 0v]
    // RA1 (18) ------0- ---> JP1-A6 ---> RELAY1-S
    // RA2 (17) -----1-- <--- RC4 (6) C2OUT (T0CKI from relaxation osc)
    // RA3 ( 4) ----1--- MCLR only [high-Z]
    // RA4 ( 3) ---1---- <--- JP1-A3 <--- AC POWER HEARTBEAT
    // RA5 ( 2) --0----- ---> JP1-A1 ---> R23 --> 2-way COM TX [3v when red, 0v when blue]
    // unimpl   xx------
    
    TRISB   = 0b00101111;
    // unimpl   ----xxxx
    // RB4 (13) ---0---- ---> JP1-B3 ---> RF5 External feedback OUT (or soft-uart TX)
    // RB5 (12) --1----- <--- JP1-B4 <--- RF4 External feedback IN
    // RB6 (11) -0------ ---> LED1 (0=blue, 1=red)
    // RB7 (10) 0------- <--- JP1-A2 <--- R24 <--- 2-way COM RX (out if unpopulated to save power)
    
    TRISC   = 0b00001010;
    // RC0 (16) -------0 ---> JP1-A4 ---> BUZZER (if any) [4 ms high-Z pulse on start, then 0v]
    // RC1 (15) ------1- C12IN1- (negative feedback of C2), CAP READ 2 (2-gang only)
    // RC2 (14) -----0-- ---> JP1-A5 ---> RELAY2-S (2-gang only)
    // RC3 ( 7) ----1--- C12IN3- (negative feedback of C2), CAP READ 1
    // RC4 ( 6) ---0---- C2OUT
    // RC5 ( 5) --0----- ---> LED2 (0=blue, 1=red) (2-gang only)
    // RC6 ( 8) -0------ ---> JP1-B5 ---> RELAY2-R (2-gang only)
    // RC7 ( 9) 0------- ---> JP1-B6 ---> RELAY1-R

    // All pins digital
    ANSEL   = 0b00000000;
    ANSELH  = 0b00000000;

    // Pull-ups
    WPUA    = 0b00000000;
    // RA0 (19) -------0
    // RA1 (18) ------0-
    // RA2 (17) -----0--
    // RA4 ( 3) ---0----
    // RA5 ( 2) --0-----
    // unimpl   xx--x---

    WPUB    = 0b00100000;
    // unimpl   ----xxxx
    // RB4 (13) ---0----
    // RB5 (12) --1----- yes
    // RB6 (11) -0------
    // RB7 (10) 0-------
    
    nRABPU  = 0; // PORTA/PORTB pull-ups are enabled by individual PORT latch values
            
    // Initial pin values
    PORTA   = 0b00000000;
    PORTB   = 0b00000000;
    PORTC   = 0b00000000;
    
    // Wait until osc is stable
    while (HTS == 0) ;
    
    switch_init();
    capsensor_init();

    // WDT prescaler
    CLRWDT();
    WDTCON  = 0b00001010;
    // unimpl   xxx-----
    // WDTPS    ---0100- 1:512  ~ 34/17/11ms @ 15/31/45 KHz LFINTOSC (min/typ/max)
    // WDTPS    ---0101- 1:1024 ~ 68/33/23ms @ 15/31/45 KHz LFINTOSC (min/typ/max)
    // SWDTEN   -------0 WDT is turned off (for now)

    for (;;) {

#ifdef TIME_TO_SHUTDOWN
        heartbeat_update();
        if (heartbeat_outage()) {
            // Switch off only if we're on so as not to energize the coil
            // pointlessly, which would drain the cap and cause voltage
            // drops everywhere.
            if (switch_status == SWITCH_ON) {
                switch_off();
            }
        }
#endif
        
        if ((capsensor_is_button_pressed() || extrigger_read()) && !heartbeat_outage()) {
            switch_toggle();
        }

#ifdef DEBUG
        printf("%u,%u,%u,%u,%u,%u\r\n", 
                cap_rolling_avg / 16, 
                cap_frozen_avg / 16, 
                cap_raw,
                cap_cycles,
                heartbeat_cycles,
                switch_status);
#endif

        SWDTEN = 1;
        SLEEP();    // clear WDT and sleep
        SWDTEN = 0;
    }
}
