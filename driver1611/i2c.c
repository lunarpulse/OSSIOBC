/*
 * i2c.c
 *
 *  Created on: 2013. 1. 6.
 *      Author: OSSI
 */
#include "i2c.h"

#define I2C_MASTER_RX_BUFFER_SIZE 16
#define I2C_MASTER_TX_BUFFER_SIZE 16

#define I2C_SLAVE_BUFFER_SIZE 64

static uint8_t *masterRxData;
static uint8_t *masterTxData;

static volatile uint8_t masterTxCurrentCnt;
static volatile uint8_t masterRxCurrentCnt;

static volatile uint8_t i2cMasterStatus;

static volatile uint8_t *slaveData;
static volatile uint8_t *defaultSlaveData;

static volatile uint16_t slaveIndex;
static volatile uint16_t slaveBufSize;

static volatile uint8_t i2cInteralAddress;

static volatile uint8_t i2cSlaveTxDone;
static volatile uint8_t i2cSlaveRxDone;

static volatile uint8_t i2cSlaveTxInProgress;
static volatile uint8_t i2cSlaveRxInProgress;

static volatile uint8_t i2cStartDetected;
static volatile uint8_t i2cGeneralCallDetected;


void i2c_portSetup(void)
{
	P3SEL |= I2C_SDA_PIN + I2C_SCL_PIN;
}

//void i2c_enable(void)
//{
//	U0CTL |= I2CEN;
//}
//
//void i2c_disable(void)
//{
//	U0CTL &= ~I2CEN;
//}

void i2c_reset(void)
{
	U0CTL &= ~I2CEN;
	U0CTL |= I2CEN;
}

void i2c_enableInterrupt(uint8_t interruptSelect)
{
	I2CIE |= interruptSelect;
}

void i2c_disableInterrupt(uint8_t interruptSelect)
{
	I2CIE &= ~interruptSelect;
}

void i2c_disableAllInterrupt(void)
{
	I2CIE = 0;
}

uint8_t i2c_getMasterStatus(void)
{
	return i2cMasterStatus;
}

void i2c_setMasterStatus(uint8_t status)
{
	i2cMasterStatus = status;
}

//uint8_t i2c_waitForBusReady(void)
//{
//	// check bus status
//	while (I2CDCTL & I2CBB)
//	{
//		if(i2cTimeOut)
//		{
//			// when timeout, stop the timeout timer first
//			i2c_timerTimeoutStop();
//
//			// check whether BUS is power down
//			// change pin function only when I2C bus is busy for timeout period otherwise I2C module will not be functioning
//			P3SEL &= ~(I2C_SDA_PIN + I2C_SCL_PIN);			// set pins to GPIO
//			P3DIR &= ~(I2C_SDA_PIN + I2C_SCL_PIN);			// set input direction
//
//			// check SDA and SCL are both LOW
//			// TODO: checking only one time is enough?
//			if (((P3IN & I2C_SDA_PIN) == 0) && ((P3IN &I2C_SDA_PIN) == 0))
//			{
//				P3SEL |= I2C_SDA_PIN + I2C_SCL_PIN;			// set pins back to I2C function
//				// report error
//				return I2C_BUS_POWERDOWN;
//			}
//			else
//			{
//				P3SEL |= I2C_SDA_PIN + I2C_SCL_PIN;			// set pins back to I2C function
//				// report error
//				return I2C_BUS_NOT_READY;
//			}
//
//		}
//	}
//
//	return I2C_IDLE;
//}

uint8_t i2c_waitForBusReady(void)
{
	P3SEL &= ~(I2C_SDA_PIN + I2C_SCL_PIN);			// set pins to GPIO
	P3DIR &= ~(I2C_SDA_PIN + I2C_SCL_PIN);			// set input direction

	// check bus status
	while (!((P3IN & I2C_SDA_PIN) && (P3IN &I2C_SDA_PIN)))
	{
		if(i2cTimeOut)
		{
			// when timeout, stop the timeout timer first
			i2c_timerTimeoutStop();

			// check whether BUS is power down
			// change pin function only when I2C bus is busy for timeout period otherwise I2C module will not be functioning

			// check SDA and SCL are both LOW
			// TODO: checking only one time is enough?
			if (((P3IN & I2C_SDA_PIN) == 0) && ((P3IN &I2C_SDA_PIN) == 0))
			{
				P3SEL |= I2C_SDA_PIN + I2C_SCL_PIN;			// set pins back to I2C function
				// report error
				return I2C_BUS_POWERDOWN;
			}
			else
			{
				P3SEL |= I2C_SDA_PIN + I2C_SCL_PIN;			// set pins back to I2C function
				// report error
				return I2C_BUS_NOT_READY;
			}

		}
	}

	P3SEL |= I2C_SDA_PIN + I2C_SCL_PIN;			// set pins back to I2C function
	return I2C_IDLE;
}



