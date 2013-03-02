#ifndef UART_H_
#define UART_H_

#include "ossi_obc.h"
#include  <msp430f1611.h>

void uart_start(void);
char uart_getc(void);
uint8_t uart_available();
void uart_putc(char c);
char uart_puts(char* buf);

#endif
