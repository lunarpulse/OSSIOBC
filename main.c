#include "ossi_obc.h"

// 3 Hours = 10800 secs but we update the bootTime every 10 secs
#define BOOT_TIME (1080)

/*
 * main.c
 */

parmOperationMode obc_mode;

int main(void)
{
	// Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // 1 MHz DCO
    clock_setup();
    clock_dividerSetup(MCLK_DIVIDED_BY_1, SMCLK_DIVIDED_BY_1, ACLK_DIVIDED_BY_1);

    P1OUT &= ~(LED_PIN + EXT_WDT_PIN);
    P1DIR |= (LED_PIN + EXT_WDT_PIN);

	P4OUT &= ~(COMMS_OFF_PIN + BEACON_OFF_PIN + LED_OFF_PIN + SCOUT_EN_OUT_PIN + SCCHG_EN_OUT_PIN +ANT_DEPLOY2_PIN +ANT_DEPLOY1_PIN+VEXT_OFF_PIN );
	P4DIR |= (COMMS_OFF_PIN + BEACON_OFF_PIN + LED_OFF_PIN + SCOUT_EN_OUT_PIN + SCCHG_EN_OUT_PIN +ANT_DEPLOY2_PIN +ANT_DEPLOY1_PIN+VEXT_OFF_PIN);


	P3OUT &= ~OBC_I2C_OFF_PIN;
	P3DIR |= OBC_I2C_OFF_PIN;

	P2OUT |= IO_OE_PIN;
	P2DIR |= IO_OE_PIN;

	// init external interface
	interface_init();
	_EINT();


	uint8_t bootTime[2];
	obc_mode = BOOT_MODE;

    while ( obc_mode == BOOT_MODE)
    {

    	volatile uint8_t i;

		flash_readData(0,2,bootTime);
		uint16_t currentBootTime =  (bootTime[0] << 8) | bootTime[1];

		// init the memory for the first time
		if(currentBootTime == 0xFFFF)
		{
			bootTime[0] = 0;
			bootTime[1] = 0;
			flash_writeBegin(FLASH_SMCLK,2);
			flash_writeData(0,2,bootTime);
			flash_writeEnd();
			currentBootTime = 0;
		}

		if(currentBootTime  > BOOT_TIME)
		{
			P1OUT &= ~LED_PIN;
			obc_mode = DEPLOY_MODE;
			break;
		}

    	// 10 seconds delay
    	for (i = 0; i < 10 ; i++)
    	{
    		// blink LED every second
    		P1OUT |= (LED_PIN + EXT_WDT_PIN);
    		delay_ms(500);
    		P1OUT &= ~(LED_PIN + EXT_WDT_PIN);
    		delay_ms(500);
    	}

		currentBootTime += 1;

		bootTime[0] = (currentBootTime >> 8) & 0xFF;
		bootTime[1] = currentBootTime & 0xFF;
		flash_writeBegin(FLASH_SMCLK,2);
		flash_writeData(0,2,bootTime);
		flash_writeEnd();
		_NOP(); // for debug purpose
    	interface_check();
    }


    while(1)
    {
    	switch (obc_mode)
    	{
    		case DEPLOY_MODE:
    			// deploy antenna

    			obc_mode = NORMAL_MODE;
    			break;
    		case NORMAL_MODE:
    			// send i2c message to beacon
    			break;
    		default:
    			break;
    	}

    	interface_check();

    }
}

