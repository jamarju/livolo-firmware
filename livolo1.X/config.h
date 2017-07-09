/* 
 * File:   config.h
 * Author: javi
 *
 * Created on July 2, 2017, 1:19 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

#define _XTAL_FREQ          4000000UL // for __delay_ms and __delay_us
#define BAUD_RATE           57600UL // bps; 1 byte @57.6K = 173.611 usecs
#define TRIP_THRESHOLD      10 // raw freq must be below TRIP*freq/256 of the average for button press (17 for 5V)
#define MIN_TIME_TO_RELEASE 4 // min reads below TRIP_T for a button press
#define MAX_TIME_TO_RELEASE 250 // max reads below TRIP_T before it's considered new normal condition (ie water drops)
#define HYST_THRESHOLD      4 // raw freq must be above freq - HYST*freq/256 for button release
#define AVERAGING_RATE      8 // use 1 every 8 raw values when averaging
#define TIME_BETWEEN_READS  16384 // in Fosc/4 cycles (= usec), @ 57.6Kbps can send 94 debug bytes between reads

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

