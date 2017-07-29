#ifndef CAPSENSOR_H
#define	CAPSENSOR_H

#include <stdint.h>
#include "config.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
uint16_t cap_raw;
uint16_t cap_rolling_avg;
uint16_t cap_frozen_avg;
uint8_t cap_cycles;


/*
 * Public functions
 */    
void capsensor_init(void);
bit capsensor_is_button_pressed(void);


#ifdef	__cplusplus
}
#endif

#endif	/* CAPSENSOR_H */

