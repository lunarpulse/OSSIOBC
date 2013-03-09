#include "obc_eeprom.h"

#define SUCCESS 1
#define ERROR 0
static uint8_t eepromI2CAddr = EEPROM_ADDR; // default EEPROM!

void eeprom_setAddress(uint8_t i2cAddress)
{
	eepromI2CAddr = i2cAddress;
}


void eeprom_ackPolling(uint8_t i2cAddress)
{
	if (i2cAddress == EEPROM_ADDR)
		i2c_ackPolling(i2cAddress);
	return;
}

uint8_t eeprom_byteWrite(uint16_t addr, uint8_t _data)
{
    int result;
    uint8_t buf[3];

    buf[0] = addr >> 8;
    buf[1] = addr & 0xFF;
    buf[2] = _data;
    result = i2c_masterWrite(eepromI2CAddr, 3, buf);
    if (result == ERROR){return result;}

    // TODO: confirm data is valid.
    return SUCCESS;
}


uint8_t eeprom_byteRead(uint16_t addr, uint8_t* data)
{
    uint8_t addrPoint[2];
    uint8_t result;

    addrPoint[0] = addr >> 8;
    addrPoint[1] = addr & 0xFF;

    result = i2c_masterWrite(eepromI2CAddr, 2, addrPoint);
    if (result == ERROR){return result;}
    result = i2c_masterRead(eepromI2CAddr, 1, data);
    if (result == ERROR){return result;}
    eeprom_ackPolling(eepromI2CAddr);

    return SUCCESS;
}


uint8_t eeprom_pageWrite(uint16_t addr, uint8_t dataSize, const uint8_t* data)
{
    #define PAGE_BUF_SIZE (64)
    uint8_t result = SUCCESS;
    uint16_t dataPtr;
    uint8_t buf[PAGE_BUF_SIZE+2]; // 64+2
    volatile uint16_t i;

    dataPtr = 0;

	buf[0] = addr >> 8;
	buf[1] = addr & 0xFF;
	if (dataSize <= PAGE_BUF_SIZE) {
		for(i = 0; i < dataSize; i++) {
			buf[i+2] = data[i+dataPtr];
		}
		result = i2c_masterWrite(eepromI2CAddr, dataSize+2, buf);
	} else {
		// TODO: Through the error: over page buffer size.
	}

    eeprom_ackPolling(eepromI2CAddr);
    if (result == ERROR){return result;}
    return SUCCESS;
}

uint8_t eeprom_pageRead(uint16_t addr, uint16_t readSize, uint8_t* data)
{
    #define READ_BUF_SIZE (256-1)
    uint8_t addrPoint[2];
    uint8_t result;

    addrPoint[0] = addr >> 8;
    addrPoint[1] = addr & 0xFF;

    if (readSize > 255) {
        result = i2c_masterWrite(eepromI2CAddr, 2, addrPoint);
        if (result == ERROR){return result;}
        result = i2c_masterRead(eepromI2CAddr, 255, data);
        if (result == ERROR){return result;}
        eeprom_ackPolling(eepromI2CAddr);
        result = eeprom_pageRead(addr+255, readSize-255, data+255);
    } else {
        result = i2c_masterWrite(eepromI2CAddr, 2, addrPoint);
        if (result == ERROR){return result;}
        result = i2c_masterRead(eepromI2CAddr, readSize, data);
    }
    eeprom_ackPolling(eepromI2CAddr);

    if (result == ERROR){return result;}
    return SUCCESS;
}

uint8_t obc_eepromRawTest()
{
	#define EEPROM_TEST2_SIZE 64  // Maxium page size is 64
	uint8_t result;
	uint8_t test_data[EEPROM_TEST2_SIZE]; // (2+64)*2
	volatile int i;

	test_data[0] = 0x0;
	test_data[1] = 0;

	for(i = 2; i < sizeof(test_data); i++) {
        test_data[i] = i;
    }

    //eeprom_ackPolling(eepromI2CAddr);
	result = i2c_masterWrite(eepromI2CAddr, EEPROM_TEST2_SIZE, test_data);
    if (result == ERROR){
    	log_withNum(I2C_SERVICE, ERROR_LEVEL, "eeprom: write error but ", i2c_getMasterStatus());
    	P1OUT ^= BIT1; // P1.1 = toggle
    	return ERROR;
    }

    return result;
}


uint8_t obc_framTest()
{
    uint8_t result;

	#define EEPROM_TEST_SIZE 64
    uint8_t readData[EEPROM_TEST_SIZE]; // (2+64)*2
    uint8_t testData[EEPROM_TEST_SIZE]; // (2+64)*2
    volatile int i;

    for(i = 0; i < sizeof(testData); i++) {
        testData[i] = i;
    }

    eeprom_setAddress(FRAM_ADDR);

    result = eeprom_pageWrite(0, EEPROM_TEST_SIZE, testData);
    if (result == ERROR){
    	log_withNum(I2C_SERVICE, ERROR_LEVEL, "fram: write error but ", i2c_getMasterStatus());
    	return result;
    }

    result = eeprom_pageRead(0, EEPROM_TEST_SIZE, readData);
    if (result == ERROR){
    	log_withNum(I2C_SERVICE, ERROR_LEVEL, "fram: read error but ", i2c_getMasterStatus());
    	return result;
    }

    for(i = 0; i < sizeof(readData); i++) {
         readData[i] = i;
         eeprom_byteRead(i,&(readData[0]));
         log_withNum(I2C_SERVICE, NOTICE_LEVEL, "fram: expect 0~255, but ", readData[0]);
     }

    return result;
}



uint8_t obc_eepromTest()
{
    uint8_t result;

    //result = obc_eepromRawTest();
	//return result;

//    result = obc_framTest();
//    return result;

	#define EEPROM_TEST_SIZE 64

    uint8_t readData[EEPROM_TEST_SIZE]; // (2+64)*2
    uint8_t testData[EEPROM_TEST_SIZE]; // (2+64)*2
    volatile int i;

    for(i = 0; i < sizeof(testData); i++)
    {
        testData[i] = i;
    }
    result = eeprom_pageWrite(0, EEPROM_TEST_SIZE, testData);
    if (result == ERROR){
    	log_withNum(I2C_SERVICE, ERROR_LEVEL, "eeprom: write error but ", i2c_getMasterStatus());
    	return result;
    }

    result = eeprom_pageRead(0, EEPROM_TEST_SIZE, readData);
    if (result == ERROR){
    	log_withNum(I2C_SERVICE, ERROR_LEVEL, "eeprom: read error but ", i2c_getMasterStatus());
    	return result;
    }

    log_withNum(I2C_SERVICE, NOTICE_LEVEL, "eeprom: t[0] expect 0, but ", testData[0]);
    log_withNum(I2C_SERVICE, NOTICE_LEVEL, "eeprom: r[0] expect 0, but ", readData[0]);

    log_withNum(I2C_SERVICE, NOTICE_LEVEL, "eeprom: t[0] expect 300, but ", testData[EEPROM_TEST_SIZE-1]);
    log_withNum(I2C_SERVICE, NOTICE_LEVEL, "eeprom: r[0] expect 300, but ", readData[EEPROM_TEST_SIZE-1]);

    return result;
}
