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

// Space out averages 1 every AVERAGING_RATE raw values. Should be a power of
// 2 optimally
#define AVERAGING_RATE      8

// In Fosc/4 cycles (= usec). Slowed down when debugging to see what's going on
#ifdef DEBUG
#define TIME_BETWEEN_READS  49152
#else
#define TIME_BETWEEN_READS  16384
#endif

// Time to shutdown the relays after an outage is detected (usec)
#define TIME_TO_SHUTDOWN    1500000UL

// Relay operation time (ms), must be >recommended max spec (10 ms), see
// Hongfa HFE60 datasheet.
#define RELAY_OP_TIME       20

// Power up delay: time to charge the cap before the relays can switch for
// the first time. In ms.
#define POWERUP_TIME        2000

// Police lights frequency controls the speed of the powerup light sequence.
// Must be a power of 2 <=128. Hint: 4 in XC8 free mode, 1 in pro mode.
#define POLICE_LIGHTS_FREQ  4
   

#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