// TODO: make inline function later
uint8_t i2c_checkArbLost(void)
{
	if (I2CIFG & ALIFG)
	{
		i2c_disableAllInterrupt();
		// when NACK is received, stop the timeout timer first
		i2c_timerTimeoutStop();
		// clear NACKIFG flag manually
		I2CIFG &= ~ALIFG;
		return 1;
	}
	else
	{
	return 0;
	}
}

// TODO: make inline function later
uint8_t i2c_checkNACK(void)
{
	if (I2CIFG & NACKIFG)
	{
		i2c_disableAllInterrupt();
		// when NACK is received, stop the timeout timer first
		i2c_timerTimeoutStop();
		// clear NACKIFG flag manually
		I2CIFG &= ~NACKIFG;

		return 1;
	}
	else
	{
		return 0;
	}
}

// TODO: make inline function later
uint8_t i2c_checkTimeout(void)
{
	if(i2cTimeOut)
	{
		i2c_disableAllInterrupt();
		// when timeout, stop the timeout timer first
		i2c_timerTimeoutStop();
		return 1;
	}
	else
	{
		return 0;
	}
}

// TODO: make inline function later
/*
 *  Use this function only after previous transfer to check whether the transfer is finished
 */
uint8_t i2c_waitForPreviousTransfer(uint8_t byteCount)
{

//	while(I2CTCTL & I2CSTP)
//	{
//		if(i2cTimeOut)
//		{
//			// when timeout, stop the timeout timer first
//			i2c_timerTimeoutStop();
//			return I2C_TRANSFER_TIMEOUT;
//		}
//	}

	if (byteCount > 1)
	{
		// when write transfer is finished, read
		// check transfer with or without STOP condition is completed
		// DO NOT ENALBE Access Ready Interrupt.
		// As soon as TX is done PC goes to Access Ready ISR
		// and it will clear ARDYIFG and time out will occur
		while ((~I2CIFG) & ARDYIFG)				// check all data are sent
		{
			if(i2cTimeOut)
			{
				// when timeout, stop the timeout timer first
				i2c_timerTimeoutStop();
				return I2C_TRANSFER_TIMEOUT;
			}
		}

		// Manually Clear ARDYIFG as we're not using ARDYIE to auto clear the IFG
		I2CIFG &= ~ARDYIFG;						// Clear Access ready IFG;
	}
	else if (byteCount == 1)
	{
		while(I2CTCTL & I2CSTP)
		{
			if(i2cTimeOut)
			{
				// when timeout, stop the timeout timer first
				i2c_timerTimeoutStop();
				return I2C_TRANSFER_TIMEOUT;
			}
		}
	}

	// when transfer is completed,
	// disable I2C module to prevent accidental behavior

	return I2C_TRANSFER_DONE;
}


/*
 * we're not going to separate i2c_masterInit() from Write and Read function
 * as we're calling masterInit every time we do transfer
 * and it's it's a good practice to reset I2C module every time we do transfer to prevent i2c hang to happen
 * And since we're going to SMCLK only we'll fix this
 * And for SCL clock, we fix this in the Write and Read function as well
 */

void i2c_masterInit(uint8_t selctClockSource, uint8_t preScalerValue, uint8_t clockHighValue, uint8_t clockLowValue)
{
	volatile uint8_t i;

	// No repeat mode implementation I2CRM = 0
	// DO NOT Init when previous transfer is being processed!!!
	// checking the i2c status before init is important as we'll call init every time we transfer
	U0CTL |= I2C + SYNC;                      	// Recommended init procedure

	// This is weired code
	// but to make sure reset I2C module
	// I clear I2CEN 10 times (no solid reason)
	for( i = 0 ; i < 10 ; i++)
	{
		U0CTL &= ~I2CEN;
		U0CTL |= I2CEN;
	}

	U0CTL &= ~I2CEN;                          	// Disable I2C
	I2CTCTL = selctClockSource;               	// SMCLK mostly
	I2CPSC = preScalerValue; 					//I2CPSC should be <=4 or error might occur
	I2CSCLH = clockHighValue; 					// High period of SCL (I2CSCLH+2) x (I2CPSC+ 1)
	I2CSCLL = clockLowValue;  					// Low period of SCL (I2CSCLL+2) x (I2CPSC + 1)
	I2CNDAT = 0;                           		// initialize data cnt;
	U0CTL |= I2CEN;                         	// Enable I2C
	U0CTL |= MST; 								// set to Master mode to I2C register
}

