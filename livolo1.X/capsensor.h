/* 
 * File:   capsensor.h
 * Author: javi
 *
 * Created on July 3, 2017, 12:29 PM
 */

#ifndef CAPSENSOR_H
#define	CAPSENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif

void capsensor_init(void);
bit capsensor_is_button_pressed(void);
bit capsensor_is_ready_for_another(void);
inline uint16_t capsensor_get_rolling_average(void);



#ifdef	__cplusplus
}
#endif

#endif	/* CAPSENSOR_H */

