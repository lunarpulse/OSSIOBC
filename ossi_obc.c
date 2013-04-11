/*
 * ossi_obc.c
 *
 *  Created on: 2013. 1. 25.
 *      Author: OSSI
 */

#include "ossi_obc.h"

parmOperationMode obc_mode;


statusData_t statusData;
viData_t viData;
tempData_t tempData;
moduleData_t moduleData;
ledTimeData_t ledTimeData;

static volatile uint8_t obc_i2cStatus;

uint8_t supercapPathOnFlag = 0;
uint8_t supercapChargerEnFlag = 0;

void supercapPathOn(uint8_t val)
{
	if(val)
	{
		P4OUT |= SCOUT_EN_OUT_PIN;
		supercapPathOnFlag = 1;
	}
	else
	{
		P4OUT &= ~SCOUT_EN_OUT_PIN;
		supercapPathOnFlag = 0;
	}
}

void supercapChargerOn(uint8_t val)
{
	if(val)
	{
		P4OUT |= SCCHG_EN_OUT_PIN;
		supercapChargerEnFlag = 1;
	}
	else
	{
		P4OUT &= ~SCCHG_EN_OUT_PIN;
		supercapChargerEnFlag = 0;
	}
}

uint8_t getSupercapPathOn(void)
{
	return supercapPathOnFlag;
}

uint8_t getSupercapChargerOn(void)
{
	return supercapChargerEnFlag;
}

void obc_daqOnData(void)
{
	statusData.u8.onStatus = 0;
	if(P2IN & COMMS_FAULT_IN_PIN)
	{
		statusData.u8.onStatus |= COMMS_ON;
	}

	if(P2IN & BEACON_FAULT_IN_PIN)
	{
		statusData.u8.onStatus |= BEACON_ON;
	}

	if(P2IN & LED_FAULT_IN_PIN)
	{
		statusData.u8.onStatus |= LED_ON;
	}
}

void obc_daqEpsData(void)
{
	statusData.u8.epsStatus = 0;
	// buffer enable
	P2OUT &= ~IO_OE_PIN;
	delay_ms(10);

	if(P1IN & BAT_ON_IN_PIN)
	{
		statusData.u8.epsStatus |= BATTERY_MODE;
	}

	if(P2IN & SOLAR_ON_IN_PIN)
	{
		statusData.u8.epsStatus &= ~BATTERY_MODE;
	}

	if(getSupercapPathOn())
	{
		statusData.u8.epsStatus |= SUPERCAP_PATH_ON;
	}
	else
	{
		statusData.u8.epsStatus &= ~SUPERCAP_PATH_ON;
	}

	if(P1IN & BAT_CHRG_IN_PIN)
	{
		statusData.u8.epsStatus |= BATTERY_CHARGED;
	}
	else
	{
		statusData.u8.epsStatus &= ~BATTERY_CHARGED; // charging
	}

	if(!(P1IN & BAT_FAULT_IN_PIN))
	{
		statusData.u8.epsStatus |= BATTERY_FAULT;
	}
	else
	{
		statusData.u8.epsStatus &= ~BATTERY_FAULT; // charging
	}

	if(getSupercapChargerOn())
	{
		statusData.u8.epsStatus |= SUPERCAP_CHARGER_EN;
	}
	else
	{
		statusData.u8.epsStatus &= ~SUPERCAP_CHARGER_EN;
	}

	if (P1IN & SC_PFO_IN_PIN)
	{
		statusData.u8.epsStatus |= SUPERCAP_CHARGED;
	}
	else
	{
		statusData.u8.epsStatus &= ~SUPERCAP_CHARGED;
	}
	delay_ms(10);
	P2OUT |= IO_OE_PIN;

}

