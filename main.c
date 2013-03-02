#include "ossi_obc.h"


/*
 * main.c
 */

uint8_t RxData[8] = {0};
uint8_t obcData[64]={0};
uint8_t ossiData[64]={0};

uint8_t obc_i2cStatus;
statusData_t statusData;
viData_t viData;
tempData_t tempData;
moduleData_t moduleData;
ledTimeData_t ledTimeData;

// ossi data

void obc_dataProcessing(void);
uint8_t obc_tempSensorRead(void);

volatile uint8_t sendCnt;

int main(void)
{
	// Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;
//
    clock_setup();
    uint8_t systemClock;
    systemClock = clock_getMode();
    if(systemClock == CLOCK_XT2_LFXT1)
    {
    	clock_dividerSetup(MCLK_DIVIDED_BY_1, SMCLK_DIVIDED_BY_1, ACLK_DIVIDED_BY_1);
    }
    else if(systemClock == CLOCK_DCO_LFXT1)
    {
    	clock_dividerSetup(MCLK_DIVIDED_BY_1, SMCLK_DIVIDED_BY_1, ACLK_DIVIDED_BY_1);
    }
    // set clock related peripherals based one clock mode and divider

    // change divider when peripheral need slower clock

    /*
    // Olimex Board LED For testing
    //P6DIR |= BIT0 + BIT1+BIT2+BIT3+BIT4;   // P6.0  p6.1 2 3 4 output
    P6OUT |= BIT0;                           	// timeout
	  //P6OUT |= BIT0;                           	// command test
    //P6OUT &= ~BIT1;								// timeout stop
    //P6OUT &= ~BIT2;								// arbitration lost
	  //P6OUT &= ~BIT3;							// in slave mode
    //P6OUT &= ~BIT4;								// Access ready int check
    //P6OUT &= ~BIT3;								// General Call Detect
     */
    P6OUT |= BIT0;                           	// timeout
    P6DIR |= BIT0;
    i2c_portSetup();

//	systimer_init(TIMER_B_ACLK, TIMER_B_DIVIDED_BY_1, TIMER_B_UP_MODE, 33, 32763);
//	systimer_start();
//	systimer_setWakeUpPeriod(240); // wake up every 4 mins
//	systimer_startWakeUpPeriod();



	log_init();
//  	obc_rtcInit();
  	P1DIR |= BIT1;

  	adc12_portSetup(ADC12_PIN_6_1+ADC12_PIN_6_2+ADC12_PIN_6_3+ ADC12_PIN_6_4+ADC12_PIN_6_5);
  	adc12_init(ADC12_CLOCKSOURCE_SMCLK, ADC12_CLOCKDIVIDER_8, ADC12_CYCLEHOLD_16_CYCLES);

    // make sure you enable global interrupts before you go into while loop
    _EINT();

    // after global interrupt is enabled
    // task scheduler should take care of messages from master when
    // i2c ISR is generated in Slave mode
    // i2c_slaveInit(0x50, 64, ossiData);
    // i2c_slaveStart();
    sendCnt = 0;

    log(I2C_SERVICE, NOTICE_LEVEL, "Default I2C Register Value");
    log_withNum(I2C_SERVICE, NOTICE_LEVEL,"U0CTL",U0CTL);
    log_withNum(I2C_SERVICE, NOTICE_LEVEL,"I2CDCTL",I2CDCTL);
    log_withNum(I2C_SERVICE, NOTICE_LEVEL,"I2CIFG",I2CIFG);

    while(1)
    {
    	systimer_stop();
    	systimer_stopWakeUpPeriod();
      // RTC TEST

//      if(obc_rtcTest() == 0)
//      {
//    		obc_i2cStatus = i2c_getMasterStatus();
//    	    log_withNum(I2C_SERVICE, ERROR_LEVEL, "RTC Read Error: ", obc_i2cStatus);
//      }
//
//      __delay_cycles(1000000);
//      log_printAllWithNum();
//
//      // RTC END
//
//       // ADC Test
//       if(obc_adcTest() == 0)
//       {
//    	   obc_i2cStatus = i2c_getMasterStatus();
//    	   log_withNum(ADC_SERVICE, ERROR_LEVEL, "ADC Error: ", obc_i2cStatus);
//       }

       /*
      // EEPROM TEST
      if(obc_eepromTest() == 0)
      {
    		obc_i2cStatus = i2c_getMasterStatus();
      }
      */

//      // TEMP Sensor TEST
//    	tempData_t tempData;
    	volatile uint8_t i;

    	for (i = 0 ; i < STATUS_DATA_SIZE ; i++)
    	{
    		statusData.data[i] = i;
    	}

    	for (i = 0 ; i < VI_DATA_SIZE ; i++)
    	{
    		viData.data[i] = i;
    	}

    	for (i = 0 ; i < TEMP_DATA_SIZE ; i++)
    	{
    		tempData.data[i] = i;
    	}

    	for (i = 0 ; i < MODULE_DATA_SIZE ; i++)
    	{
    		moduleData.data[i] = i;
    	}

    	for (i = 0 ; i < LEDTIME_DATA_SIZE ; i++)
    	{
    		ledTimeData.data[i] = i;
    	}

    	volatile uint8_t result;

    	log_saveWithNum(I2CDCTL);
    	log_saveWithNum(I2CIFG);
    	result = obc_sendData(BEACON_ADDR, STATUS_DATA_ADDR, STATUS_DATA_SIZE, statusData.data);
    	if (result == ERROR){log_withNum(I2C_SERVICE, ERROR_LEVEL,"status",i2c_getMasterStatus());}
    	else {log_saveWithNum(I2CIFG);}

    	log_saveWithNum(I2CDCTL);
    	log_saveWithNum(I2CIFG);
    	result = obc_sendData(BEACON_ADDR, VI_DATA_ADDR, VI_DATA_SIZE, viData.data);
    	if (result == ERROR){log_withNum(I2C_SERVICE, ERROR_LEVEL,"vi",i2c_getMasterStatus());}
    	else {log_saveWithNum(I2CIFG);}

    	log_saveWithNum(I2CDCTL);
    	log_saveWithNum(I2CIFG);
    	result = obc_sendData(BEACON_ADDR, TEMP_DATA_ADDR, TEMP_DATA_SIZE, tempData.data);
    	if (result == ERROR){log_withNum(I2C_SERVICE, ERROR_LEVEL,"temp",i2c_getMasterStatus());}
    	else {log_saveWithNum(I2CIFG);}

    	log_saveWithNum(I2CDCTL);
    	log_saveWithNum(I2CIFG);
    	result = obc_sendData(BEACON_ADDR, MODULE_DATA_ADDR, MODULE_DATA_SIZE, moduleData.data);
    	if (result == ERROR){log_withNum(I2C_SERVICE, ERROR_LEVEL,"module",i2c_getMasterStatus());}
    	else {log_saveWithNum(I2CIFG);}

    	log_saveWithNum(I2CDCTL);
    	log_saveWithNum(I2CIFG);
    	result = obc_sendData(BEACON_ADDR, LEDTIME_DATA_ADDR, LEDTIME_DATA_SIZE, ledTimeData.data);
    	if (result == ERROR){log_withNum(I2C_SERVICE, ERROR_LEVEL,"ledtime",i2c_getMasterStatus());}
    	else {log_saveWithNum(I2CIFG);}


    	sendCnt++;

    	if (sendCnt > 11)
    	{
    		sendCnt = 0;
			log_saveWithNum(I2CDCTL);
			log_saveWithNum(I2CIFG);
			result = obc_sendCmd(BEACON_ADDR,BEACON_CMD1_ADDR, MORSE_SEND_START);
			if (result == ERROR){log_withNum(I2C_SERVICE, ERROR_LEVEL,"command",i2c_getMasterStatus());}
			else {log_saveWithNum(I2CIFG);}

    	}


    	log_printAllWithNum();
//    	i2c_masterRead(0x49,8,RxData);
//
//    	for (i = 0 ; i < TEMP_DATA_SIZE; i++)
//    	{
//    		tempData.data[i] = 64+i;
//        }
//
//    	uint8_t txData[TEMP_DATA_SIZE+1] = {TEMP_DATA_ADDR};
//
//    	for (i = 0 ; i < TEMP_DATA_SIZE ; i++ )
//    	{
//    		txData[i+1] = tempData.data[i];
//    	}
//
//    	txData[0]=TEMP_DATA_ADDR;
//
//    	i2c_masterWrite(0x49,1+TEMP_DATA_SIZE,txData);
//
//    	uint8_t cmdData[2]={0};
//    	cmdData[0]= 61;
//    	cmdData[1]= MORSE_SEND_START;
//
//    	i2c_masterWrite(0x49,2,cmdData);
//
////    	i2c_masterRead(0x49,8,RxData);
////
////    	if(obc_tempSensorRead() == 0)
////    	{
////    		obc_i2cStatus = i2c_getMasterStatus();
////    	}
////
////    	switch(obc_i2cStatus)
////    	{
////    	case I2C_BUS_POWERDOWN:
////    		// I2C Recovery
////    		// back to normal mode
////    		i2c_setMasterStatus(I2C_IDLE);
////    		break;
////    	default:
////    		break;
////    	}
//
    	systimer_init(TIMER_B_ACLK, TIMER_B_DIVIDED_BY_1, TIMER_B_UP_MODE, 33, 32763);
    	systimer_start();
    	systimer_setWakeUpPeriod(20); // wake up every 20sec
    	systimer_startWakeUpPeriod();
    	__bis_SR_register(LPM3_bits + GIE);

    }
}