uint8_t i2c_generalCall(void)
{
	// disable all i2c interrupts
	i2c_disableAllInterrupt();

	// set default I2C status
	i2cMasterStatus = I2C_IDLE;

	// timer start
	i2c_timerInit(TIMER_A_ACLK, TIMER_A_DIVIDED_BY_8, TIMER_A_UP_MODE , DEFAULT_CCR0); // set i2c timeout timer
	i2c_timerTimeoutStart();

	i2cMasterStatus = i2c_waitForBusReady();
	if (i2cMasterStatus != I2C_IDLE)
	{
//		i2c_disable();
		return 0;
	}

	// init master everytime you start new transfer
	i2c_masterInit(I2C_CLOCKSOURCE_SMCLK, 0, 3, 3);

	U0CTL &= ~I2CEN;                        // clear I2CEN bit => necessary to re-configure I2C module
	I2CTCTL |= I2CRM;                       // transmission is software controlled
	U0CTL |= I2CEN;                         // enable I2C module

	i2c_disableAllInterrupt();

	// Clear UCALIFG if you previous transfer ended with arbitration lost
	I2CIFG &= ~ALIFG;

	// Clear NACKIFG:
	// This Is Very Important If you Want Ignore NACK from previous transfer
	// and proceed next transfer
	I2CIFG &= ~NACKIFG;

	I2CSA = 0x00;							// general call address
	U0CTL |= MST;                           // define Master Mode
	I2CTCTL |= I2CTRX;                      // I2CTRX=1 => Transmit Mode (R/W bit = 0)

	I2CTCTL |= I2CSTT;                      // start condition is generated
	while (I2CTCTL&I2CSTT)                 // wait till I2CSTT bit is cleared
	{
		// check Arbitration Lost before NACK as when Arbitration is lost you cannot send STOP after NACK
		if(i2c_checkArbLost())
		{
//			i2c_disable();
			i2cMasterStatus = I2C_ARB_LOST;
			return 0;
		}

		// check NACK before checking timeout to prevent timeout when we have NACK
		if(i2c_checkNACK())
		{
//			i2c_disable();
			i2cMasterStatus = I2C_NACK;
			return 0;
		}

		if(i2c_checkTimeout())
		{
			U0CTL &= ~I2CEN;                        // clear I2CEN bit => necessary to re-configure I2C module
			I2CTCTL &= ~I2CRM;                      // transmission is by the I2C module
//			U0CTL |= I2CEN;                         // enable I2C module
			i2cMasterStatus = I2C_TIMEOUT;
			return 0;
		}
	}

	I2CTCTL |= I2CSTP;                      // stop condition is generated after
	while (I2CTCTL&I2CSTP)					// wait till I2CSTP bit is cleared
	{
		if(i2c_checkTimeout())
		{
			U0CTL &= ~I2CEN;                        // clear I2CEN bit => necessary to re-configure I2C module
			I2CTCTL &= ~I2CRM;                      // transmission is by the I2C module
//			U0CTL |= I2CEN;                         // enable I2C module
			i2cMasterStatus = I2C_TIMEOUT;
			return 0;
		}
	}

	U0CTL &= ~I2CEN;                        // clear I2CEN bit => necessary to re-configure I2C module
	I2CTCTL &= ~I2CRM;                      // transmission is by the I2C module
//	U0CTL |= I2CEN;                         // enable I2C module

	// After STOP condition is transfered, disable all i2c interrupt
	i2c_disableAllInterrupt();

	// timer stop after sending STOP condition
	i2c_timerTimeoutStop();

	return 1;
}

/*
 * Producing many NACKs if address is not matched or busy
 * This function will be finished before it treats last NACK
 * So Make sure following transfer clear the NACKIFG if you want to ignore
 * Otherwise TXRDYIFG and RXRDYIFG will not be set
 */
