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
#define BAUD_RATE           57600UL // bps
#define TRIP_THRESHOLD      17 // raw freq must be below TRIP*freq/256 of the average for button press
#define MIN_TIME_TO_RELEASE 4 // min reads below TRIP_T for a button press
#define MAX_TIME_TO_RELEASE 250 // max reads below TRIP_T before it's considered new normal condition (ie water drops)
#define HYST_THRESHOLD      4 // raw freq must be above freq - HYST*freq/256 for button release
#define AVERAGING_RATE      8 // use 1 every 8 raw values when averaging
#define TIME_BETWEEN_READS  16384 // in Fosc/4 cycles

// Friendly pin names
#define LED         RB6
#define LED_BLUE    0
#define LED_RED     1

#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

