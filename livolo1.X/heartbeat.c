#include <xc.h>
#include <stdint.h>
#include "config.h"
#include "heartbeat.h"
#include "util.h"

/*
 * Public vars
 */

uint16_t heartbeat_cycles;

/*
 * Public functions
 */

void
heartbeat_preinit(void)
{
#ifdef TIME_TO_SHUTDOWN
    IOCA4 = 1;  // Interrupt-on-change enabled on A4
    PORTA;  // Read RA4 to clear mismatch
    RABIF = 0;
    heartbeat_cycles = 0;
#endif
}

/**
 * Check power status.
 * 
 * The livolo has a sort of heartbeat mechanism that pulsates on RA4 at 50 Hz
 * while it's powered on. As soon as AC goes down, the big capacitor keeps
 * powering the PIC for just enough time to turn the relays off.
 */
void
heartbeat_update(void)
{
#ifdef TIME_TO_SHUTDOWN
    if (RABIF) {
        PORTA;  // Read RA4 to clear mismatch
        RABIF = 0;  // Clear IF
        heartbeat_cycles = 0;
    } else {
        if (heartbeat_cycles <= TIME_TO_SHUTDOWN) {
            heartbeat_cycles++;
        }
    }
#endif
}

bit
heartbeat_outage(void)
{
#ifdef TIME_TO_SHUTDOWN
    return heartbeat_cycles > TIME_TO_SHUTDOWN;
#else
    return 0;
#endif
}