uint8_t i2c_ackPolling(uint8_t slaveAddress)
{
	// disable all i2c interrupts
	i2c_disableAllInterrupt();

	// set default I2C status
	i2cMasterStatus = I2C_IDLE;

	// timer start
	i2c_timerInit(TIMER_A_ACLK, TIMER_A_DIVIDED_BY_8, TIMER_A_UP_MODE , DEFAULT_CCR0); // set i2c timeout timer
	i2c_timerTimeoutStart();

	i2cMasterStatus = i2c_waitForBusReady();
	if (i2cMasterStatus != I2C_IDLE)
	{
//		i2c_disable();
		return 0;
	}

	// init master everytime you start new transfer
	i2c_masterInit(I2C_CLOCKSOURCE_SMCLK, 0, 3, 3);

	U0CTL &= ~I2CEN;                        // clear I2CEN bit => necessary to re-configure I2C module
	I2CTCTL |= I2CRM;                       // transmission is software controlled
	U0CTL |= I2CEN;                         // enable I2C module

	i2c_disableAllInterrupt();
	I2CSA = slaveAddress;					// slave address

	I2CIFG |= NACKIFG;                       // set NACKIFG

	while (NACKIFG & I2CIFG)
	{
		I2CIFG &= ~NACKIFG;                        // clear I2C interrupt flags

		if(i2c_checkTimeout())
		{
			U0CTL &= ~I2CEN;                        // clear I2CEN bit => necessary to re-configure I2C module
			I2CTCTL &= ~I2CRM;                      // transmission is by the I2C module
			U0CTL |= I2CEN;                         // enable I2C module
			i2cMasterStatus = I2C_NACK;
			return 0;
		}

		U0CTL |= MST;                       // define Master Mode
		I2CTCTL |= I2CTRX;                  // I2CTRX=1 => Transmit Mode (R/W bit = 0)

		I2CTCTL |= I2CSTT;                  // start condition is generated
		while (I2CTCTL & I2CSTT)             // wait till I2CSTT bit was cleared
		{
			if(i2c_checkTimeout())
			{
				U0CTL &= ~I2CEN;                        // clear I2CEN bit => necessary to re-configure I2C module
				I2CTCTL &= ~I2CRM;                      // transmission is by the I2C module
				U0CTL |= I2CEN;                         // enable I2C module
				i2cMasterStatus = I2C_TIMEOUT;
				return 0;
			}
		}

		I2CTCTL |= I2CSTP;                  // stop condition is generated after slave address was sent => I2C
		while (I2CTCTL & I2CSTP)             	// wait till stop bit is reset
		{
			if(i2c_checkTimeout())
			{
				U0CTL &= ~I2CEN;                        // clear I2CEN bit => necessary to re-configure I2C module
				I2CTCTL &= ~I2CRM;                      // transmission is by the I2C module
				U0CTL |= I2CEN;                         // enable I2C module
				i2cMasterStatus = I2C_TIMEOUT;
				return 0;
			}

		}
//		__delay_cycles(500);                // Software delay
	}

	U0CTL &= ~I2CEN;                        // clear I2CEN bit => necessary to re-configure I2C module
	I2CTCTL &= ~I2CRM;                      // transmission is by the I2C module
	U0CTL |= I2CEN;     					// enable I2C module

	i2cMasterStatus = I2C_ACK;

	// After STOP condition is transfered, disable all i2c interrupt
	i2c_disableAllInterrupt();

	// timer stop after sending STOP condition
	i2c_timerTimeoutStop();

	return 1;
}

