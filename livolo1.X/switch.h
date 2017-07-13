#ifndef SWITCH_H
#define	SWITCH_H

#include <xc.h>

#ifdef	__cplusplus
extern "C" {
#endif

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

