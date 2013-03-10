/*
 * ossi_obc.c
 *
 *  Created on: 2013. 1. 25.
 *      Author: OSSI
 */

#include "ossi_obc.h"


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

//void obc_sendCmdToBeacon(uint8_t cmd)
//{
//	cmdData[0]= BEACON_CMD1_ADDR;
//	cmdData[1]= MORSE_SEND_START;
//	obc_sendCmd(0x49,BEACON_CMD1_ADDR,cmd);
//}