uint8_t i2c_masterWrite(uint8_t slaveAddress, uint8_t byteCount, uint8_t *data)
{
	// disable all i2c interrupts
	i2c_disableAllInterrupt();

	I2CIFG =0;

	// copy Tx buffer address
	masterTxData = data;

	// curent Tx byte cnt = 0;
	masterTxCurrentCnt = 0;

	// set default I2C status
	i2cMasterStatus = I2C_IDLE;

	// timer start
	i2c_timerInit(TIMER_A_ACLK, TIMER_A_DIVIDED_BY_8, TIMER_A_UP_MODE , DEFAULT_CCR0); // set i2c timeout timer
	i2c_timerTimeoutStart();

	//TODO: testing
	// check bus status after reset I2CEN in masterInit
	i2cMasterStatus = i2c_waitForBusReady();
	if (i2cMasterStatus != I2C_IDLE)
	{
//		i2c_disable();
		return 0;
	}

	// init master every time you start new transfer
	i2c_masterInit(I2C_CLOCKSOURCE_SMCLK, 0, 3, 3);


	// TODO: check the location of interrupt enable
//	i2c_enableInterrupt(I2C_ARB_LOST_INT);
	I2CSA = slaveAddress;

	// change I2CNDAT after Access Ready is ok from previous transfer
	I2CNDAT = byteCount;					// Set how many bytes to send
	// Clear Access ready IFG to check whether this transfer is done with i2c_waitForPreviousTransfer()
	I2CIFG &= ~ARDYIFG;

	// Clear UCALIFG if you previous transfer ended with arbitration lost
	I2CIFG &= ~ALIFG;

	// Clear NACKIFG:
	// This Is Very Important If you Want Ignore NACK from previous transfer
	// and proceed next transfer
	I2CIFG &= ~NACKIFG;
	U0CTL |= MST;                           // Set to master mode every time you send data as this is cleared after sending
//	I2CTCTL |= I2CSTT +I2CSTP+ I2CTRX;    // write data with START and STOP and set to TX mode
	I2CTCTL |= I2CSTT + I2CTRX;    // write data with START and set to TX mode

	// check data ACK is received while sending data
	// When NACK is received, NO TXRDYIFG set
	// So the below should not be executed, or timeout will occur
	for(masterTxCurrentCnt = 0 ; masterTxCurrentCnt < byteCount ; masterTxCurrentCnt++)
	{
		while((I2CIFG & TXRDYIFG) == 0)
		{
			// check Arbitration Lost before NACK as when Arbitration is lost you cannot send STOP after NACK
			if(i2c_checkArbLost())
			{
//				i2c_disable();
				i2cMasterStatus = I2C_ARB_LOST;
				return 0;
			}

			// check NACK before checking timeout to prevent timeout when we have NACK
			if(i2c_checkNACK())
			{
//				i2c_disable();
				i2cMasterStatus = I2C_NACK;
				return 0;
			}

			if(i2c_checkTimeout())
			{
//				i2c_disable();
				i2cMasterStatus = I2C_TIMEOUT;
				return 0;
			}
		}
		// writing data and TXRDYIFG cleared
		I2CDRB = masterTxData[masterTxCurrentCnt];
	}

	// when NACK is received in the transfer we need to send stop manually
	// ARDYIFG seems unreliable,
	// so we check I2CSTP for transfer completion
	I2CTCTL |= I2CSTP;
	while (I2CTCTL & I2CSTP)             	// wait till stop bit is reset
	{
		if(i2c_checkTimeout())
		{
			i2cMasterStatus = I2C_TIMEOUT;
			return 0;
		}

	}

//	i2cMasterStatus = i2c_waitForPreviousTransfer(byteCount);
//	if (i2cMasterStatus != I2C_TRANSFER_DONE)
//	{
////		i2c_disable();
//		return 0;
//	}

//	i2c_disable();
	// After STOP condition is transfered, disable all i2c interrupt
	i2c_disableAllInterrupt();

	// timer stop after sending STOP condition
	i2c_timerTimeoutStop();

	// when everything goes well we return final count
	// value 0 means something wrong
	return masterTxCurrentCnt;
}

/*
Note: I2CNDAT Register
Do not change the I2CNDAT register after setting I2CSTT and before
I2CNDAT number of bytes have been transmitted. Otherwise, unpredictable
operation may occur. If the I2CNDAT contents must be updated for a
RESTART, wait for ARDYIFG to become set before modifying the contents
of I2CNDAT.
*/

