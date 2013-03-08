/*
 * i2c.h
 *
 *  Created on: 2013. 1. 6.
 *      Author: OSSI
 */

#ifndef I2C_H_
#define I2C_H_

#include "ossi_1611.h"
#include "timerA.h"

// TODO: Temporary Test
// I2C states and errors
#define I2C_WRITE 			0
#define I2C_READ 			1
#define I2C_RECOVERY		2

/*
 * I2C Master Status
 */
#define I2C_IDLE				(0)
#define I2C_NACK				(1)
#define I2C_TIMEOUT				(2)
#define I2C_BUS_NOT_READY		(3)
#define I2C_BUS_POWERDOWN		(4)
#define	I2C_TRANSFER_DONE		(5)
#define I2C_TRANSFER_TIMEOUT 	(6)
#define I2C_ACK					(7)
#define I2C_ARB_LOST			(8)

#define I2C_SDA_PIN 			(BIT1)
#define I2C_SCL_PIN 			(BIT3)

#define I2C_CLOCKSOURCE_ACLK 		I2CSSEL_1
#define I2C_CLOCKSOURCE_SMCLK 		I2CSSEL_2

#define I2C_ARB_LOST_INT 			ALIE
#define I2C_NACK_INT 				NACKIE
#define I2C_OWN_ADDR_INT 			OAIE
#define I2C_ACCESS_RDY_INT 			ARDYIE
#define I2C_RX_RDY_INT 				RXRDYIE
#define I2C_TX_RDY_INT 				TXRDYIE
#define I2C_GENERAL_CALL_INT 		GCIE
#define I2C_START_INT 				STTIE

void i2c_portSetup(void);

//void i2c_enable(void);
//void i2c_disable(void);
void i2c_reset(void);

void i2c_enableInterrupt(uint8_t interruptSelect);
void i2c_disableInterrupt(uint8_t interruptSelect);
void i2c_disableAllInterrupt(void);

uint8_t i2c_getMasterStatus(void);
void i2c_setMasterStatus(uint8_t status);

uint8_t i2c_generalCall(void);
uint8_t i2c_ackPolling(uint8_t slaveAddress);
uint8_t i2c_masterWrite(uint8_t slaveAddress, uint8_t byteCount, uint8_t *data);
uint8_t i2c_masterRead(uint8_t slaveAddress, uint8_t byteCount, uint8_t *data);
uint8_t i2c_masterWriteAndRead(uint8_t slaveAddress, uint8_t txByteCount, uint8_t *txData, uint8_t rxByteCount, uint8_t *rxData);

void i2c_slaveInit(uint8_t ownAddress, uint8_t byteCount, uint8_t *data);
void i2c_slaveStart(void);

uint8_t i2c_getInternalAddress(void);

uint8_t i2c_getSlaveTxDone(void);
void i2c_setSlaveTxDone(uint8_t status);
uint8_t i2c_getSlaveRxDone(void);
void i2c_setSlaveRxDone(uint8_t status);
uint8_t i2c_getGeneralCallReceived(void);
void i2c_setGeneralCallReceived(uint8_t status);

void i2c_busRecoverySequence(void);

#endif /* I2C_H_ */
