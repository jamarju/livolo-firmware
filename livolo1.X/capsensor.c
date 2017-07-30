#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "capsensor.h"
#include "config.h"
#include "util.h"

/*
 * ISR
 */
void interrupt isr()
{
#ifdef DEBUG
    // We're only expecting T0IF so panic otherwise
    if (! T0IF) {
        for (;;) {
            LED ^= 1;
        }
    }
#endif
    TMR1ON = 0;
    T0IF = 0;
}

/*
 * Private macros (needs XC8 optimization for speed on to be inlined)
 */

#define CAPSENSOR_START_INT() \
    TMR0    = 128; /* Reset T0. Increment will be inhibited for 2 instructions */ \
    T0IF    = 0; /* clear interrupt flag */ \
    T0IE    = 1; /* Enables the Timer0 interrupt */ \
    GIE     = 1; /* Enables all unmasked interrupts */ \
    C1VREN  = 1; /* CVREF circuit powered on and routed to C1VREF input of Comparator C1 */ \
    C1ON    = 1; /* Comparator C1 is enabled */ \
    C2ON    = 1; /* Comparator C2 is enabled */ \
    TMR1    = 0; /* Reset T1 */ \
    TMR1ON  = 1; /* Start T1 */


#define CAPSENSOR_WAIT_T0_OVERFLOW_INT() \
    while (TMR1ON) ; /* The ISR will stop T1, so poll this */ \
    C1ON    = 0; /* Comparator C1 is disabled */ \
    C2ON    = 0; /* Comparator C2 is disabled */ \
    C1VREN  = 0; /* 0.6 Volt constant reference routed to C1VREF input of Comparator C2 */ \
    T0IE    = 0; /* Disables the Timer0 interrupt */ \
    GIE     = 0; /* Disables all interrupts */


/*
 * Private vars
 */
static uint8_t avgs = 0;

/*
 * Public vars (for debugging from main)
 */
uint16_t cap_raw;
uint16_t cap_rolling_avg = 0; // fixed point 12.4
uint16_t cap_frozen_avg = 0; // fixed point 12.4
uint8_t cap_cycles = 0;


/*
 * Public functions
 */

void
capsensor_init(void) 
{
    // Configure C1 and C2 as a relaxation oscillator
    // See Microchip AN1101
    CM1CON0 = 0b00010111; 
    // C1ON     0------- disable (-> C1OUT=0) to save power
    // C1OUT    -r------ r/o
    // C1OE     --0----- no output on pin
    // C1POL    ---1---- inverse pol
    // unimpl   ----x--- 
    // C1R      -----1-- C1Vin+ = C1Vref
    // C1CH     ------11 C1Vin- = C12IN3-
    
    CM2CON0 = 0b00100111;
    // C2ON     0------- disable (-> C2OUT=0) to save power
    // C2OUT    -r------ r/o
    // C2OE     --1----- connected to pin
    // C2POL    ---0---- normal pol
    // unimpl   ----x--- 
    // C2R      -----1-- C2Vin+ = C2Vref
    // C2CH     ------11 C2Vin- = C12IN3-

    VRCON   = 0b00011100; // C1VREN = CVRef, C2VREN = 0.6, Vr = (8/32 + 12/32)*Vdd
    // C1VREN   0------- 0.6 Volt constant reference routed to C1VREF input of Comparator C2
    // C2VREN   -0------ 0.6 Volt constant reference routed to C2VREF input of Comparator C2
    // VRR      --0----- high range
    // VP6EN    ---1---- enable 0.6V ref
    // VR       ----1100 Vr = (8/32 + 12/32) * Vdd = 0.625 * Vdd = 1.88V
    // VR       ----1101 Vr = (8/32 + 13/32) * Vdd = 0.656 * Vdd = 1.97V

    SRCON   = 0b10110000; // C2OUT = !Q, connect C1/C2 outs to S/R
    // SR1      1------- C2OUT pin is the latch ~Q output
    // SR0      -0------ C1OUT pin is the Comparator C1 output
    // C1SEN    --1----- C1 comparator output sets SR latch
    // C2REN    ---1---- C2 comparator output resets SR latch
    // PULSS    ----0--- Does not trigger pulse generator
    // PULSR    -----0-- Does not trigger pulse generator
    // unimpl   ------xx

    // Configure T0 as counter with T0CKI as clock source.
    // The maximum frequency measurable by the pin is 50 MHz, however the
    // maximum period AFTER prescaler must be >4Tosc + 40 ns, at 4 MHz:
    // T >1040 ns -> f <0.96 MHz, so we should use at least a 1:4 prescaler
    // for ~3.0 MHz oscillator freqs.

    OPTION_REG = 0b00000010 | (OPTION_REG & 0b11110000U);
    // dont' care  0000----
    // PSA         ----0---  Prescaler is assigned to the Timer0 module
    // PS2         -----010  1:8 Timer0 rate
    
    // Init rolling average
    cap_rolling_avg = 0;
    for (int8_t i = 0; i < 16; i++) {
        CAPSENSOR_START_INT();
        CAPSENSOR_WAIT_T0_OVERFLOW_INT();

        // The value in TMR1 is the time (us) elapsed time after 256*8 periods
        // of the relaxation oscillator, thus its frequency is:
        // F = 256*8 / TMR1 MHz
        cap_rolling_avg += TMR1;
    }
}


bit
capsensor_is_button_pressed(void)
{
    uint8_t do_switch = 0;
    
    CAPSENSOR_START_INT();
    CAPSENSOR_WAIT_T0_OVERFLOW_INT();
    cap_raw = TMR1;
    
    if (cap_cycles < READS_TO_SWITCH) {
        if ((int16_t)(cap_raw - cap_rolling_avg / 16) > (int16_t)(TRIP_THRESHOLD * (cap_rolling_avg / 16) / 256)) {
            cap_cycles++;
            if (cap_cycles >= READS_TO_SWITCH) {
                do_switch = 1;
            }
        } else {
            cap_frozen_avg = cap_rolling_avg;
            cap_cycles = 0;
        }
    } else {
        if ((int16_t)(cap_raw - cap_frozen_avg / 16) > (int16_t)(HYST_THRESHOLD * (cap_frozen_avg / 16) / 256)) {
            cap_cycles++;
            if (cap_cycles >= RELEASE_TIMEOUT) {
                cap_cycles = 0;
            }
        } else {
            cap_cycles = 0;
            cap_rolling_avg = cap_frozen_avg;
        }
    }
    
    // when the button is tripped make the rolling every cycle in case we need
    // to adapt to the new situation fast (ie water drop)
    avgs++;
    if (cap_cycles >= READS_TO_SWITCH || (avgs % AVERAGING_RATE == 0)) {
        cap_rolling_avg = cap_rolling_avg - cap_rolling_avg / 16 + cap_raw;
    }
    
    return do_switch;
}