uint8_t i2c_masterRead(uint8_t slaveAddress, uint8_t byteCount, uint8_t *data)
{
	// disable all i2c interrupts
	i2c_disableAllInterrupt();

	// copy Master Rx Buffer Address
	masterRxData = data;

	// current Tx byte cnt = 0;
	masterRxCurrentCnt = 0;

	// set default I2C status
	i2cMasterStatus = I2C_IDLE;

	// timer start
	i2c_timerInit(TIMER_A_ACLK, TIMER_A_DIVIDED_BY_8, TIMER_A_UP_MODE , DEFAULT_CCR0); // set i2c timeout timer
	i2c_timerTimeoutStart();

	// check bus status
	i2cMasterStatus = i2c_waitForBusReady();
	if (i2cMasterStatus != I2C_IDLE)
	{
//		i2c_disable();
		return 0;
	}

	// init master everytime you start new transfer
	i2c_masterInit(I2C_CLOCKSOURCE_SMCLK, 0, 3, 3);

	// TODO: check the location of interrupt enable
//	i2c_enableInterrupt(I2C_ARB_LOST_INT);
	I2CSA = slaveAddress;
	I2CNDAT = byteCount;					// Set how many bytes to receive
	// Clear Access ready IFG to check whether this transfer is done with i2c_waitForPreviousTransfer()
	I2CIFG &= ~ARDYIFG;

	// Clear UCALIFG if you previous transfer ended with arbitration lost
	I2CIFG &= ~ALIFG;

	// Clear NACKIFG:
	// This Is Very Important If you Want Ignore NACK from previous transfer
	// and proceed next transfer
	I2CIFG &= ~NACKIFG;
	U0CTL |= MST;                           // Set to master mode every time you send data as this is cleared after sending
	I2CTCTL &= ~I2CTRX;						// reset if the mode was TX before
//	I2CTCTL |= I2CSTT +I2CSTP;    			// receive data with START and STOP condition
	I2CTCTL |= I2CSTT;    			// receive data with START and STOP condition


	// check data ACK is received while sending data
	// When NACK is received, NO RXRDYIFG set
	// So the below should not be executed, or timeout will occur
	for(masterRxCurrentCnt = 0 ; masterRxCurrentCnt < byteCount ; masterRxCurrentCnt++)
	{
		while((I2CIFG & RXRDYIFG) == 0)
		{
			// check Arbitration Lost before NACK as when Arbitration is lost you cannot send STOP after NACK
			if(i2c_checkArbLost())
			{
//				i2c_disable();
				i2cMasterStatus = I2C_ARB_LOST;
				return 0;
			}

			// check NACK before checking timeout to prevent timeout when we have NACK
			if(i2c_checkNACK())
			{
//				i2c_disable();
				i2cMasterStatus = I2C_NACK;
				return 0;
			}

			if(i2c_checkTimeout())
			{
//				i2c_disable();
				i2cMasterStatus = I2C_TIMEOUT;
				return 0;
			}
		}

		// writing data and RXRDYIFG cleared
		masterRxData[masterRxCurrentCnt]=I2CDRB;
	}

	// when NACK is received in the transfer we need to send stop manually
	// ARDYIFG seems unreliable,
	// so we check I2CSTP for transfer completion
	I2CTCTL |= I2CSTP;
	while (I2CTCTL & I2CSTP)             	// wait till stop bit is reset
	{
		if(i2c_checkTimeout())
		{
			i2cMasterStatus = I2C_TIMEOUT;
			return 0;
		}

	}

//	i2cMasterStatus = i2c_waitForPreviousTransfer(byteCount);
//	if (i2cMasterStatus != I2C_TRANSFER_DONE)
//	{
////		i2c_disable();
//		return 0;
//	}

//	i2c_disable();

	// After STOP condition is transfered, disable all i2c interrupt
	i2c_disableAllInterrupt();

	// timer stop after sending STOP condition
	i2c_timerTimeoutStop();

	// when everything goes well we return final count
	// value 0 means something wrong
	return masterRxCurrentCnt;
}


uint8_t i2c_masterWriteAndRead(uint8_t slaveAddress, uint8_t txByteCount, uint8_t *txData, uint8_t rxByteCount, uint8_t *rxData)
{
	if(!i2c_masterWrite(slaveAddress, txByteCount, txData))
	{
		return 0;
	}
	if(!i2c_masterRead(slaveAddress, rxByteCount, rxData))
	{
		return 0;
	}

	return 1;
}

/*
 Errata sheet I2C16 USART Module

Function I2C Slave may not detect own address correctly
Description When an interrupt occurs between ACK and stop conditions of a slave transmission, the
slave may not acknowledge the slave address byte if all below conditions are fulfilled:
- STT interrupt is enabled
- Device is in LPMx during start condition.
If the failure occurs, the I2C state machine switches into IDLE state.

Workaround
(1)Do not use the STT interrupt for slave transmission.
Or
(2)Disable all interrupts between ACK and stop condition on I2C
 */

void i2c_slaveInit(uint8_t ownAddress, uint8_t dataSize, uint8_t *data)
{
	// disable all i2c interrupt in the beginning
	i2c_disableAllInterrupt();

	i2cInteralAddress = 0; 						// Init internal address
	slaveIndex = 0;								// reset slave index
	slaveBufSize = dataSize;					// Store slave buffer size
	slaveData = data;							// store buffer address
	defaultSlaveData = data;					// Store default slave data address

	U0CTL |= I2C + SYNC;                      	// Recommended init procedure
	U0CTL &= ~I2CEN;                          	// Disable I2C
	I2COA = ownAddress;

	// slave needs clock setting!!!
	I2CTCTL = I2C_CLOCKSOURCE_SMCLK;            // Slave need to set clock!!!!!! Important !!! SMCLK mostly
	// From Delfi-n3Xt document, Second, the I2C slave implementation does not need
	// to configure the I2C bus speed including the prescaler and the clock high and low period.
//	I2CPSC = 0; 								// I2CPSC should be <=4 or error might occur
//	I2CSCLH = 32; 								// High period of SCL (I2CSCLH+2) x (I2CPSC+ 1)
//	I2CSCLL = 32;  								// Low period of SCL (I2CSCLL+2) x (I2CPSC + 1)
	I2CNDAT = 0;                           		// initialize data cnt;
	U0CTL |= I2CEN;                         	// Enable I2C
}


uint8_t i2c_getSlaveTxDone(void)
{
	return i2cSlaveTxDone;
}

