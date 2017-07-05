/* 
 * File:   uart.h
 * Author: javi
 *
 * Created on July 2, 2017, 1:36 PM
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif

uint8_t uart_init2(uint32_t baudrate);
void uart_init();
uint8_t uart_data_ready();
uint8_t uart_tx_empty();
void uart_write(char data);
uint8_t uart_read();
void uart_write_text(uint8_t *buf);
void uart_read_text(uint8_t *buf, uint8_t len);


#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

