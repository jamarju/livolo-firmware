#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "config.h"

extern void uart_bitbang_write_57600(uint8_t byte);


void 
putch(char byte)
{
    uart_bitbang_write_57600(byte);
}
