/*
 * obc_interface.c
 *
 *  Created on: 2013. 3. 11.
 *      Author: OSSI
 */


#include "obc_interface.h"

void interface_init(void)
{
	// External Interface 485 TX DE
	P5OUT &= ~RS485_DE_PIN;
	P5DIR |= RS485_DE_PIN;
	uart1_init();
	uart1_start();
}


void interface_check(void)
{
	char readChar;
	if(uart1_available())
	{
		readChar = uart1_getc();

		if (readChar =='r')
		{
			uint8_t resetBootTime[2];
			resetBootTime[0] = 0;
			resetBootTime[1] = 0;
			flash_writeBegin(FLASH_SMCLK,2);
			flash_writeData(0,2,resetBootTime);
			flash_writeEnd();
			P5OUT |= RS485_DE_PIN;
			delay_ms(500);
			printf("Boot Time Reset Completed\r\n");
			delay_ms(500);
			P5OUT &= ~RS485_DE_PIN;

		}

		if(readChar == ' ')
		{
			// printout all the status
			IE2 &= ~URXIE1;
			P5OUT |= RS485_DE_PIN;
			delay_sec(2);

			puts("12345678999999999999999999999999999");
			puts("123456789\r\n");
			puts("123456789\r\n");
			puts("123456789\r\n");
			puts("123456789\r\n");
			puts("123456789\r\n");
			puts("123456789\r\n");
			puts("123456789\r\n");
			puts("123456789\r\n");

			printf("Test1longlong2long3long4\r\n");
			printf("Test2\r\n");
			printf("Test3\r\n");
			printf("Test4\r\n");
			printf("Test5\r\n");
			printf("Test6\r\n");
			printf("Test7\r\n");
			printf("Test8\r\n");
			printf("Test9\r\n");
			printf("Test10\r\n");
			printf("Test11\r\n");
			printf("Test12\r\n");
			printf("Test13\r\n");
			printf("Test14\r\n");
			delay_sec(4);
			P5OUT &= ~RS485_DE_PIN;
			IE2 |= URXIE1;

		}

		if (readChar =='a')
		{
			// supercap powerpath connect
			P4OUT ^= SCOUT_EN_OUT_PIN;


		}
		if(readChar =='s')
		{
			// buffer enable
			P2OUT &= ~IO_OE_PIN;

			if(P1IN & SC_ON_IN_PIN)
			{
				P5OUT |= RS485_DE_PIN;
				__delay_cycles(500000);
				printf("SCPPOn\r\n");
				__delay_cycles(500000);
				P5OUT &= ~RS485_DE_PIN;
			}
			else
			{
				P5OUT |= RS485_DE_PIN;
				__delay_cycles(500000);
				printf("SCPPOff\r\n");
				__delay_cycles(500000);
				P5OUT &= ~RS485_DE_PIN;
			}
			P2OUT |= IO_OE_PIN;
		}


		if (readChar=='d')
		{

			// supercap charge enable
			P4OUT ^= SCCHG_EN_OUT_PIN;


		}

		if(readChar =='q')
		{
//    			 buffer enable
			P2OUT &= ~IO_OE_PIN;

			if(P1IN & BAT_ON_IN_PIN)
			{
				P5OUT |= RS485_DE_PIN;
				__delay_cycles(500000);
				printf("Conduct\r\n");
				__delay_cycles(500000);
				P5OUT &= ~RS485_DE_PIN;
			}
			else
			{
				P5OUT |= RS485_DE_PIN;
				__delay_cycles(500000);
				printf("NotConduct\r\n");
				__delay_cycles(500000);
				P5OUT &= ~RS485_DE_PIN;
			}

			P2OUT |= IO_OE_PIN;
		}

		if(readChar == 'z')
		{
			P4OUT ^= VEXT_OFF_PIN;
		}

		if (readChar == 'c')
		{
			P4OUT ^= LED_OFF_PIN;
		}


	}

}
