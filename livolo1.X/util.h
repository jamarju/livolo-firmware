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
#define CLK_125KHZ() \
    OSCCONbits.IRCF = 0b001;    /* 125 kHz (HFINTOSC) */ \

#define CLK_4MHZ() \
    OSCCONbits.IRCF = 0b110;    /* 4 MHz (default) */ \
    
#define DELAY_125KHZ(x) \
    _delay((unsigned long) ((x)*125/4))

#define DELAY_4MHZ(x) \
    _delay((unsigned long) ((x)*1000))



#ifdef	__cplusplus
}
#endif

#endif	/* UTIL_H */

