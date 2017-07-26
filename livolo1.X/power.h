#ifndef POWER_H
#define	POWER_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define POWER_OUTAGE    0
#define POWER_OK        1

bit power_status;

void power_preinit(void);
bit power_read(void);


#ifdef	__cplusplus
}
#endif

#endif	/* POWER_H */