void obc_daqVIData(void)
{
	adg708_setChannel(MUX_VSOLAR);
	viData.u8.vsolar = (adc12_readChannel(1) >> 4) & 0xFF;

	adg708_setChannel(MUX_VBUS);
	viData.u8.vbus = (adc12_readChannel(1) >> 4) & 0xFF;

	adg708_setChannel(MUX_VBAT);
	viData.u8.vbat = (adc12_readChannel(1) >> 4) & 0xFF;

	adg708_setChannel(MUX_VSUPERCAP);
	viData.u8.vsupercap = (adc12_readChannel(1) >> 4) & 0xFF;

	// VOUT = (GAIN)(RSENSE)(ILOAD)
	// * 0.8056 / 2.5 = * 0.3222 for 0.05 RSENSE
	// * 0.8056 / 0.5 = * 1.611 for 0.01 (10mOhm) RSENSE
	adg708_setChannel(MUX_SOLAR_CS1);
	viData.u8.solarCs1MinusZ = (adc12_readChannel(0) >> 4) & 0xFF;

	adg708_setChannel(MUX_SOLAR_CS2);
	viData.u8.solarCs2MinusY = (adc12_readChannel(0) >> 4) & 0xFF;

	adg708_setChannel(MUX_SOLAR_CS3);
	viData.u8.solarCs3MinusX = (adc12_readChannel(0) >> 4) & 0xFF;

	adg708_setChannel(MUX_SOLAR_CS4);
	viData.u8.solarCs4PlusZ = (adc12_readChannel(0) >> 4) & 0xFF;

	adg708_setChannel(MUX_SOLAR_CS5);
	viData.u8.solarCs5PlusY = (adc12_readChannel(0) >> 4) & 0xFF;

	adg708_setChannel(MUX_BUS_CS);
	viData.u8.busCs = (adc12_readChannel(0) >> 4) & 0xFF;


}

void read_i2cTemp(uint8_t slaveAddress, uint8_t *data)
{
	volatile uint8_t result;
	uint8_t tempVal[2];
	result = i2c_masterRead(slaveAddress,2,tempVal);
	if (result)
	{
		*data = tempVal[0];
	}
	else
	{
		obc_i2cStatus = i2c_getMasterStatus();
		*data = 0;
	}
}

void obc_daqTempData(void)
{
	// need to convert to celsius degree
	adg708_setChannel(MUX_BAT_TEMP);
	tempData.u8.battery = (adc12_readChannel(0) >> 4) & 0xFF;

	adg708_setChannel(MUX_SUPERCAP_TEMP);
	tempData.u8.supercap = (adc12_readChannel(0) >> 4) & 0xFF;


	//TODO
//	mux_setChannel(MUX_COMMS_CHANNEL);
//	result = i2c_masterRead(COMMS_ADDR, 2, tempVal);
//	tempData.u8.comms = tempVal[0];
	tempData.u8.comms = 0;
	tempData.u8.beacon = 0;

	// reset internal OBC I2C power
	P3OUT &= ~OBC_I2C_OFF_PIN;
	delay_ms(1);
	P3OUT |= OBC_I2C_OFF_PIN;
	delay_ms(1);
	P3OUT &= ~OBC_I2C_OFF_PIN;
	delay_ms(1);
	mux_setChannel(MUX_OBC_INTERNAL_CHANNEL);
	read_i2cTemp(OBC_TEMP_ADDR,&(tempData.u8.obc));

	mux_setChannel(MUX_PANELTEMP_CHANNEL);
	read_i2cTemp(TEMP_PLUS_Y_ADDR,&(tempData.u8.plusY));
	read_i2cTemp(TEMP_PLUS_Z_ADDR,&(tempData.u8.plusZ));
	read_i2cTemp(TEMP_MINUS_X_ADDR,&(tempData.u8.minusX));
	read_i2cTemp(TEMP_MINUS_Y_ADDR,&(tempData.u8.minusY));
	read_i2cTemp(TEMP_MINUS_Z_ADDR,&(tempData.u8.minusZ));
}

void obc_daqModuleData(void)
{
	//TODO: this is mock data
	moduleData.u8.commRxCmd = 0x96; // 0b10010110
	moduleData.u8.commsRxCnt = 5;
	moduleData.u8.commsRssi = 13;
	moduleData.u8.commsTxDataId = 0x03;
	moduleData.u8.gyroSensor = 0;
	moduleData.u8.sunSensor = 0;
	moduleData.u8.obcLatchUpCnt = 0;
	moduleData.u8.commsLatchUpCnt = 0;
	moduleData.u8.beaconLatchUpCnt = 0;
	moduleData.u8.ledLatchUpCnt = 0;
}

