/* 
 * File:   switch.h
 * Author: javi
 *
 * Created on July 6, 2017, 1:15 AM
 */

#ifndef SWITCH_H
#define	SWITCH_H

#ifdef	__cplusplus
extern "C" {
#endif

void switch_preinit();
void switch_init();
void switch_toggle();
inline bit switch_is_on();


#ifdef	__cplusplus
}
#endif

#endif	/* SWITCH_H */

