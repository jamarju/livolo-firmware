#include <xc.h>
#include <stdint.h>
#include "switch.h"
#include "config.h"
#include "util.h"

/*
 * Private constants
 */
#define RELAY1_R    RC7 // relay 1 reset (Hongfa HFE60 2-coil latched)
#define RELAY1_S    RC6 // relay 1 set

#define RELAY2_R    RC2 // relay 2 reset
#define RELAY2_S    RA1 // relay 2 set

/*
 * Public vars
 */

bit switch_status;

/*
 * Public functions
 */

/**
 * HFE60 datasheet says to not energize both set and reset coils at the
 * same time. Since PORTx output status is unknown on reset, let's pull
 * them down early
 * 
 * If R and S are pulled high at the same time the pic hangs and the whole
 * thing starts drawing a lot of current.
 */
void
switch_preinit()
{
    RELAY1_R = RELAY1_S = 0;
    LED = LED_RED;
    switch_status = SWITCH_OFF;
}

void
switch_init()
{
    // Delay until the cap is charged before we can switch for the first time
    uint16_t t = POWERUP_TIME * 1000UL / 65536;
    uint8_t pwmt = 0;   // time
    uint8_t pwmd = 0;   // duty
    TMR1ON = 0;
    TMR1 = 0;
    TMR1ON = 1;
    while (t) {
        if (pwmt == pwmd || (pwmt ^ pwmd) == 0xff) LED = ~LED;
        if (++pwmt == 0) pwmd += POLICE_LIGHTS_FREQ;
        if (TMR1IF) { 
            t--;
            TMR1IF = 0;
        }
    }
    TMR1ON = 0;
    switch_off();
}


void
switch_toggle()
{
    if (switch_status) {
        switch_off();
    } else {
        switch_on();
    }
}


void
switch_on(void)
{
    switch_status = SWITCH_ON;
    RELAY1_S = 1;
    CLK_125KHZ();
    DELAY_125KHZ(RELAY_OP_TIME);
    RELAY1_S = 0;
    CLK_4MHZ();
    LED = LED_RED;
}

void
switch_off(void)
{
    switch_status = SWITCH_OFF;
    CLK_125KHZ();
    RELAY1_R = 1;
    DELAY_125KHZ(RELAY_OP_TIME);
    RELAY1_R = 0;
    CLK_4MHZ();
    LED = LED_BLUE;
}

