#ifndef CAPSENSOR_H
#define	CAPSENSOR_H

#include <stdint.h>
#include "config.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
uint16_t capsensor_freq;
uint16_t capsensor_rolling_avg;
uint16_t capsensor_frozen_avg;
bit capsensor_status;


/*
 * Public functions
 */    
void capsensor_init(void);
bit capsensor_is_button_pressed(void);


#ifdef	__cplusplus
}
#endif

#endif	/* CAPSENSOR_H */

