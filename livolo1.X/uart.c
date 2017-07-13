#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "config.h"

#define MY_SPBRG ((_XTAL_FREQ / BAUD_RATE / 4) - 1)

extern void uart_bitbang_write_57600(uint8_t byte);


void 
putch(char byte)
{
#ifdef UART_TX_BITBANG_ON_RB4
    uart_bitbang_write_57600(byte);
#else
    uart_write(byte);
#endif
}

#ifdef UART_TX_BITBANG_ON_RB4
void uart_init()
{
    TRISB5 = 1; // RX in
    TRISB4 = 0; // TX (bitbang) out

    SPBRGH = MY_SPBRG >> 8;
    SPBRG  = MY_SPBRG & 0xff; // set baud rate
    BRGH = 1; // use x64 baud scaler
    BRG16 = 1; // use 16 bit baud generator
    CREN = 1; // enable continuous RX
    SYNC = 0; // async (not needed)
    SPEN = 1; // enable UART
}
#else
void uart_init()
{
    TRISB5 = 1; // RX in
    TRISB7 = 1; // TX in (yes)

    SPBRGH = MY_SPBRG >> 8;
    SPBRG  = MY_SPBRG & 0xff; // set baud rate
    BRGH = 1; // use x64 baud scaler
    BRG16 = 1; // use 16 bit baud generator
    TXEN = 1; // enable TX  
    CREN = 1; // enable continuous RX
    SYNC = 0; // async (not needed)
    SPEN = 1; // enable UART
}
#endif

bit uart_data_ready()
{
    return RCIF;
}

#ifndef UART_TX_BITBANG_ON_RB4
void uart_write(char data)
{
    while (!TRMT) ;
    TXREG = data;
}
#endif

uint8_t uart_read()
{
    if (OERR) {
        puts("OVF\r");
        CREN = 0;
        CREN = 1;
    }

    while (!RCIF) ;
    return RCREG;
}

