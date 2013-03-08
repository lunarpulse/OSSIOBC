#ifndef UART_H_
#define UART_H_

#include "ossi_1611.h"

#define UART_TXD0_PIN	(BIT4)
#define UART_RXD0_PIN	(BIT5)
#define UART_TXD1_PIN	(BIT6)
#define UART_RXD1_PIN	(BIT7)

void uart1_init(void);
void uart1_start(void);
char uart1_getc(void);
uint8_t uart1_available();
void putc(unsigned);
void puts(char *);

#endif
