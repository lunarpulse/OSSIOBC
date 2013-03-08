#include "uart.h"

static char uart1_rxBuffer[32];

uint8_t step = 0;
uint16_t rx1Cnt = 0;
uint8_t sending = 0;

void uart1_init(void)
{
	P3SEL |= (UART_TXD1_PIN + UART_RXD1_PIN);
	UCTL1 |= SWRST;                          // Initialize USART state machine
	UCTL1 |= CHAR + SWRST;
	UTCTL1 |= SSEL0;						// ACLK
	ME2 |= UTXE1 + URXE1;                     // Enable USART1 TXD/RXD
	UBR01 = 0x03;
	UBR11 = 0x00;
	UMCTL1 = 0x4A;
	UCTL1 &= ~SWRST;                          // Initialize USART state machine
}

void uart1_start(void)
{
	  IE2 |= URXIE1;                            // Enable USART0 RX interrupt
}


void uart1_sendByte(unsigned char byte)
{
	while (!(IFG2 & UTXIFG1));                // USART1 TX buffer ready?
	TXBUF1 = byte;                          // RXBUF1 to TXBUF1
}

void putc(unsigned b)
{
    uart1_sendByte(b);
}

void puts(char* s) {
    char c;
    while(c = *s++) {
    	uart1_sendByte(c);
    }
}

uint8_t uart1_available() {
    return (rx1Cnt >= 1);
}

char uart1_getc() {
    rx1Cnt--;
    return uart1_rxBuffer[step-1];
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

  uart1_rxBuffer[step] = RXBUF1;
//  _EINT(); // Bad habit!
  step++;
  rx1Cnt++;
}
