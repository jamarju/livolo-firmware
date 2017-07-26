#include <xc.h>
#include <stdint.h>
#include "config.h"
#include "power.h"


/*
 * Private vars
 */
static uint16_t last_tmr1;
static uint32_t elapsed_us;

/*
 * Public vars
 */
bit power_status;

/*
 * Public functions
 */

void
power_preinit(void)
{
    IOCA4 = 1;  // Interrupt-on-change enabled on A4
    if (RA4) asm("nop");  // Read RA4 to clear mismatch
    RABIF = 0;
    last_tmr1 = TMR1;
    power_status = POWER_OK; // assume we're on AC initially
}

/**
 * Check power status.
 * 
 * The livolo has a sort of heartbeat mechanism that pulsates on RA4 at 50 Hz
 * while it's powered on. As soon as AC goes down, the big capacitor keeps
 * powering the PIC for just enough time to turn the relays off.
 * 
 * @return POWER_OK = 1 = on AC, POWER_OUTAGE = 0 = on cap
 */
bit
power_read(void)
{
    uint16_t t1 = TMR1;
    elapsed_us += (t1 - last_tmr1);
    last_tmr1 = t1;
    
    if (elapsed_us > TIME_TO_SHUTDOWN) {
        elapsed_us = 0;
        if (RABIF) {
            if (RA4) asm("nop");  // Read RA4 to clear mismatch
            RABIF = 0;  // Clear IF
            power_status = POWER_OK;
        } else {
            power_status = POWER_OUTAGE;
        }
    }
    return power_status;
}