void obc_daqLedTime(void)
{
	//TODO: this is mock data
	ledTimeData.u8.date = 5;
	ledTimeData.u8.hour = 2;
	ledTimeData.u8.min = 0;
	ledTimeData.u8.ledSensor = 0;
}

void obc_dataAcquire(void)
{
	volatile uint8_t result;

	statusData.u8.operationMode = obc_mode;

	obc_daqOnData();
	obc_daqEpsData();
	statusData.u8.errStatus = obc_i2cStatus;
	statusData.u8.obcTick3 = (systimer_getSecTick() >> 24) & 0xFF;
	statusData.u8.obcTick2 = (systimer_getSecTick() >> 16) & 0xFF;
	statusData.u8.obcTick1 = (systimer_getSecTick() >> 8) & 0xFF;
	statusData.u8.obcTick0 = (systimer_getSecTick() >> 0) & 0xFF;

	obc_daqVIData();
	obc_daqTempData();
	obc_daqModuleData();
	obc_daqLedTime();

	// back to beacon
	mux_setChannel(MUX_BEACON_CHANNEL);

}

void obc_reportSendDataToBeacon(void)
{
	// adc chan 0
	volatile uint16_t adc_vsolar;
	volatile uint16_t adc_vbus;
	volatile uint16_t adc_vbat;
	volatile uint16_t adc_vsupercap;

	// adc chan 1
	volatile uint16_t adc_solar_cs1;
	volatile uint16_t adc_solar_cs2;
	volatile uint16_t adc_solar_cs3;
	volatile uint16_t adc_solar_cs4;
	volatile uint16_t adc_solar_cs5;
	volatile uint16_t adc_bus_cs;

	volatile uint16_t adc_batTemp;
	volatile uint16_t adc_supercapTemp;
	volatile int16_t i2c_commsTemp;
	volatile int16_t i2c_beaconTemp;
	volatile int16_t i2c_obcTemp;
	volatile int16_t i2c_plusYTemp;
	volatile int16_t i2c_plusZTemp;
	volatile int16_t i2c_minusXTemp;
	volatile int16_t i2c_minusYTemp;
	volatile int16_t i2c_minusZTemp;


	begin_report();

	printf("***************** General Status *************\r\n");
	printf("- Operation Mode: %u \r\n", statusData.u8.operationMode);
	printf("- On Status: %u \r\n", statusData.u8.onStatus);
	volatile uint8_t aa;
	aa = statusData.u8.epsStatus;
	printf("- EPS Status: %u \r\n", statusData.u8.epsStatus);
	printf("- Error Status: %u \r\n", statusData.u8.errStatus);
	printf("- Obc Tick3: %u \r\n",statusData.u8.obcTick3);
	printf("- Obc Tick2: %u \r\n",statusData.u8.obcTick2);
	printf("- Obc Tick1: %u \r\n",statusData.u8.obcTick1);
	printf("- Obc Tick0: %u \r\n",statusData.u8.obcTick0);


	printf("***************** Volts and Currents *************\r\n");

	adc_vsolar = ((uint16_t)(viData.u8.vsolar)<<4) * 0.8056 * 2;
	adc_vbus = ((uint16_t)(viData.u8.vbus)<<4) * 0.8056 * 2;
	adc_vbat =((uint16_t)(viData.u8.vbat)<<4)  * 0.8056 * 2;
	adc_vsupercap = ((uint16_t)(viData.u8.vsupercap)<<4)  * 0.8056 * 2;
	adc_solar_cs1 = ((uint16_t)(viData.u8.solarCs1MinusZ)<<4)  * 0.3222;
	adc_solar_cs2 = ((uint16_t)(viData.u8.solarCs2MinusY)<<4)  * 0.3222;
	adc_solar_cs3 = ((uint16_t)(viData.u8.solarCs3MinusX)<<4)  * 0.3222;
	adc_solar_cs4 = ((uint16_t)(viData.u8.solarCs4PlusZ)<<4)  * 0.3222;
	adc_solar_cs5 =((uint16_t)(viData.u8.solarCs5PlusY)<<4)  * 0.3222;
	adc_bus_cs = ((uint16_t)(viData.u8.busCs)<<4) * 1.611;

	printf("- Solar Voltage: %u mV\r\n", adc_vsolar);
	printf("- Battery Voltage: %u mV\r\n", adc_vbat);
	printf("- Bus Voltage: %u mV\r\n", adc_vbus);
	printf("- Supercap Voltage: %u mV\r\n", adc_vsupercap);
	printf("- Solar Panel 1 (+Y) Current: %u mA\r\n",adc_solar_cs1);
	printf("- Solar Panel 2 (-Y) Current: %u mA\r\n",adc_solar_cs2);
	printf("- Solar Panel 3 (+Z) Current: %u mA\r\n",adc_solar_cs3);
	printf("- Solar Panel 4 (-Z) Current: %u mA\r\n",adc_solar_cs4);
	printf("- Solar Panel 5 (-X) Current: %u mA\r\n",adc_solar_cs5);
	printf("- Total Current Consumption: %u mA\r\n",adc_bus_cs);


	printf("***************** Temp *************\r\n");
	adc_batTemp = ((uint16_t)(tempData.u8.battery) <<4)  * 0.8056;
	adc_supercapTemp = ((uint16_t)(tempData.u8.supercap)<<4)  * 0.8056;
	i2c_commsTemp = ((int16_t)(tempData.u8.comms) << 4)  * 0.0625;
	i2c_beaconTemp = ((int16_t)(tempData.u8.beacon) << 4)  * 0.0625;
	i2c_obcTemp = ((int16_t)(tempData.u8.obc) << 4)  * 0.0625;
	i2c_plusYTemp = ((int16_t)(tempData.u8.plusY) << 4)  * 0.0625;
	i2c_plusZTemp = ((int16_t)(tempData.u8.plusZ) << 4)  * 0.0625;
	i2c_minusXTemp = ((int16_t)(tempData.u8.minusX) << 4)  * 0.0625;
	i2c_minusYTemp = ((int16_t)(tempData.u8.minusY) << 4)  * 0.0625;
	i2c_minusZTemp = ((int16_t)(tempData.u8.minusZ) << 4)  * 0.0625;

	printf("- Battery Temp: %u \r\n",adc_batTemp);
	printf("- Supercap Temp: %u \r\n", adc_supercapTemp);
	printf("- COMMS Temp: %i \r\n",i2c_commsTemp);
	printf("- Beacon Temp: %i \r\n", i2c_beaconTemp);
	printf("- OBC Temp: %i \r\n",i2c_obcTemp);
	printf("- Plus Y Temp: %i \r\n",i2c_plusYTemp);
	printf("- Plus Z Temp: %i \r\n",i2c_plusZTemp);
	printf("- MINUS X Temp: %i \r\n", i2c_minusXTemp);
	printf("- MINUS Y Temp: %i \r\n", i2c_minusYTemp);
	printf("- MINUS Z Temp: %i \r\n", i2c_minusZTemp);


	printf("***************** Module Data *************\r\n");
	printf("- COMMS RX Cmd: %u \r\n", moduleData.u8.commRxCmd);
	printf("- COMMS RX Count: %u \r\n",moduleData.u8.commsRxCnt);
	printf("- COMMS RSSI: %u \r\n", moduleData.u8.commsRssi);
	printf("- COMMS TX Data Id: %u \r\n", moduleData.u8.commsTxDataId);
	printf("- GYRO Sensor: %u \r\n", moduleData.u8.gyroSensor);
	printf("- Sun Sensor: %u \r\n", moduleData.u8.sunSensor);
	printf("- OBC Latchup Count: %u \r\n", moduleData.u8.obcLatchUpCnt);
	printf("- COMMS Latchup Count: %u \r\n", moduleData.u8.commsLatchUpCnt);
	printf("- BEACON Latchup Count: %u \r\n", moduleData.u8.beaconLatchUpCnt);
	printf("- LED Latchup Count: %u \r\n", moduleData.u8.ledLatchUpCnt);

	printf("***************** LED Tiem *************\r\n");
	printf("- LED DATE: %u \r\n", ledTimeData.u8.date);
	printf("- LED HOUR: %u \r\n", ledTimeData.u8.date);
	printf("- LED MIN: %u \r\n", ledTimeData.u8.min);
	printf("- LED SENSOR: %u \r\n", ledTimeData.u8.ledSensor);

	end_report();
}

