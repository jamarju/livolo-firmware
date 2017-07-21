#ifndef CAPSENSOR_H
#define	CAPSENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif
    
uint16_t capsensor_freq;
uint16_t capsensor_rolling_avg;
uint16_t capsensor_rolling_avg_on_trip;
uint8_t capsensor_status;

void capsensor_init(void);
bit capsensor_is_button_pressed(void);
bit capsensor_is_ready_for_another(void);


#ifdef	__cplusplus
}
#endif

#endif	/* CAPSENSOR_H */