void i2c_setSlaveTxDone(uint8_t status)
{
	i2cSlaveTxDone = status;
}

uint8_t i2c_getSlaveRxDone(void)
{
	return i2cSlaveRxDone;
}

void i2c_setSlaveRxDone(uint8_t status)
{
	i2cSlaveRxDone = status;
}

uint8_t i2c_getGeneralCallDetected(void)
{
	return i2cGeneralCallDetected;
}

void i2c_setGeneralCallDetected(uint8_t status)
{
	   i2cGeneralCallDetected = status;
}


/*
 * Start the i2c slave to wait for the data
 */

void i2c_slaveStart(void)
{
	// init I2C slave status
	i2cSlaveTxDone = 0;
	i2cSlaveRxDone = 0;
	i2cSlaveTxInProgress = 0;
	i2cSlaveRxInProgress = 0;
	i2cStartDetected = 0;
	i2cGeneralCallDetected = 0;

	i2c_enableInterrupt(I2C_TX_RDY_INT+ I2C_RX_RDY_INT + I2C_START_INT + I2C_GENERAL_CALL_INT +I2C_ACCESS_RDY_INT);
}

/*
 * Stop i2c slave for data processing
 */

void i2c_slaveStop(void)
{
	i2c_disableAllInterrupt();
}

uint8_t i2c_getInternalAddress(void)
{
	return i2cInteralAddress;
}

uint8_t i2c_checkInternalAddress(uint8_t addr)
{
	if (addr >= 0 && addr <8)
		return 1;
	else
		return 0;
}

// TODO: NOT WORKING YET
void i2c_busRecoverySequence(void)
{
	volatile uint8_t i;

	P3SEL &= ~(I2C_SDA_PIN + I2C_SCL_PIN);		// set pins to GPIO
	P3DIR |= I2C_SDA_PIN + I2C_SCL_PIN;			// set output direction
	P3OUT |= I2C_SDA_PIN;						// set SDA HIGH
	for (i = 0 ; i < 9 ; i++)
	{
		P3OUT |= I2C_SCL_PIN;					// toggle SCL 9 times
		// TODO: delay 5us
		P3OUT &= ~I2C_SCL_PIN;
		// TODO: delay 5us
	}
	P3SEL |= I2C_SDA_PIN + I2C_SCL_PIN;			// set pins back to I2C
}