uint8_t obc_sendData(uint8_t slaveAddress, uint8_t dataAddr, uint8_t size, uint8_t* data)
{
	#define MAX_SEND_DATA_SIZE	(16)

	volatile uint8_t i;
	volatile uint8_t result;
	uint8_t txData[MAX_SEND_DATA_SIZE] ={0};

	for (i = 0 ; i < size ; i++ )
	{
		txData[i+1] = data[i];
	}

	txData[0]=dataAddr;

	result = i2c_masterWrite(slaveAddress,1+size,txData);
	if (result == ERROR){return ERROR;}

	return SUCCESS;
}

void obc_sendDataToBeacon(void)
{
	volatile uint8_t result;
	result = obc_sendData(BEACON_ADDR, STATUS_DATA_ADDR, STATUS_DATA_SIZE, statusData.data);
	if (result == ERROR){obc_i2cStatus = i2c_getMasterStatus();}
	result = obc_sendData(BEACON_ADDR, VI_DATA_ADDR, VI_DATA_SIZE, viData.data);
	if (result == ERROR){obc_i2cStatus = i2c_getMasterStatus();}
	result = obc_sendData(BEACON_ADDR, TEMP_DATA_ADDR, TEMP_DATA_SIZE, tempData.data);
	if (result == ERROR){obc_i2cStatus = i2c_getMasterStatus();}
	result = obc_sendData(BEACON_ADDR, MODULE_DATA_ADDR, MODULE_DATA_SIZE, moduleData.data);
	if (result == ERROR){obc_i2cStatus = i2c_getMasterStatus();}
	result = obc_sendData(BEACON_ADDR, LEDTIME_DATA_ADDR, LEDTIME_DATA_SIZE, ledTimeData.data);
	if (result == ERROR){obc_i2cStatus = i2c_getMasterStatus();}

}

