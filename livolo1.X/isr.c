#include <xc.h>
#include <stdint.h>
#include "config.h"
#include "capsensor.h"

/*
void interrupt isr(void)
{
    // Determine which flag generated the interrupt
    else if (T0IE && T0IF) {
        T0IF = 0;
    }
    else {
        PIR1 = PIR2 = 0;
        T0IF = 0;
        INTF = 0;
        RABIF = 0;
    }
}

*/