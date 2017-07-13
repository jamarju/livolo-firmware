#include <xc.h>
#include <stdint.h>
#include "switch.h"
#include "config.h"

static bit switch_status = 0;

/*
 * Private functions
 */


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
}

void
switch_init()
{
    __delay_ms(1000); // relays won't start accepting commands right away
    switch_off();
    LED = LED_BLUE;
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
    switch_status = 1;
    LED = LED_RED;
    RELAY1_S = 1;
    __delay_ms(20);
    RELAY1_S = 0;
}

void
switch_off(void)
{
    switch_status = 0;
    LED = LED_BLUE;
    RELAY1_R = 1;
    __delay_ms(20);
    RELAY1_R = 0;
}

