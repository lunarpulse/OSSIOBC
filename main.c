#include "ossi_obc.h"

// 3 Hours = 10800 secs but we update the bootTime every 10 secs
#define BOOT_TIME (1080)

/*
 * main.c
 */
volatile uint8_t sendCnt;
volatile uint16_t wakeUpCnt;
volatile uint8_t superCapChargeTime;
volatile uint8_t ledOnTime;
#define superCapChargePeriod (270) // 90 mins = 5400 sec = 270 *20(wakeup period)
//#define superCapChargePeriod (2) // testing
#define tipOffChargeTime	(15) // 5 mins
#define ledOnTimeDuration (6) // 60 sec = 3 *20(wakeup period)

int main(void)
{
	// Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // 1 MHz DCO
    clock_setup();
    clock_dividerSetup(MCLK_DIVIDED_BY_1, SMCLK_DIVIDED_BY_1, ACLK_DIVIDED_BY_1);

    // TODO: Init All IOs!!!!!!!
    P1OUT &= ~(LED_PIN + EXT_WDT_PIN);
    P1DIR |= (LED_PIN + EXT_WDT_PIN);

	P4OUT &= ~(COMMS_OFF_PIN + BEACON_OFF_PIN + LED_OFF_PIN + SCOUT_EN_OUT_PIN + SCCHG_EN_OUT_PIN +ANT_DEPLOY2_PIN +ANT_DEPLOY1_PIN+VEXT_OFF_PIN );
	P4DIR |= (COMMS_OFF_PIN + BEACON_OFF_PIN + LED_OFF_PIN + SCOUT_EN_OUT_PIN + SCCHG_EN_OUT_PIN +ANT_DEPLOY2_PIN +ANT_DEPLOY1_PIN+VEXT_OFF_PIN);

	// adc mux port init
	P5OUT &= ~(MUX_A0_PIN + MUX_A1_PIN + MUX_A2_PIN);
	P5DIR |= MUX_A0_PIN + MUX_A1_PIN + MUX_A2_PIN;

	P3OUT |= I2C_RST_PIN; // default high
	P3OUT &= ~OBC_I2C_OFF_PIN;
	P3DIR |= (I2C_RST_PIN + OBC_I2C_OFF_PIN);

	P2OUT |= IO_OE_PIN;
	P2DIR |= IO_OE_PIN;


	P1DIR &= ~(SC_PFO_IN_PIN + BAT_FAULT_IN_PIN + BAT_CHRG_IN_PIN + SC_ON_IN_PIN + BAT_ON_IN_PIN);
	P2DIR &= ~(SOLAR_ON_IN_PIN + COMMS_FAULT_IN_PIN + BEACON_FAULT_IN_PIN + LED_FAULT_IN_PIN);

	// ADC
	// ADC MUX
	// default 0 / 0 / 0
	// ADC12 setup
	adc12_portSetup(ADC0_PIN + ADC1_PIN);
	adc12_init(ADC12_CLOCKSOURCE_SMCLK, ADC12_CLOCKDIVIDER_8, ADC12_CYCLEHOLD_16_CYCLES);
	adc12_setVolReference(ADC12_REF_VCC_VSS); 	// Vref = VCC

	// I2C setup
	i2c_portSetup();



	// init external interface
	interface_init();


	_EINT();

	uint8_t bootTime[2];
	obc_mode = BOOT_MODE;
	//obc_mode = NORMAL_MODE;

	P4OUT |= (COMMS_OFF_PIN + LED_OFF_PIN);

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
		// All module turn off during boot mode
		P4OUT |= (COMMS_OFF_PIN + BEACON_OFF_PIN + LED_OFF_PIN);

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
    	interface_check(); // to take care of external interface while in BOOT_MODE
    }

    sendCnt = 0;
    wakeUpCnt = 0;
    systimer_init(TIMER_B_ACLK, TIMER_B_DIVIDED_BY_1, TIMER_B_UP_MODE, 33, 32763);
    systimer_start();
    superCapChargeTime = 0;
    ledOnTime = 0;


    while(1)
    {

    	switch (obc_mode)
    	{
    		case DEPLOY_MODE:
    			// deploy antenna
    			P4OUT |= ANT_DEPLOY2_PIN;
    			delay_sec(5);
    			P4OUT |= ANT_DEPLOY1_PIN;
    			delay_sec(5);
    			P4OUT &= ~(ANT_DEPLOY2_PIN + ANT_DEPLOY1_PIN);
    			P4OUT &= ~(COMMS_OFF_PIN + BEACON_OFF_PIN);
    			obc_mode = NORMAL_MODE;
    			break;
    		case NORMAL_MODE:
    			// acquire data
    			// send i2c message to beacon
    			wakeUpCnt++;
    			obc_dataAcquire();
    			mux_setChannel(MUX_BEACON_CHANNEL);
    			obc_sendDataToBeacon();
    			sendCnt++;
    			if (sendCnt  > 11)
    			{
    				sendCnt = 0;
					obc_sendCmd(BEACON_ADDR,BEACON_CMD1_ADDR, MORSE_SEND_START);
					interface_txEnable();
					printf("OBC commanded Beacon\r\n");
					interface_txDisable();
					uint8_t rxdata[1];
					rxdata[0]= 0;
					i2c_masterRead(BEACON_ADDR,1,rxdata);

					if(rxdata[0] == BEACON_CMD1_CLEAR || rxdata[0] == SENDING || rxdata[0] == SENT)
					{

						interface_txEnable();
						printf("BEACON FINE\r\n");
						interface_txDisable();
					}
					else if(rxdata[0] == BEACON_STANDALONE)

					{
						interface_txEnable();
						printf("BEACON STANDALONE\r\n");
						interface_txDisable();
					}
					else
					{
						interface_txEnable();
						printf("NOT OK\r\n");
						interface_txDisable();
					}
    			}

    			if (wakeUpCnt >= superCapChargePeriod)
    			{

    				obc_supercapCharge();
    				// if supercap v is >= 3.8V
    				// wait 20 secs more and LED ON
    				volatile uint16_t adc_vsupercap;
    				adg708_setChannel(MUX_VSUPERCAP);
    				adc_vsupercap = adc12_readChannel(1) * 0.8056 * 2;

    				interface_txEnable();
					printf("SuperCap Charging ...... \r\n");
					printf("Wake Up Count: %u\r\n", wakeUpCnt);
					printf("Super Cap Voltage: %u mV\r\n", adc_vsupercap);
					interface_txDisable();

    				if ( adc_vsupercap >= 3500)
    				{
    					if ( wakeUpCnt >= superCapChargePeriod + tipOffChargeTime)
    					{
    						obc_ledOn();
    					}
    				}

    				if ( wakeUpCnt >= superCapChargePeriod + tipOffChargeTime +ledOnTimeDuration)
    				{
    					wakeUpCnt = 0;
    					obc_ledOff();
    				}

    			}
    			else
    			{
    				obc_ledOff();
    			}

    			break;
    		default:
    			break;
    	}
    	// when using LPM, let uart wake up the OBC!!!
		systimer_setWakeUpPeriod(20); // wake up every 20sec
		systimer_startWakeUpPeriod();
    	__bis_SR_register(LPM3_bits + GIE);
    	P1OUT |= (LED_PIN + EXT_WDT_PIN);
    	delay_ms(10);
    	P1OUT &= ~(LED_PIN + EXT_WDT_PIN);
    	interface_check();
    }
}

