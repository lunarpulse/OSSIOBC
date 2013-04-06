/*
 * obc_interface.c
 *
 *  Created on: 2013. 3. 11.
 *      Author: OSSI
 */


#include "obc_interface.h"

void begin_report(void)
{
	IE2 &= ~URXIE1;
	P5OUT |= RS485_DE_PIN;
	delay_ms(10);
	printf("\r\n");
	printf("******************************\r\n");
	printf("[OSSI-1 Satellite Report Begin]\r\n");
	printf("\r\n");
}

void end_report(void)
{
	printf("\r\n");
	printf("[OSSI-1 Satellite Report End]\r\n");
	printf("******************************\r\n");
	printf("\r\n");
	delay_ms(10);
	P5OUT &= ~RS485_DE_PIN;
	IE2 |= URXIE1;
}


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
			begin_report();
			printf("Boot Time Reset Completed\r\n");
			end_report();
		}

		if(readChar == ' ')
		{
			// printout all the status

			// get current boot time
			uint8_t bootTime[2];
			flash_readData(0,2,bootTime);
			uint16_t currentBootTime =  (bootTime[0] << 8) | bootTime[1];

			// test i2c communication with modules

			// get total current consumption

			// get battery voltage

			begin_report();
			printf("- Current Boot Time in Sec: %u sec\r\n",currentBootTime * 10);
			end_report();

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
			P4OUT ^= ANT_DEPLOY2_PIN;
		}

		if (readChar == 'v')
		{
			P4OUT ^= ANT_DEPLOY1_PIN;
		}

	}

}
