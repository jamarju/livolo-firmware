#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "capsensor.h"
#include "config.h"

// arithmetic shift right because xc8 is stupid
#define m_asr16(x,y) (((x) >> (y)) | (((int16_t)(x) < 0)?(0xffff << (16-(y))):0))
#define ABS(x) ((int16_t)(x) >= 0 ? (x) : (-(x)))


static uint16_t rolling_avg = 0;
static uint16_t rolling_avg_on_press = 0;
static uint8_t avgs = 0;
static bit pressed = 0;
static uint8_t cycles_pressed = 0;

/*
 * Private functions
 */
static inline void
capsensor_start(void)
{
    TMR1ON  = 0;
    TMR1    = 0; // reset T1 (= stopwatch)
    T1IF    = 0; // also clear T1's if since we use it to time between readings
    TMR0    = 0; // reset T0
    T0IF    = 0; // clear interrupt flag since T0 is always running
    TMR1ON  = 1; // stoptwatch on
}

static inline uint16_t
capsensor_time(void)
{
    return TMR1;
}

static inline void
capsensor_stop(void)
{
    TMR1ON  = 0; // stopwatch off
}

static inline void
capsensor_wait_t0_overflow(void)
{
    while (!T0IF) ;
    T0IF = 0;
}

/*
 * Public functions
 */

bit
capsensor_is_button_pressed(void)
{
    uint8_t valid_press = 0;
    
    capsensor_start();
    capsensor_wait_t0_overflow();
    uint16_t freq = capsensor_time();
    
    //rolling_average = rolling_average + m_asr16(TMR1 - rolling_average, 4);
    
    if (! pressed) {
        if (ABS(rolling_avg - freq) > TRIP_THRESHOLD * rolling_avg / 256) {
            puts("TRIP\r");
            pressed = 1;
            cycles_pressed = 0;
            rolling_avg_on_press = rolling_avg;
        }
    } else {
        cycles_pressed++;
        if (cycles_pressed > MAX_TIME_TO_RELEASE) {
            puts("WATER\r");
            // it's a water drop, adapt to the new conditions
            // (ie. keep the latest rolling average)
            pressed = 0;
        }
        else if (ABS(rolling_avg_on_press - freq) < HYST_THRESHOLD * rolling_avg_on_press / 256) {
            // finger release before MAX_TIME_TO_RELEASE
            pressed = 0;
            rolling_avg = rolling_avg_on_press;
            puts("RELEASE\r");

            if (cycles_pressed > MIN_TIME_TO_RELEASE) {
                // short press -> ignore
                valid_press = 1;
                puts("PRESS\r");
            }
        }
    }
    
    // when the button is press make the rolling every cycle in case we need
    // to adapt to the new situation fast (ie water drop))
    avgs++;
    if (pressed || (avgs % AVERAGING_RATE == 0)) {
        rolling_avg = (rolling_avg * 15 + freq) / 16;
    }
    
    return valid_press;
}

void
capsensor_init(void) 
{
    // Configure C1 and C2 as an astable multivibrators using C12IN3- as negative feedback
    CM1CON0 = 0b10010111; // on, no C1OUT, inverse pol, C1Vin+ = C1Vref, C1Vin- = C12IN3- 
    CM2CON0 = 0b10100111; // on, yes C2OUT, normal pol, C2Vin+ = C2Vref, C2Vin- = C12IN3-
    VRCON   = 0b10011100; // C1VREN = CVRef, C2VREN = 0.6, Vr = (8/32 + 12/32)*Vdd
    SRCON   = 0b10110000; // C2OUT = !Q, connect C1/C2 outs to S/R

    // Configure T0 as counter with T0CKI as clock source
    PSA     = 0; // prescaler assigned to Timer0
    OPTION_REGbits.PS = 0b000; // 1:2 prescaler (default 1:256)
    
    // Init rolling average
    capsensor_start();
    for (int i = 0; i < 16; i++) {
        capsensor_wait_t0_overflow();
    }
    uint16_t tmr1 = capsensor_time();
    rolling_avg = tmr1 / 16;
}


bit
capsensor_is_ready_for_another(void)
{
    return (TMR1 > TIME_BETWEEN_READS);
}

inline uint16_t
capsensor_get_rolling_average(void)
{
    return rolling_avg;
}
