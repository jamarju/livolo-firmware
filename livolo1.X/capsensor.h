#ifndef CAPSENSOR_H
#define	CAPSENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif

void capsensor_init(void);
bit capsensor_is_button_pressed(void);
bit capsensor_is_ready_for_another(void);


#ifdef	__cplusplus
}
#endif

#endif	/* CAPSENSOR_H */

