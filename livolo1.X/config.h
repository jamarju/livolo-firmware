#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------
// Compilation flags
// --------------------------------------------------------------------

// enable debug serial out, note this also slows down time between sensor reads
#define DEBUG

// Use RB4 (RF module pin #5 which is easier to solder) as TX instead of RB7/TX
#define UART_TX_BITBANG_ON_RB4
    
// Accept commands via USART (RX pin)
#define ACCEPT_CMDS_VIA_UART
    
// --------------------------------------------------------------------
// Constants
// --------------------------------------------------------------------

// For __delay_ms and __delay_us
#define _XTAL_FREQ          4000000UL 

// BPS; 1 byte @57.6K = 173.611 usecs
#define BAUD_RATE           57600UL

// Trip threshold in 1/256 fractions of the average frequency
// working (empirical) values: 17 for 5V, 10 for 3V
#define TRIP_THRESHOLD      10

// Release threshold in 1/256 fractions of the average frequency
#define HYST_THRESHOLD      4

// Min reads below threshold to trip the switch
#define READS_TO_TRIP       2

// If sensor is not released after RELEASE_TIMEOUT cycles, the last frequency
// is assumed to be the new condition, eg. if dropped some water on the plate
#define RELEASE_TIMEOUT     30

// Space out averages 1 every AVERAGING_RATE raw values
#define AVERAGING_RATE      8

// In Fosc/4 cycles (= usec). Slowed down when debugging to see what's going on
#ifdef DEBUG
#define TIME_BETWEEN_READS  49152
#else
#define TIME_BETWEEN_READS  16384
#endif

// Friendly pin names
#define LED         RB6
#define LED_BLUE    0
#define LED_RED     1
    
#define RELAY1_R    RC7 // relay 1 reset (Hongfa HFE60 2-coil latched)
#define RELAY1_S    RC6 // relay 1 set

#define RELAY2_R    RC2 // relay 2 reset
#define RELAY2_S    RA1 // relay 2 set

#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

