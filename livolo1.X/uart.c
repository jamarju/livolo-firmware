#include <xc.h>
#include <stdint.h>
#include "uart.h"
#include "config.h"

#define MY_SPBRG ((_XTAL_FREQ / BAUD_RATE / 4) - 1)

void putch(uint8_t byte)
{
//    TXREG = byte;
//    while (!TXIF) ;
//    TXIF = 0;
    while (!TRMT) ;
    TXREG = byte;
}

void uart_init()
{
    TRISB5 = 1; // RX in
    TRISB7 = 1; // TX in (yes)
    ANS11 = 0; // do not analog on RX pin

    SPBRGH = MY_SPBRG >> 8;
    SPBRG  = MY_SPBRG & 0xff; // set baud rate
    BRGH = 1; // use x64 baud scaler
    BRG16 = 1; 
    TXEN = 1; // enable TX  
    CREN = 1; // enable continuous RX
    SYNC = 0; // async (not needed)
    SPEN = 1; // enable UART
}

uint8_t uart_init2(uint32_t baudrate)
{
    int16_t x;
    x = (_XTAL_FREQ - baudrate * 64) / baudrate / 64;   //SPBRG for Low Baud Rate
    if (x > 255) {                                      //If High Baud Rage Required
        x = (_XTAL_FREQ - baudrate * 16) / baudrate / 16; //SPBRG for High Baud Rate
        BRGH = 1;                                     //Setting High Baud Rate
    }
    if (x < 256) {
        SPBRG = x;
        TRISB5 = 1;
        TRISB7 = 1;
        ANS11 = 0;
        SYNC = 0; // asynchronous mode, ie UART
        CREN = 1; // enable continuous reception
        TXEN = 1; // enable transmission
        SPEN = 1; // enable serial
        return 1; // ok
    }
    return 0; // not ok
}

uint8_t uart_data_ready()
{
    return RCIF;
}

uint8_t uart_tx_empty()
{
    return TRMT;
}

void uart_write(char data)
{
    while (!TRMT) ;
    TXREG = data;
}   

uint8_t uart_read()
{
    while (!RCIF) ;
    return RCREG;
}

void uart_write_text(uint8_t *buf)
{
    while (*buf) {
        uart_write(*buf);
        buf++;
    }
}

void uart_read_text(uint8_t *buf, uint8_t len)
{
    for (int i = 0; i < len; i++)
        buf[i] = uart_read();
}