// Common ISR for I2C Module
#pragma vector=USART0TX_VECTOR
__interrupt void I2C_ISR(void)
{
 switch(I2CIV)
 {
   case  0: break;                          	// No interrupt
   case  2:										// Arbitration lost
	   // USART I2C is Multi-Master by default
	   // Master becom Slave when:
	   // 1. Arbtrtion is lost
	   // 2. After Data Transfer
	   // To prevent I2C module automatically become Slave mode from Master mode,
	   // we explicitly manage below as below
	   //
	   // when we lost arbitration during data transfer
	   // we first need to disable current interrupt to prevent the routine keep coming back to ISR
	   // and need to restart the transfer
	   //P6OUT ^= BIT2;
//	   i2c_disableAllInterrupt();
//	   __bic_SR_register_on_exit(LPM3_bits);;	// Clear LPM3
	   break;
   case  4:                          	 		// No Acknowledge
   	   break;
   case  6:										// Own Address
	  // P6OUT &= ~0x01;
	   break;
   case  8:										// Register Access Ready
	   if(U0CTL & MST)							// Master mode
	   {

	   }
	   else										// Slave mode
	   {
		   // When I2CRM = 0 and Slave mode, ARDYIFG flag means STOP is detected
		   // When Stop is detected,
		   // clear the flag first (even it's automatic do it for sure)
		   // I2CIFG &= ~ARDYIFG;
		   // Initialize the slave buffer
		   // do not disable i2c interrupts
		   // exit ISR

		   // if receive mode
		   if(i2cSlaveRxInProgress)
		   {
			   i2cSlaveRxInProgress = 0;
			   i2cSlaveRxDone = 1;
		   }

		   // if transmit mode
		   if(i2cSlaveTxInProgress)
		   {
			   i2cSlaveTxInProgress = 0;
			   i2cSlaveTxDone = 1;
		   }
		   __bic_SR_register_on_exit(LPM3_bits);
	   }
//	   P6OUT ^= BIT4; 							// TODO:Test
	   break;
   case 10:                                 	// Receive Ready
	   if(U0CTL & MST)
	   {

	   }
	   else										// Slave mode
	   {
		   if (i2cStartDetected)
		   {
			   i2cStartDetected = 0;
			   i2cSlaveRxInProgress = 1;
			   i2cInteralAddress = I2CDRB;

			   // boundary check
			   if((i2cInteralAddress >=0) && (i2cInteralAddress < slaveBufSize))
			   {
				   slaveData = slaveData + i2cInteralAddress;
				   slaveIndex = i2cInteralAddress;
			   }
			   else
			   {
				   // if internal address is not matched, set to default address
				   i2cInteralAddress = 0x00;
				   slaveIndex = 0;
				   // from the datasheet, USART module cannot send NACK when in Slave mode
				   // so make master sure to send right internal address otherwise master will read trash value
				   // TODO: do something when slave has wrong internal address
			   }
		   }
		   else
		   {
			   // check Overflows
			   if(slaveIndex < slaveBufSize)
			   {
				   *slaveData = I2CDRB;
				   slaveData++;
				   slaveIndex++;
			   }
			   else
			   {
				   volatile uint8_t dummy;
				   // To clear the interrupt flag, write data to dummy
				   dummy = I2CDRB;
			   }
		   }

		   //P6OUT ^= BIT3;						// TODO: Test
//		   if(slaveIndex - 1)
//		   {
//			   if(slaveIndex == defaultSlaveIndex)
//			   {
//				   i2cInteralAddress = I2CDRB;
//				   if(i2c_checkInternalAddress(i2cInteralAddress))
//				   {
//					   slaveData = slaveData + i2cInteralAddress * 8;
//					   slaveIndex = 8;
//				   }
//				   else
//				   {
//					   // if internal address is not matched, set to default address
//					   i2cInteralAddress = 0x00;
//					   // from the datasheet, USART module cannot send NACK when in Slave mode
//					   // so make master sure to send right internal address otherwise master will read trash value
//					   // TODO: do something when slave has wrong internal address
//				   }
//			   }
//			   else
//			   {
//				   *slaveData = I2CDRB;
//				   slaveData++;
//				   slaveIndex--;
//			   }
//		   }
//		   else
//		   {
//			   // all data we want are received
//			   *slaveData = I2CDRB;
//			   i2cSlaveRxDone = 1;
//			   // Disable Rx interrupt and wake up for data processing
//			   __bic_SR_register_on_exit(LPM3_bits);
//
//		   }
	   }
	   break;
   case 12: 									// Transmit Ready
	   if(U0CTL & MST)
	   {

	   }
	   else										// Slave mode
	   {
//		   P6OUT ^= BIT3;	// TODO: Test

		   if(i2cStartDetected)
		   {
			   i2cStartDetected = 0;
			   i2cSlaveTxInProgress = 1;
			   slaveData = slaveData + i2cInteralAddress;
			   slaveIndex = i2cInteralAddress;
			   I2CDRB = *slaveData;
			   slaveData++;
			   slaveIndex++;
		   }
		   else
		   {
			   // check overflows
			   if(slaveIndex < slaveBufSize)
			   {
				   I2CDRB = *slaveData;
				   slaveData++;
				   slaveIndex++;
			   }
			   else
			   {
				   I2CDRB = 0xFF;
			   }
		   }
//		   if(slaveIndex -1)
//		   {
//			   if(slaveIndex == defaultSlaveIndex)	// first byte?
//			   {
//				   // move base address of the tx buffer and send the data
//				   slaveData = slaveData + i2cInteralAddress*8;
//				   I2CDRB = *slaveData;
//				   slaveIndex = 8;
//				   slaveData++;
//				   slaveIndex --;
//			   }
//			   else									// rest of the bytes
//			   {
//				   I2CDRB = *slaveData;
//				   slaveData++;
//				   slaveIndex--;
//			   }
//		   }
//		   else										// send the last byte
//		   {
//				I2CDRB = *slaveData;
//				i2cSlaveTxDone = 1;
//				// Disable Tx interrupt and wake up for data processing
//				__bic_SR_register_on_exit(LPM3_bits);
//
//		   }
	   }
	   break;
   case 14: 									// General Call
	   // when general call is received
	   // do something to prevent the system going into standalone mode
	   // and make sure standalone mode has i2c slave on!
	   I2CIFG &= ~GCIFG;
	   // TODO: change this to flag. Only the slave can receive general call
	   i2cGeneralCallDetected = 1;
	   __bic_SR_register_on_exit(LPM3_bits);
//	   P6OUT ^= BIT3;							// TODO:Test
	   break;
   case 16: 									// Start Condition Detected when in Slave mode TODO: check errata sheet
	   i2cStartDetected = 1;
	   slaveIndex = 0;							// Initialize the index counter for slave mode
	   slaveData = defaultSlaveData;			// Initialize the base address of slave buffer
	   break;
 }
}
