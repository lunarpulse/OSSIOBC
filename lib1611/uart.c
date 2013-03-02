#include "uart.h"

static char uart_buffer[255];

uint8_t step = 0;
int read = 0;
uint8_t sending = 0;

void uart_start() {
      P3SEL |= 0xC0;                            // P3.6,7 = USART1 TXD/RXD
	  UCTL1 |= SWRST;                          // Initialize USART state machine
	  UCTL1 |= CHAR + SWRST;
	  UTCTL1 |= SSEL0;
	  ME2 |= UTXE1 + URXE1;                     // Enable USART1 TXD/RXD
      UBR01 = 0x03;
      UBR11 = 0x00;
      UMCTL1 = 0x4A;
	  UCTL1 &= ~SWRST;                          // Initialize USART state machine          
	  IE2 |= URXIE1;                            // Enable USART0 RX interrupt
}

uint8_t uart_available() {
    return (read >= 1);
}

char uart_getc() {
    read--;
    return uart_buffer[step-1];
}
void uart_putc(char c)
{
    while (!(IFG2 & UTXIFG1));                // USART1 TX buffer ready?
    TXBUF1 = c;                          // RXBUF1 to TXBUF1
}

char uart_puts(char* buf) {
    char c;
    while(c = *buf++) {
        while (!(IFG2 & UTXIFG1));                // USART1 TX buffer ready?
        TXBUF1 = c;                          // RXBUF1 to TXBUF1
    }
}

#ifndef TEST
#pragma vector=USART1RX_VECTOR
__interrupt
#endif
void usart1_rx (void)
{
  /* while (!(IFG2 & UTXIFG1));                // USART1 TX buffer ready? */
  /* TXBUF1 = RXBUF1;                          // RXBUF1 to TXBUF1 */

  if (step >255) step = 0;

  uart_buffer[step] = RXBUF1;
  _EINT();
  step++;
  read++;
}
