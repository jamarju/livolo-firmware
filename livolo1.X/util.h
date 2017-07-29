#ifndef UTIL_H
#define	UTIL_H

#include <xc.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Shared constants
 */
#define LED         RB6
#define LED_BLUE    0
#define LED_RED     1

/*
 * Shared macros
 */
#define READ_TMR1_ATOMICALLY(x) \
    TMR1ON = 0;                 /* Stop timer */ \
    x = TMR1;                   /* Read T1 count */ \
    TMR1ON = 1;                 /* Start timer */

#define CLK_31KHZ() \
    OSCCONbits.IRCF = 0b000;    /* 31 kHz (LFINTOSC) */ \
    while (LTS == 0) ;          /* Wait to stabilize */

#define CLK_4MHZ() \
    OSCCONbits.IRCF = 0b110;    /* 4 MHz (default) */ \
    while (HTS == 0) ;          /* Wait to stabilize */
    
// Switch to LFINTOSC, this takes 0.64 ms (measured)
// Actual clock is NOT callibrated, eg 25974 Hz (measured)
#define DELAY_31KHZ(x) \
    _delay((unsigned long) ((x)*31/4))

// Switch to HFINTOSC, this takes 0.796 ms (measured)
#define DELAY_4MHZ(x) \
    _delay((unsigned long) ((x)*1000))


#ifdef	__cplusplus
}
#endif

#endif	/* UTIL_H */