// for Master Mode

// temp sensor service layer
uint8_t temp_readSensor(uint8_t address, uint8_t *data)
{
	uint8_t internalAddr[1] = {0};

	// set internal address
	if(i2c_masterWrite(address,1,internalAddr) == 0)
	{
		return 0;
	}

	// read temp value
	if(i2c_masterRead(address,2,data) == 0)
	{
		return 0;
	}

	return 1;
}

// temp sensors task layer
uint8_t obc_tempSensorRead(void)
{
	uint8_t tempData[2]={0};
	uint16_t value;

	// read temp sensor 1
	if(temp_readSensor(0x48, tempData))
	{
		value = 0;
		value = (value | tempData[0]) << 8;
		value |= tempData[1];
		// Throw 4 last 0's
		value = value >> 4;
		// High Byte
		obcData[0] = (value >> 8) & 0xFF;
		// Low Byte
		obcData[1] = value & 0xFF;
	}
	else
	{
		return 0;
	}

	// change Multiplexer

	// read temp sensor 2
	if(temp_readSensor(0x48, tempData))
	{
		value = 0;
		value = (value | tempData[0]) << 8;
		value |= tempData[1];
		// Throw 4 last 0's
		value = value >> 4;
		// High Byte
		obcData[2] = (value >> 8) & 0xFF;
		// Low Byte
		obcData[3] = value & 0xFF;
	}
	else
	{
		return 0;
	}

	return 1;
}
