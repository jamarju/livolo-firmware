#ifndef UART_H
#define	UART_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

void uart_init();
bit uart_data_ready();
uint8_t uart_read();


#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

