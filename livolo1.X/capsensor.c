#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "capsensor.h"
#include "config.h"

#define ABS(x) ((int16_t)(x) >= 0 ? (x) : (-(x)))


static uint16_t rolling_avg = 0;
static uint16_t rolling_avg_on_trip = 0;
static uint8_t avgs = 0;
static bit tripped = 0;
static uint8_t cycles_pressed = 0;

/*
 * Inline macros (silly xc8 doesn't seem to inline functions)
 */
#define CAPSENSOR_WAIT_T0_OVERFLOW() \
    while (!T0IF) ; \
    T0IF = 0;

#define CAPSENSOR_TIME() TMR1

/*
 * Private functions
 */
static inline void
capsensor_start(void)
{
    TMR1ON  = 0;
    TMR1    = 0; // reset T1 (= stopwatch)
    T1IF    = 0; // also clear T1's IF since we use it to time between readings
    TMR0    = 0; // reset T0
    T0IF    = 0; // clear interrupt flag since T0 is always running
    TMR1ON  = 1; // stopwatch on
}


/*
 * Public functions
 */

void
capsensor_init(void) 
{
    // Configure C1 and C2 as an astable multivibrators using C12IN3- as negative feedback
    // See Microchip AN1101
    CM1CON0 = 0b10010111; 
    // C1ON     1------- enable
    // C1OUT    -r------ r/o
    // C1OE     --0----- no output on pin
    // C1POL    ---1---- inverse pol
    // unimpl   ----x--- 
    // C1R      -----1-- C1Vin+ = C1Vref
    // C1CH     ------11 C1Vin- = C12IN3-
    
    CM2CON0 = 0b10100111;
    // C2ON     1------- enable
    // C2OUT    -r------ r/o
    // C2OE     --1----- connected to pin
    // C2POL    ---0---- normal pol
    // unimpl   ----x--- 
    // C2R      -----1-- C2Vin+ = C2Vref
    // C2CH     ------11 C2Vin- = C12IN3-

    VRCON   = 0b10011100; // C1VREN = CVRef, C2VREN = 0.6, Vr = (8/32 + 12/32)*Vdd
    // C1VREN   1------- CVRef
    // C2VREN   -0------ 0.6
    // VRR      --0----- high range
    // VP6EN    ---1---- enable 0.6V ref
    // VR       ----1100 Vr = (8/32 + 12/32) * Vdd
    
    SRCON   = 0b10110000; // C2OUT = !Q, connect C1/C2 outs to S/R
    // SR1      1------- C2OUT pin is the latch ~Q output
    // SR0      -0------ C1OUT pin is the Comparator C1 output
    // C1SEN    --1----- C1 comparator output sets SR latch
    // C2REN    ---1---- C2 comparator output resets SR latch
    // PULSS    ----0--- Does not trigger pulse generator
    // PULSR    -----0-- Does not trigger pulse generator
    // unimpl   ------xx

    // Configure T0 as counter with T0CKI as clock source
    PSA     = 0; // prescaler assigned to Timer0
    OPTION_REGbits.PS = 0b000; // 1:2 prescaler (default 1:256)
    
    // Init rolling average
    capsensor_start();
    for (int i = 0; i < 16; i++) {
        CAPSENSOR_WAIT_T0_OVERFLOW();
    }
    uint16_t tmr1 = CAPSENSOR_TIME();
    rolling_avg = tmr1 / 16;
}


bit
capsensor_is_ready_for_another(void)
{
    return (TMR1 > TIME_BETWEEN_READS);
}


bit
capsensor_is_button_pressed(void)
{
    uint8_t valid_press = 0;
    
    capsensor_start();
    CAPSENSOR_WAIT_T0_OVERFLOW();
    uint16_t freq = CAPSENSOR_TIME();
    
    printf("%u %u %u\r", rolling_avg, rolling_avg_on_trip, freq);
    
    if (! tripped) {
        rolling_avg_on_trip = rolling_avg;
        if (ABS(rolling_avg - freq) > TRIP_THRESHOLD * rolling_avg / 256) {
            puts("\nTRIP\r");
            tripped = 1;
            cycles_pressed = 0;
        }
    } else {
        cycles_pressed++;
        if (cycles_pressed > MAX_READS_TO_RELEASE) {
            puts("\nWATER\r");
            // WATER
            // it's a water drop, adapt to the new conditions
            // (ie. keep the latest rolling average)
            tripped = 0;
        }
        else if (ABS(rolling_avg_on_trip - freq) < HYST_THRESHOLD * rolling_avg_on_trip / 256) {
            // finger release before MAX_TIME_TO_RELEASE
            tripped = 0;
            rolling_avg = rolling_avg_on_trip;
            puts("\nRELEASE\r");
            // RELEASE

            if (cycles_pressed > MIN_READS_TO_RELEASE) {
                // short trip -> ignore
                // VALID PRESS
                puts("\nSW!\r");
                valid_press = 1;
            }
        }
    }
    
    // when the button is tripped make the rolling every cycle in case we need
    // to adapt to the new situation fast (ie water drop))
    avgs++;
    if (tripped || (avgs % AVERAGING_RATE == 0)) {
        rolling_avg = (rolling_avg * 15 + freq + 8) / 16;
    }
    
    return valid_press;
}

