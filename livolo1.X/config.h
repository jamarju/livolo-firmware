#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------
// Compilation flags
// --------------------------------------------------------------------

// Use RB4 (RF module pin #5 which is easier to solder) as TX instead of RB7/TX
#define UART_TX_BITBANG_ON_RB4
    
    
// --------------------------------------------------------------------
// Constants
// --------------------------------------------------------------------

// for __delay_ms and __delay_us
#define _XTAL_FREQ          4000000UL 

// bps; 1 byte @57.6K = 173.611 usecs
#define BAUD_RATE           57600UL

// trip threshold in 1/256 fractions of the average frequency
// working (empirical) values: 17 for 5V, 10 for 3V
#define TRIP_THRESHOLD      10

// release threshold in 1/256 fractions of the average frequency
#define HYST_THRESHOLD      4

// min reads tripped for a button press
#define MIN_READS_TO_RELEASE 4

// max reads tripped after which it's considered permanent capacitance change
// (eg. water drops)
#define MAX_READS_TO_RELEASE 50

// space out averages 1 every 8 raw values
#define AVERAGING_RATE      8

// in Fosc/4 cycles (= usec)
#define TIME_BETWEEN_READS  16384

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

