#include <xc.h>
#include <stdint.h>
#include "switch.h"
#include "config.h"

static bit switch_on = 0;

void
switch_init()
{
    LED = LED_BLUE;
}


void
switch_toggle()
{
    switch_on = !switch_on;
    // Toggle led
    LED = switch_on ? LED_RED : LED_BLUE;
    // Toggle relay (TODO)
}


inline bit 
switch_is_on()
{
    return switch_on;
}