//TODO
//uint8_t obc_readData(uint8_t slaveAddress, uint8_t dataAddr, uint8_t size, uint8_t* data)
//{
//	volatile uint8_t result;
//	result = i2c_masterRead(slaveAddress,size,data);
//	if (result == ERROR){return ERROR;}
//
//	return SUCCESS;
//}

//void obc_sendDataToBeacon(void)
//{
//	obc_sendData(BEACON_ADDR, STATUS_DATA_ADDR, STATUS_DATA_SIZE, statusData.data);
//	obc_sendData(BEACON_ADDR, VI_DATA_ADDR, VI_DATA_SIZE, viData.data);
//	obc_sendData(BEACON_ADDR, TEMP_DATA_ADDR, TEMP_DATA_SIZE, tempData.data);
//	obc_sendData(BEACON_ADDR, MODULE_DATA_ADDR, MODULE_DATA_SIZE, moduleData.data);
//	obc_sendData(BEACON_ADDR, LEDTIME_DATA_ADDR, LEDTIME_DATA_SIZE, ledTimeData.data);
//}

uint8_t obc_sendCmd(uint8_t slaveAddress, uint8_t cmdAddr, uint8_t cmd)
{
	volatile uint8_t result;
	uint8_t cmdData[2]={0};
	cmdData[0]= cmdAddr;
	cmdData[1]= cmd;
	result = i2c_masterWrite(slaveAddress,2,cmdData);
	if (result == ERROR){ return ERROR;}

	return SUCCESS;
}

void obc_supercapCharge(void)
{
	// supercap path on
	P4OUT |= SCOUT_EN_OUT_PIN;
	// charge enable
	P4OUT |= SCCHG_EN_OUT_PIN;
}

void obc_ledOn(void)
{
	P4OUT &= ~LED_OFF_PIN;
}

void obc_ledOff(void)
{
	P4OUT |= LED_OFF_PIN;
	// charge disable
	P4OUT &= ~SCCHG_EN_OUT_PIN;
	// supercap path off
	P4OUT &= ~SCOUT_EN_OUT_PIN;
}


//void obc_sendCmdToBeacon(uint8_t cmd)
//{
//	cmdData[0]= BEACON_CMD1_ADDR;
//	cmdData[1]= MORSE_SEND_START;
//	obc_sendCmd(0x49,BEACON_CMD1_ADDR,cmd);
//}
