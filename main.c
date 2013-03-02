#include "ossi_obc.h"

/*
 * main.c
 */

int main(void)
{
	// Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    clock_setup();
    clock_dividerSetup(MCLK_DIVIDED_BY_1, SMCLK_DIVIDED_BY_1, ACLK_DIVIDED_BY_1);

    // LED Test
	P1OUT &= ~(LED_PIN+EXT_WDT_PIN);
	P1DIR |= (LED_PIN +EXT_WDT_PIN);

	uart_start();

	// External Interface 485 TX DE
	P5OUT &= ~RS485_DE_PIN;
	P5DIR |= RS485_DE_PIN;

	P4OUT &= ~(COMMS_OFF_PIN + BEACON_OFF_PIN + LED_OFF_PIN + SCOUT_EN_OUT_PIN + SCCHG_EN_OUT_PIN +ANT_DEPLOY2_PIN +ANT_DEPLOY1_PIN+VEXT_OFF_PIN );
	P4DIR |= (COMMS_OFF_PIN + BEACON_OFF_PIN + LED_OFF_PIN + SCOUT_EN_OUT_PIN + SCCHG_EN_OUT_PIN +ANT_DEPLOY2_PIN +ANT_DEPLOY1_PIN+VEXT_OFF_PIN);


	P3OUT &= ~OBC_I2C_OFF_PIN;
	P3DIR |= OBC_I2C_OFF_PIN;

	P2OUT |= IO_OE_PIN;
	P2DIR |= IO_OE_PIN;
	char readChar;


	P1OUT |= (LED_PIN);

    while(1)
    {

//    	P1OUT ^= (LED_PIN+EXT_WDT_PIN);
//
//    	__delay_cycles(1000000);

    	if(uart_available())
    	{
    		readChar = uart_getc();

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
					uart_puts("SCPPOn\r\n");
					__delay_cycles(500000);
					P5OUT &= ~RS485_DE_PIN;
				}
				else
				{
					P5OUT |= RS485_DE_PIN;
					__delay_cycles(500000);
					uart_puts("SCPPOff\r\n");
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
					uart_puts("Conduct\r\n");
					__delay_cycles(500000);
					P5OUT &= ~RS485_DE_PIN;
				}
				else
				{
					P5OUT |= RS485_DE_PIN;
					__delay_cycles(500000);
					uart_puts("NotConduct\r\n");
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
    			P4OUT ^= ANT_DEPLOY1_PIN;
    		}

    		if (readChar == 'v')
    		{
    			P4OUT ^= ANT_DEPLOY2_PIN;
    		}
    	}



//    	if(uart_available())
//    	{
//    		readChar = uart_getc();
//
//    		if (readChar == 'a')
//    		{
//    			P1OUT ^= LED_PIN;
//    			P5OUT |= RS485_DE_PIN;
//    			uart_puts("LEDToggle\r\n");
//    			__delay_cycles(100000);
//    			__delay_cycles(100000);
//    			__delay_cycles(100000);
//    			P5OUT &= ~RS485_DE_PIN;
//    		}
//
//    		if (readChar == 's')
//    		{
//    			P4OUT ^= (COMMS_OFF_PIN + BEACON_OFF_PIN + LED_OFF_PIN + SCOUT_EN_OUT_PIN + SCCHG_EN_OUT_PIN+ANT_DEPLOY2_PIN +ANT_DEPLOY1_PIN);
//    			P5OUT |= RS485_DE_PIN;
//				uart_puts("OFFpinToggleSupercapToggle\r\n");
//				__delay_cycles(100000);
//				__delay_cycles(100000);
//				__delay_cycles(100000);
//				P5OUT &= ~RS485_DE_PIN;
//    		}
//
//    		if (readChar =='d')
//    		{
//    			if(P2IN & COMMS_FAULT_IN_PIN)
//    			{
//    				P1OUT |= LED_PIN;
//    			}
//    			else
//    			{
//    				P1OUT &= ~LED_PIN;
//    			}
//    		}
//
//    		if (readChar =='f')
//    		{
//    			P3OUT ^= OBC_I2C_OFF_PIN;
//    		}
//
//    		if(readChar == 'z')
//    		{
//    			// buffer enable
//    			P2OUT &= ~IO_OE_PIN;
//
//    			if(P2IN & SOLAR_ON_IN_PIN)
//    			{
//    				P1OUT |= LED_PIN;
//    			}
//    			else
//    			{
//    				P1OUT &= ~LED_PIN;
//    			}
//    		}
//
//
//
//    	}


    }
}

