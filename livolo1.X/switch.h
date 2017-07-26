#ifndef SWITCH_H
#define	SWITCH_H

#include <xc.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define SWITCH_OFF  0
#define SWITCH_ON   1


bit switch_status;

void switch_preinit();
void switch_init();
void switch_toggle();
inline bit switch_is_on();
void switch_on(void);
void switch_off(void);


#ifdef	__cplusplus
}
#endif

#endif	/* SWITCH_H */

