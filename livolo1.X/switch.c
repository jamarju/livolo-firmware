#include <xc.h>
#include <stdint.h>
#include "switch.h"
#include "config.h"

static bit switch_status = 0;

/*
 * Private functions
 */

static void
switch_on(void)
{
    RELAY1_S = 1;
    __delay_ms(20);
    RELAY1_S = 0;
}

static void
switch_off(void)
{
    RELAY1_R = 1;
    __delay_ms(20);
    RELAY1_R = 0;
}


/*
 * Public functions
 */

/**
 * HFE60 datasheet says to not energize both set and reset coils at the
 * same time. Since PORTx output status is unknown on reset, let's pull
 * them down early (even before
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
    switch_status = !switch_status;
    // Toggle led
    LED = switch_status ? LED_RED : LED_BLUE;
    // Toggle relay
    if (switch_status) {
        switch_on();
    } else {
        switch_off();
    }
}


inline bit 
switch_is_on()
{
    return switch_status;
}


